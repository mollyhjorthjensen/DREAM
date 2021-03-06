/**
 * \class SiPMsd
 * \brief Implementation of the user class SiPMsd
 */

#include "SiPMsd.hh"
#include "TrackInfo.hh"

#include "G4OpticalPhoton.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"

SiPMsd::SiPMsd(G4String SDname, G4String HCname, const G4int NofModules, const G4int NofFibers)
    : G4VSensitiveDetector(SDname), fNofModules(NofModules), fNofFibers(NofFibers)
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

G4int SiPMsd::GetShowerID(G4Track *aTrack) {
  auto info = static_cast<TrackInfo *>(aTrack->GetUserInformation());
  if (!info)
  {
    G4ExceptionDescription msg;
    msg << "Missing user track information";
    G4Exception("SiPMsd::GetShowerID()", "MyCode0004", FatalException, msg);
  }
  return info->GetShowerID();
}

G4int SiPMsd::GetRowMajorIndex(const G4int module, const G4int fibre) {
  G4int NiModule = module / fNofModules;
  G4int NiFibre = fibre / fNofFibers;
  G4int Ni = fNofFibers * NiModule + NiFibre;

  G4int NjModule = module % fNofModules;
  G4int NjFibre = fibre % fNofFibers;
  G4int Nj = fNofFibers * NjModule + NjFibre;
  return fNofFibers*fNofModules * Ni + Nj;
}

G4bool SiPMsd::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
  G4Track *aTrack = aStep->GetTrack();

  // check kinetic energy nonzero
  G4double edep = aTrack->GetTotalEnergy();
  if (edep == 0.) return false;

  // get copy number
  G4TouchableHandle touchable = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4int copyNoSiPM = touchable->GetCopyNumber(0);
  G4int copyNoModule = touchable->GetCopyNumber(1);
  G4int copyNo = GetRowMajorIndex(copyNoModule , copyNoSiPM);

  // particle is secondary
  if (aTrack->GetParentID() == 0) return false;
  G4String creatorProcess = aTrack->GetCreatorProcess()->GetProcessName();
  if (((SensitiveDetectorName == "C_SiPMsd") && (creatorProcess != "Cerenkov")) ||
      ((SensitiveDetectorName == "S_SiPMsd") && (creatorProcess != "Scintillation"))) return false;

  // get user track information and insert hit
  G4int count = 1;
  G4int showerID = GetShowerID(aTrack);
  fHitCollection.at(showerID)->add(copyNo, count);

  // kill track
  aTrack->SetTrackStatus(fKillTrackAndSecondaries);

  return true;
}

void SiPMsd::EndOfEvent(G4HCofThisEvent *) {}
