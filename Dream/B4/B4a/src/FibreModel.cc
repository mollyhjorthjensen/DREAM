/**
 * \class FibreModel
 * \brief Implementation of the user class FibreModel
 */

#include "FibreModel.hh"

#include "G4OpAbsorption.hh"
#include "G4OpticalPhoton.hh"
#include "G4ProcessManager.hh"
#include "G4TouchableHandle.hh"
#include "G4Tubs.hh"
#include "G4OpBoundaryProcess.hh"

FibreModel::FibreModel(G4String aName, G4Region *anEnvelope)
    : G4VFastSimulationModel(aName, anEnvelope) {}

FibreModel::~FibreModel() {}

G4bool FibreModel::IsApplicable(const G4ParticleDefinition &particleType)
{
  return &particleType == G4OpticalPhoton::OpticalPhotonDefinition();
}

G4bool FibreModel::ModelTrigger(const G4FastTrack &aFastTrack)
{
  G4OpBoundaryProcessStatus theStatus = Undefined;
  G4ProcessManager *pManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();
  if (pManager)
  {
    G4int MAXofPostStepLoops = pManager->GetPostStepProcessVector()->entries();
    G4ProcessVector *postStepDoItVector = pManager->GetPostStepProcessVector(typeDoIt);
    for (G4int i = 0; i < MAXofPostStepLoops; i++)
    {
      G4VProcess *currentProcess = (*postStepDoItVector)[i];
      G4OpBoundaryProcess *opProcess = dynamic_cast<G4OpBoundaryProcess *>(currentProcess);
      if (opProcess)
      {
        theStatus = opProcess->GetStatus();
        break;
      }
    }
  }

  return theStatus == TotalInternalReflection;
}

void FibreModel::DoIt(const G4FastTrack &aFastTrack, G4FastStep &aFastStep)
{
  // kill tracks moving away from SiPM
  if (aFastTrack.GetPrimaryTrackLocalDirection().z() < 0.)
  {
    aFastStep.KillPrimaryTrack();
    return;
  }

  // transport optical photon to the end of the fibre
  G4ThreeVector localPosition = aFastTrack.GetPrimaryTrackLocalPosition();
  G4Tubs *fibreS = static_cast<G4Tubs *>(aFastTrack.GetEnvelopeSolid());
  G4double zHalfLength = fibreS->GetZHalfLength();
  G4double trackLength = zHalfLength - localPosition.z();

  aFastStep.ProposePrimaryTrackPathLength(trackLength);

  double c = G4UniformRand();
  localPosition.setX(c * localPosition.x());
  localPosition.setY(c * localPosition.y());
  localPosition.setZ(zHalfLength);

  aFastStep.ProposePrimaryTrackFinalPosition(localPosition);

  // light attenuation
  const G4Track *aTrack = aFastTrack.GetPrimaryTrack();
  G4ProcessManager *pManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();
  G4double I = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
  if (pManager)
  {
    auto opProcess = dynamic_cast<G4OpAbsorption *>(pManager->GetProcess("OpAbsorption"));
    if (opProcess)
    {
      // unused but required arguments for G4OpAbsorption::GetMeanFreePath()
      G4double previousStepSize = 0.;
      G4ForceCondition *condition = 0;
      G4double lambda = opProcess->GetMeanFreePath(*aTrack, previousStepSize, condition);
      I *= exp(-trackLength / lambda);
    }
  }

  aFastStep.ProposePrimaryTrackFinalKineticEnergy(I);
}
