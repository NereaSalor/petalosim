// ----------------------------------------------------------------------------
// petalosim | PETitFilter.cc
//
// This class implements the geometry of a box of LXe with Hamamatsu tiles
// with the VUV filter.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "PETitFilter.h"
#include "PETitBox.h"
#include "TileHamamatsuVUV.h"
#include "PetMaterialsList.h"
#include "PetOpticalMaterialProperties.h"
#include "TeflonBlockHamamatsu.h"
#include "TeflonBlockHamamatsuFilter.h"
#include "PetaloUtils.h"
#include "NeutralFilterVUV.h"
#include "nexus/Visibilities.h"
#include "nexus/IonizationSD.h"
#include "nexus/FactoryBase.h"
#include "nexus/SpherePointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

using namespace nexus;

REGISTER_CLASS(PETitFilter, GeometryBase)

PETitFilter::PETitFilter() : GeometryBase(),
                 visibility_(0),
                 box_vis_(0),
                 tile_vis_(1),
                 tile_refl_(0.),
                 dist_dice_flange_(18.6 * mm),
                 n_tile_rows_(2),
                 n_tile_columns_(2),
                 specific_vertex_{},
                 sipm_cells_(false),
                 max_step_size_(1. * mm),
                 pressure_(1 * bar)

{
  // Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/PETitFilter/",
                                "Control commands of geometry PETitFilter.");
  msg_->DeclareProperty("visibility", visibility_, "Visibility");
  msg_->DeclareProperty("box_vis", box_vis_,
                        "Visibility of the basic structure");
  msg_->DeclareProperty("tile_vis", tile_vis_, "Visibility of tiles");
  msg_->DeclareProperty("tile_refl", tile_refl_, "Reflectivity of SiPM boards");
  msg_->DeclareProperty("sipm_cells", sipm_cells_,
                        "True if each cell of SiPMs is simulated");

  msg_->DeclarePropertyWithUnit("specific_vertex", "mm",  specific_vertex_,
                                "Set generation vertex.");

  G4GenericMessenger::Command &press_cmd =
    msg_->DeclareProperty("pressure", pressure_,
                          "Pressure of LXe");
  press_cmd.SetUnitCategory("Pressure");
  press_cmd.SetParameterName("pressure", false);
  press_cmd.SetRange("pressure>0.");


}

PETitFilter::~PETitFilter()
{
}

void PETitFilter::Construct()
{
  // Volume of air surrounding the detector //
  G4double lab_size = 1. * m;
  G4Box *lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

  G4Material *air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  lab_logic_ = new G4LogicalVolume(lab_solid, air, "LAB");
  lab_logic_->SetVisAttributes(G4VisAttributes::GetInvisible());
  this->SetLogicalVolume(lab_logic_);

  BuildBox();
  BuildSensors();
}

void PETitFilter::BuildBox()
{
  G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
  LXe->SetMaterialPropertiesTable(petopticalprops::LXe(pressure_));

  // Set the ACTIVE volume as an ionization sensitive det
  IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
  G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);


  box_ = new PETitBox();
  box_->SetLXePressure(pressure_);
  box_->SetXeMaterial(LXe);
  box_->SetIoniSD(ionisd);
  box_->SetVisibility(box_vis_);
  box_->Construct();
  G4LogicalVolume* box_logic = box_->GetLogicalVolume();

  new G4PVPlacement(0, G4ThreeVector(0., 0, 0.),
                    box_logic, "ALUMINUM_BOX", lab_logic_,
                    false, 0, false);

  active_logic_ = box_->GetActiveVolume();
  G4double ih_z_size = box_->GetHatZSize();

  // Add both teflon blocks
  TeflonBlockHamamatsu teflon_block_hama = TeflonBlockHamamatsu();
  teflon_block_hama.SetHoleMaterial(LXe);
  teflon_block_hama.SetIoniSD(ionisd);
  teflon_block_hama.SetMaxStepSize(max_step_size_);
  teflon_block_hama.Construct();
  G4LogicalVolume* teflon_block_logic = teflon_block_hama.GetLogicalVolume();

  G4double teflon_block_thick = teflon_block_hama.GetTeflonThickness();
  G4double block_z_pos = ih_z_size/2. + teflon_block_thick/2.;
  new G4PVPlacement(0, G4ThreeVector(0., 0., -block_z_pos), teflon_block_logic,
                    "TEFLON_BLOCK_HAMA", active_logic_, false, 0, false);

  TeflonBlockHamamatsuFilter teflon_block_hama_filt = TeflonBlockHamamatsuFilter();
  teflon_block_hama_filt.SetHoleMaterial(LXe);
  teflon_block_hama_filt.SetIoniSD(ionisd);
  teflon_block_hama_filt.SetMaxStepSize(max_step_size_);
  teflon_block_hama_filt.Construct();
  G4LogicalVolume* teflon_block_hama_filt_logic = teflon_block_hama_filt.GetLogicalVolume();

  G4double teflon_block_filter_thick = teflon_block_hama_filt.GetTeflonThickness();
  G4double teflon_recess_depth = teflon_block_hama_filt.GetTeflonRecessDepth();
  G4double block_z_pos_filt = ih_z_size/2. + teflon_block_filter_thick/2.;

  G4RotationMatrix rot_teflon;
  rot_teflon.rotateY(pi);
  new G4PVPlacement(G4Transform3D(rot_teflon,
                                  G4ThreeVector(0., 0., block_z_pos_filt)),
                    teflon_block_hama_filt_logic,
                    "TEFLON_BLOCK_HAMA_FILT", active_logic_, false, 0, false);

  // Optical surface for teflon
  G4OpticalSurface* teflon_optSurf =
    new G4OpticalSurface("TEFLON_OPSURF", unified, ground, dielectric_metal);
  teflon_optSurf->SetMaterialPropertiesTable(petopticalprops::PTFE());
  new G4LogicalSkinSurface("TEFLON_OPSURF", teflon_block_logic, teflon_optSurf);

  G4OpticalSurface* teflon_filt_optSurf =
    new G4OpticalSurface("TEFLON_FILTER_OPSURF", unified, ground, dielectric_metal);
  teflon_filt_optSurf->SetMaterialPropertiesTable(petopticalprops::PTFE());
  new G4LogicalSkinSurface("TEFLON_FILTER_OPSURF", teflon_block_hama_filt_logic, teflon_filt_optSurf);

  //Build VUV filter

  NeutralFilterVUV filter_VUV = NeutralFilterVUV();
  filter_VUV.Construct();
  G4LogicalVolume* filter_VUV_logic = filter_VUV.GetLogicalVolume();
  G4double filter_depth = filter_VUV.GetFilterDepth();
  G4double dist_teflon_filter = 0.5 * mm;
  G4double filter_z_pos = ih_z_size/2. + teflon_block_filter_thick - teflon_recess_depth + filter_depth/2. + dist_teflon_filter;

  new G4PVPlacement(0, G4ThreeVector(0., 0., filter_z_pos), filter_VUV_logic,
                    "FILTER_VUV", active_logic_, false, 0, false);

  if (visibility_) {
    G4VisAttributes block_col = nexus::LightBlue();
    teflon_block_logic->SetVisAttributes(block_col);
    teflon_block_hama_filt_logic->SetVisAttributes(block_col);
    G4VisAttributes filt_col = nexus::LightGrey();
    filter_VUV_logic->SetVisAttributes(filt_col);
  }


}


void PETitFilter::BuildSensors()
{
  TileHamamatsuVUV tile = TileHamamatsuVUV();
  tile.SetBoxConf(hama);
  tile.SetTileVisibility(tile_vis_);
  tile.SetTileReflectivity(tile_refl_);
  tile.SetSiPMCells(sipm_cells_);
  tile.Construct();

  G4double tile_size_x = tile.GetDimensions().x();
  G4double tile_size_y = tile.GetDimensions().y();
  G4double tile_thickn = tile.GetDimensions().z();
  G4double full_row_size = n_tile_columns_ * tile_size_x;
  G4double full_col_size = n_tile_rows_ * tile_size_y;

  G4LogicalVolume* tile_logic = tile.GetLogicalVolume();
  G4String vol_name;
  G4int copy_no = 0;
  G4double z_pos =
    -box_->GetBoxSize()/2. + box_->GetBoxThickness() + dist_dice_flange_
    + tile_thickn/2.;

  for (G4int j = 0; j < n_tile_rows_; j++) {
    G4double y_pos = full_col_size/2. - tile_size_y/2. - j*tile_size_y;
    for (G4int i = 0; i < n_tile_columns_; i++) {
      G4double x_pos = -full_row_size/2. + tile_size_x/2. + i*tile_size_x;
      vol_name = "TILE_" + std::to_string(copy_no);

      new G4PVPlacement(0, G4ThreeVector(x_pos, y_pos, z_pos), tile_logic,
                        vol_name, active_logic_, false, copy_no, false);
      copy_no += 1;
    }
  }

  G4RotationMatrix rot;
  rot.rotateY(pi);

  for (G4int j=0; j<n_tile_rows_; j++) {
    G4double y_pos = full_col_size/2. - tile_size_y/2. - j*tile_size_y;
    for (G4int i=0; i<n_tile_columns_; i++) {
      G4double x_pos = full_row_size/2. - tile_size_x/2. - i*tile_size_x;
      vol_name = "TILE_" + std::to_string(copy_no);

      new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(x_pos, y_pos, -z_pos)),
                        tile_logic, vol_name, active_logic_,
                        false, copy_no, false);
      copy_no += 1;
    }
  }

}

G4ThreeVector PETitFilter::GenerateVertex(const G4String &region) const
{
  G4ThreeVector vertex(0., 0., 0.);

  if (region == "CENTER") {
    return vertex;
  } else if (region == "AD_HOC") {
    vertex = specific_vertex_;
  } else if (region == "SOURCE") {
    vertex = box_->GenerateVertex("SOURCE");
  } else {
    G4Exception("[PETit]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }
  return vertex;
}
