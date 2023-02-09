#include "ECalMLParameters.hh"

#include "TSubDetectorInfo.hh"

#include <iostream>
#include "cstdlib"

ECalMLParameters* ECalMLParameters::fInstance = 0;

ECalMLParameters* ECalMLParameters::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalMLParameters(); }
  return fInstance;
}

ECalMLParameters::ECalMLParameters()
{

  // Initialize parameters to some default values (length is in mm)

  fECalMLNRows = 29;
  fECalMLNCols = 29;

  fCrystalSizeX = 21.;
  fCrystalSizeY = 21.;
  fCrystalSizeZ =230.;
  fCrystalGap = 0.25;

  fECalMLFrontFacePosZ = 2300.;

  fSignalToEnergy = 0.4; // This is 1/fEHtoNPEConversion from MC digitization process

  fIslandEThreshold = 1.; // 1MeV
  fIslandEThresholdSeed = 10.; // 10MeV

  fRadiusEThreshold = 1.; // 1MeV
  fRadiusEThresholdSeed = 10.; // 10MeV
  fRadiusRadius = 50.; // 5cm

}
//
ECalMLParameters::~ECalMLParameters()
{;}

void ECalMLParameters::SetMCDetInfo(TSubDetectorInfo* detInfo)
{
  char par[100],sval[100];
  Double_t gap = 0.;
  Double_t coat = 0.;
  std::vector<TString> subPar = detInfo->GetGeometryParameters();
  for(UInt_t iPar = 0; iPar < subPar.size(); iPar++) {
    sscanf(subPar[iPar].Data(),"%s %s",par,sval);
    //printf("par %s sval %s\n",par,sval);
    if (strcmp(par,"fECalMLNRows")==0) fECalMLNRows = atoi(sval);
    if (strcmp(par,"fECalMLNCols")==0) fECalMLNCols = atoi(sval);
    if (strcmp(par,"fCrystalSizeX")==0) fCrystalSizeX = atof(sval);
    if (strcmp(par,"fCrystalSizeY")==0) fCrystalSizeY = atof(sval);
    if (strcmp(par,"fCrystalSizeZ")==0) fCrystalSizeZ = atof(sval);
    if (strcmp(par,"fECalMLFrontFacePosZ")==0) fECalMLFrontFacePosZ = atof(sval);
    if (strcmp(par,"fCrystalGap")==0) gap = atof(sval);
    if (strcmp(par,"fCrystalCoating")==0) coat = atof(sval);
  }
  fCrystalGap = gap+2.*coat; // Compute the total gap between two crystals
  printf("ECalMLParameters received from MC:\n");
  printf("fECalMLNRows %d fECalMLNCols %d fCrystalSizeX %f fCrystalSizeY %f fCrystalSizeZ %f fCrystalGap %f fECalMLFrontFacePosZ %f\n",
	 fECalMLNRows,fECalMLNCols,fCrystalSizeX,fCrystalSizeY,fCrystalSizeZ,fCrystalGap,fECalMLFrontFacePosZ);
}
