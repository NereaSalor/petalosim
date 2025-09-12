// ----------------------------------------------------------------------------
// petalosim | LYSOCrystal6x6.cc
//
// LYSO crystal with 6x6x5 mm.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "LYSOCrystal6x6.h"
#include "PetMaterialsList.h"
#include "PetOpticalMaterialProperties.h"

#include <G4Box.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4SDManager.hh>
#include <G4VisAttributes.hh>

using namespace nexus;
using namespace CLHEP;

LYSOCrystal6x6::LYSOCrystal6x6() : GeometryBase(),
                                   LYSO_z_(5. * mm),
                                   LYSO_xy_(6. * mm)
{
}


LYSOCrystal6x6::~LYSOCrystal6x6()
{
}

void LYSOCrystal6x6::Construct()
{
  G4Box* lyso_solid =
      new G4Box("CRYSTAL", LYSO_xy_/2., LYSO_xy_/2., LYSO_z_/2.);

  G4Material* lyso = petmaterials::LYSO();
  lyso->SetMaterialPropertiesTable(petopticalprops::LYSO());

  G4LogicalVolume* lyso_logic =
    new G4LogicalVolume(lyso_solid, lyso, "CRYSTAL");

  this->SetLogicalVolume(lyso_logic);


  G4VisAttributes crystal_col = G4Colour::Yellow();
  lyso_logic->SetVisAttributes(crystal_col);

}
