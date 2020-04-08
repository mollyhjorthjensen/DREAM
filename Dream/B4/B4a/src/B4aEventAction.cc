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

#include "SiPMsd.hh"
#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aEventAction::B4aEventAction()
 : G4UserEventAction(),
   Energyem(0.),
   EnergyScin(0.),
   EnergyCher(0.),
   NofCherenkovDetected(0),
   //NofScintillationDetected(0),
   EnergyTot(0.),
   PrimaryParticleEnergy(0.),
   EscapedEnergy(0.),
   VectorSignals(0.),
   VectorSignalsCher(0.),
   fSSiPMhcID(-1), 
   fCSiPMhcID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aEventAction::~B4aEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aEventAction::BeginOfEventAction(const G4Event* /*event*/)
{  
  // initialisation per event
  Energyem = 0.;
  EnergyScin = 0.;
  EnergyCher = 0.;
  NofCherenkovDetected = 0;
  EscapedEnergy = 0;
  //NofScintillationDetected = 0;
  EnergyTot = 0;
  /*for(int i=0;i<64;i++){
    Signalfibre[i]=0;
  }*///only if you want to use SignalFibre[64]
  for (int i=0;i<VectorSignals.size();i++){
  VectorSignals.at(i)=0.;
}
  for (int i=0;i<VectorSignalsCher.size();i++){
  VectorSignalsCher.at(i)=0.;
  }
  PrimaryParticleEnergy = 0;  
  for(int i=0;i<322624;i++){
    if(VectorSignals.size() < 322624){
  VectorSignals.push_back(0.);}}
  //VectorSignals.at(i)=0;}
  for(int k=0;k<322624;k++){
    if(VectorSignalsCher.size() < 322624){
  VectorSignalsCher.push_back(0.);}}
  //VectorSignalsCher[k]=0;}  

  S_vec_key.clear();
  S_vec_val.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SiPMhitsCollection* 
B4aEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<SiPMhitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4aEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}  

void B4aEventAction::EndOfEventAction(const G4Event* event)
{
  // Accumulate statistics
  //

  // get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  //analysisManager->FillH1(1, Energymodule);
  //analysisManager->FillH1(2, TrackLmodule);
  //analysisManager->FillH1(3, EnergyScin);
  
  

  // fill ntuple event by event
  analysisManager->FillNtupleDColumn(0, Energyem);
  analysisManager->FillNtupleDColumn(1, EnergyScin);
  analysisManager->FillNtupleDColumn(2, EnergyCher);
  analysisManager->FillNtupleDColumn(3, NofCherenkovDetected);
  analysisManager->FillNtupleDColumn(4, EnergyTot);
  analysisManager->FillNtupleDColumn(5, PrimaryParticleEnergy);
  analysisManager->FillNtupleSColumn(6, PrimaryParticleName);
  analysisManager->FillNtupleSColumn(7, AbsorberMaterial);
  analysisManager->FillNtupleDColumn(8, EscapedEnergy);

  //print here if you need event by event some information of the screen
  //G4cout<<EnergyTot<<G4endl;  
  //G4cout<<EnergyTot <<" "<< EnergyScin <<" "<< NofCherenkovDetected <<G4endl;
  /*for(int a=0; a<64; a++){
  G4cout<< a<<" "<< Signalfibre[a] <<" "<< EnergyTot <<G4endl;
  }*/
  //G4cout<< EnergyTot <<" "<< energyionization <<" "<< energyphoton << G4endl;

  // Get hits collections IDs (only once)
  auto sdMan = G4SDManager::GetSDMpointer();
  auto S_sd = static_cast<const SiPMsd *>(sdMan->FindSensitiveDetector("S_SiPMsd"));
  for (G4int i = 0; i < S_sd->GetNumberOfCollections(); ++i)
  {
    G4int HCID = sdMan->GetCollectionID(S_sd->GetCollectionName(i));
    // Get hits collections
    auto S_HC = GetHitsCollection(HCID, event);
    // auto C_HC = GetHitsCollection(fCSiPMhcID, event);
    S_HC->PrintAllHits();
  }
  auto C_sd = static_cast<const SiPMsd *>(sdMan->FindSensitiveDetector("C_SiPMsd"));
  for (G4int i = 0; i < C_sd->GetNumberOfCollections(); ++i)
  {
    G4int HCID = sdMan->GetCollectionID(C_sd->GetCollectionName(i));
    // Get hits collections
    auto C_HC = GetHitsCollection(HCID, event);
    // auto C_HC = GetHitsCollection(fCSiPMhcID, event);
    C_HC->PrintAllHits();
  }

  // for (auto const& [key, val] : *(S_HC->GetMap()) )
  // {
  //   S_vec_key.push_back(key);
  //   S_vec_val.push_back(*val);
  //   G4cout << "here : " << key << "\t" << *val << G4endl;
  // }

  analysisManager->AddNtupleRow(); //columns with vector are automatically filled with this function

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
