#include "TargetParameters.hh"

#include "TSubDetectorInfo.hh"

#include <iostream>
#include "cstdlib"

TargetParameters* TargetParameters::fInstance = 0;

TargetParameters* TargetParameters::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TargetParameters(); }
  return fInstance;
}

TargetParameters::TargetParameters()
{

  // Initialize parameters to some default values (length is in mm)

  // MC Geometry-Run parameters
  fTargetSizeX =   20.;
  fTargetSizeY =   20.;
  fTargetSizeZ = 0.1;  
  fTargetFrontFacePosZ = -700.; // Relative to center of magnet
  
  fTargetFastDigitization = true; // Use fast digitization
  fTargetSaveWaveformToDigi = false; // Do not save waveforms to digi

  // Digi-Reco parameters
  fTargetEfficiency = false;

  fPitch = 1.;// unit mm
  fSigmaQi = 1.e-9;// unit C

  fTimeErr = 0.;// unit ns



}
//
TargetParameters::~TargetParameters()
{;}

void TargetParameters::SetMCDetInfo(TSubDetectorInfo* detInfo)
{
  char par[100],sval[100];
 
  std::vector<TString> subPar = detInfo->GetGeometryParameters();
  for(UInt_t iPar = 0; iPar < subPar.size(); iPar++) {
    sscanf(subPar[iPar].Data(),"%s %s",par,sval);
    //printf("par %s sval %s\n",par,sval);
    if (strcmp(par,"fTargetSizeX")==0)  fTargetSizeX= atof(sval);
    if (strcmp(par,"fTargetSizeY")==0) fTargetSizeY = atof(sval);
    if (strcmp(par,"fTargetSizeZ")==0) fTargetSizeZ = atof(sval);
    if (strcmp(par,"fTargetFrontFacePosZ")==0)  fTargetFrontFacePosZ = atof(sval);
    if (strcmp(par,"fTargetFastDigitization")==0)  fTargetFastDigitization = atoi(sval);
    if (strcmp(par,"fTargetSaveWaveformToDigi")==0)  fTargetSaveWaveformToDigi = atoi(sval);
  }

  printf("TargetParameters received from MC:\n");
  printf("fTargetSizeX %f fTargetSizeY %f fTargetSizeZ %f fTargetFrontFacePosZ %f fTargetFastDigitization %d fTargetSaveWaveformToDigi %d \n",
	 fTargetSizeX,fTargetSizeY,fTargetSizeZ,fTargetFrontFacePosZ,fTargetFastDigitization,fTargetSaveWaveformToDigi);
}
