#include "Configuration.hh"

Configuration* Configuration::fInstance = 0;

Configuration::Configuration()
{
  // Set default configuration parameters
  fRunNumber = 0;
  fMergerId = -1;
  fInputStreamList = "input_streams.list";
  fOutputStreamList = "output_streams.list";
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
