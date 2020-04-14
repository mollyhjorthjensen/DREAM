/**
 * \class TrackingAction
 * \brief Implementation of the user class TrackingAction
 */

#include "TrackingAction.hh"
#include "TrackInfo.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

TrackingAction::TrackingAction() : G4UserTrackingAction() {}

TrackingAction::~TrackingAction() {}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
  auto info = static_cast<TrackInfo*>(aTrack->GetUserInformation());
  if (info) {
    G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
    if (secondaries) {
      size_t nofSecondaris = secondaries->size();
      if (nofSecondaris > 0) {
        for (size_t i = 0; i < nofSecondaris; ++i) {
          (*secondaries)[i]->SetUserInformation(new TrackInfo(info));
        }
      }
    }
  }
}
