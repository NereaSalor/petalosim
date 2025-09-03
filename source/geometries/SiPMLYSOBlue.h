// ----------------------------------------------------------------------------
// petalosim | SiPMLYSOBlue.h
//
// Hamamatsu blue sensor geometry of 6x6 mm with 50 um cell pitch.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_BLUE_H
#define SIPM_BLUE_H

#include "nexus/GeometryBase.h"

using namespace nexus;

class SiPMLYSOBlue : public GeometryBase
{
 public:
  /// Constructor
  SiPMLYSOBlue();
  /// Destructor
  ~SiPMLYSOBlue();
  
  void Construct();
  
  G4double GetCellThickness() const;
  G4double GetCellSize() const;
  
 private:
 
  G4double cell_thickn_;
  G4double cell_size_;

};

inline G4double SiPMLYSOBlue::GetCellThickness() const {return cell_thickn_;}
inline G4double SiPMLYSOBlue::GetCellSize() const {return cell_size_;}


#endif
