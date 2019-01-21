#include <stdio.h>
#include "Riostream.h"
#include "ReadConf.hh"


using namespace std;
ReadConf* ReadConf::fInstance = 0;
ReadConf* ReadConf::GetInstance()
{
  //  ntupl=0;
  if ( fInstance == 0 ) { fInstance = new ReadConf(); }
  return fInstance;
}

ReadConf::ReadConf() 
{
  fTargetisON =true;
  fEVetoisON  =true;  
  fPVetoisON  =true;
  fHEPVetoisON=true;
  fECalisON   =true;
  fSACisON    =true;
  fTPixisON   =true;
  
  fUseMonitorMode = false;
  fUseFullMode    = true;

}
ReadConf::~ReadConf()
{;}

void ReadConf::ReadConfig(){
  TString ConfFileName = "config/PadmeReconstruction.conf";
  
  std::ifstream confFile(ConfFileName.Data());
  if (!confFile.is_open()) {
    perror(ConfFileName);
    std::cout<<"cannot open config file : "<<ConfFileName.Data()<<std::endl;
    exit(1);
  }
  //  
  TString Line;
  //  Bool_t  fECalisON;
  while (Line.ReadLine(confFile)) {
    if(Line.BeginsWith("#")) continue;
    if(Line.Contains("ECALReco")    && Line.Contains("false"))    fECalisON=false;
    if(Line.Contains("TargetReco")  && Line.Contains("false"))  fTargetisON=false;
    if(Line.Contains("SACReco")     && Line.Contains("false"))     fSACisON=false;
    if(Line.Contains("PVetoReco")   && Line.Contains("false"))   fPVetoisON=false;
    if(Line.Contains("EVetoReco")   && Line.Contains("false"))   fEVetoisON=false;
    if(Line.Contains("HEPVetoReco") && Line.Contains("false")) fHEPVetoisON=false;    
    if(Line.Contains("TPixReco")    && Line.Contains("false"))    fTPixisON=false;   
    if(Line.Contains("UseMonitorMode") && Line.Contains("false")) fUseMonitorMode=false;
    if(Line.Contains("UseFullMode")    && Line.Contains("false")) fUseFullMode=false;
    std::cout<<Line.Data()<<std::endl;
  }
  confFile.close();  
}
