// ----------------------------------------------------------------------------
// petalosim | TileHamamatsuBlue.h
//
// Hamamatsu Blue sensor tile geometry.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef TILE_HAMAMATSU_BLUE_H
#define TILE_HAMAMATSU_BLUE_H

#include "TileGeometryBase.h"

#include <G4ThreeVector.hh>

class G4GenericMessenger;
class SiPMHamamatsuBlue;

using namespace nexus;

class TileHamamatsuBlue : public TileGeometryBase
{
public:
  /// Constructor
  TileHamamatsuBlue();
  /// Destructor
  ~TileHamamatsuBlue();

  /// Return dimensions of the SiPM
  //G4ThreeVector GetDimensions() const;

  /// Invoke this method to build the volumes of the geometry
  void Construct();

private:
  // Tile dimensions
  G4double tile_x_, tile_y_, tile_z_;
  G4double epoxy_depth_;

  // SiPM pitch
  G4double sipm_pitch_;

  // Optical properties to be used for epoxy
  G4double refr_index_;

  // Rows and columns of SiPMs
  G4int n_rows_, n_columns_;

  SiPMHamamatsuBlue *sipm_;
};

#endif
