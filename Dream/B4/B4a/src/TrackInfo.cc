/**
 * \class TrackInfo
 * \brief Implementation of the user class TrackInfo
 */

#include "TrackInfo.hh"

G4ThreadLocal G4Allocator<TrackInfo>* TrackInfoAllocator = 0;

TrackInfo::TrackInfo(const G4int aShowerID)
    : G4VUserTrackInformation(), fShowerID(aShowerID) {}

TrackInfo::TrackInfo(const TrackInfo* anInfo)
    : G4VUserTrackInformation(), fShowerID(anInfo->fShowerID) {}

TrackInfo::~TrackInfo() {}
