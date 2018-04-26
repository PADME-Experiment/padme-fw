#include "Configuration.hh"

Configuration* Configuration::fInstance = 0;

Configuration::Configuration()
{
  fRunNumber = 0;
  fMergerId = -1;
  fStreamListFile = "";
  fOutputFileHeader = "rawdata/rawdata";
  fNEventsPerFile = 10000;
  fVerbose = 0;
}

Configuration::~Configuration()
{;}

Configuration* Configuration::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new Configuration(); }
  return fInstance;
}
