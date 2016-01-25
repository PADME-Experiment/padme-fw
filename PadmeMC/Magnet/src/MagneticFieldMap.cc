#include "MagneticFieldMap.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

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

  // Get local coordinates (x,y are centered at 0,0 by definition)
  G4double x = p[0];
  G4double y = p[1];
  G4double z = p[2]-fMagneticVolumePosZ;

  G4double B0 = 1.;
  if ( (x<-0.5*fMagneticVolumeLengthX) || (x>0.5*fMagneticVolumeLengthX) ||
       (y<-0.5*fMagneticVolumeLengthY) || (y>0.5*fMagneticVolumeLengthY) ||
       (z<-0.5*fMagneticVolumeLengthZ) || (z>0.5*fMagneticVolumeLengthZ) ) {
    // Field outside magnetic volume is always null
    B0 = 0.;
  } else if (z<fConstantMagneticFieldZmin) {
    // Use gaussian to model upstream magnetic field rise
    G4double dZS = (z-fConstantMagneticFieldZmin)/fSigmaFront;
    B0 = exp(-dZS*dZS);
  } else if (z>fConstantMagneticFieldZmax) {
    // Use gaussian to model downstream magnetic field fall
    G4double dZS = (z-fConstantMagneticFieldZmax)/fSigmaBack;
    B0 = exp(-dZS*dZS);
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

  printf("Magnetic field at (%7.2f,%7.2f,%7.2f) cm is %7.4f tesla\n",x/cm,y/cm,z/cm,B[0]/tesla);
  //G4cout << "Magnetic field at ( " << G4BestUnit(x,"Length") << " , " << G4BestUnit(y,"Length") << " , " << G4BestUnit(z,"Length") << " ) is " << B[0]/tesla << " tesla" << G4endl;
  //G4cout << "Magnetic field at ( " << x/cm << " , " << y/cm << " , " << z/cm << " ) is " << B[0]/tesla << " tesla" << G4endl;

}
