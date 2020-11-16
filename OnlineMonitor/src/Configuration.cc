#include "Configuration.hh"

Configuration* Configuration::fInstance = 0;

Configuration::Configuration()
{
  // Set default configuration parameters
  fRunName = "";
  fDataDirectory = ".";
  fOutputDirectory = "watchdir";
  fTmpDirectory = "tmp";
  fConfigFile = "config/OnlineMonitor.cfg";
  fStopFile = "run/OnlineMonitor.stop";
  fNumberOfStreams = 1;
  fFollowMode = false;
  fResumeMode = false;
  fDebugScale = 100;
  fVerbose = 0;
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
