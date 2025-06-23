#include "Configuration.hh"

Configuration* Configuration::fInstance = 0;

Configuration::Configuration()
{
  // Set default configuration parameters
  fRunName = "";
  fChamberRunName = "";
  fEventsPerFile = 1000;
  fDataDirectory = "rawdata";
  fChamberDataDirectory = "mmchdata";
  fNumberOfEventsToSkip = 0;
  fOutputDirectory = "watchdir";
  fTrendDirectory = "trend";
  fTmpDirectory = "tmp";
  fConfigFile = "config/OnlineMonitor.cfg";
  fStopFile = "run/OnlineMonitor.stop";
  fEndRunFile = "run/OnlineMonitor.endrun";
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
  sprintf(tform,"%04d/%02d/%02d %02d:%02d:%02d UTC",
	  1900+t->tm_year,1+t->tm_mon,t->tm_mday,
	  t->tm_hour,t->tm_min,t->tm_sec);
  return tform;
}

char* Configuration::FormatCurrentTime()
{
  struct timespec now;
  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  return FormatTime(now.tv_sec);
}
