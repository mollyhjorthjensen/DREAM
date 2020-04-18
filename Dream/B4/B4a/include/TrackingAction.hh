#ifndef TrackingAction_h
#define TrackingAction_h 1

/**
 * \class TrackingAction
 * \brief Definition of the user class TrackingAction
 */

#include "G4UserTrackingAction.hh"

class TrackingAction : public G4UserTrackingAction {
 public:
  TrackingAction();
  virtual ~TrackingAction();

  virtual void PostUserTrackingAction(const G4Track* aTrack);
};

#endif