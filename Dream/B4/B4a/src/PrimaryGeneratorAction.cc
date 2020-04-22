/**
 * \class PrimaryGeneratorAction
 * \brief Implementation of the user class PrimaryGeneratorAction
 */

#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4AutoLock.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4RunManager.hh"
#include "G4VPrimaryGenerator.hh"
#include "HepMCG4AsciiReader.hh"
#include "Randomize.hh"

namespace
{
G4Mutex PrimGenDestrMutex = G4MUTEX_INITIALIZER;
G4Mutex PrimGenMutex = G4MUTEX_INITIALIZER;
} // namespace

G4VPrimaryGenerator *PrimaryGeneratorAction::fpHepMC = 0;

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(), fUseHepMC(false)
{
  G4AutoLock lock(&PrimGenDestrMutex);
  if (!fpHepMC)
  {
    fpHepMC = new HepMCG4AsciiReader();
  }
  lock.unlock();

  fpGPS = new G4GeneralParticleSource();

  fpMessenger = new PrimaryGeneratorMessenger(this);
  fUseHepMC = true;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  G4AutoLock lock(&PrimGenDestrMutex);
  if (fpHepMC)
  {
    delete fpHepMC;
    fpHepMC = 0;
  }
  delete fpGPS;
  delete fpMessenger;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
  if (fUseHepMC)
  {
    G4AutoLock lock(&PrimGenMutex);
    fpHepMC->GeneratePrimaryVertex(anEvent);
  }
  else
  {
    fpGPS->GeneratePrimaryVertex(anEvent);
    G4int nOfPrimaries = dynamic_cast<G4GeneralParticleSource *>(fpGPS)->GetNumberOfParticles();
    if (nOfPrimaries != 1)
    {
      G4ExceptionDescription msg;
      msg << "Multiple primary particles";
      G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
                  "MyCode0002", FatalException, msg);
    }
  }
}
