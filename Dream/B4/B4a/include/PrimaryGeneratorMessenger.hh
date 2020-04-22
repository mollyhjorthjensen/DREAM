#ifndef PrimaryGeneratorMessenger_h
#define PrimaryGeneratorMessenger_h 1

/**
 * \class PrimaryGeneratorMessenger
 * \brief Definition of the user class PrimaryGeneratorMessenger
 */

#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;

class PrimaryGeneratorMessenger : public G4UImessenger
{
public:
  PrimaryGeneratorMessenger(PrimaryGeneratorAction *genaction);
  virtual ~PrimaryGeneratorMessenger();

  virtual void SetNewValue(G4UIcommand *command, G4String newValue);
  virtual G4String GetCurrentValue(G4UIcommand *command);

private:
  PrimaryGeneratorAction *fpPrimaryAction;
  G4UIdirectory *fpDetDir, *fpGenDir;
  G4UIcmdWithAString *fpGenCmd;
};

#endif
