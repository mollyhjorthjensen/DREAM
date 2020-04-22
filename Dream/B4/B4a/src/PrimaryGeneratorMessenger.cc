/**
 * \class PrimaryGeneratorMessenger
 * \brief Implementation of the user class PrimaryGeneratorMessenger
 */

#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4UIcommandTree.hh"
#include "G4UIdirectory.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(
    PrimaryGeneratorAction *genaction)
    : G4UImessenger(), fpPrimaryAction(genaction), fpGenDir(0), fpGenCmd(0)
{
  G4UImanager *uiManager(G4UImanager::GetUIpointer());
  if (!uiManager->GetTree()->FindCommandTree("/DR/"))
  {
    fpDetDir = new G4UIdirectory("/DR/");
    fpDetDir->SetGuidance("Dual-readout control.");
  }

  fpGenDir = new G4UIdirectory("/DR/generator/");
  fpGenDir->SetGuidance("Primary generator control.");

  fpGenCmd = new G4UIcmdWithAString("/DR/generator/select", this);
  fpGenCmd->SetGuidance("Select primary generator.");
  fpGenCmd->SetGuidance("Available generators : gps, hepmc");
  fpGenCmd->SetParameterName("generator", true);
  fpGenCmd->SetDefaultValue("gps");
  fpGenCmd->SetCandidates("gps hepmc");
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  if (fpDetDir)
  {
    delete fpDetDir;
  }
  delete fpGenDir;
  delete fpGenCmd;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
  do
  {
    if (command == fpGenCmd)
    {
      fpPrimaryAction->SetHepMCGenerator(newValue == "hepmc");
      G4cout << "current generator type : " << GetCurrentValue(command) << G4endl;
      break;
    }
  } while (false);
}

G4String PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand *command)
{
  G4String cv;
  do
  {
    if (command == fpGenCmd)
    {
      if (fpPrimaryAction->GetHepMCGenerator())
      {
        cv = "hepmc";
      }
      else
      {
        cv = "gps";
      }
      break;
    }
  } while (false);

  return cv;
}
