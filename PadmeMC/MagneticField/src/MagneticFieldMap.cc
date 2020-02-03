#include "MagneticFieldMap.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

MagneticFieldMap::MagneticFieldMap()
{

  // Default value: can be changed with set method
  //  fConstantMagneticField = 0.55*tesla;
  fConstantMagneticField = 0.4542*tesla; // new default at 232A 07/03/2019 MR EL  B(gauss) = 19.44 I(A) + 32.8
  //fConstantMagneticField = 0.;

  // Define the X value where we change from grade 4 polynomial to gaussian fits along X
  fConstantMagneticFieldXmin = -260.*mm;
  fConstantMagneticFieldXmax =  260.*mm;

  //fConstantMagneticFieldYmin = -26.0*cm;
  //fConstantMagneticFieldYmax =  26.0*cm;

  fConstantMagneticFieldZmin = -37.5*cm;
  fConstantMagneticFieldZmax =  37.5*cm;

//  fSigmaFront = 22.5*cm;
//  fSigmaBack  = 22.5*cm;

  fSigmaFront = 27.4*cm;   //Based of the LNF magnetic measurement M. Raggi .ppt nov 2018
  fSigmaBack  = 27.4*cm;   //Based of the LNF magnetic measurement M. Raggi .ppt nov 2018

  // Grade 4 polynomial coefficients for field tails along X
  // M coefficients are valid in the -260mm to 0mm range
  // P coefficients are valid in the 0mm to 260mm range
  // Fitted from LNF measurements at Z=0 and Y=0 (variations for Y!=0 not taken into account)
  fXPol4M[0] = 1.;
  fXPol4M[1] = -8.61E-5/mm;
  fXPol4M[2] = -2.73E-5/(mm*mm);
  fXPol4M[3] = -2.63E-8/(mm*mm*mm);
  fXPol4M[4] = -1.04E-10/(mm*mm*mm*mm);
  fXPol4P[0] = 1.;
  fXPol4P[1] = 6.18E-5/mm;
  fXPol4P[2] = -2.26E-6/(mm*mm);
  fXPol4P[3] = 2.33E-8/(mm*mm*mm);
  fXPol4P[4] = -0.99E-10/(mm*mm*mm*mm);

  // Gaussian coefficients for field tails along X
  // M coefficients are valid in the -400mm to -260mm range
  // P coefficients are valid in the 260mm to 400mm range
  // Fitted from LNF measurements at Z=0 and Y=0 (variations for Y!=0 not taken into account)
  fXGausM[0] =    1.1;
  fXGausM[1] = -126.8*mm;
  fXGausM[2] =  177.3*mm;
  fXGausP[0] =    1.1;
  fXGausP[1] =  123.4*mm;
  fXGausP[2] =  177.8*mm;

  // The magnetic volume is a box centered at magnet center with x and y dimensions
  // as the magnet cavity and with z extends 50cm outside both sides of the magnet
  fMagneticVolumePosZ = 0.;
  //fMagneticVolumeLengthX = 112.*cm;
  //fMagneticVolumeLengthY = 23.*cm;
  //fMagneticVolumeLengthZ = 200.*cm;
  fMagneticVolumeXmin =  -56.0*cm;
  fMagneticVolumeXmax =   56.0*cm;
  fMagneticVolumeYmin =  -11.5*cm;
  fMagneticVolumeYmax =   11.5*cm;
  fMagneticVolumeZmin = -125.0*cm;
  fMagneticVolumeZmax =  100.0*cm;

  // Relative value of By along Z with X=Y=0
  // 191 values from -950mm to 950mm with step of 10mm
  fMagValueZMin  = -950.*mm;
  fMagValueZMax  =  950.*mm;
  fMagValueZStep =   10.*mm;
  G4double magvalz[MAGVALUEZ_SIZE] = {
    0.0238,0.0262,0.0288,0.0318,0.0350,0.0386,0.0426,0.0471,0.0520,0.0575,
    0.0634,0.0701,0.0773,0.0852,0.0938,0.1031,0.1132,0.1240,0.1357,0.1481,
    0.1614,0.1755,0.1905,0.2064,0.2232,0.2410,0.2597,0.2795,0.3003,0.3222,
    0.3454,0.3698,0.3956,0.4227,0.4513,0.4815,0.5131,0.5462,0.5806,0.6161,
    0.6523,0.6889,0.7252,0.7607,0.7946,0.8263,0.8552,0.8809,0.9033,0.9224,
    0.9383,0.9514,0.9619,0.9704,0.9771,0.9823,0.9863,0.9895,0.9920,0.9938,
    0.9953,0.9964,0.9972,0.9979,0.9984,0.9987,0.9990,0.9993,0.9994,0.9996,
    0.9997,0.9997,0.9998,0.9999,0.9999,0.9999,0.9999,0.9999,1.0000,1.0000,
    1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,
    1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,0.9998,0.9998,0.9998,0.9998,
    0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,
    0.9997,0.9997,0.9997,0.9997,0.9996,0.9996,0.9996,0.9996,0.9995,0.9994,
    0.9994,0.9993,0.9991,0.9990,0.9987,0.9985,0.9981,0.9976,0.9970,0.9962,
    0.9951,0.9937,0.9919,0.9895,0.9865,0.9825,0.9774,0.9709,0.9627,0.9525,
    0.9398,0.9243,0.9058,0.8841,0.8592,0.8311,0.8005,0.7677,0.7334,0.6984,
    0.6632,0.6283,0.5944,0.5614,0.5299,0.4996,0.4710,0.4436,0.4177,0.3930,
    0.3696,0.3473,0.3260,0.3057,0.2863,0.2678,0.2500,0.2331,0.2169,0.2015,
    0.1867,0.1727,0.1593,0.1466,0.1347,0.1234,0.1127,0.1028,0.0935,0.0848,
    0.0769,0.0695,0.0628,0.0566,0.0510,0.0459,0.0413,0.0371,0.0334,0.0300,
    0.0270};
  for (int i=0;i<MAGVALUEZ_SIZE;i++) fMagValueZ[i] = magvalz[i];
  fMagValueZTailMConst =    1.649;
  fMagValueZTailMMean  = -197.298*mm;
  fMagValueZTailMSigma =  256.181*mm;
  fMagValueZTailPConst =    1.637;
  fMagValueZTailPMean  = -182.432*mm;
  fMagValueZTailPSigma =  271.259*mm;
 
  // Relative value of By along X with Z=Y=0
  // 81 values from -400mm to 400mm with step of 10mm
  fMagValueXMin  = -400.*mm;
  fMagValueXMax  =  400.*mm;
  fMagValueXStep =   10.*mm;
  G4double magvalx[MAGVALUEX_SIZE] = {
    0.3383,0.3672,0.3975,0.4293,0.4624,0.4970,0.5330,0.5702,0.6082,0.6469,
    0.6854,0.7235,0.7602,0.7951,0.8274,0.8567,0.8826,0.9050,0.9240,0.9398,
    0.9527,0.9632,0.9715,0.9780,0.9832,0.9872,0.9903,0.9927,0.9945,0.9959,
    0.9970,0.9978,0.9984,0.9989,0.9993,0.9995,0.9997,0.9999,0.9999,1.0000,
    1.0000,1.0000,0.9999,0.9998,0.9997,0.9994,0.9992,0.9988,0.9983,0.9976,
    0.9968,0.9957,0.9942,0.9922,0.9897,0.9865,0.9823,0.9769,0.9700,0.9613,
    0.9504,0.9369,0.9204,0.9008,0.8776,0.8510,0.8211,0.7882,0.7528,0.7156,
    0.6774,0.6386,0.6000,0.5620,0.5249,0.4891,0.4547,0.4218,0.3903,0.3603,
    0.3316};
  for (int i=0;i<MAGVALUEX_SIZE;i++) fMagValueX[i] = magvalx[i];
  fMagValueXTailMConst =    1.256;
  fMagValueXTailMMean  =  -88.228*mm;
  fMagValueXTailMSigma =  192.396*mm;
  fMagValueXTailPConst =    1.353;
  fMagValueXTailPMean  =   67.340*mm;
  fMagValueXTailPSigma =  198.199*mm;

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
  //if ( (x<-0.5*fMagneticVolumeLengthX) || (x>0.5*fMagneticVolumeLengthX) ||
  //     (y<-0.5*fMagneticVolumeLengthY) || (y>0.5*fMagneticVolumeLengthY) ||
  //     (z<-0.5*fMagneticVolumeLengthZ) || (z>0.5*fMagneticVolumeLengthZ) ) {
  if ( (x<fMagneticVolumeXmin) || (x>fMagneticVolumeXmax) ||
       (y<fMagneticVolumeYmin) || (y>fMagneticVolumeYmax) ||
       (z<fMagneticVolumeZmin) || (z>fMagneticVolumeZmax) ) {
    // Field outside magnetic volume is always null
    B0 = 0.;
  //} else if (x<fConstantMagneticFieldXmin || x>fConstantMagneticFieldXmax) {
  //  // Will need a function/map to smoothly send B0 to 0 along X
  //  B0 = 0.;
  } else {

    // Apply correction for Z displacement
    G4double zcor = 1.;
    //if (z<fConstantMagneticFieldZmin) {
    //  // Use gaussian to model upstream magnetic field rise
    //  G4double dZS = (z-fConstantMagneticFieldZmin)/fSigmaFront;
    //  zcor = exp(-dZS*dZS);
    //} else if (z>fConstantMagneticFieldZmax) {
    //  // Use gaussian to model downstream magnetic field fall
    //  G4double dZS = (z-fConstantMagneticFieldZmax)/fSigmaBack;
    //  zcor = exp(-dZS*dZS);
    //}
    // Use value table and linear interpolation. Tails are extrapolated with a gaussian.
    if (z < fMagValueZMin) {
      G4double arg = (z-fMagValueZTailMMean)/fMagValueZTailMSigma;
      zcor = fMagValueZTailMConst*exp(-0.5*arg*arg);
    } else if (z >= fMagValueZMax) {
      G4double arg = (z-fMagValueZTailPMean)/fMagValueZTailPSigma;
      zcor = fMagValueZTailPConst*exp(-0.5*arg*arg);
    } else {
      G4double dist = (z-fMagValueZMin)/fMagValueZStep;
      G4int bin = (int)dist;
      zcor = fMagValueZ[bin]+(dist-1.*bin)*(fMagValueZ[bin+1]-fMagValueZ[bin]);
    }
    //printf("MagneticField - z: %7.2f mm - correction: %7.4f\n",z/mm,B0);

    // Apply correction for X displacement
    G4double xcor = 1.;
    //if (x<fConstantMagneticFieldXmin) {
    //  G4double dXS = (x-fXGausM[1])/fXGausM[2];
    //  xcor = fXGausM[0]*exp(-dXS*dXS);
    //} else if (x<0.) {
    //  xcor = fXPol4M[0]+fXPol4M[1]*x+fXPol4M[2]*x*x+fXPol4M[3]*x*x*x+fXPol4M[4]*x*x*x*x;
    //} else if (x<fConstantMagneticFieldXmax) {
    //  xcor = fXPol4P[0]+fXPol4P[1]*x+fXPol4P[2]*x*x+fXPol4P[3]*x*x*x+fXPol4P[4]*x*x*x*x;
    //} else {
    //  G4double dXS = (x-fXGausP[1])/fXGausP[2];
    //  xcor = fXGausP[0]*exp(-dXS*dXS);
    //}
    // Use value table and linear interpolation. Tails are extrapolated with a gaussian.
    if (x < fMagValueXMin) {
      G4double arg = (x-fMagValueXTailMMean)/fMagValueXTailMSigma;
      xcor = fMagValueXTailMConst*exp(-0.5*arg*arg);
    } else if (x >= fMagValueXMax) {
      G4double arg = (x-fMagValueXTailPMean)/fMagValueXTailPSigma;
      xcor = fMagValueXTailPConst*exp(-0.5*arg*arg);
    } else {
      G4double dist = (x-fMagValueXMin)/fMagValueXStep;
      G4int bin = (int)dist;
      xcor = fMagValueX[bin]+(dist-1.*bin)*(fMagValueX[bin+1]-fMagValueX[bin]);
    }
    //printf("MagneticField - x: %7.2f mm - correction: %7.4f\n",x/mm,xcor);

    // Final relative value of magnetic field taking into account X and Z corrections
    B0 = B0*zcor*xcor;
    printf("MagneticField - x: %7.2f mm %7.4f - z: %7.2f mm %7.4f - relative field value: %7.4f\n",x/mm,xcor,z/mm,zcor,B0);

  }

  // Field lines are directed along Y
  B[0] = 0.;
  B[1] = -fConstantMagneticField*B0;
  B[2] = 0.;

  // Used for debug: comment out to reduce output
  //printf("Magnetic field at (%7.2f,%7.2f,%7.2f) cm is (%7.4f,%7.4f,%7.4f) tesla\n",x/cm,y/cm,z/cm,B[0]/tesla,B[1]/tesla,B[2]/tesla);

}
