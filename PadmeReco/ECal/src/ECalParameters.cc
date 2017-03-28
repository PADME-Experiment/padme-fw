#include "ECalParameters.hh"
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
//  printf("Calling Init histograms ");
}
//
ECalParameters::~ECalParameters()
{
//  printf("Closing Init histograms ");
}


