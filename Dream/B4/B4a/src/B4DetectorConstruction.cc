//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B4DetectorConstruction.cc 87359 2014-12-01 16:04:27Z gcosmo $
// 
/// \file B4DetectorConstruction.cc
/// \brief Implementation of the B4DetectorConstruction class

#include "B4DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4OpBoundaryProcess.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SDManager.hh"
#include "TrackerSD.hh"
#include "G4SDParticleWithEnergyFilter.hh"
#include "SiPMsd.hh"

#include "G4RegionStore.hh"
#include "FibreModel.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B4DetectorConstruction::fMagFieldMessenger = 0; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4DetectorConstruction::B4DetectorConstruction()
 : G4VUserDetectorConstruction(),
  //  modulePV(0),
   fAbsMateName(""),
   fVoxelsAlongY(-1),
   fCheckOverlaps(true)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4DetectorConstruction::~B4DetectorConstruction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4DetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4DetectorConstruction::DefineMaterials()
{ 
  G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
  G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYSTYRENE");
  G4NistManager::Instance()->FindOrBuildMaterial("G4_PLEXIGLASS");
  G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  // G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  
  // Fluorinated Polymer (C2F2)
  G4Material *fluorinatedPolymer = new G4Material("FluorinatedPolymer", 1.43 * g / cm3, 2, kStateSolid); // Kuraray
  fluorinatedPolymer->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 2);
  fluorinatedPolymer->AddElement(G4NistManager::Instance()->FindOrBuildElement("F"), 2);

  // Cartridge Brass 70/30, C26000
  G4Material *brass = new G4Material("Brass", 0.308 * 27.68 * g / cm3, 2, kStateSolid);
  brass->AddElement(G4NistManager::Instance()->FindOrBuildElement("Cu"), 70 * perCent);
  brass->AddElement(G4NistManager::Instance()->FindOrBuildElement("Zn"), 30 * perCent);

  //G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4DetectorConstruction::DefineVolumes()
{
  // Geometry parameters of world, module, fibers, SiPM

  // Geometry parameters of the module
  G4int Nofmodules = 71; //the actual number of modules is Nofmodules^2, choose 3,5,7,9
  G4int NofFibers = 32; // 32 of each type
  G4int NofScinFibers = NofFibers/2;
  G4int NofCherFibers = NofFibers/2;
  G4int NofFibersrow = NofFibers/4;
  fVoxelsAlongY = Nofmodules * NofFibersrow;
  G4int NofFiberscolumn = NofFibersrow;
  G4double moduleZ = 112.*cm;
  G4double moduleX = 12.*mm; 
  G4double moduleY = moduleX;

  // Geometry parameters of the passive layer1
  G4double layer1X = 15.*mm;
  G4double layer1Y = 1.5*mm;
  G4double layer1Z = moduleZ;

  // Geometry parameters of the passive layer2
  G4double layer2X = 1.5*mm;
  G4double layer2Y = moduleY;
  G4double layer2Z = moduleZ;

  // Geometry parameters of the world, world is a box
  G4double worldX = 200 * moduleX;
  G4double worldY = 200 * moduleY;
  G4double worldZ = 60 * moduleZ;

  // Distance from origo to calorimeter surface
  G4double CalRin = 2.5 * m;

  // Geometry parameters of the fiber
  G4double fiberradius = 0.5*mm;
  G4double fiberZ = moduleZ;

  // Geometry parameters of the core
  G4double coreradius = 0.48*mm;
  G4double coreZ = moduleZ;

  // Geometry parameters of the cladding
  G4double claddingradiusmin = 0.48*mm;
  G4double claddingradiusmax = 0.50*mm;
  G4double claddingZ = moduleZ;

  // Geometry parameters of the SiPM
  G4double SiPMX = 1.*mm;
  G4double SiPMY = SiPMX;
  G4double SiPMZ = 0.36*mm;

  // Geometry parameters of the SiPM, active silicon layer
  G4double SiX = 1.*mm;
  G4double SiY = SiX;
  G4double SiZ = 0.05*mm;

  // Geometry parameters of the module equipped with SiPM
  // I build it so I can replicate the entire module + SiPM 
  G4double moduleequippedZ = moduleZ + SiPMZ;
  G4double moduleequippedX = moduleX; 
  G4double moduleequippedY = moduleY;

  // Get materials for vacuum, absorber, scintillating and cherenkov fibers, SiPM
  G4Material* defaultMaterial = G4Material::GetMaterial("G4_Galactic"); // G4_AIR or G4_Galactic 
  G4Material* absorberMaterial = G4Material::GetMaterial("Brass");      // Brass or G4_Pb
  fAbsMateName = absorberMaterial->GetName();
  G4Material* ScinMaterial = G4Material::GetMaterial("G4_POLYSTYRENE");
  G4Material* CherMaterial = G4Material::GetMaterial("G4_PLEXIGLASS");
  G4Material* GlassMaterial = G4Material::GetMaterial("G4_SILICON_DIOXIDE");
  G4Material* SiMaterial = G4Material::GetMaterial("G4_Si");
  G4Material* CladCherMaterial = G4Material::GetMaterial("FluorinatedPolymer");

  // I need to specify the optical properties of the scintillating fiber material,
  // optical proprieties are different from scintillating proprieties and 
  // scintillating proprieties will be defined later.
  // We don't have to add WLS proprieties to scintillating fibers

  const G4int ENTRIES = 32;
  
  G4double photonEnergy[ENTRIES] =                    // Use Energy(eV)=1.24/waevelenght(um)
            { 2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV, // 2.034eV is 610nm RED  
              2.177*eV, 2.216*eV, 2.256*eV, 2.298*eV,     
              2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV,
              2.532*eV, 2.585*eV, 2.640*eV, 2.697*eV,
              2.757*eV, 2.820*eV, 2.885*eV, 2.954*eV, // 2.75eV is 450nm BLUE (peak of scintillating fibers)
              3.026*eV, 3.102*eV, 3.181*eV, 3.265*eV, // 3.09eV is 400nm VIOLET (end of visible)
              3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
              3.760*eV, 3.877*eV, 4.002*eV, 4.136*eV }; //4.1eV is 300nm UV (cherenkov peak is 310-350nm)

  G4double rindexScin[ENTRIES] =  // Kuraray
            { 1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59,
              1.59, 1.59, 1.59, 1.59 };

  G4double absorptionScin[ENTRIES] =  // Kuraray
             { 467*cm, 467*cm, 467*cm, 467*cm,
               467*cm, 467*cm, 467*cm, 467*cm,
               467*cm, 467*cm, 467*cm, 467*cm,
               467*cm, 467*cm, 467*cm, 467*cm,
               467*cm, 467*cm, 467*cm, 467*cm,
               467*cm, 467*cm, 467*cm, 467*cm,
               467*cm, 467*cm, 467*cm, 467*cm,
               467*cm, 467*cm, 467*cm, 467*cm };
         
  G4MaterialPropertiesTable *MPTScin = new G4MaterialPropertiesTable();
  MPTScin -> AddProperty("RINDEX", photonEnergy, rindexScin, ENTRIES)->SetSpline(true);
  MPTScin -> AddProperty("ABSLENGTH", photonEnergy, absorptionScin, ENTRIES)->SetSpline(true);

  // I need to specify the optical proprieties of the cherenkov fiber material
  // there are no scintillating proprieties for PMMA (clear fibres)
  // we don't have to add WLS proprieties

  G4double rindexCher[ENTRIES] =    // SK-40 product information
            { 1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49,
              1.49, 1.49, 1.49, 1.49 };

 G4double absorptionCher[ENTRIES] =   // missing reference
            { 890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm,
              890*cm, 890*cm, 890*cm, 890*cm };

  G4MaterialPropertiesTable *MPTCher = new G4MaterialPropertiesTable();
  MPTCher -> AddProperty("RINDEX", photonEnergy, rindexCher, ENTRIES)->SetSpline(true);
  MPTCher -> AddProperty("ABSLENGTH", photonEnergy, absorptionCher, ENTRIES)->SetSpline(true);
  CherMaterial -> SetMaterialPropertiesTable(MPTCher);

  // I need to specify the optical proprieties of the cherenkov cladding material

  G4double rindexCherclad[ENTRIES] =  // Kuraray
            { 1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42,
              1.42, 1.42, 1.42, 1.42 };

  G4MaterialPropertiesTable *MPTCherclad = new G4MaterialPropertiesTable();
  MPTCherclad -> AddProperty("RINDEX", photonEnergy, rindexCherclad, ENTRIES)->SetSpline(true);
  CladCherMaterial -> SetMaterialPropertiesTable(MPTCherclad);

  // We don't set any optical proriety for the absorber, if you want uncomment it
  // I need to specify the optical proprieties of the absorber material

  /*G4double rindexabsorber[ENTRIES] =
            { 1.1, 1.1, 1.1, 1.1,
              1.1, 1.1, 1.1, 1.1,
              1.1, 1.1, 1.1, 1.1,
              1.1, 1.1, 1.1, 1.1,
              1.1, 1.1, 1.1, 1.1,
              1.1, 1.1, 1.1, 1.1,
              1.1, 1.1, 1.1, 1.1,
              1.1, 1.1, 1.1, 1.1 };

  G4double absorptionabsorber[ENTRIES] =   // I set 1nm absorption lenght so light doesn't 
            { 1.*nm, 1.*nm, 1.*nm, 1.*nm,  // propagate inside copper
              1.*nm, 1.*nm, 1.*nm, 1.*nm,
              1.*nm, 1.*nm, 1.*nm, 1.*nm,
              1.*nm, 1.*nm, 1.*nm, 1.*nm,
              1.*nm, 1.*nm, 1.*nm, 1.*nm,
              1.*nm, 1.*nm, 1.*nm, 1.*nm,
              1.*nm, 1.*nm, 1.*nm, 1.*nm,
              1.*nm, 1.*nm, 1.*nm, 1.*nm };
             
  G4MaterialPropertiesTable *MPTabsorber = new G4MaterialPropertiesTable();
  MPTabsorber -> AddProperty("RINDEX", photonEnergy, rindexabsorber, ENTRIES)->SetSpline(true);
  MPTabsorber -> AddProperty("ABSLENGTH", photonEnergy, absorptionabsorber, ENTRIES)->SetSpline(true);
  absorberMaterial -> SetMaterialPropertiesTable(MPTabsorber);*/

  // I need to specify the optical proprieties of the air material
  // turn on if you use Air

  /*G4double rindexair[ENTRIES] =
              { 1.0003, 1.0003, 1.0003, 1.0003,
                1.0003, 1.0003, 1.0003, 1.0003,
                1.0003, 1.0003, 1.0003, 1.0003,
                1.0003, 1.0003, 1.0003, 1.0003,
                1.0003, 1.0003, 1.0003, 1.0003,
                1.0003, 1.0003, 1.0003, 1.0003,
                1.0003, 1.0003, 1.0003, 1.0003,
                1.0003, 1.0003, 1.0003, 1.0003 };

  G4MaterialPropertiesTable *MPTair = new G4MaterialPropertiesTable();
  MPTair -> AddProperty("RINDEX", photonEnergy, rindexair, ENTRIES)->SetSpline(true);
  defaultMaterial -> SetMaterialPropertiesTable(MPTair);*/

  // I need to specify the optical proprieties of the vacuum material
  // turn on if you use Galactic

  /*G4double rindexvacuum[ENTRIES] = // By definition
            { 1.0, 1.0, 1.0, 1.0,
              1.0, 1.0, 1.0, 1.0,
              1.0, 1.0, 1.0, 1.0,
              1.0, 1.0, 1.0, 1.0,
              1.0, 1.0, 1.0, 1.0,
              1.0, 1.0, 1.0, 1.0,
              1.0, 1.0, 1.0, 1.0,
              1.0, 1.0, 1.0, 1.0 };

  G4MaterialPropertiesTable *MPTvacuum = new G4MaterialPropertiesTable();
  MPTvacuum -> AddProperty("RINDEX", photonEnergy, rindexvacuum, ENTRIES)->SetSpline(true);
  defaultMaterial -> SetMaterialPropertiesTable(MPTvacuum);*/

  // I need to specify the optical proprieties of the glass material

  G4double rindexglass[ENTRIES] =   // Hamamatsu
            { 1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51,
              1.51, 1.51, 1.51, 1.51 };

  G4MaterialPropertiesTable *MPTglass = new G4MaterialPropertiesTable();
  MPTglass -> AddProperty("RINDEX", photonEnergy, rindexglass, ENTRIES)->SetSpline(true);
  GlassMaterial -> SetMaterialPropertiesTable(MPTglass);

  // I need to specify the optical proprieties of the Si material

  G4double rindexSi[ENTRIES] =    // missing reference
            { 3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42,
              3.42, 3.42, 3.42, 3.42 };

  G4double absorptionSi[ENTRIES] =    // missing reference
            { 0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm,
              0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm };

  G4MaterialPropertiesTable *MPTSi = new G4MaterialPropertiesTable();
  MPTSi -> AddProperty("RINDEX", photonEnergy, rindexSi, ENTRIES)->SetSpline(true);
  MPTSi -> AddProperty("ABSLENGHT", photonEnergy, absorptionSi, ENTRIES)->SetSpline(true);
  SiMaterial -> SetMaterialPropertiesTable(MPTSi); 
  
  // I need to specify the SCINTILLATING proprieties of the scintillating fiber material
  // I specify also the Birk Constant of the polystyrene

  G4double Scin_FAST[ENTRIES] = // Emission spectrum for the fast component 
            { 0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.1,
              0.2, 0.4, 0.6, 0.8,
              1., 0.8, 0.6, 0.1,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0. };

  G4double Scin_SLOW[ENTRIES] = // Emission spectrum for the slow component
            { 0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0.,
              0., 0., 0., 0. };

  // Set Briks Constant for scintillator
  ScinMaterial->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

  MPTScin -> AddProperty("FASTCOMPONENT", photonEnergy, Scin_FAST, ENTRIES);
  MPTScin -> AddProperty("SLOWCOMPONENT", photonEnergy, Scin_SLOW, ENTRIES);
  MPTScin -> AddConstProperty("SCINTILLATIONYIELD", 10000./MeV); // Typical is 10000./MeV (this is what makes full simulations long as hell)
  MPTScin -> AddConstProperty("RESOLUTIONSCALE", 1.0); // Broad the fluctuation of photons produced
  MPTScin -> AddConstProperty("FASTTIMECONSTANT", 2.8*ns);  // Kuraray
  MPTScin -> AddConstProperty("SLOWTIMECONSTANT", 10.*ns);
  MPTScin -> AddConstProperty("YIELDRATIO", 1.0); // I don't want a slow component, if you want it must change
  ScinMaterial -> SetMaterialPropertiesTable(MPTScin);
  
  if ( ! defaultMaterial || ! absorberMaterial || ! ScinMaterial || ! CherMaterial || ! GlassMaterial || ! CladCherMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("B4DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }
   
  // Building the calorimeter

  // Here I build the world

  G4VSolid* worldS 
    = new G4Box("World",                        // its name
                 worldX/2, worldY/2, worldZ/2); // its size
                         
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material (Galactic or Air)
                 "World");         // its name
  
  // I set the world as invisible
  worldLV->SetVisAttributes(G4VisAttributes::Invisible);
                                   
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

   // Here I build the module equipped with SiPM

   G4VSolid* moduleequippedS
    = new G4Box("moduleequipped",                                          // its name
                 moduleequippedX/2, moduleequippedY/2, moduleequippedZ/2); // its size
                         
  G4LogicalVolume* moduleequippedLV
    = new G4LogicalVolume(
                 moduleequippedS,           // its solid
                 defaultMaterial,           // its material
                 "moduleequipped");         // its name
  
  moduleequippedLV->SetVisAttributes(G4VisAttributes::Invisible);

  // Here I build the layer1 and layer2 surrounding the active part
  // I need only layer2

  G4VSolid* layer1 = new G4Box("layer1", layer1X/2, layer1Y/2, layer1Z/2); //its name and size

  G4LogicalVolume* layer1LV
   = new G4LogicalVolume(
                layer1,             //its solid
                absorberMaterial,   // its material
                "layer1");          // its name

  G4VSolid* layer2 = new G4Box("layer2", layer2X/2, layer2Y/2, layer2Z/2); //its name and size

  G4LogicalVolume* layer2LV
   = new G4LogicalVolume(
                layer2,             //its solid
                absorberMaterial,   // its material
                "layer2");          // its name

  // Here I place the single module equipped with SiPM in the center of the world
  // I rotate it a bit in order to avoid problems due to the perfect alinment 
  // between the beam and the module. (1.;1.51)*deg
  /*
  G4RotationMatrix rotm  = G4RotationMatrix();
  rotm.rotateY(2.0*deg); // Set the rotation angles
  rotm.rotateX(2.0*deg); // 0.*deg no rotation!     
  G4ThreeVector position;
  position.setX(0.);
  position.setY(0.);
  position.setZ(0.);
  G4Transform3D transform = G4Transform3D(rotm,position);

  G4VPhysicalVolume* moduleequippedPV = new G4PVPlacement(
                                                transform,        // its position and rotation
                                                moduleequippedLV, // its logical volume                         
                                                "moduleequipped", // its name
                                                worldLV,          // its mother  volume
                                                false,            // no boolean operation
                                                0,                // copy number
                                                fCheckOverlaps);  // checking overlaps 
  
  */
  // Here I build the calorimeter itself. As calorimeter I mean the matrix of
  // modules equipped. Uncomment it only if you want more than one module.
  
  /* G4VSolid* CalorimeterS 
    = new G4Box("CalorimeterS",                              // its name
                 (12.*mm/2)*Nofmodules/2, (12.*mm/2)*Nofmodules/2, moduleequippedZ/2); // its size*/
  auto CalorimeterS 
    = new G4Box("CalorimeterS",                              // its name
                 moduleequippedX*Nofmodules/2, moduleequippedY*Nofmodules/2, moduleequippedZ/2); // its size                     
  G4LogicalVolume* CalorimeterLV
    = new G4LogicalVolume(
                 CalorimeterS,           // its solid
                 defaultMaterial,        // its material 
                 "CalorimeterLV");       // its name

  auto blue = G4Colour(114. / 255., 158. / 255., 206. / 255.);
  auto orange = G4Color(255. / 255., 158. / 255., 74. / 255.);
  auto green = G4Color(103. / 255., 191. / 255., 92. / 255.);
  auto red = G4Color(237. / 255., 102. / 255., 93. / 255.);
  auto purple = G4Color(173. / 255., 139. / 255., 201. / 255.);


  // Here I build the air tubre for layer1 and layer2 and place them

  G4Tubs* airtube = new G4Tubs("airtube", 0., fiberradius+0.1*mm, fiberZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_airtube = new G4LogicalVolume(airtube,          //its solid
                                                       defaultMaterial,  //its material
                                                       "airtube");       //its name

  // I set the visualization attributes of the air tube
  G4VisAttributes* airtubeVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,1.0)); //cyan
  airtubeVisAtt->SetVisibility(true);
  airtubeVisAtt->SetForceWireframe(true);
  airtubeVisAtt->SetForceSolid(true);
  logic_airtube->SetVisAttributes(airtubeVisAtt); //end of visualization attributes
  
  // Here I place the airtube inside layer1 and layer2
  // I don't need layer1
  
  G4double airtubex;
  G4ThreeVector vec_airtube;
  G4VPhysicalVolume* physi_airtube[10];
  for(int row=0; row<10; row++){
        airtubex = (-7.5*mm + 0.75*mm + (1.5*row)*mm);
           
        vec_airtube.setX(airtubex);
        vec_airtube.setY(0.0*mm);
        vec_airtube.setZ(0.);

        physi_airtube[row] = new G4PVPlacement(0,     
                                               vec_airtube,              
                                               logic_airtube,     
                                               "airtube",                        
                                                layer1LV,                      
                                                false,                          
                                                0); 
      };

  G4double airtubey;
  G4ThreeVector vec_airtube2;
  G4VPhysicalVolume* physi_airtube2[8];
  for(int column=0; column<8; column++){
        airtubey = (-6.0*mm + 0.75*mm + (1.5*column)*mm);
           
        vec_airtube2.setX(0);
        vec_airtube2.setY(airtubey);
        vec_airtube2.setZ(0.);

        physi_airtube2[column] = new G4PVPlacement(0,     
                                               vec_airtube2,              
                                               logic_airtube,     
                                               "airtube",                        
                                                layer2LV,                      
                                                false,                          
                                                0); 
      }; 

  // Here I place the modules equipped inside the calorimeter
  // There is no rotation of the modules, I will later rotate the entire calorimeter

  G4int copynumbermodule = 0;
  G4double m_x, m_y;
  G4ThreeVector vec_m;
  G4VPhysicalVolume* physi_moduleequipped[Nofmodules][Nofmodules];
  for(int row=0; row<Nofmodules; row++){
     for(int column=0; column<Nofmodules; column++){
        m_x = -(((Nofmodules-1)/2)*moduleX - moduleX*row);
        m_y = -(((Nofmodules-1)/2)*moduleY - moduleY*column);
           
        vec_m.setX(m_x);
        vec_m.setY(m_y);
        vec_m.setZ(0.);
        
        copynumbermodule = (1+row)+(column*Nofmodules);

        physi_moduleequipped[row][column] = new G4PVPlacement(0,
                                                        vec_m,              
                                                        moduleequippedLV,     
                                                        "moduleequipped",                        
                                                        CalorimeterLV,                      
                                                        false,                          
                                                        copynumbermodule); 
      };
   }; 
 
  // Here I place layer1 in the calorimeter up and down

  G4ThreeVector up_layer1;
  up_layer1.setX(0.);
  up_layer1.setY(moduleY/2 + layer1Y/2);
  up_layer1.setZ(-0.18);

  G4ThreeVector down_layer1;
  down_layer1.setX(0.);
  down_layer1.setY(-(moduleY/2+layer1Y/2));
  down_layer1.setZ(-0.18);

  /*G4VPhysicalVolume* phys_uplayer1 = new G4PVPlacement(0,
                                                     up_layer1,
                                                     layer1LV,
                                                     "layer",
                                                      CalorimeterLV,
                                                      false,
                                                      0);

  G4VPhysicalVolume* phys_downlayer1 = new G4PVPlacement(0, 
                                                         down_layer1,
                                                         layer1LV,
                                                         "layer",
                                                         CalorimeterLV,
                                                         false,
                                                         0);*/

  // I set the visualization attributes of layer1
  G4VisAttributes* layer1VisAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0)); //magenta
  layer1VisAtt->SetVisibility(true);
  layer1VisAtt->SetForceWireframe(true);
  layer1VisAtt->SetForceSolid(true);
  layer1LV->SetVisAttributes(layer1VisAtt); //end of visualization attributes*/

  //Here I place layer2 in the calorimeter right and left

  G4ThreeVector left_layer2;
  left_layer2.setX(-moduleX/2 - layer2X/2);
  left_layer2.setY(0.);
  left_layer2.setZ(-0.18);

  G4ThreeVector right_layer2;
  right_layer2.setX(moduleX/2 + layer2X/2);
  right_layer2.setY(0.);
  right_layer2.setZ(-0.18);

  /*G4VPhysicalVolume* phys_leftlayer2 = new G4PVPlacement(0,
                                                     left_layer2,
                                                     layer2LV,
                                                     "leftlayer",
                                                      CalorimeterLV,
                                                      false,
                                                      0);

  G4VPhysicalVolume* phys_rightlayer2 = new G4PVPlacement(0, 
                                                         right_layer2,
                                                         layer2LV,
                                                         "rightlayer",
                                                         CalorimeterLV,
                                                         false,
                                                         0);*/
  
  // I set the visualization attributes of layer2
  G4VisAttributes* layer2VisAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0)); //magenta
  layer2VisAtt->SetVisibility(true);
  layer2VisAtt->SetForceWireframe(true);
  layer2VisAtt->SetForceSolid(true);
  layer2LV->SetVisAttributes(layer2VisAtt); //end of visualization attributes */

  // Here I place and rotate the entire calorimeter

  G4RotationMatrix rotm  = G4RotationMatrix();
  rotm.rotateY(1.25*deg);  // Set the rotation angles //0.75
  rotm.rotateX(1.0*deg);  //0.75
  G4ThreeVector position;
  position.setX(0.);
  position.setY(0.);
  position.setZ(CalRin + CalorimeterS->GetZHalfLength());
  G4Transform3D transform = G4Transform3D(rotm,position); 

  G4VPhysicalVolume* CalorimeterPV = new G4PVPlacement(
                                                transform,        // its position and rotation
                                                CalorimeterLV,    // its logical volume                         
                                                "Calorimeter",    // its name
                                                worldLV,          // its mother  volume
                                                false,            // no boolean operation
                                                0,                // copy number
                                                fCheckOverlaps);  // checking overlaps 

  // Here I build the module: to do that I build the rectangular absorber
  // I will later put fibers into it  

  G4VSolid* moduleS
    = new G4Box("module",                          // its name
                 moduleX/2, moduleY/2, moduleZ/2); // its size
                         
  G4LogicalVolume* moduleLV
    = new G4LogicalVolume(
                 moduleS,           // its solid
                 absorberMaterial,  // its material
                 "module");         // its name
  
  G4VisAttributes* moduleVisAtt = new G4VisAttributes(blue);
  moduleVisAtt->SetVisibility(true);
  moduleLV->SetVisAttributes(moduleVisAtt);
  
  CalorimeterLV->SetVisAttributes(G4VisAttributes::Invisible);
  //CalorimeterLV->SetVisAttributes(moduleVisAtt);

  G4ThreeVector pos_module;
  pos_module.setX(0.);
  pos_module.setY(0.);
  pos_module.setZ(-0.18);
                              
  G4VPhysicalVolume* modulePV = new G4PVPlacement(
                                                0,                // no rotation
                                                pos_module,       // at (0,0,-0.18)
                                                moduleLV,         // its logical volume                         
                                                "module",         // its name
                                                moduleequippedLV, // its mother  volume
                                                false,            // no boolean operation
                                                0,                // copy number
                                                fCheckOverlaps);  // checking overlaps 


  // Here I define the Optical Surface PROPRIETIES between the glass and the Si of the SiPM

  G4OpticalSurface* OpSurfaceGlassSi = new G4OpticalSurface("OpSurfaceGlassSi");
  
  OpSurfaceGlassSi -> SetType(dielectric_metal);
  OpSurfaceGlassSi -> SetModel(glisur);
  OpSurfaceGlassSi -> SetFinish(polished);

  G4double efficiencyOpSurfaceGlassSi[ENTRIES] =     // detection efficiency from Hamamatsu
                                    {0.1762, 0.1823, 0.1897, 0.1972,
                                     0.2037, 0.2102, 0.2151, 0.2202,
                                     0.2241, 0.2282, 0.2351, 0.2421,
                                     0.2461, 0.2499, 0.2485, 0.2470, 
                                     0.2455, 0.2439, 0.2395, 0.2348,
                                     0.2275, 0.2198, 0.2147, 0.2007, 
                                     0.1889, 0.1849, 0.1777, 0.1668, 
                                     0.1586, 0.1426, 0.1234, 0.0933};
                                      

   G4double reflectivityOpSurfaceGlassSi[ENTRIES] =  // 0% reflection
                                    { 0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0.,
                                      0., 0., 0., 0. };

  G4MaterialPropertiesTable* MPTOpSurfaceGlassSi = new G4MaterialPropertiesTable();
  MPTOpSurfaceGlassSi -> AddProperty("EFFICIENCY", photonEnergy, efficiencyOpSurfaceGlassSi, ENTRIES)->SetSpline(true);
  MPTOpSurfaceGlassSi -> AddProperty("REFLECTIVITY", photonEnergy, reflectivityOpSurfaceGlassSi, ENTRIES)->SetSpline(true);
  OpSurfaceGlassSi -> SetMaterialPropertiesTable(MPTOpSurfaceGlassSi);

  // Here I build the SiPM

  G4VSolid* S_SiPMS = new G4Box("S_SiPM", SiPMX/2, SiPMY/2, SiPMZ/2);
  G4VSolid* C_SiPMS = new G4Box("C_SiPM", SiPMX/2, SiPMY/2, SiPMZ/2);
                         
  G4LogicalVolume* S_SiPMLV = new G4LogicalVolume(S_SiPMS, GlassMaterial, "S_SiPM");
  G4LogicalVolume* C_SiPMLV = new G4LogicalVolume(C_SiPMS, GlassMaterial, "C_SiPM");

  S_SiPMLV->SetVisAttributes(G4VisAttributes::Invisible);
  C_SiPMLV->SetVisAttributes(G4VisAttributes::Invisible);

  // Here I build the Si of the SiPM

  G4VSolid* S_SiS = new G4Box("S_Si", SiX/2, SiY/2, SiZ/2);
  G4VSolid* C_SiS = new G4Box("C_Si", SiX/2, SiY/2, SiZ/2);
  
  G4LogicalVolume* S_SiLV = new G4LogicalVolume(S_SiS, SiMaterial, "S_Si");
  G4LogicalVolume* C_SiLV = new G4LogicalVolume(C_SiS, SiMaterial, "C_Si");

  // I put the Si inside the SiPM, I will put the SiPMs next to fibers later

  G4ThreeVector vec_Si = G4ThreeVector(0., 0., SiPMZ/2-SiZ/2); // Si at the end of SiPM
  
  G4VPhysicalVolume* S_SiPV = new G4PVPlacement(0, vec_Si, S_SiLV, "S_Si", S_SiPMLV, false, 0, fCheckOverlaps);   // checking overlaps 
  G4VPhysicalVolume* C_SiPV = new G4PVPlacement(0, vec_Si, C_SiLV, "C_Si", C_SiPMLV, false, 0, fCheckOverlaps);   // checking overlaps 
 
  // I set the visualization attributes of the Si of the SiPM
  G4VisAttributes* SiVisAtt = new G4VisAttributes(G4Colour(0.0,0.8,0.0)); //green
  SiVisAtt->SetVisibility(false);
  SiVisAtt->SetForceWireframe(true);
  SiVisAtt->SetForceSolid(true);
  S_SiLV->SetVisAttributes(SiVisAtt);
  C_SiLV->SetVisAttributes(SiVisAtt); //end of visualization attributes

  // Here I place the Logical Skin Surface around the silicon of the SiPM
  G4LogicalSkinSurface* S_OpsurfaceSi = new G4LogicalSkinSurface("S_OpsurfaceSi", S_SiLV, OpSurfaceGlassSi);
  G4LogicalSkinSurface* C_OpsurfaceSi = new G4LogicalSkinSurface("C_OpsurfaceSi", C_SiLV, OpSurfaceGlassSi);

  // Here I define the Optical Surface PROPRIETIES between the scintillating fibers and the default material
  // air or vacuum
  // I'm trying to define an optical surface completly blacked because we absorb the light at one end of fibers

  G4OpticalSurface* OpSurfacedefault = new G4OpticalSurface("OpSurfacedefault");
  
  OpSurfacedefault -> SetType(dielectric_dielectric);
  OpSurfacedefault -> SetModel(unified);
  OpSurfacedefault -> SetFinish(polishedbackpainted); // Painted from inside the fibers, light is absorbed
 
  // Here I build the Scintillating fiber with its core and cladding
  // I will put the fibers later inside the module

  G4Tubs* S_fiber = new G4Tubs("S_fiber", 0., fiberradius+0.1*mm, fiberZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_S_fiber = new G4LogicalVolume(S_fiber,          //its solid
                                                       defaultMaterial,  //its material
                                                       "S_fiber");       //its name
  
  logic_S_fiber->SetVisAttributes(G4VisAttributes::Invisible);

  G4Tubs* Core_S_fiber = new G4Tubs("Core_S_fiber", 0., coreradius, coreZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_Core_S_fiber = new G4LogicalVolume(Core_S_fiber,   //its solid
                                                            ScinMaterial,   //its material
                                                            "Core_S_fiber");//its name

  // I set the visualization attributes of the scintillating core fibers
  G4VisAttributes* ScincoreVisAtt = new G4VisAttributes(red);
  ScincoreVisAtt->SetVisibility(true);
  ScincoreVisAtt->SetForceWireframe(true);
  ScincoreVisAtt->SetForceSolid(true);
  logic_Core_S_fiber->SetVisAttributes(ScincoreVisAtt); //end of visualization attributes

  G4ThreeVector vec_Core_S;
  vec_Core_S.setX(0.);
  vec_Core_S.setY(0.);
  vec_Core_S.setZ(0.); 
                             
  G4VPhysicalVolume* Core_S_PV = new G4PVPlacement(
                                             0,                        // no rotation
                                             vec_Core_S,               // its position
                                             logic_Core_S_fiber,       // its logical volume                         
                                             "Core_S_fiber",           // its name
                                             logic_S_fiber,            // its mother  volume
                                             false,                    // no boolean operation
                                             0,                        // copy number
                                             fCheckOverlaps);          // checking overlaps
 
  // Here I place the optical surface "OpSurfacedefault" between the scintillatinf core and the default material
  G4LogicalBorderSurface* logic_OpSurface_SCoredefault;
  logic_OpSurface_SCoredefault = new G4LogicalBorderSurface("logic_OpSurface_SCoredefault", Core_S_PV, worldPV, OpSurfacedefault);

  G4Tubs* Clad_S_fiber = new G4Tubs("Clad_S_fiber", claddingradiusmin, claddingradiusmax, claddingZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_Clad_S_fiber = new G4LogicalVolume(Clad_S_fiber,   //its solid
                                                            CherMaterial,   //its material
                                                            "Clad_S_fiber");//its name

  // I set the visualization attributes of the scintillating clad fibers
  G4VisAttributes* ScincladVisAtt = new G4VisAttributes(red);
  ScincladVisAtt->SetVisibility(true);
  ScincladVisAtt->SetForceWireframe(true);
  ScincladVisAtt->SetForceSolid(true);
  logic_Clad_S_fiber->SetVisAttributes(ScincladVisAtt); //end of visualization attributes

  G4ThreeVector vec_Clad_S;
  vec_Clad_S.setX(0.);
  vec_Clad_S.setY(0.);
  vec_Clad_S.setZ(0.); 
                              
  G4VPhysicalVolume* Clad_S_PV = new G4PVPlacement(
                                              0,                        // no rotation
                                              vec_Clad_S,               // its position
                                              logic_Clad_S_fiber,       // its logical volume                         
                                              "Clad_S_fiber",           // its name
                                              logic_S_fiber,            // its mother  volume
                                              false,                    // no boolean operation
                                              0,                        // copy number
                                              fCheckOverlaps);          // checking overlaps

  // Here I place the optical surface "OpSurfacedefault" between the scintillating clad and the default material
  G4LogicalBorderSurface* logic_OpSurface_SCladdefault;
  logic_OpSurface_SCladdefault = new G4LogicalBorderSurface("logic_OpSurface_SCladdefault", Clad_S_PV, worldPV, OpSurfacedefault);
  
  // Here I build the Cherenkov fiber with its cladding
  // I will put the fibers later inside the module
  
  G4Tubs* C_fiber = new G4Tubs("C_fiber", 0., fiberradius+0.1*mm, fiberZ/2, 0., 2.*pi);
  
  G4LogicalVolume* logic_C_fiber = new G4LogicalVolume(C_fiber,       //it solid
                                                       CherMaterial,  //its material
                                                       "C_fiber");     //its name

  logic_C_fiber->SetVisAttributes(G4VisAttributes::Invisible);

  G4Tubs* Core_C_fiber = new G4Tubs("Core_C_fiber", 0., coreradius, coreZ/2, 0., 2.*pi);

  G4LogicalVolume* logic_Core_C_fiber = new G4LogicalVolume(Core_C_fiber,   //its solid
                                                          CherMaterial,   //its material
                                                          "Core_C_fiber");//its name

  // I set the visualization attributes of the cherenkov core fibers
  G4VisAttributes* ChercoreVisAtt = new G4VisAttributes(green);
  ChercoreVisAtt->SetVisibility(true);
  ChercoreVisAtt->SetForceWireframe(true);
  ChercoreVisAtt->SetForceSolid(true);
  logic_Core_C_fiber->SetVisAttributes(ChercoreVisAtt); //end of visualization attributes

  G4ThreeVector vec_Core_C;
  vec_Core_C.setX(0.);
  vec_Core_C.setY(0.);
  vec_Core_C.setZ(0.); 
                              
  G4VPhysicalVolume* Core_C_PV = new G4PVPlacement(
                                              0,                        // no rotation
                                              vec_Core_C,               // its position
                                              logic_Core_C_fiber,       // its logical volume                         
                                              "Core_C_fiber",           // its name
                                              logic_C_fiber,            // its mother  volume
                                              false,                    // no boolean operation
                                              0,                        // copy number
                                              fCheckOverlaps);          // checking overlaps
 
  // Here I place the optical surface "OpSurfacedefault" between the cherenkov core and the default material
  G4LogicalBorderSurface* logic_OpSurface_CCoredefault;
  logic_OpSurface_CCoredefault = new G4LogicalBorderSurface("logic_OpSurface_CCoredefault", Core_C_PV, worldPV, OpSurfacedefault);
 
  G4Tubs* Clad_C_fiber = new G4Tubs("Clad_C_fiber", claddingradiusmin, claddingradiusmax, claddingZ/2, 0., 2.*pi);
 
  G4LogicalVolume* logic_Clad_C_fiber = new G4LogicalVolume(Clad_C_fiber,   //its solid
                                                            CladCherMaterial,   //its material
                                                            "Clad_C_fiber");//its name
 
  // I set the visualization attributes of the cherenkov clad fibers
  G4VisAttributes* ChercladVisAtt = new G4VisAttributes(green);
  ChercladVisAtt->SetVisibility(true);
  ChercladVisAtt->SetForceWireframe(true);
  ChercladVisAtt->SetForceSolid(true);
  logic_Clad_C_fiber->SetVisAttributes(ChercladVisAtt); //end of visualization attributes

  G4ThreeVector vec_Clad_C;
  vec_Clad_C.setX(0.);
  vec_Clad_C.setY(0.);
  vec_Clad_C.setZ(0.); 
                              
  G4VPhysicalVolume* Clad_C_PV = new G4PVPlacement(
                                              0,                        // no rotation
                                              vec_Clad_C,               // its position
                                              logic_Clad_C_fiber,       // its logical volume                         
                                              "Clad_C_fiber",           // its name
                                              logic_C_fiber,            // its mother  volume
                                              false,                    // no boolean operation
                                              0,                        // copy number
                                              fCheckOverlaps);          // checking overlaps

  // Here I place the optical surface "OpSurfacedefault" between the cherenkov clad and the default material
  G4LogicalBorderSurface* logic_OpSurface_CCladdefault;
  logic_OpSurface_CCladdefault = new G4LogicalBorderSurface("logic_OpSurface_CCladdefault", Clad_C_PV, worldPV, OpSurfacedefault);

  // Here I place the Scintillating fibers and the SiPM next to them
  // Attention: I place an optical surface painted (blacked) from the moduleequippedPV 
  // to the SiPMPV, in so doing I completly avoid any cross talk between SiPMs
 
  G4VPhysicalVolume* physi_S_fiber[NofFibersrow][NofFiberscolumn];
  G4VPhysicalVolume* physi_SiPM[NofFibersrow][NofFiberscolumn];  
  G4LogicalBorderSurface* logic_OpSurface_defaultAir[NofFibersrow][NofFiberscolumn];

  G4int copynumber=0;

  for(int row=0; row<NofFibersrow; row++){
     std::stringstream S_fiber_row;
     S_fiber_row.str("");
     S_fiber_row << row;
     for(int column=0; column<NofFiberscolumn; column++){
        std::stringstream S_fiber_column;
        S_fiber_column.str("");
        S_fiber_column << column;
        std::string S_name;
        std::string SiPM_name;
        S_name = "S_row" + S_fiber_row.str() + "_column_" + S_fiber_column.str(); 
        SiPM_name = "SiPMS_row" + S_fiber_row.str() + "_column_" + S_fiber_column.str();

        // I need to specify the position of each scintillating fiber before placing them
        G4double S_x, S_y, S_z;
        G4ThreeVector vec_S_fiber;
        G4ThreeVector vec_SiPM;
        
        if(row == 0 || row == 2 || row == 4 || row == 6){
          if(column == 0 || column == 2 || column == 4 || column == 6){
            S_x = -moduleX/2 + 0.250 + fiberradius + (0.50+fiberradius*2)*column;
            S_y = -moduleY/2 + 0.250 + fiberradius + (0.50+fiberradius*2)*row;
         
            vec_S_fiber.setX(S_x);
            vec_S_fiber.setY(S_y);
            vec_S_fiber.setZ(0.);

            vec_SiPM.setX(S_x);
            vec_SiPM.setY(S_y);
            vec_SiPM.setZ(fiberZ/2+SiPMZ/2-0.18);
            
            copynumber = (8*row+column);

            // I need to place the scintillating fibers
            physi_S_fiber[row][column] = new G4PVPlacement(0,
                                                         vec_S_fiber,     //its position
                                                         logic_S_fiber,   //its logical volume
                                                         S_name,          //its name
                                                         moduleLV,        //its mother
                                                         false,           //no boulean operat
                                                         copynumber); 

            // I need to place the SiPMs
            physi_SiPM[row][column] = new G4PVPlacement(0,
                                                        vec_SiPM,                      //its position
                                                        S_SiPMLV,                        //its logical volume
                                                        SiPM_name,                    //its name
                                                        moduleequippedLV,                      //its mother
                                                        false,                        //no boulean operat
                                                        copynumber); 

          logic_OpSurface_defaultAir[NofFibersrow][NofFiberscolumn] = new G4LogicalBorderSurface("logic_OpSurface_defaultAir", CalorimeterPV, 
            physi_SiPM[row][column], OpSurfacedefault);
          }
        }
       if(row == 1 || row == 3 || row == 5 || row == 7){
         if(column == 1 || column == 3 || column == 5 || column == 7){
         S_x = -moduleX/2 + 0.250 + fiberradius + (0.50+fiberradius*2)*column;
         S_y = -moduleY/2 + 0.250 + fiberradius + (0.50+fiberradius*2)*row;

         vec_S_fiber.setX(S_x);
         vec_S_fiber.setY(S_y);
         vec_S_fiber.setZ(0.);

         vec_SiPM.setX(S_x);
         vec_SiPM.setY(S_y);
         vec_SiPM.setZ(fiberZ/2+SiPMZ/2-0.18);

         copynumber = (8*row+column);

         // I need to place the scintillating fibers
         physi_S_fiber[row][column] = new G4PVPlacement(0,               //no rotation
                                                        vec_S_fiber,     //its position
                                                        logic_S_fiber,   //its logical volume
                                                        S_name,          //its name
                                                        moduleLV,        //its mother
                                                        false,           //no boulean operat
                                                        copynumber); 

         // I need to place the SiPMs
         physi_SiPM[row][column] = new G4PVPlacement(0,
                                                     vec_SiPM,                     //its position
                                                     S_SiPMLV,                       //its logical volume
                                                     SiPM_name,                    //its name
                                                     moduleequippedLV,             //its mother
                                                     false,                        //no boulean operat
                                                     copynumber); 
         logic_OpSurface_defaultAir[NofFibersrow][NofFiberscolumn] = new G4LogicalBorderSurface("logic_OpSurface_defaultAir", CalorimeterPV, 
           physi_SiPM[row][column], OpSurfacedefault);
         }
       }
     };
  };

  // Here I place the Cherenkov fibers
  G4VPhysicalVolume* physi_C_fiber[NofFibersrow][NofFiberscolumn];
  
  for(int row=0; row<NofFibersrow; row++){
     std::stringstream C_fiber_row;
     C_fiber_row.str("");
     C_fiber_row << row;
     for(int column=0; column<NofFiberscolumn; column++){
        std::stringstream C_fiber_column;
        C_fiber_column.str("");
        C_fiber_column << column;
        std::string C_name;
        std::string SiPM_name;
        C_name = "C_row" + C_fiber_row.str() + "_column_" + C_fiber_column.str(); 
        SiPM_name = "SiPMC_row" + C_fiber_row.str() + "_column_" + C_fiber_column.str();

        // I need to specify the position of each cherenkov fiber
        G4double C_x, C_y, C_z;
        G4ThreeVector vec_C_fiber;
        G4ThreeVector vec_SiPM;
        
        if(row == 0 || row == 2 || row == 4 || row == 6){
          if(column == 1 || column == 3 || column == 5 || column == 7){
            C_x = -moduleX/2 + 0.250 + fiberradius + (0.50+fiberradius*2)*(column);
            C_y = -moduleY/2 + 0.250 + fiberradius + (0.50+fiberradius*2)*(row);
         
            vec_C_fiber.setX(C_x);
            vec_C_fiber.setY(C_y);
            vec_C_fiber.setZ(0.);

            vec_SiPM.setX(C_x);
            vec_SiPM.setY(C_y);
            vec_SiPM.setZ(fiberZ/2+SiPMZ/2-0.18);

            copynumber = (8*row+column);

            // I need to place the cherenkov fibers
            physi_C_fiber[row][column] = new G4PVPlacement(0,
                                                         vec_C_fiber,      //its position
                                                         logic_C_fiber,    //its logical volume
                                                         C_name,           //its name
                                                         moduleLV,         //its mother
                                                         false,            //no boulean operat
                                                         copynumber);

            // I need to place the SiPMs
            physi_SiPM[row][column] = new G4PVPlacement(0,
                                                        vec_SiPM,            //its position
                                                        C_SiPMLV,              //its logical volume
                                                        SiPM_name,           //its name
                                                        moduleequippedLV,    //its mother
                                                        false,               //no boulean operat
                                                        copynumber); 
            logic_OpSurface_defaultAir[NofFibersrow][NofFiberscolumn] = new G4LogicalBorderSurface("logic_OpSurface_defaultAir", CalorimeterPV, 
             physi_SiPM[row][column], OpSurfacedefault);
          }
        }
       if(row == 1 || row == 3 || row == 5 || row == 7){
         if(column == 0 || column == 2 || column == 4 || column == 6){
         C_x = -moduleX/2 + 0.250 + fiberradius + (0.50+fiberradius*2)*(column);
         C_y = -moduleY/2 + 0.250 + fiberradius + (0.50+fiberradius*2)*(row);

         vec_C_fiber.setX(C_x);
         vec_C_fiber.setY(C_y);
         vec_C_fiber.setZ(0.);
          
         vec_SiPM.setX(C_x);
         vec_SiPM.setY(C_y);
         vec_SiPM.setZ(fiberZ/2+SiPMZ/2-0.18);

         copynumber = (8*row+column);

         // I need to place the cherenkov fibers
         physi_C_fiber[row][column] = new G4PVPlacement(0,
                                                         vec_C_fiber,       //its position
                                                         logic_C_fiber,     //its logical volume
                                                         C_name,            //its name
                                                         moduleLV,          //its mother
                                                         false,             //no boulean operation
                                                         copynumber); 
         
          // I need to place the SiPMs
          physi_SiPM[row][column] = new G4PVPlacement(0,
                                                      vec_SiPM,                      //its position
                                                      C_SiPMLV,                        //its logical volume
                                                      SiPM_name,                    //its name
                                                      moduleequippedLV,                      //its mother
                                                      false,                        //no boulean operat
                                                      copynumber); 
          logic_OpSurface_defaultAir[NofFibersrow][NofFiberscolumn] = new G4LogicalBorderSurface("logic_OpSurface_defaultAir", CalorimeterPV, 
           physi_SiPM[row][column], OpSurfacedefault);
         }
       }
     };
  };

  // ------------------ region attributes -----------------------
  G4RegionStore *regionStore = G4RegionStore::GetInstance();
  G4Region *regFibre(regionStore->FindOrCreateRegion("Fibre"));
  regFibre->AddRootLogicalVolume(logic_S_fiber);
  regFibre->AddRootLogicalVolume(logic_C_fiber);

  // I return the physical World
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4DetectorConstruction::ConstructSDandField()
{ 
  // --------------- sensitive detectors ------------------------
  auto sdMan = G4SDManager::GetSDMpointer();
  auto trackerSD = new TrackerSD("TrackerSD");
  sdMan->AddNewDetector(trackerSD);
  SetSensitiveDetector("World", trackerSD);

  // SiPM particle and energy filter
  static const double hc = h_Planck * c_light;
  G4double elow = hc / (900. * nm);
  G4double ehigh = hc / (300. * nm);
  auto SiPMfilter = new G4SDParticleWithEnergyFilter("SiPMfilter");
  SiPMfilter->add("opticalphoton");
  SiPMfilter->SetKineticEnergy(elow, ehigh);

  auto S_SiPMsd = new SiPMsd("S_SiPMsd", "S_HitsCollection", 71, 8);
  S_SiPMsd->SetFilter(SiPMfilter);
  sdMan->AddNewDetector(S_SiPMsd);
  SetSensitiveDetector("S_Si", S_SiPMsd, true);

  auto C_SiPMsd = new SiPMsd("C_SiPMsd", "C_HitsCollection", 71, 8);
  C_SiPMsd->SetFilter(SiPMfilter);
  sdMan->AddNewDetector(C_SiPMsd);
  SetSensitiveDetector("C_Si", C_SiPMsd, true);

  // --------------- fast simulation ----------------------------
  G4RegionStore *regionStore = G4RegionStore::GetInstance();
  G4Region *regFibre = regionStore->GetRegion("Fibre");
  auto fastSimModelFibre = new FibreModel("FibreModel", regFibre);
  G4AutoDelete::Register(fastSimModelFibre);

  // --------------- magnetic field -----------------------------
  // Create global magnetic field messenger,
  // Uniform magnetic field is then created automatically if
  // the field value is not zero
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
