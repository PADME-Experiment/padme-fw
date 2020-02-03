#include "Configuration.hh"

Configuration* Configuration::fInstance = 0;

Configuration::Configuration()
{
  // Set default configuration parameters
  fRunNumber = 0;
  fProcessId = -1;
  fInputStream = "";
  fInputStreamList = "";
  fOutputStreamList = "";
  fRawFileHeader = "rawdata/rawdata";
  fNEventsPerFile = 10000;
  fVerbose = 0;
  fDebugScale = 100;
}

Configuration::~Configuration()
{;}

Configuration* Configuration::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new Configuration(); }
  return fInstance;
}

char* Configuration::FormatTime(const time_t tt)
{
  static char tform[20];
  struct tm* t = gmtime(&tt);
  sprintf(tform,"%04d/%02d/%02d %02d:%02d:%02d",
	  1900+t->tm_year,1+t->tm_mon,t->tm_mday,
	  t->tm_hour,t->tm_min,t->tm_sec);
  return tform;
}
