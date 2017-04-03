#include "ECalParameters.hh"

#include "TSubDetectorInfo.hh"

#include <iostream>
#include "cstdlib"

ECalParameters* ECalParameters::fInstance = 0;

ECalParameters* ECalParameters::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalParameters(); }
  return fInstance;
}

ECalParameters::ECalParameters()
{

  // Initialize parameters to some default values (length is in mm)

  fECalNRows = 29;
  fECalNCols = 29;

  fCrystalSizeX = 21.;
  fCrystalSizeY = 21.;
  fCrystalSizeZ =230.;
  fCrystalGap = 0.25;

  fECalFrontFacePosZ = 2300.;

  fSignalToEnergy = 0.4; // This is 1/fEHtoNPEConversion from MC digitization process

  fIslandEThreshold = 1.; // 1MeV
  fIslandEThresholdSeed = 10.; // 10MeV

  fRadiusEThreshold = 1.; // 1MeV
  fRadiusEThresholdSeed = 10.; // 10MeV
  fRadiusRadius = 50.; // 5cm

}
//
ECalParameters::~ECalParameters()
{;}

void ECalParameters::SetMCDetInfo(TSubDetectorInfo* detInfo)
{
  char par[100],sval[100];
  Double_t gap = 0.;
  Double_t coat = 0.;
  std::vector<TString> subPar = detInfo->GetGeometryParameters();
  for(UInt_t iPar = 0; iPar < subPar.size(); iPar++) {
    sscanf(subPar[iPar].Data(),"%s %s",par,sval);
    //printf("par %s sval %s\n",par,sval);
    if (strcmp(par,"fECalNRows")==0) fECalNRows = atoi(sval);
    if (strcmp(par,"fECalNCols")==0) fECalNCols = atoi(sval);
    if (strcmp(par,"fCrystalSizeX")==0) fCrystalSizeX = atof(sval);
    if (strcmp(par,"fCrystalSizeY")==0) fCrystalSizeY = atof(sval);
    if (strcmp(par,"fCrystalSizeZ")==0) fCrystalSizeZ = atof(sval);
    if (strcmp(par,"fECalFrontFacePosZ")==0) fECalFrontFacePosZ = atof(sval);
    if (strcmp(par,"fCrystalGap")==0) gap = atof(sval);
    if (strcmp(par,"fCrystalCoating")==0) coat = atof(sval);
  }
  fCrystalGap = gap+2.*coat; // Compute the total gap between two crystals
  printf("ECalParameters received from MC:\n");
  printf("fECalNRows %d fECalNCols %d fCrystalSizeX %f fCrystalSizeY %f fCrystalSizeZ %f fCrystalGap %f fECalFrontFacePosZ %f\n",
	 fECalNRows,fECalNCols,fCrystalSizeX,fCrystalSizeY,fCrystalSizeZ,fCrystalGap,fECalFrontFacePosZ);
}
