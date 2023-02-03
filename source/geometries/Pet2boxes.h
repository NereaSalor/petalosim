// ----------------------------------------------------------------------------
// petalosim | Pet2boxes.h
//
// This class consists of a steel box filled with LXe.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef PET2BOXES_H
#define PET2BOXES_H

#include "nexus/GeometryBase.h"

class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;
class PetKDBFixedPitch;
class PetPlainDice;
namespace nexus {class BoxPointSampler;}

class Pet2boxes : public nexus::GeometryBase
{
public:
  /// Constructor
  Pet2boxes();

  /// Destructor
  ~Pet2boxes();

  /// Generate a vertex within a given region of the geometry
  G4ThreeVector GenerateVertex(const G4String &region) const;

  void Construct();

private:
  void BuildDetector();
  void BuildLXe();
  void BuildActive();
  void BuildSiPMPlane();

  G4LogicalVolume *lab_logic_;
  G4LogicalVolume *det_logic_;
  G4LogicalVolume *lXe_logic_;

  // Detector dimensions
  //   const G4double vacuum_thickn_; // thickness of vaccuum layer between walls
  //  const G4double outer_wall_thickn_; // Thickness of the outer wall
  const G4double det_thickness_; // Thickness of the walls of the detector
  //   const G4double det_size_; /// Size of the detector
  const G4double active_size_; /// Size of the LXe active volume

  // ACTIVE gas Xenon
  G4Material *lXe_;

  // Parameters
  G4double max_step_size_; /// Maximum Step Size

  //Vertex generator
  nexus::BoxPointSampler *active_gen_;

  /// Messenger for the definition of control commands
  G4GenericMessenger *msg_;

  /// Vertex generator from a surface just outside the detector walls
  nexus::BoxPointSampler *surf_gen_;

  // Dice class
  PetKDBFixedPitch *db_;
  PetPlainDice *pdb_;

  G4double db_z_;

  G4double z_size_;
};

#endif
