/**
 * \class SiPMsd
 * \brief Implementation of the user class SiPMsd
 */

#include "SiPMsd.hh"

#include "G4OpticalPhoton.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"

SiPMsd::SiPMsd(G4String SDname, G4String HCname)
    : G4VSensitiveDetector(SDname)
{
  for (size_t i = 0; i < MAX; ++i)
  {
    collectionName.insert(HCname + "_" + std::to_string(i));
  }
}

SiPMsd::~SiPMsd()
{
}

void SiPMsd::Initialize(G4HCofThisEvent *aHCE)
{
  for (G4int i = 0; i < GetNumberOfCollections(); ++i)
  {
    // create hit collection
    fHitCollection.at(i) = new SiPMhitsCollection(SensitiveDetectorName, GetCollectionName(i));
    G4int HCID = GetCollectionID(i);

    // store this in event
    aHCE->AddHitsCollection(HCID, fHitCollection.at(i));
  }
}

G4bool SiPMsd::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
  G4Track *aTrack = aStep->GetTrack();

  // check kinetic energy nonzero
  G4double edep = aTrack->GetTotalEnergy();
  if (edep == 0.) return false;

  // kill track
  aTrack->SetTrackStatus(fKillTrackAndSecondaries);

  // get shower number
  G4int showerNo = 0;

  // get copy number
  G4TouchableHandle touchable = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4int copyNo = touchable->GetVolume(0)->GetCopyNo();

  // insert hit
  if (aTrack->GetParentID() > 0) {  // particle is secondary
    G4String creatorProcess = aTrack->GetCreatorProcess()->GetProcessName();
    if (((SensitiveDetectorName == "S_SiPMsd") && (creatorProcess == "Scintillation")) || 
        ((SensitiveDetectorName == "C_SiPMsd") && (creatorProcess == "Cerenkov")))
    {
      G4int count = 1;
      fHitCollection.at(showerNo)->add(copyNo, count);
      G4cout << "hit : " << copyNo << G4endl; 
      return true;
    }
  } else return false;

}

void SiPMsd::EndOfEvent(G4HCofThisEvent *) {}
