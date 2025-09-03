// ----------------------------------------------------------------------------
// petalosim | SetUpCristales.cc
//
// Basic set-up with the LYSO crystals and the VUV neutral filter.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef SETUP_CRISTALES_H
#define SETUP_CRISTALES_H

#include "nexus/GeometryBase.h"

class G4Material;
class G4GenericMessenger;
class G4LogicalVolume;
class G4VPhysicalVolume;


namespace nexus
{
class SpherePointSampler;
class IonizationSD;
}

using namespace nexus;

class SetUpCristales : public GeometryBase
{

 public:
  // Constructor
  SetUpCristales();
  //Destructor
  ~SetUpCristales();


  void Construct();
  void SetIoniSD(IonizationSD* ionisd);
  void SetMaxStepSize(G4double step_size);
  void SetVisibility(G4bool vis);

  G4LogicalVolume* GetActiveVolume() const;

  G4ThreeVector GenerateVertex(const G4String &region) const;

private:
  G4bool visibility_;
  
  G4double  max_step_size_;

  SpherePointSampler* source_gen_;

  IonizationSD* ionisd_;

  G4LogicalVolume* lab_logic_;
  G4LogicalVolume* active_logic_;
  
  /// Messenger for the definition of control commands
  G4GenericMessenger* msg_;
};

inline void SetUpCristales::SetIoniSD(IonizationSD* ionisd) {ionisd_ = ionisd;}
inline void SetUpCristales::SetMaxStepSize(G4double step_size) {max_step_size_ = step_size;}
inline void SetUpCristales::SetVisibility(G4bool vis) {visibility_ = vis;}


#endif
