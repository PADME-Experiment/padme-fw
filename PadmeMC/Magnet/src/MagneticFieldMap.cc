#include "MagneticFieldMap.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"

MagneticFieldMap::MagneticFieldMap()
{

  fConstantMagneticField = 0.32*tesla;

  fConstantMagneticFieldZmin = -37.5*cm;
  fConstantMagneticFieldZmax =  37.5*cm;

  fSigmaFront = 22.5*cm;
  fSigmaBack  = 22.5*cm;

}

MagneticFieldMap::~MagneticFieldMap()
{;}

void MagneticFieldMap::GetFieldValue(const G4double p[4],G4double* B) const
{

  // Get local X and Z coordinates
  //G4double x = p[0]; Not needed yet
  G4double z = (p[2]-fMagneticVolumePosZ);

  G4double B0 = 1.;
  if (z<-0.5*fMagneticVolumeLengthZ || z>0.5*fMagneticVolumeLengthZ) {
    B0 = 0.;
  } else if (z<fConstantMagneticFieldZmin) {
    G4double dZ = (z-fConstantMagneticFieldZmin)/cm;
    B0 = exp(-dZ*dZ/(fSigmaFront*fSigmaFront));
  } else if (z>fConstantMagneticFieldZmax) {
    G4double dZ = (z-fConstantMagneticFieldZmax)/cm;
    B0 = exp(-dZ*dZ/(fSigmaFront*fSigmaFront));
  }

  /*
  // Trapezoid approximation. Will be replaced with a real map
  if (z<0) z = -z;
  if ( z > 40. ) {
    B0 -= 0.03*(z-40.);
    if ( B0 < 0. ) B0 = 0.;
  }
  */

  B[0] = -fConstantMagneticField*B0;
  B[1] = 0.;
  B[2] = 0.;

  G4cout << "Magnetic field at " << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << " (z=" << z/cm << " cm) is " << B[0]/tesla << " tesla" << G4endl;

}
