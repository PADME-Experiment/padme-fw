#include "TMMInfo.hh"

ClassImp(TMMInfo)

TMMInfo::TMMInfo()
{
  fDaqTimeSec = 0;
  fDaqTimeMicroSec = 0;
  fSrsTimeStamp = 0;
  fSrsTrigger = 0;
  fSrsRunTime = 0; // roll over corrected
  fRunTimeDiff = 0; // roll over corrected
  for (int i=0; i<2; i++){
    for (int j=0; j<16; j++){
      fNumberOfProblematicChannels[i][j] = 0; // total number of problematic channels
      fNumberOfFiredChannels[i][j] = 0; // total number of problematic channels
    }
  }
}

TMMInfo::~TMMInfo()
{;}


void TMMInfo::Clear(Option_t* option){
  fDaqTimeSec = 0;
  fDaqTimeMicroSec = 0;
  fSrsTimeStamp = 0;
  fSrsTrigger = 0;
  fSrsRunTime = 0; // roll over corrected
  fRunTimeDiff = 0; // roll over corrected
  for (int i=0; i<2; i++){
    for (int j=0; j<16; j++){
      fNumberOfProblematicChannels[i][j] = 0; // total number of problematic channels
      fNumberOfFiredChannels[i][j] = 0; // total number of problematic channels
    }
  }
  
}
