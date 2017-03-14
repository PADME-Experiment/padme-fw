#ifndef MagneticFieldMap_H
#define MagneticFieldMap_H

#include "G4MagneticField.hh"

class MagneticFieldMap : public G4MagneticField
{

public:

  MagneticFieldMap();
  ~MagneticFieldMap();

  void GetFieldValue(const G4double p[4],G4double* B) const;

  void SetConstantMagneticFieldValue(G4double v) { fConstantMagneticField = v; }
  G4double GetConstantMagneticFieldValue() { return fConstantMagneticField; }

  void SetMagneticVolumePosZ(G4double z)    { fMagneticVolumePosZ    = z; }

  void SetMagneticVolumeLengthX(G4double x) { fMagneticVolumeLengthX = x; }
  void SetMagneticVolumeLengthY(G4double y) { fMagneticVolumeLengthY = y; }
  void SetMagneticVolumeLengthZ(G4double z) { fMagneticVolumeLengthZ = z; }

private:

  // Value of magnetic field in the constant field zone
  G4double fConstantMagneticField;

  // Constant magnetic field limits in X
  G4double fConstantMagneticFieldXmin;
  G4double fConstantMagneticFieldXmax;

  // Constant magnetic field limits in Y
  //G4double fConstantMagneticFieldYmin;
  //G4double fConstantMagneticFieldYmax;

  // Constant magnetic field limits in Z
  G4double fConstantMagneticFieldZmin;
  G4double fConstantMagneticFieldZmax;

  // Sigma of gaussian in the front and back sections of the magnetic field
  G4double fSigmaFront;
  G4double fSigmaBack;

  // Global position along Z of center of magnetic volume (assumes that center is always at X=0,Y=0)
  G4double fMagneticVolumePosZ;

  // Dimensions of magnetic volume
  G4double fMagneticVolumeLengthX;
  G4double fMagneticVolumeLengthY;
  G4double fMagneticVolumeLengthZ;

};
#endif
