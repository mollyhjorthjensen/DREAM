/**
 * @file
 * @brief Implementation of the user class TrackerSD
 */

#include "TrackerSD.hh"
#include "TrackInfo.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VSolid.hh"

TrackerSD::TrackerSD(const G4String &aName)
    : G4VSensitiveDetector(aName), fpHitsCollection(0)
{
  collectionName.insert("TrackerHitsCollection");
}

TrackerSD::~TrackerSD() {}

void TrackerSD::Initialize(G4HCofThisEvent *aHCE)
{
  // create hit collection
  fpHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);
  // store this in event
  G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  aHCE->AddHitsCollection(HCID, fpHitsCollection);
}

G4bool TrackerSD::IsLateralBoundary(const G4ThreeVector globalPoint)
{
  G4PhysicalVolumeStore *pvs(G4PhysicalVolumeStore::GetInstance());
  G4VPhysicalVolume *caloPV = pvs->GetVolume("Calorimeter");
  G4RotationMatrix *Rm = caloPV->GetRotation();
  const G4ThreeVector &Tr = caloPV->GetTranslation();
  G4AffineTransform transform = G4AffineTransform(Rm, Tr).Invert();
  G4VSolid *caloS = caloPV->GetLogicalVolume()->GetSolid();
  G4ThreeVector localPoint = transform.TransformPoint(globalPoint);
  G4ThreeVector norm = caloS->SurfaceNormal(localPoint);

  auto inSolid = caloS->Inside(localPoint);
  if (inSolid != kSurface) return false;
  // if (inSolid != kSurface)
  // {
  //   G4cout << globalPoint << "\t" << localPoint << G4endl;
  //   G4ExceptionDescription msg;
  //   msg << "Hit not on surface";
  //   G4Exception("TrackerSD::IsLateralBoundary()", "MyCode0004", FatalException, msg);
  // }

  if (norm.z() == 0) return true;
  else return false;
}

G4bool TrackerSD::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
  G4Track *aTrack = aStep->GetTrack();
  G4StepPoint *preStep = aStep->GetPreStepPoint();
  G4StepPoint *postStep = aStep->GetPostStepPoint();
  
  // get user track information
  auto info = static_cast<TrackInfo *>(aTrack->GetUserInformation());
  if (!info && (aStep->IsLastStepInVolume()) && (postStep->GetStepStatus() == fGeomBoundary)) {
    // add user information to track
    G4int showerID = fpHitsCollection->entries();
    info = new TrackInfo(showerID);
    aTrack->SetUserInformation(info);

    // create and store hit
    TrackerHit *hit = new TrackerHit(showerID, *postStep, aTrack->GetParticleDefinition());
    fpHitsCollection->insert(hit);
    return true;
  } else if (info && (aStep->IsFirstStepInVolume()) && (preStep->GetStepStatus() == fGeomBoundary)) {
    if (IsLateralBoundary(preStep->GetPosition())) {
      G4int showerID = info->GetShowerID();
      auto hit = static_cast<TrackerHit *>(fpHitsCollection->GetHit(showerID));
      auto leakage = aTrack->GetKineticEnergy();
      hit->AddLateralLeakage(leakage);
      
      // kill track and secondaries
      aTrack->SetTrackStatus(fKillTrackAndSecondaries);
      return true;
    } else return false;
  } else return false;
}
