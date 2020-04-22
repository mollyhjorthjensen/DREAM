/**
 * \class PhysicsList
 * \brief Implementation of the user class PhysicsList
 */

#include "PhysicsList.hh"
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "G4FastSimulationPhysics.hh"
#include "G4SystemOfUnits.hh"

// FTFP_BERT + G4OpticalPhysics physics list

PhysicsList::PhysicsList():  G4VModularPhysicsList()
{
  // SetVerboseLevel(1);

  // --------------- reference list -----------------------------
  auto physicsList = new FTFP_BERT;

  for (G4int i = 0;; ++i)
  {
    auto elem = const_cast<G4VPhysicsConstructor *>(physicsList->GetPhysics(i));
    if (elem == NULL) break;
    G4cout << "RegisterPhysics: " << elem->GetPhysicsName() << G4endl;
    RegisterPhysics(elem);
  }

  // --------------- optical physics ----------------------------
  // Add optical physics
  ReplacePhysics(new G4EmStandardPhysics_option4());
  G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();

  // Disable Rayleigh scattering, Mie scattering, and wavelength shifting
  opticalPhysics->Configure(kRayleigh, false);
  opticalPhysics->Configure(kMieHG, false);
  opticalPhysics->Configure(kWLS, false);

  // Further optical process settings
  opticalPhysics->SetMaxNumPhotonsPerStep(1000);
  opticalPhysics->SetScintillationYieldFactor(1.);
  opticalPhysics->SetScintillationExcitationRatio(0.);
  
  RegisterPhysics(opticalPhysics);

  // --------------- fast simulation ----------------------------
  auto fastSimulationPhysics = new G4FastSimulationPhysics();
  fastSimulationPhysics->ActivateFastSimulation("opticalphoton"); 
  RegisterPhysics(fastSimulationPhysics);
}

PhysicsList::~PhysicsList()
{
}

void PhysicsList::SetCuts()
{
  SetDefaultCutValue(0.7 * mm);
  SetCutValue(0.02 * mm, "e-");
  SetCutValue(0.02 * mm, "e+");
//   SetCutsWithDefault();
  DumpCutValuesTable();
}
