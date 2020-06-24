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
// $Id: B4aEventAction.hh 75215 2013-10-29 16:07:06Z gcosmo $
// 
/// \file B4aEventAction.hh
/// \brief Definition of the B4aEventAction class

#ifndef B4aEventAction_h
#define B4aEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

#include "SiPMsd.hh"
#include "TrackerSD.hh"

/// Event action class 

class B4aEventAction : public G4UserEventAction
{
  public:
    B4aEventAction();
    virtual ~B4aEventAction();

    virtual void  BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);
    
    std::vector<G4int>& GetVecShowerPDG() { return fVecShowerPDG; }
    std::vector<G4double>& GetVecShowerCharge() { return fVecShowerCharge; }
    std::vector<G4double>& GetVecShowerPosition() { return fVecShowerPosition; }
    std::vector<G4double>& GetVecShowerEnergy() { return fVecShowerEnergy; }
    std::vector<G4double>& GetVecShowerMomentum() { return fVecShowerMomentum; }
    std::vector<G4double>& GetVecShowerCkovCoMi() { return fVecShowerCoMi.at(kCkov); }
    std::vector<G4double>& GetVecShowerCkovCoMj() { return fVecShowerCoMj.at(kCkov); }
    std::vector<G4double>& GetVecShowerCkovRad() { return fVecShowerRad.at(kCkov); }
    std::vector<G4double>& GetVecShowerScntCoMi() { return fVecShowerCoMi.at(kScnt); }
    std::vector<G4double>& GetVecShowerScntCoMj() { return fVecShowerCoMj.at(kScnt); }
    std::vector<G4double>& GetVecShowerScntRad() { return fVecShowerRad.at(kScnt); }
    std::vector<G4int>& GetVecIndexCkov() { return fVecIndex.at(kCkov); } 
    std::vector<G4int>& GetVecSignalCkov() { return fVecSignal.at(kCkov); }
    std::vector<G4int>& GetVecIndexScnt() { return fVecIndex.at(kScnt); }
    std::vector<G4int>& GetVecSignalScnt() { return fVecSignal.at(kScnt); }

  private:
    G4String fAbsMateName;  ///< Absorber material name
    G4int fVoxelsAlongY;
    G4bool fUseHepMC;

    TrackerHitsCollection* GetTrackerHitsCollection(G4int hcID, const G4Event* event) const;
    SiPMhitsCollection* GetSiPMhitsCollection(G4int hcID, const G4Event* event) const;
    std::tuple<std::vector<int>, std::vector<int>> GetVectors(SiPMhitsCollection *HC) const;
    std::tuple<G4double, G4double, G4double> GetCentreOfMass(SiPMhitsCollection *HC) const;

    enum ProcessIndex {
      kCkov,  ///< Cerenkov process index
      kScnt,  ///< Scintillation process index
      kNProc  ///< Number of processes
    };

    std::vector<G4int> fVecShowerPDG;
    std::vector<G4double> fVecShowerCharge;
    std::vector<G4double> fVecShowerPosition;
    std::vector<G4double> fVecShowerEnergy;
    std::vector<G4double> fVecShowerMomentum;
    std::array<std::vector<G4double>, kNProc> fVecShowerCoMi;
    std::array<std::vector<G4double>, kNProc> fVecShowerCoMj;
    std::array<std::vector<G4double>, kNProc> fVecShowerRad;
    std::array<std::vector<G4int>, kNProc> fVecIndex;   ///< Scintillating fibre p.e.
    std::array<std::vector<G4int>, kNProc> fVecSignal;  ///< Cherenkov fibre p.e.

    G4int fTrackerHCID;
};

#endif
