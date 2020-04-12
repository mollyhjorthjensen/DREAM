#ifndef SiPMsd_h
#define SiPMsd_h 1

/**
 * @file
 * @class SiPMsd
 * @brief Definition of the user class SiPMsd
 */

#include "G4THitsMap.hh"
#include "G4VSensitiveDetector.hh"

#include <array>

static const size_t MAX = 10;

// define "hit collection" using the template class G4THitsMap
typedef G4THitsMap<G4int> SiPMhitsCollection;

class SiPMsd : public G4VSensitiveDetector
{
public:
  /** Constructor.
   * @param SDname the sensitive detector name.
   * @param HCname the hit collection name.
   */
  SiPMsd(G4String SDname, G4String HCname, const G4int NofModules, const G4int NofFibers);
  virtual ~SiPMsd();

  virtual void Initialize(G4HCofThisEvent *aHCE);
  virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *);
  virtual void EndOfEvent(G4HCofThisEvent *);

private:
  std::array<SiPMhitsCollection*, MAX> fHitCollection;
  const G4int fNofModules;
  const G4int fNofFibers;
  G4int GetRowMajorIndex(const G4int module, const G4int fibre);
};

#endif
