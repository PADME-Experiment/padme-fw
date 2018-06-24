#include "Configuration.hh"

Configuration* Configuration::fInstance = 0;

Configuration::Configuration()
{
  // Set default configuration parameters
  fRunNumber = 0;
  fMergerId = -1;
  fInputStream = "";
  fInputStreamList = "";
  fOutputStreamList = "";
  fRawFileHeader = "rawdata/rawdata";
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
