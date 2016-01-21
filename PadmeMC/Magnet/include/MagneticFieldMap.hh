#ifndef MagneticFieldMap_H
#define MagneticFieldMap_H

#include "G4MagneticField.hh"

class MagneticFieldMap : public G4MagneticField
{

public:

  MagneticFieldMap();
  ~MagneticFieldMap();

  void GetFieldValue(const G4double p[4],G4double* B) const;

  void     SetConstantMagneticFieldValue(G4double v) { fConstantMagneticField = v; }
  G4double GetConstantMagneticFieldValue()           { return fConstantMagneticField; }

  void SetMagneticVolumePosZ(G4double z)    { fMagneticVolumePosZ    = z; }
  void SetMagneticVolumeLengthZ(G4double z) { fMagneticVolumeLengthZ = z; }

private:

  // Value of magnetic field in the constant field zone
  G4double fConstantMagneticField;

  // Global position along Z of center of magnetic volume (assumes that center is at X=0,Y=0)
  G4double fMagneticVolumePosZ;

  // Length of magnetic volume along Z
  G4double fMagneticVolumeLengthZ;

};
#endif
