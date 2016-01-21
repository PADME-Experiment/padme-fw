#include "MagneticFieldMap.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"

MagneticFieldMap::MagneticFieldMap()
{
  fConstantMagneticField = 0.55*tesla;
}

MagneticFieldMap::~MagneticFieldMap()
{;}

void MagneticFieldMap::GetFieldValue(const G4double p[4],G4double* B) const
{

  G4double z,B0;

  // Compute local Z coordinate in cm (absolute value as field is symmetric)
  z = (p[2]-fMagneticVolumePosZ)/cm;
  if (z<0) z = -z;

  // Trapezoid approximation. Will be replaced with a real map
  B0 = 1.;
  if ( z > 40. ) {
    B0 -= 0.03*(z-40.);
    if ( B0 < 0. ) B0 = 0.;
  }

  B[0] = -fConstantMagneticField*B0;
  B[1] = 0.;
  B[2] = 0.;

  G4cout << "Magnetic field at " << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << " (z=" << z << " cm) is " << B[0]/tesla << " tesla" << G4endl;

}
