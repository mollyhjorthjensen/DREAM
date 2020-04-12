#ifndef FibreModel_h
#define FibreModel_h 1

/**
 * \class FibreModel
 * \brief Definition of the user class FibreModel
 */

#include "G4VFastSimulationModel.hh"

class FibreModel : public G4VFastSimulationModel
{
public:
  FibreModel(G4String aName, G4Region *anEnvelope);
  ~FibreModel();

  virtual G4bool IsApplicable(const G4ParticleDefinition &);
  virtual G4bool ModelTrigger(const G4FastTrack &);
  virtual void DoIt(const G4FastTrack &, G4FastStep &);
};

#endif
