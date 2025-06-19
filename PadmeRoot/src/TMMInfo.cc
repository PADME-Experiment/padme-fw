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
}

TMMInfo::~TMMInfo()
{;}
