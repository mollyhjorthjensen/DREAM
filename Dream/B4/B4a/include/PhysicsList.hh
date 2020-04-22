#ifndef PhysicsList_h
#define PhysicsList_h 1

/**
 * \class PhysicsList
 * \brief Definition of the user class PhysicsList
 */

#include "globals.hh"
#include "G4VModularPhysicsList.hh"

class PhysicsList: public G4VModularPhysicsList
{
public:
    PhysicsList();
    virtual ~PhysicsList();
    
    void SetCuts();
};

#endif
