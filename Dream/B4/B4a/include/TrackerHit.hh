#ifndef TrackerHit_h
#define TrackerHit_h 1

/**
 * @file
 * @brief Definition of the user class TrackerHit
 */

#include "G4Allocator.hh"
#include "G4ParticleDefinition.hh"
#include "G4THitsCollection.hh"
#include "G4VHit.hh"
#include "G4StepPoint.hh"

class G4Track;
class G4StepPoint;

class TrackerHit : public G4VHit
{
public:
  /** Constructor.
   * @param aTrack the track generating the hit.
   */
  TrackerHit(const G4int aShowerID,
             const G4StepPoint &aStepPoint,
             const G4ParticleDefinition *aParticleType);
  virtual ~TrackerHit();

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void AddLateralLeakage(G4double leakage) { fLateralLeakage += leakage; }
  G4double GetLateralLeakage() const {return fLateralLeakage; } 

  G4int GetShowerID() const { return fShowerID; }
  G4int GetPDGEncoding() const { return fpParticleType->GetPDGEncoding(); }
  G4double GetPDGCharge() const { return fpParticleType->GetPDGCharge(); }
  const G4ThreeVector &GetPosition() const { return fStepPoint.GetPosition(); }
  G4double GetTotalEnergy() const { return fStepPoint.GetTotalEnergy(); }
  G4ThreeVector GetMomentum() const { return fStepPoint.GetMomentum(); }

private:
  const G4int fShowerID;
  const G4StepPoint fStepPoint;
  const G4ParticleDefinition *fpParticleType;
  G4double fLateralLeakage;
};

// define "hit collection" using the template class G4THitsCollection
typedef G4THitsCollection<TrackerHit> TrackerHitsCollection;

// overloading new and delete operators
extern G4ThreadLocal G4Allocator<TrackerHit> *TrackerHitAllocator;

inline void *TrackerHit::operator new(size_t)
{
  if (!TrackerHitAllocator)
  {
    TrackerHitAllocator = new G4Allocator<TrackerHit>;
  }
  return (void *)TrackerHitAllocator->MallocSingle();
}

inline void TrackerHit::operator delete(void *aHit)
{
  TrackerHitAllocator->FreeSingle((TrackerHit *)aHit);
}

#endif
