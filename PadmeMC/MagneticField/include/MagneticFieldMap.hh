#ifndef MagneticFieldMap_H
#define MagneticFieldMap_H

#define MAGVALUEZ_SIZE 191
#define MAGVALUEX_SIZE  81

#include "G4MagneticField.hh"

class MagneticFieldMap : public G4MagneticField
{

public:

  MagneticFieldMap();
  ~MagneticFieldMap();

  void GetFieldValue(const G4double p[4],G4double* B) const;

  void SetConstantMagneticFieldValue(G4double v) { fConstantMagneticField = v; }
  G4double GetConstantMagneticFieldValue() { return fConstantMagneticField; }

  //void SetMagneticVolumePosZ(G4double z)    { fMagneticVolumePosZ    = z; }

  //void SetMagneticVolumeLengthX(G4double x) { fMagneticVolumeLengthX = x; }
  //void SetMagneticVolumeLengthY(G4double y) { fMagneticVolumeLengthY = y; }
  //void SetMagneticVolumeLengthZ(G4double z) { fMagneticVolumeLengthZ = z; }

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

  // Sigma of gaussian in the front and back sections of the magnetic field along Z
  G4double fSigmaFront;
  G4double fSigmaBack;

  // Coefficients for grade 4 polynomial describing field decrease along X
  // P range: 0mm to 260mm - M range: -260mm to 0mm
  G4double fXPol4P[5];
  G4double fXPol4M[5];

  // Coefficients for gaussian describing field decrease along X
  // P range: 260mm to 400mm - M range: -400mm to -260mm
  G4double fXGausP[3];
  G4double fXGausM[3];

  // Table of magnetic field values along Z
  G4double fMagValueZMin;
  G4double fMagValueZMax;
  G4double fMagValueZStep;
  G4double fMagValueZ[MAGVALUEZ_SIZE];
  // Parameters to estrapolate tails with a gaussian
  G4double fMagValueZTailMConst;
  G4double fMagValueZTailMMean;
  G4double fMagValueZTailMSigma;
  G4double fMagValueZTailPConst;
  G4double fMagValueZTailPMean;
  G4double fMagValueZTailPSigma;

  // Table of magnetic field values along X
  G4double fMagValueXMin;
  G4double fMagValueXMax;
  G4double fMagValueXStep;
  G4double fMagValueX[MAGVALUEX_SIZE];
  // Parameters to estrapolate tails with a gaussian
  G4double fMagValueXTailMConst;
  G4double fMagValueXTailMMean;
  G4double fMagValueXTailMSigma;
  G4double fMagValueXTailPConst;
  G4double fMagValueXTailPMean;
  G4double fMagValueXTailPSigma;

  // Global position along Z of center of magnetic volume (assumes that center is always at X=0,Y=0)
  G4double fMagneticVolumePosZ;

  // Dimensions of magnetic volume
  //G4double fMagneticVolumeLengthX;
  //G4double fMagneticVolumeLengthY;
  //G4double fMagneticVolumeLengthZ;
  G4double fMagneticVolumeXmin;
  G4double fMagneticVolumeXmax;
  G4double fMagneticVolumeYmin;
  G4double fMagneticVolumeYmax;
  G4double fMagneticVolumeZmin;
  G4double fMagneticVolumeZmax;

};
#endif
