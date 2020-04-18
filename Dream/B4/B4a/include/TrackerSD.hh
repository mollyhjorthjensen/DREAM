#ifndef TrackerSD_h
#define TrackerSD_h 1

/**
 * @file
 * @class TrackerSD
 * @brief Definition of the user class TrackerSD
 */

#include "TrackerHit.hh"

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;

class TrackerSD : public G4VSensitiveDetector
{
public:
  /** Constructor.
   * @param aName the name of the sensitive detector.
   */
  TrackerSD(const G4String &aName);
  virtual ~TrackerSD();

  /** Create and store hit collection. */
  virtual void Initialize(G4HCofThisEvent *aHCE);

  /** Hit triggered at calorimeter surface. New shower ID per hit. */
  virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *);

private:
  TrackerHitsCollection *fpHitsCollection;
  G4bool IsLateralBoundary(const G4ThreeVector globalPoint);
};

#endif
