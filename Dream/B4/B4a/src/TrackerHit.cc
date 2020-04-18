/**
 * @file
 * @brief Implementation of the user class TrackerHit
 */

#include "TrackerHit.hh"

#include "G4Track.hh"

G4ThreadLocal G4Allocator<TrackerHit> *TrackerHitAllocator = 0;

TrackerHit::TrackerHit(const G4int aShowerID,
					   const G4StepPoint &aStepPoint,
					   const G4ParticleDefinition *aParticleType)
	: G4VHit(), fShowerID(aShowerID), fStepPoint(aStepPoint), fLateralLeakage(0.)
{
	fpParticleType = aParticleType;
}

TrackerHit::~TrackerHit() {}
