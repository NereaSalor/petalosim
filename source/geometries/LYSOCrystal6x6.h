// ----------------------------------------------------------------------------
// petalosim | LYSOCrystal6x6.h
//
// LYSO crystal of 6x6x5 mm.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef LYSO_CRYSTAL_H
#define LYSO_CRYSTAL_H

#include "nexus/GeometryBase.h"

using namespace nexus;

class LYSOCrystal6x6 : public GeometryBase
{
 public:
  /// Constructor
  LYSOCrystal6x6();
  /// Destructor
  ~LYSOCrystal6x6();
  
  void Construct();
  
  G4double GetCrystalDepth() const;
  
 private:
 
  G4double LYSO_z_;

};

inline G4double LYSOCrystal6x6::GetCrystalDepth() const {return LYSO_z_;}

#endif
