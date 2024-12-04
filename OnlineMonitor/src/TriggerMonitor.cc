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

  // Get event output rate from config file
  fEventOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","EventOutputRate") ) {
    try {
      fEventOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","EventOutputRate"));
    } catch (...) {
      printf("TriggerMonitor::Initialize - WARNING - 'EventOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","EventOutputRate").c_str(),fEventOutputRate);
    }
  }

  // Get board to show on monitor page
  fBoardToShow = 0;
  if ( fConfigParser->HasConfig("RECO","BoardToShow") ) {
    try {
      fBoardToShow = std::stoi(fConfigParser->GetSingleArg("RECO","BoardToShow"));
    } catch (...) {
      printf("TriggerMonitor::Initialize - WARNING - 'BoardToShow' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","BoardToShow").c_str(),fBoardToShow);
    }
  }

  // Reset counters
  ResetWaveforms(fEventWF);
  fEventCounter = 0;

}

void TriggerMonitor::StartOfEvent()
{
  fEventCounter++;
}

void TriggerMonitor::EndOfEvent()
{

  // If we read enough events, dump PadmeMonitor file
  if (fEventCounter % fEventOutputRate == 0) {
    OutputData();
  }

}

void TriggerMonitor::Finalize()
{
  printf("TriggerMonitor::Finalize - Total number of events: %d\n",fEventCounter);

}

void TriggerMonitor::AnalyzeBoard(UChar_t board)
{;}

void TriggerMonitor::AnalyzeGroup(UChar_t board,UChar_t group,Short_t* samples)
{

  // Compute trigger time for this group
  ComputeTriggerTime(board,group,samples);

  // Save waveforms once every few events
  if ( (fEventCounter % fEventOutputRate == 0) && (board == fBoardToShow) ) {
    for(UInt_t i = 0; i<1024; i++) fEventWF[group][i] = samples[i];
  }

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

void TriggerMonitor::ResetWaveforms(Short_t map[4][1024])
{
  // Set all samples to 0
  for (UChar_t g=0; g<4; g++) {
    for (UShort_t s=0; s<1024; s++) {
      map[g][s] = 0;
    }
  }
}

Int_t TriggerMonitor::OutputData()
{
  if (fConfig->Verbose()>0) printf("TriggerMonitor::OutputData - Writing output files\n");

  // Write Target histograms
  TString ftname = fConfig->TmpDirectory()+"/TriggerMon_Timing.txt";
  TString ffname = fConfig->OutputDirectory()+"/TriggerMon_Timing.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Show waveforms
  for (UInt_t g = 0; g < 4; g++) {
    fprintf(outf,"PLOTID TriggerMon_evtwf_%d\n",g);
    fprintf(outf,"PLOTTYPE scatter\n");
    fprintf(outf,"PLOTNAME Trigger Waveform - Board %d Group %d - Run %d Event %d - %s\n",fBoardToShow,g,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"RANGE_X 0 1024\n");
    //fprintf(outf,"RANGE_Y 0 4096\n");
    fprintf(outf,"TITLE_X Sample\n");
    fprintf(outf,"TITLE_Y Counts\n");
    fprintf(outf,"MODE [ \"lines\" ]\n");
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
    fprintf(outf,"DATA [ [");
    Bool_t first = true;
    for(UInt_t j = 0; j<1024; j++) {
      if (first) { first = false; } else { fprintf(outf,","); }
      fprintf(outf,"[%d,%d]",j,fEventWF[g][j]);
    }
    fprintf(outf,"] ]\n\n");
  }

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("TriggerMonitor::OutputData - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}
