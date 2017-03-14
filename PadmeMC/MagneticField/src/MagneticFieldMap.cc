#include "MagneticFieldMap.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

MagneticFieldMap::MagneticFieldMap()
{

  // Default value: can be changed with set method
  fConstantMagneticField = -0.55*tesla;
  //fConstantMagneticField = 0.;

  fConstantMagneticFieldXmin = -26.0*cm;
  fConstantMagneticFieldXmax =  26.0*cm;

  //fConstantMagneticFieldYmin = -26.0*cm;
  //fConstantMagneticFieldYmax =  26.0*cm;

  fConstantMagneticFieldZmin = -37.5*cm;
  fConstantMagneticFieldZmax =  37.5*cm;

  fSigmaFront = 22.5*cm;
  fSigmaBack  = 22.5*cm;

  // The magnetic volume is a box centered at magnet center with x and y dimensions
  // as the magnet cavity and with z extends 50cm outside both sides of the magnet
  fMagneticVolumePosZ = 0.;
  fMagneticVolumeLengthX = 112.*cm;
  fMagneticVolumeLengthY = 23.*cm;
  fMagneticVolumeLengthZ = 200.*cm;

}

MagneticFieldMap::~MagneticFieldMap()
{;}

void MagneticFieldMap::GetFieldValue(const G4double p[4],G4double* B) const
{

  //printf("Magnetic field map called\n");
  //B[0] = 0.;
  //B[1] = 0.;
  //B[2] = 0.;
  //return;

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
  } else if (x<fConstantMagneticFieldXmin || x>fConstantMagneticFieldXmax) {
    // Will need a function/map to smoothly send B0 to 0 along X
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

  // Field lines are directed along Y
  B[0] = 0.;
  B[1] = -fConstantMagneticField*B0;
  B[2] = 0.;

  // Used for debug: comment out to reduce output
  //printf("Magnetic field at (%7.2f,%7.2f,%7.2f) cm is (%7.4f,%7.4f,%7.4f) tesla\n",x/cm,y/cm,z/cm,B[0]/tesla,B[1]/tesla,B[2]/tesla);

}
