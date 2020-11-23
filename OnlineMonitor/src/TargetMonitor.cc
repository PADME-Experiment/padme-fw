#include <stdio.h>

#include "TargetMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

TargetMonitor::TargetMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

TargetMonitor::~TargetMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }
}

void TargetMonitor::Initialize()
{

  // Get Target map from configuration file
  if (fConfigParser->HasConfig("ADC","ADC28")) {
    std::vector<std::string> bMap = fConfigParser->GetConfig("ADC","ADC28");
    for (unsigned int ic = 0; ic < bMap.size(); ic++) fTarget_map[ic] = std::stoi(bMap[ic]);
  } else {
    printf("TargetMonitor::Initialize - WARNING - No channel map provided: using default map\n");
  }

  // ADC parameters
  fVoltageBin = 1./4096.; // Volt per count (1V = 4096 counts)
  fTimeBin    = 1.;       // Time per sample in ns (1 Msps)
  fImpedance  = 50.;      // ADC input impedance in Ohm (50 Ohm)

  // Get running parameters from configuration file. Use some default if not found
  fUseAbsSignal = false;
  fEventOutputScale = fConfigParser->HasConfig("RECO","EventScale")?std::stoi(fConfigParser->GetSingleArg("RECO","EventOutputScale")):500;
  fPedestalSamples = fConfigParser->HasConfig("RECO","PedestalSamples")?std::stoi(fConfigParser->GetSingleArg("RECO","PedestalSamples")):200;
  fSignalSamplesStart = fConfigParser->HasConfig("RECO","SignalSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesStart")):200;
  fSignalSamplesEnd = fConfigParser->HasConfig("RECO","SignalSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesEnd")):700;
  if (fConfig->Verbose()>0) {
    printf("TargetMonitor::Initialize - Number of samples in pedestal: %d\n",fPedestalSamples);
    printf("TargetMonitor::Initialize - Signal samples interval: %d - %d\n",fSignalSamplesStart,fSignalSamplesEnd);
    if (fUseAbsSignal) {
      printf("TargetMonitor::Initialize - Absolute conversion of signal samples is ENABLED\n");
    } else {
      printf("TargetMonitor::Initialize - Absolute conversion of signal samples is DISABLED\n");
    }
  }

  // Reset counters
  fEventCounter = 0;
  for (UChar_t i=0;i<32;i++) fStrip_charge[i] = 0.;

}

void TargetMonitor::StartOfEvent()
{
  // Check if event is a beam event
  if (fConfig->GetEventTrigMask() & 0x01) {
    fEventCounter++;
    fIsBeam = true;
  } else {
    fIsBeam = false;
  }
}

void TargetMonitor::EndOfEvent()
{
  // Do not analyze off-beam events
  if (! fIsBeam) return;

  // If we read enough events, dump PadmeMonitor file
  if (fEventCounter == fEventOutputScale) {

    if (fConfig->Verbose()>0) printf("TargetMonitor::EndOfEvent - Writing output files\n");

    // Write Target histograms
    TString ftname = fConfig->TmpDirectory()+"/TargetMon_Charge.txt";
    TString ffname = fConfig->OutputDirectory()+"/TargetMon_Charge.txt";
    FILE* outf = fopen(ftname.Data(),"a");

    // X Profile
    fprintf(outf,"PLOTID TargetMon_ProfileX\n");
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME Target Profile X - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
    fprintf(outf,"CHANNELS 16\n");
    fprintf(outf,"RANGE_X -7.5 8.5\n");
    fprintf(outf,"TITLE_X X[mm]\n");
    fprintf(outf,"TITLE_Y pC/strip\n");
    fprintf(outf,"DATA [[");
    for(UChar_t i = 0;i<16;i++) {
      if (i>0) fprintf(outf,",");
      // Show average per-event charge for this strip
      fprintf(outf,"%.3f",fStrip_charge[i]/fEventOutputScale);
    }
    fprintf(outf,"]]\n");

    fprintf(outf,"\n");

    // Y Profile
    fprintf(outf,"PLOTID TargetMon_ProfileY\n");
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME Target Profile Y - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
    fprintf(outf,"CHANNELS 16\n");
    fprintf(outf,"RANGE_X -7.5 8.5\n");
    fprintf(outf,"TITLE_X Y[mm]\n");
    fprintf(outf,"TITLE_Y pC/strip\n");
    fprintf(outf,"DATA [[");
    for(UChar_t i = 16;i<32;i++) {
      if (i>16) fprintf(outf,",");
      // Show average per-event charge for this strip
      fprintf(outf,"%.3f",fStrip_charge[i]/fEventOutputScale);
    }
    fprintf(outf,"]]\n");

    // Waveforms
    for(UInt_t i=0; i<32; i++) {

      fprintf(outf,"\n");

      fprintf(outf,"PLOTID TargetMon_Waveform%2.2d\n",i);
      fprintf(outf,"PLOTNAME Target ch%2.2d %d/%d %s\n",i,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime().GetSec()));
      //fprintf(outf,"PLOTTYPE line\n");
      //fprintf(outf,"MODE [ \"lines\" ]\n");
      fprintf(outf,"PLOTTYPE histo1d\n");
      fprintf(outf,"CHANNELS 1024\n");
      fprintf(outf,"RANGE_X -0.5 1023.5\n");
      fprintf(outf,"TITLE_X Sample\n");
      fprintf(outf,"TITLE_Y Counts\n");
      //fprintf(outf,"DATA [");
      //for(UInt_t s = 0; s<1024; s++) {
      //	if (s>0) fprintf(outf,",");
      //	fprintf(outf,"[%d,%d]",s,fWaveform[i][s]);
      //}
      //fprintf(outf,"]\n");
      fprintf(outf,"DATA [[");
      for(UInt_t s = 0; s<1024; s++) {
	if (s>0) fprintf(outf,",");
	fprintf(outf,"%d",fWaveform[i][s]);
      }
      fprintf(outf,"]]\n");

    }


    fclose(outf);
    if ( std::rename(ftname,ffname) != 0 ) perror("Error renaming file");

    // Reset counters
    fEventCounter = 0;
    for (UChar_t i=0;i<32;i++) fStrip_charge[i] = 0.;
  }
}

void TargetMonitor::Finalize()
{;}

void TargetMonitor::Analyze(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Do not analyze off-beam events
  if (! fIsBeam) return;
  ComputeChannelCharge(board,channel,samples);
  fStrip_charge[fTarget_map[channel]-1] += fCharge[channel];
  // Save waveforms of last event. Center on pedestal to improve visibility
  if (fEventCounter == fEventOutputScale) for(UInt_t i=0;i<1024;i++) fWaveform[channel][i] = samples[i]-(Short_t)fPedestal[channel];
}

void TargetMonitor::ComputeChannelCharge(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get total signal area using first 100 samples as pedestal and dropping last 30 samples
  Int_t sum_sig = 0;
  Int_t sum_ped = 0;
  for(UInt_t s = 0; s<fSignalSamplesEnd; s++) {
    Short_t ss = samples[s];
    if (s<fPedestalSamples) sum_ped += ss; // For pedestals we use the unmodified counts
    if (fUseAbsSignal && ss < 2048) ss = 4096-ss; // Make all counts positive
    if (s>=fSignalSamplesStart) sum_sig += ss;
  }
  // Compute pedestal
  fPedestal[channel] = (Double_t)sum_ped/(Double_t)fPedestalSamples; // Compute pedestal for this channel
  fCharge[channel] = (Double_t)sum_sig-(Double_t)(fSignalSamplesEnd-fSignalSamplesStart)*fPedestal[channel]; // Subtract pedestal from signal
  fCharge[channel] *= 1000.*fVoltageBin*fTimeBin/fImpedance; // Convert to charge in pC
}
