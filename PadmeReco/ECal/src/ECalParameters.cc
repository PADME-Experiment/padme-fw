#include "ECalParameters.hh"

#include "TSubDetectorInfo.hh"

#include <TH1D.h>
#include <TH2D.h>
#include <TGraph.h>
#include <TList.h>
#include <iostream>
#include "cstdlib"

ECalParameters* ECalParameters::fInstance = 0;

ECalParameters* ECalParameters::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalParameters(); }
  return fInstance;
}

void ECalParameters::InitGeomParam()
{
  FILE* parfile;
  char line[200];
  //  printf("Calling parameter init\n");
  //  line    = (char *) malloc(100*sizeof(char));
  parfile = fopen("geom.txt","r");
  if (parfile==NULL){
    printf("Cannot open param file\n");
    exit(-1);
  }
  printf("ECalParameters file succesfully opened\n");
  fgets(line,100,parfile);
  if(line[0]=='#') fgets(line,100,parfile);
  sscanf(line,"%lf",&ECalToTarg);

  fgets(line,100,parfile);
  if(line[0]=='#') fgets(line,100,parfile);
  sscanf(line,"%lf %lf %lf",&a,&b,&c);

  fgets(line,100,parfile);
  if(line[0]=='#') fgets(line,100,parfile);
  sscanf(line,"%d %d",&NCryX,&NCryY);

  fgets(line,100,parfile);
  while(line[0]=='#') fgets(line,100,parfile);
  sscanf(line,"%lf %lf %lf",&TargetZ,&ECALZ,&SACZ); // in cm;
  fclose(parfile);
}

void ECalParameters::WriteGeomParam()
{
  printf("Calo to Targ %f\n",ECalToTarg);
  printf("ECal Crystal size %f %f %f\n",a,b,c);
  printf("N Calo crystals lines  X=%d Y=%d \n",NCryX,NCryY);
  printf("Target Z= %f EcalZ= %f SACZ= %f\n",TargetZ,ECALZ,SACZ);
}

ECalParameters::ECalParameters()
{

  // Initialize parameters to some default values (length is in mm)
  fECalNRows = 29;
  fECalNCols = 29;
  fECalFrontFacePosZ = 3000.; // Relative to the front face of the target
  fCrystalSizeX = 21.;
  fCrystalSizeY = 21.;
  fCrystalSizeZ =230.;
  fCrystalGap = 0.25;

}
//
ECalParameters::~ECalParameters()
{
//  printf("Closing Init histograms ");
}

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
    if (strcmp(par,"fCrystalGap")==0) gap = atof(sval);
    if (strcmp(par,"fCrystalCoating")==0) coat = atof(sval);
  }
  fCrystalGap = gap+2.*coat; // Compute the total gap between two crystals
  printf("ECalParameters received from MC:\n");
  printf("fECalNRows %d fECalNCols %d fCrystalSizeX %f fCrystalSizeY %f fCrystalSizeZ %f fCrystalGap %f\n",
	 fECalNRows,fECalNCols,fCrystalSizeX,fCrystalSizeY,fCrystalSizeZ,fCrystalGap);
}
