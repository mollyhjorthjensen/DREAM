#ifndef TrackInfo_h
#define TrackInfo_h 1

/**
 * \class TrackInfo
 * \brief Definition of the user class TrackInfo
 */

#include "G4Allocator.hh"
#include "G4VUserTrackInformation.hh"
#include "globals.hh"
#include "tls.hh"

class TrackInfo : public G4VUserTrackInformation {
 public:
  TrackInfo(const G4int aShowerID);
  TrackInfo(const TrackInfo* anInfo);
  virtual ~TrackInfo();

  inline void* operator new(size_t);
  inline void operator delete(void* anInfo);

  G4int GetShowerID() const { return fShowerID; }

 private:
  const G4int fShowerID;
};

// overloading new and delete operators
extern G4ThreadLocal G4Allocator<TrackInfo>* TrackInfoAllocator;

inline void* TrackInfo::operator new(size_t) {
  if (!TrackInfoAllocator) {
    TrackInfoAllocator = new G4Allocator<TrackInfo>;
  }
  return (void*)TrackInfoAllocator->MallocSingle();
}

inline void TrackInfo::operator delete(void* anInfo) {
  TrackInfoAllocator->FreeSingle((TrackInfo*)anInfo);
}

#endif
