#include "ECalCrystal.hh"

#include "ECalParameters.hh"

#include <iostream>
#include "cstdlib"
#include "math.h"

ECalCrystal::ECalCrystal(Int_t ix ,Int_t iy)
{

  ECalParameters* para = ECalParameters::GetInstance();

  Double_t calFrontFaceCenterX = 0.;
  Double_t calFrontFaceCenterY = 0.;
  Double_t calFrontFaceCenterZ = para->GetECalToTarg();

  Int_t calNCryX = para->GetNCryX();
  Int_t calNCryY = para->GetNCryY();

  Double_t cryXSize = para->GetCryXSize();
  Double_t cryYSize = para->GetCryYSize();
  Double_t cryZSize = para->GetCryZSize();

  // Verify if crystal to be created is inside the map
  if ( ix<0 || ix>=calNCryX || iy<0 || iy>=calNCryY ){
    std::cout << "ERROR - Crystal::Crystal - Crystal outside map: " << ix << " " << iy << std::endl;
    exit(1);
  }

  fXi=ix;
  fYi=iy;

  fStatus=0;

  fEnergy=-1;
  fTime=-1;
  fCharge=-1;

  fXCenter = calFrontFaceCenterX+cryXSize*(-0.5*calNCryX+0.5+ix);
  fYCenter = calFrontFaceCenterY+cryYSize*(-0.5*calNCryY+0.5+iy);
  fZCenter = calFrontFaceCenterZ+0.5*cryZSize;

}

ECalCrystal::~ECalCrystal()
{;}

void ECalCrystal::Print()
{
  std::cout << "ECalCrystal " << fXi << " " << fYi << " Status " << fStatus << " Energy " << fEnergy
	    << " Charge " << fCharge << " Time " << fTime << std::endl;
}
