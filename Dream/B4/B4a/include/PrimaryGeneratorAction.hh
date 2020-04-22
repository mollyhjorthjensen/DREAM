#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

/**
 * \class PrimaryGeneratorAction
 * \brief Definition of the user class PrimaryGeneratorAction
 */

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4VPrimaryGenerator;
class PrimaryGeneratorMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event *anEvent);

  void SetHepMCGenerator(G4bool flag) { fUseHepMC = flag; }
  G4bool GetHepMCGenerator() const { return fUseHepMC; }

private:
  G4VPrimaryGenerator *fpGPS;
  static G4VPrimaryGenerator *fpHepMC;
  PrimaryGeneratorMessenger *fpMessenger;
  G4bool fUseHepMC;
};

#endif
