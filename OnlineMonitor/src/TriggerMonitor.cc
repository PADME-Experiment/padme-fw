#include <stdio.h>

#include "TriggerMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

TriggerMonitor::TriggerMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

TriggerMonitor::~TriggerMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }
}

void TriggerMonitor::Initialize()
{

  // ADC parameters
  fVoltageBin = 1./4096.; // Volt per count (1V = 4096 counts)
  fImpedance  = 50.;      // ADC input impedance in Ohm (50 Ohm)

  // Get running parameters from configuration file. Use some default if not found
  fEventOutputScale = fConfigParser->HasConfig("RECO","EventScale")?std::stoi(fConfigParser->GetSingleArg("RECO","EventOutputScale")):500;
  if (fConfig->Verbose()>0) {
    printf("TriggerMonitor::Initialize - Event output scale: %d\n",fEventOutputScale);
  }

  // Reset counters
  fEventCounter = 0;

}

void TriggerMonitor::StartOfEvent()
{;}

void TriggerMonitor::EndOfEvent()
{
  /*
  // If we read enough events, dump PadmeMonitor file
  if (fEventCounter == fEventOutputScale) {

    if (fConfig->Verbose()>0) printf("TriggerMonitor::EndOfEvent - Writing output files\n");

    // Write Target histograms
    TString ftname = fConfig->TmpDirectory()+"/TriggerMon_Timing.txt";
    TString ffname = fConfig->OutputDirectory()+"/TriggerMon_Timing.txt";
    FILE* outf = fopen(ftname.Data(),"a");

    fclose(outf);
    if ( std::rename(ftname,ffname) != 0 ) perror("Error renaming file");

    // Reset counters
    fEventCounter = 0;
  }
  */
}

void TriggerMonitor::Finalize()
{;}

void TriggerMonitor::Analyze(UChar_t board,UChar_t group,Short_t* samples)
{
  ComputeTriggerTime(board,group,samples);
}

void TriggerMonitor::ComputeTriggerTime(UChar_t board,UChar_t group,Short_t* samples)
{

  //static Bool_t first = true;

  fTriggerTime = 0.;

  // Define trigger time finder parameters
  UInt_t jump = 2; // Derivative interval
  Short_t threshold = 100; // Derivative threshold

  // Find bins when derivative goes above (start) and below (end) threshold
  // Trigger time is the average of the two bins
  Bool_t aboveThr = false;
  UInt_t s_start = 0;
  for (UInt_t s = 0; s<(1024-jump); s++) {
    Short_t d = samples[s] - samples[s+jump];
    if (aboveThr) {
      if (d < threshold) {
	fTriggerTime = (Double_t)(s+s_start-1)/2.;
	break;
      }
    } else if (d >= threshold) {
	s_start = s;
	aboveThr = true;
    }
  }
  /*
  if (first) {
    first = false;
    for(UInt_t j = 1;j<=10;j++) {
      for(Short_t t = 100; t<1000; t += 50) {
	double_t ttime = 0.;
	Bool_t aboveThr = false;
	UInt_t s_start = 0;
	UInt_t s_end = 0;
	for (UInt_t s = j; s<1024; s++) {
	  Short_t d = samples[s-j] - samples[s];
	  if (aboveThr) {
	    if (d < t) {
	      s_end = s-j;
	      if (s_end > s_start) ttime = (Double_t)(s_end+s_start-1)/2.;
	      break;
	    }
	  } else if (d >= t) {
	    s_start = s;
	    aboveThr = true;
	  }
	}
	if (ttime>0.) printf("J %2d T%4d S%4d E%4d Time%6.1f\n",j,t,s_start,s_end,ttime);
      }
      printf("\n");
    }
  }
  */
}
