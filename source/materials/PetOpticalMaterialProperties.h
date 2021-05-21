// ----------------------------------------------------------------------------
// petalosim | PetOpticalMaterialProperties.h
//
// Optical properties of relevant materials.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef P_OPTICAL_MATERIAL_PROPERTIES_H
#define P_OPTICAL_MATERIAL_PROPERTIES_H

#include <globals.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

class G4MaterialPropertiesTable;

using namespace CLHEP;

/// This is a stateless class where all methods are static functions.

class PetOpticalMaterialProperties
{
public:
  static G4MaterialPropertiesTable *Vacuum();
  static G4MaterialPropertiesTable *Epoxy();
  static G4MaterialPropertiesTable *GlassEpoxy();
  static G4MaterialPropertiesTable *EpoxyFixedRefr(G4double n);
  static G4MaterialPropertiesTable *EpoxyLXeRefr();

  static G4MaterialPropertiesTable *FusedSilica();

  static G4MaterialPropertiesTable *FakeFusedSilica(G4double transparency = .9,
                                                    G4double thickness = 1. * mm);

  static G4MaterialPropertiesTable *FakeGenericMaterial(G4double quartz_rindex = 1.6);

  static G4MaterialPropertiesTable *GAr(G4double sc_yield);
  static G4MaterialPropertiesTable *LAr();

  static G4MaterialPropertiesTable *GXe(G4double pressure = 1. * bar,
                                        G4double temperature = STP_Temperature,
                                        G4int sc_yield = 25510 / MeV);

  static G4MaterialPropertiesTable *LXe();
  static G4MaterialPropertiesTable *LXe_nconst();

  static G4MaterialPropertiesTable *Glass();

  static G4MaterialPropertiesTable *PTFE();
  static G4MaterialPropertiesTable *Pyrex_vidrasa();

  static G4MaterialPropertiesTable *TPB(G4double decay_time = 2.2 * nanosecond);
  static G4MaterialPropertiesTable *TPB_LXe(G4double decay_time = 2.2 * nanosecond);
  static G4MaterialPropertiesTable *TPB_LXe_nconst(G4double decay_time = 2.2 * nanosecond);

  static G4MaterialPropertiesTable *LYSO();
  static G4MaterialPropertiesTable *LYSO_nconst();
  static G4MaterialPropertiesTable *ReflectantSurface(G4double reflectivity = 0.95);

private:
  static constexpr G4double optPhotMinE_ = 1. * eV;
  static constexpr G4double optPhotMaxE_ = 8.21 * eV;
  static constexpr G4double noAbsLength_ = 1.e8 * m;

  static constexpr G4double lyso_minE_ = 1.9630 * eV;
  static constexpr G4double lyso_maxE_ = 3.3429 * eV;

  /// Constructor (hidden)
  PetOpticalMaterialProperties();
  /// Destructor (hidden)
  ~PetOpticalMaterialProperties();
};

#endif
