//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B4aEventAction.cc 75604 2013-11-04 13:17:26Z gcosmo $
// 
/// \file B4aEventAction.cc
/// \brief Implementation of the B4aEventAction class

#include "B4aEventAction.hh"
#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>
#include <vector>
#include <numeric>
#include <functional>
#include <algorithm>

#include "TrackerSD.hh"
#include "TrackerHit.hh"
#include "SiPMsd.hh"
#include "G4SDManager.hh"

#include "B4DetectorConstruction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aEventAction::B4aEventAction()
 : G4UserEventAction(),
   fAbsMateName(""),
   fVoxelsAlongY(-1),
   fTrackerHCID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aEventAction::~B4aEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aEventAction::BeginOfEventAction(const G4Event* /*event*/)
{  
  // initialisation per event
  fVecShowerPDG.clear();
  fVecShowerCharge.clear();
  fVecShowerPosition.clear();
  fVecShower4Momentum.clear();
  fVecShowerCoMi.at(kCkov).clear();
  fVecShowerCoMi.at(kScnt).clear();
  fVecShowerCoMj.at(kCkov).clear();
  fVecShowerCoMj.at(kScnt).clear();
  fVecIndex.at(kCkov).clear();
  fVecIndex.at(kScnt).clear();
  fVecSignal.at(kCkov).clear();
  fVecSignal.at(kScnt).clear();

  auto constUserDetector = G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  auto constDetector = static_cast<const B4DetectorConstruction*>(constUserDetector);
  auto detector = const_cast<B4DetectorConstruction*>(constDetector);
  fAbsMateName = detector->GetAbsMateName();
  fVoxelsAlongY = detector->GetVoxelsAlongY();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerHitsCollection* 
B4aEventAction::GetTrackerHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<TrackerHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4aEventAction::GetTrackerHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}    

SiPMhitsCollection* 
B4aEventAction::GetSiPMhitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<SiPMhitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4aEventAction::GetSiPMhitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}  

std::tuple<std::vector<int>, std::vector<int>> B4aEventAction::GetVectors(SiPMhitsCollection *HC) const {
  std::vector<int> keys;
  std::vector<int> values;
  keys.reserve(HC->GetSize());
  values.reserve(HC->GetSize());
  for (auto const& [key, val] : *HC->GetMap()) {
    keys.push_back(key);
    values.push_back(*val);
  }
  return {keys, values};
}

std::tuple<G4double, G4double> B4aEventAction::GetCentreOfMass(SiPMhitsCollection *HC) const {
  auto [keys, values] = GetVectors(HC);
  std::vector<int> Ni;
  std::vector<int> Nj;
  Ni.reserve(keys.size());
  Nj.reserve(keys.size());
  std::transform(keys.begin(), keys.end(), std::back_inserter(Ni), std::bind2nd(std::divides<int>(), fVoxelsAlongY));
  std::transform(keys.begin(), keys.end(), std::back_inserter(Nj), std::bind2nd(std::modulus<int>(), fVoxelsAlongY));
  G4double sum = std::accumulate(values.begin(), values.end(), 0.);
  G4double ri = std::inner_product(Ni.begin(), Ni.end(), values.begin(), 0.);
  G4double rj = std::inner_product(Nj.begin(), Nj.end(), values.begin(), 0.);
  G4double CoMi = -1.;
  G4double CoMj = -1.;
  if (sum != 0.) {
    CoMi = ri / sum;
    CoMj = rj / sum;
  }
  return {CoMi, CoMj};
}

void B4aEventAction::EndOfEventAction(const G4Event* event)
{
  G4int mode = -1;

  // get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  
  G4PrimaryParticle *primary = event->GetPrimaryVertex()->GetPrimary();

  auto sdMan = G4SDManager::GetSDMpointer();

  // Get tracker hits collections ID (only once)
  if ( fTrackerHCID == -1 ) {
    fTrackerHCID = sdMan->GetCollectionID("TrackerHitsCollection");
  }

  // Get hits collection
  auto trackerHC = GetTrackerHitsCollection(fTrackerHCID, event);
  G4double lateralLeakage = 0.;

  std::array<const SiPMsd *, kNProc> sdArray;
  sdArray.at(kCkov) = static_cast<const SiPMsd *>(sdMan->FindSensitiveDetector("C_SiPMsd"));
  sdArray.at(kScnt) = static_cast<const SiPMsd *>(sdMan->FindSensitiveDetector("S_SiPMsd"));
  std::array<SiPMhitsCollection, kNProc> hcArray;

  for (size_t i = 0; i < trackerHC->GetSize(); ++i) {
    auto hit = static_cast<TrackerHit *>(trackerHC->GetHit(i));
    lateralLeakage += hit->GetLateralLeakage();

    G4ThreeVector position = hit->GetPosition();
    G4ThreeVector momentum = hit->GetMomentum();
    fVecShowerPDG.push_back(hit->GetPDGEncoding());
    fVecShowerCharge.push_back(hit->GetPDGCharge());
    fVecShowerPosition.push_back(position.x());
    fVecShowerPosition.push_back(position.y());
    fVecShowerPosition.push_back(position.z());
    fVecShower4Momentum.push_back(hit->GetTotalEnergy());
    fVecShower4Momentum.push_back(momentum.x());
    fVecShower4Momentum.push_back(momentum.y());
    fVecShower4Momentum.push_back(momentum.z());

    for (int j = 0; j < kNProc; ++j) {
      // get hits collections ID
      G4int HCID = sdMan->GetCollectionID(sdArray.at(j)->GetCollectionName(i));
      
      // get hits collections
      auto HC = GetSiPMhitsCollection(HCID, event);

      // compute centre of mass
      auto [CoMi, CoMj] = GetCentreOfMass(HC);
      fVecShowerCoMi.at(j).push_back(CoMi);
      fVecShowerCoMj.at(j).push_back(CoMj);

      // sum shower signals
      hcArray.at(j) += *HC;
    }
  }
  
  // fill vectors with total signal
  for (int j = 0; j < kNProc; ++j) {
    auto [keys, values] = GetVectors(&(hcArray.at(j)));
    fVecIndex.at(j) = keys;
    fVecSignal.at(j) = values;
  }

  // fill ntuple event by event
  analysisManager->FillNtupleIColumn(0, primary->GetPDGcode());
  analysisManager->FillNtupleDColumn(1, primary->GetTotalEnergy());
  fVecPrimaryPolarization.push_back(primary->GetPolX());
  fVecPrimaryPolarization.push_back(primary->GetPolY());
  fVecPrimaryPolarization.push_back(primary->GetPolZ());
  analysisManager->FillNtupleIColumn(3, mode);
  analysisManager->FillNtupleDColumn(4, lateralLeakage);
  analysisManager->FillNtupleSColumn(5, fAbsMateName);
  analysisManager->FillNtupleIColumn(6, fVoxelsAlongY);

  analysisManager->AddNtupleRow();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
