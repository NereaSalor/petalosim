// ----------------------------------------------------------------------------
// petalosim | SiPMLYSOBlue.cc
//
// Hamamatsu blue sensor geometry of 6x6 mm with 50 um cell pitch.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "SiPMLYSOBlue.h"
#include "ToFSD.h"
#include "PetMaterialsList.h"
#include "PetOpticalMaterialProperties.h"

#include "nexus/Visibilities.h"
#include "nexus/IonizationSD.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4SDManager.hh>
#include <G4VisAttributes.hh>

using namespace nexus;
using namespace CLHEP;

SiPMLYSOBlue::SiPMLYSOBlue() : GeometryBase(),
                               cell_thickn_(300 * micrometer),
                               cell_size_(50 * micrometer)
{
}

SiPMLYSOBlue::~SiPMLYSOBlue()
{
}

void SiPMLYSOBlue::Construct()
{
  // Construimos primero las microceldas del SiPM

  G4Box* active_cell_solid =
      new G4Box("PHOTODIODES", cell_size_/2., cell_size_/2., cell_thickn_/2);

  G4Material* silicon =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

  G4LogicalVolume* active_cell_logic =
    new G4LogicalVolume(active_cell_solid, silicon, "PHOTODIODES");

    // SiPM efficiency set using the official Hamamatsu specs.
  const G4int entries = 51;

  G4double energies[entries] = {1.37923 * eV, 1.40506 * eV, 1.43348 * eV, 1.47492 * eV,
                                1.51343 * eV, 1.54646 * eV, 1.58292 * eV, 1.61704 * eV,
                                1.65266 * eV, 1.68988 * eV, 1.72648 * eV, 1.75741 * eV,
                                1.79956 * eV, 1.83847 * eV, 1.8791  * eV, 1.93029 * eV,
                                1.96297 * eV, 1.99366 * eV, 2.02531 * eV, 2.07473 * eV,
                                2.12662 * eV, 2.17374 * eV, 2.21912 * eV, 2.27048 * eV,
                                2.31163 * eV, 2.37623 * eV, 2.43057 * eV, 2.50209 * eV,
                                2.58842 * eV, 2.70364 * eV, 2.78633 * eV, 2.90699 * eV,
                                3.00281 * eV, 3.06781 * eV, 3.14342 * eV, 3.21473 * eV,
                                3.27246 * eV, 3.34104 * eV, 3.38538 * eV, 3.44017 * eV,
                                3.49676 * eV, 3.55524 * eV, 3.59533 * eV, 3.62599 * eV,
                                3.65718 * eV, 3.67828 * eV, 3.71038 * eV, 3.73209 * eV,
                                3.77629 * eV, 3.82155 * eV, 3.87967 * eV};

  G4double reflectivity[entries] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                    0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                    0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                    0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                    0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                    0.};

  G4double efficiency[entries] = {0.03341, 0.04131, 0.04921, 0.06014, 0.07047, 0.08019,
                                  0.09052, 0.10085, 0.11117, 0.12332, 0.13608, 0.14763,
                                  0.16038, 0.17496, 0.18955, 0.20716, 0.22114, 0.23997,
                                  0.2503 , 0.26731, 0.28554, 0.30133, 0.31713, 0.33353,
                                  0.34811, 0.36512, 0.3791 , 0.39185, 0.39793, 0.4034 ,
                                  0.39732, 0.38274, 0.3712 , 0.35965, 0.34568, 0.32503,
                                  0.30619, 0.28311, 0.26427, 0.24605, 0.22843, 0.20777,
                                  0.18347, 0.16403, 0.14337, 0.12575, 0.10206, 0.08383,
                                  0.06257, 0.04434, 0.02551};

  G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
  sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
  sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

  G4OpticalSurface* sipm_opsurf =
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
  sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

  new G4LogicalSkinSurface("SIPM_OPSURF", active_cell_logic, sipm_opsurf);

  // SENSITIVE DETECTOR ////////////////////////////////////////////

  G4String sdname = "/SIPM/SiPMLYSOBlue";
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

  if (!sdmgr->FindSensitiveDetector(sdname, false))
  {
    ToFSD* sipmsd = new ToFSD(sdname);

    sipmsd->SetDetectorVolumeDepth(0);
    sipmsd->SetMotherVolumeDepth(1);
    sipmsd->SetGrandMotherVolumeDepth(2);
    sipmsd->SetDetectorNamingOrder(0);
    sipmsd->SetBoxConf(hama);
    sipmsd->SetSiPMCells(true);

    G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
    active_cell_logic->SetSensitiveDetector(sipmsd);
  }

  // PACKAGE //

  G4double sipm_xy = 6. * mm;
  G4double sipm_z = 0.3 * mm;
  G4double n_microcells = 14400;

  G4Box* sipm_solid = new G4Box("SiPM", sipm_xy/2., sipm_xy/2., sipm_z/2.);

  G4Material* plastic = petmaterials::FR4();
  G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, plastic, "SiPM");

  this->SetLogicalVolume(sipm_logic);

  // OPTICAL SURFACE FOR REFLECTION

  G4OpticalSurface * plastic_opsurf =
      new G4OpticalSurface("PLASTIC_OPSURF", unified, polished, dielectric_metal);
  G4MaterialPropertiesTable* plastic_mt = new G4MaterialPropertiesTable();
  plastic_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

  plastic_opsurf->SetMaterialPropertiesTable(plastic_mt);

  new G4LogicalSkinSurface("PLASTIC_OPSURF", sipm_logic, plastic_opsurf);


  // Parametros de la microcelda:

  G4int n_rows    =  G4int(sipm_xy / cell_size_ + 0.5); // Para asegurar bien el redondeo
  G4int n_columns =  G4int(sipm_xy / cell_size_ + 0.5);
  assert(n_rows*n_columns == n_microcells);

  G4double x = 0;
  G4double y = 0;

  G4int copy_no = 0;

  for (G4int i=0; i<n_rows; i++) {
    for (G4int j=0; j<n_columns; j++) {
      x = - sipm_xy/2 + cell_size_/2 + i*cell_size_;
      y = - sipm_xy/2 + cell_size_/2 + j*cell_size_;
      new G4PVPlacement(0, G4ThreeVector(x, y, sipm_z/2. - cell_thickn_/2.),
                        active_cell_logic, "PHOTODIODES", sipm_logic,
                        false, copy_no, false);
      copy_no += 1;
    }
  }

  G4VisAttributes cell_col = G4Colour::Yellow();
  active_cell_logic->SetVisAttributes(cell_col);
  G4VisAttributes sipm_col = nexus::LightBlue();
  sipm_logic->SetVisAttributes(sipm_col);

}
