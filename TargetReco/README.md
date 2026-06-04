README of the Run4 target reco and analysis

how to compile:
  g++ -o "file.exe" TargetRun4.cpp  -I ../PadmeRoot/include -L ../PadmeRoot/lib/ -lPadmeRoot `root-config --libs  --cflags`

this is a dedicated version of compiling the code for the TB data analysis or a simplified framework still compatible with PADME. There would be a better and optimized one, but this works, so it's fine (for the moment at least!)

how to execute:
  ./file.exe -l "input" -o "output"

one can execute in the same way we do for the full PADME framework, using list, single inputs, giving names, deciding the number of events and so on.



////////usage of merge raw

fRunName = "";
  fChamberRunName = ""; - 
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
  fNumberOfEventsToProcess = 0;
  fFollowMode = false;
  fResumeMode = false;
  fDebugScale = 100;
  fVerbose = 0;



   void SetRunNumber(Int_t r) { fRunNumber = r; }
  Int_t GetRunNumber() { return fRunNumber; }


  void SetDataDirectory (Int_t r) { fRunNumber = r; }. --> root::....  fino a rawdata 

  
