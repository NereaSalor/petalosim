// ----------------------------------------------------------------------------
// petalosim | SetUpCristales.cc
//
// Basic set-up with the LYSO crystals and the VUV neutral filter.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "SetUpCristales.h"
#include "LYSOCrystal6x6.h"
#include "SiPMLYSOBlue.h"
#include "NeutralFilterVUV.h"
#include "PetMaterialsList.h"
#include "PetOpticalMaterialProperties.h"
#include "Na22Source.h"
#include "PetIonizationSD.h"

#include "G4ios.hh"

#include "nexus/Visibilities.h"
#include "nexus/OpticalMaterialProperties.h"
#include "nexus/SpherePointSampler.h"
#include "nexus/FactoryBase.h"

#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4GenericMessenger.hh>
#include <G4SubtractionSolid.hh>

using namespace nexus;

REGISTER_CLASS(SetUpCristales, GeometryBase)

SetUpCristales::SetUpCristales() : GeometryBase(),
                                   visibility_(0),
                                   max_step_size_(1. * mm)

{
  // Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/SetUpCristales/",
                                "Control commands of geometry SetUpCristales.");
  msg_->DeclareProperty("visibility", visibility_, "Visibility");

}

SetUpCristales::~SetUpCristales()
{
}

void SetUpCristales::Construct()
{

  //// Volume of AIR surrounding the detector ////
  G4double lab_size = 1. * m;
  G4Box *lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

  G4Material *air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  lab_logic_ = new G4LogicalVolume(lab_solid, air, "LAB");
  lab_logic_->SetVisAttributes(G4VisAttributes::GetInvisible());
  this->SetLogicalVolume(lab_logic_);

  G4VPhysicalVolume* lab_phys = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), lab_logic_, "WORLD", 0, false, 0);

  //// ENCAPSULATED SOURCE ////
  Na22Source na22 = Na22Source();
  na22.Construct();
  G4LogicalVolume* na22_logic = na22.GetLogicalVolume();

  new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), na22_logic,
                    "NA22_SOURCE", lab_logic_, false, 0, true);

  source_gen_ = new SpherePointSampler(0, na22.GetSourceDiameter()/2, 0., twopi, 0, pi,
                                         G4ThreeVector(0., 0., 0.), 0);

  //// 6X6 mm LYSO CRYSTAL as ACTIVE ////
  LYSOCrystal6x6 crystal = LYSOCrystal6x6();
  crystal.Construct();
  active_logic_ = crystal.GetLogicalVolume();

  G4double dist_crystal_source = 8 * mm;
  G4double crystal_depth = crystal.GetCrystalDepth();
  G4double crystal_xy    = crystal.GetCrystalXY();
  G4VPhysicalVolume* lyso_phys = new G4PVPlacement(0, G4ThreeVector(0., 0., dist_crystal_source + crystal_depth/2.),
                                 active_logic_, "LYSO_CRYSTAL", lab_logic_, false, 0, true);

  PetIonizationSD* ionisd = new PetIonizationSD("/SETUPCRISTALES/ACTIVE");
  G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

  active_logic_->SetSensitiveDetector(ionisd);
  active_logic_->SetUserLimits(new G4UserLimits(max_step_size_));

  // OPTICAL SURFACE FOR REFLECTION

  // For teflon surface we need to build physical volumes to use Border SURFACE
  // because the skin surface applies the teflon to all surfaces and we only need
  // it in 5 out of 6. We build a physical volume of air surrounding the crystal.

  //G4OpticalSurface *teflon_opsurf =
  //    new G4OpticalSurface("PTFE_OPSURF", unified, polished, dielectric_dielectric);
  //teflon_opsurf->SetMaterialPropertiesTable(petopticalprops::PTFE());

  //new G4LogicalBorderSurface("LYSO_to_PTFE", lyso_phys, lab_phys, teflon_opsurf);

  // envolvente PTFE: tamaño exterior = Lyso + wrap
  G4double wrap = 0.2*mm;
  G4Box* outer = new G4Box("PTFE_outer", crystal_xy/2. + wrap, crystal_xy/2. + wrap, crystal_depth/2. + wrap);
  G4Box* inner = new G4Box("PTFE_inner", crystal_xy/2., crystal_xy/2., crystal_depth/2.);

  // Nos quedamos con un recubrimiento de teflon en todas las caras
  G4SubtractionSolid* ptfe_shell = new G4SubtractionSolid("PTFE_shell", outer, inner);

  // Recortamos el lado derecho del teflon para dejar la cara en contacto con el SiPM libre
  G4double cutout_xy = crystal_xy + 2*wrap;
  G4double cutout_z  = wrap + 0.1*mm;

  G4Box* cutout = new G4Box("PTFE_cutout", cutout_xy/2., cutout_xy/2., cutout_z/2.);
  G4SubtractionSolid* ptfe_shell_with_hole = new G4SubtractionSolid("PTFE_SHELL_WITH_HOLE",
                                             ptfe_shell, cutout, 0, G4ThreeVector(0., 0., (crystal_depth + wrap)/2. - 0.05 * mm));

  G4Material *teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

  G4LogicalVolume* ptfe_logic = new G4LogicalVolume(ptfe_shell_with_hole, teflon, "PTFE_LOGIC");
  G4VPhysicalVolume* ptfe_phys = new G4PVPlacement(0, G4ThreeVector(0., 0., dist_crystal_source + crystal_depth/2.),
                                 ptfe_logic, "PTFE", lab_logic_, false, 0, true);

  G4OpticalSurface *teflon_opsurf =
       new G4OpticalSurface("PTFE_OPSURF", unified, polished, dielectric_dielectric);
  teflon_opsurf->SetMaterialPropertiesTable(petopticalprops::PTFE());

  new G4LogicalBorderSurface("LYSO_to_PTFE", lyso_phys, ptfe_phys, teflon_opsurf);
  new G4LogicalBorderSurface("PTFE_to_AIR", ptfe_phys, lab_phys, teflon_opsurf);


  //// NEUTRAL FILTER VUV ////
  NeutralFilterVUV filter = NeutralFilterVUV();
  filter.Construct();
  G4LogicalVolume* filter_logic = filter.GetLogicalVolume();

  G4double filter_depth = filter.GetFilterDepth();
  G4double dist_filter = dist_crystal_source + crystal_depth + filter_depth/2.;

  new G4PVPlacement(0, G4ThreeVector(0., 0., dist_filter), filter_logic,
                    "FILTER", lab_logic_, false, 0, true);

  //// BLUE SiPM ////
  SiPMLYSOBlue sipm = SiPMLYSOBlue();
  sipm.Construct();
  G4LogicalVolume* sipm_logic = sipm.GetLogicalVolume();

  G4double sipm_depth = sipm.GetCellThickness();
  G4double dist_sipm = dist_crystal_source + crystal_depth + filter_depth + sipm_depth/2.;

  new G4PVPlacement(0, G4ThreeVector(0., 0., dist_sipm), sipm_logic,
                    "SIPM", lab_logic_, false, 0, true);

  if (visibility_) {
    G4VisAttributes crys_col = nexus::White();
    active_logic_->SetVisAttributes(crys_col);
    G4VisAttributes filter_col = nexus::Blue();
    filter_logic->SetVisAttributes(filter_col);
    G4VisAttributes sipm_col = nexus::DarkGrey();
    sipm_logic->SetVisAttributes(sipm_col);
    G4VisAttributes na22_col = nexus::Lilla();
    na22_logic->SetVisAttributes(na22_col);


    }
}

G4ThreeVector SetUpCristales::GenerateVertex(const G4String &region) const
{
  G4ThreeVector vertex(0., 0., 0.);

    if (region == "SOURCE") {
        vertex = source_gen_->GenerateVertex(VOLUME);

  } else {
    G4Exception("[SetUpCristales]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }
  return vertex;
}
