// ----------------------------------------------------------------------------
// petalosim | TeflonBlockHamamatsuFilter.h
//
// Teflon block used with the Hamamatsu VUV tiles when using the VUV filter.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "TeflonBlockHamamatsuFilter.h"

#include "nexus/IonizationSD.h"
#include "nexus/Visibilities.h"
#include "nexus/FactoryBase.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4UserLimits.hh>
#include <G4SubtractionSolid.hh>
#include <G4VisAttributes.hh>
#include <G4GenericMessenger.hh>

using namespace nexus;

REGISTER_CLASS(TeflonBlockHamamatsuFilter, GeometryBase)

TeflonBlockHamamatsuFilter::TeflonBlockHamamatsuFilter(): GeometryBase(),
                                              visibility_(0),
                                              teflon_block_thick_(35.75 * mm),
                                              max_step_size_(1.*mm),
                                              teflon_recess_depth_(3. * mm)
{

  // Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/TeflonBlockHamamatsuFilter/",
                                "Control commands of geometry PETitFilter.");
  msg_->DeclareProperty("visibility", visibility_, "Visibility");


}

TeflonBlockHamamatsuFilter::~TeflonBlockHamamatsuFilter()
{
}


void TeflonBlockHamamatsuFilter::Construct()
{
  G4double teflon_block_xy = 67. * mm;

  // Recess dimensions
  G4double recess_x = 50. * mm;
  G4double recess_y = 58. * mm;

  //Absolute positions of recess from center
  G4double recess_y_center = (teflon_block_xy - recess_y - 0.01 * mm)/2.;
  G4double recess_z_center = -teflon_block_thick_/2. + (teflon_recess_depth_ + 0.4 * mm)/2.;
  G4ThreeVector recess_pos(0, recess_y_center, + recess_z_center + 0.01*mm);

  G4double teflon_offset_x = 3.64 * mm;
  G4double teflon_offset_y = 3.7  * mm;

  G4double teflon_central_offset_x = 3.23 * mm;
  G4double teflon_central_offset_y = 3.11 * mm;

  G4double teflon_holes_xy    = 5.75 * mm;
  G4double teflon_holes_depth = 5.    * mm;

  G4double dist_between_holes_xy = 1.75 * mm;


  // Create de teflon block
  G4Box* teflon_block_solid =
    new G4Box("TEFLON_BLOCK", teflon_block_xy/2., teflon_block_xy/2., teflon_block_thick_/2.);

  G4Material *teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  teflon->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  // Create de substraction block
  G4Box* teflon_recess_solid =
    new G4Box("TEFLON_RECESS", recess_x/2., recess_y/2., (teflon_recess_depth_ + 0.4 * mm)/2.);

  G4SubtractionSolid* subtracted_block_solid =
    new G4SubtractionSolid("TEFLON_BLOCK_RECESS", teflon_block_solid, teflon_recess_solid, 0, recess_pos);

  G4LogicalVolume* teflon_block_logic =
    new G4LogicalVolume(subtracted_block_solid, teflon, "TEFLON_BLOCK");
    this->SetLogicalVolume(teflon_block_logic);

  // Create the logic volume for the recess teflon to avoid overlaps
  G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
  G4LogicalVolume* teflon_recess_logic = new G4LogicalVolume(teflon_recess_solid, LXe, "TEFLON_RECESS_LOGIC");
  teflon_recess_logic_ = teflon_recess_logic; //To be able to call it
  new G4PVPlacement(0, recess_pos, teflon_recess_logic, "TEFLON_RECESS_PV",
                    teflon_block_logic, false, 0, false);

  // Holes in the block which are filled with LXe and defined as LXe vols
  G4double dist_four_holes = 4* teflon_holes_xy + 3*dist_between_holes_xy;

  G4Box* teflon_hole_solid =
    new G4Box("ACTIVE", teflon_holes_xy/2., teflon_holes_xy/2., teflon_holes_depth/2.);

  G4LogicalVolume* teflon_hole_logic =
    new G4LogicalVolume(teflon_hole_solid, mat_, "ACTIVE");

  // Set the ACTIVE volume as an ionization sensitive det
  teflon_hole_logic->SetSensitiveDetector(ionisd_);
  teflon_hole_logic->SetUserLimits(new G4UserLimits(max_step_size_));

  G4double holes_pos_z = -teflon_block_thick_/2. + teflon_holes_depth/2. + teflon_recess_depth_;

  G4int copy_no = 0;

  for (G4int j = 0; j < 2; j++){ // Loop over the tiles in row
    G4double set_holes_y = teflon_block_xy/2. - teflon_offset_y - dist_four_holes/2.
      - j*(teflon_central_offset_y + dist_four_holes);

    for (G4int i = 0; i < 2; i++){ // Loop over the tiles in column
      G4double set_holes_x = -teflon_block_xy/2. + teflon_offset_x + dist_four_holes/2.
        + i*(teflon_central_offset_x + dist_four_holes);

      for (G4int l = 0; l < 4; l++){ // Loop over the sensors in row
        G4double holes_pos_y = set_holes_y + 3*(teflon_holes_xy/2. + dist_between_holes_xy/2.)
          - l*(teflon_holes_xy + dist_between_holes_xy);

        for (G4int k = 0; k < 4; k++){ // Loop over the sensors in column
          G4double holes_pos_x = set_holes_x - 3*(teflon_holes_xy/2. + dist_between_holes_xy/2.)
            + k*(teflon_holes_xy + dist_between_holes_xy);

          //Global matrix index
          G4int global_row = j * 4 + l;
          G4int global_col = i * 4 + k;

          G4bool is_edge = (global_row == 0 || global_row == 7 || global_col == 0 || global_col == 7);

          G4double hole_z = is_edge ?
                            (-teflon_block_thick_/2. + teflon_holes_depth/2.) :
                            (holes_pos_z);

          new G4PVPlacement(0, G4ThreeVector(holes_pos_x, holes_pos_y, hole_z), teflon_hole_logic,
                            "ACTIVE", teflon_block_logic, false, copy_no, false);
          copy_no += 1;
        }
      }
    }
  }

  if (visibility_) {
    G4VisAttributes teflon_col = nexus::LightBlue();
    teflon_block_logic->SetVisAttributes(teflon_col);
  }

}
