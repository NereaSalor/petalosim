// ----------------------------------------------------------------------------
// petalosim | TeflonBlockHamamatsuFilter.h
//
// Teflon block used with the Hamamatsu VUV tiles when using the VUV filter.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef TEFLON_HAMA_FILT_H
#define TEFLON_HAMA_FILT_H

#include "nexus/GeometryBase.h"

class G4GenericMessenger;
class G4Material;

class PetIonizationSD;


using namespace nexus;

class TeflonBlockHamamatsuFilter: public GeometryBase
{
public:
  // Constructor
  TeflonBlockHamamatsuFilter();
  //Destructor
  ~TeflonBlockHamamatsuFilter();

  void Construct();

  void SetHoleMaterial(G4Material* mat);
  void SetIoniSD(PetIonizationSD* ionisd);
  void SetMaxStepSize(G4double step_size);

  G4double GetTeflonThickness() const;
  G4double GetTeflonRecessDepth() const;
  G4LogicalVolume* GetRecessLogic() const;

 private:

  G4Material* mat_;
  PetIonizationSD* ionisd_;
  G4double teflon_block_thick_;
  G4double max_step_size_;
  G4double teflon_recess_depth_;
  G4bool visibility_;
  
  /// Messenger for the definition of control commands
  G4GenericMessenger* msg_;
  
  // Logical volume of the recess for placing the filter
  G4LogicalVolume* teflon_recess_logic_;


};

inline void TeflonBlockHamamatsuFilter::SetHoleMaterial(G4Material* mat) {mat_ = mat;}
inline void TeflonBlockHamamatsuFilter::SetIoniSD(PetIonizationSD* ionisd) {ionisd_ = ionisd;}
inline void TeflonBlockHamamatsuFilter::SetMaxStepSize(G4double step_size) {max_step_size_ = step_size;}
inline G4double TeflonBlockHamamatsuFilter::GetTeflonThickness() const {return teflon_block_thick_;}
inline G4double TeflonBlockHamamatsuFilter::GetTeflonRecessDepth() const {return teflon_recess_depth_;}
inline G4LogicalVolume* TeflonBlockHamamatsuFilter::GetRecessLogic() const {return teflon_recess_logic_;}

#endif
