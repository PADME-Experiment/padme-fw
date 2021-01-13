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

  // Charge to PoTs conversion parameter
  // 1.60217662e-7: electron charge; 12: nominal CCD in um; 36: total number of strips
  fChargeToPoTs = 1./(1.60217662e-7*12.*36.);

  // Get beam output rate from config file
  fBeamOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","BeamOutputRate") ) {
    try {
      fBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate"));
    } catch (...) {
      printf("TargetMonitor::Initialize - WARNING - 'BeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","BeamOutputRate").c_str(),fBeamOutputRate);
    }
  }

  // Get running parameters from configuration file. Use some default if not found
  fPedestalSamples = fConfigParser->HasConfig("RECO","PedestalSamples")?std::stoi(fConfigParser->GetSingleArg("RECO","PedestalSamples")):200;
  fSignalSamplesStart = fConfigParser->HasConfig("RECO","SignalSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesStart")):200;
  fSignalSamplesEnd = fConfigParser->HasConfig("RECO","SignalSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesEnd")):700;
  fUseAbsSignal = false;
  if ( fConfigParser->HasConfig("RECO","UseAbsSignal") ) {
    int use = std::stoi(fConfigParser->GetSingleArg("RECO","UseAbsSignal"));
    if (use == 1) {
      fUseAbsSignal = true;
    } else if (use == 0) {
      fUseAbsSignal = false;
    } else {
      printf("TargetMonitor::Initialize - ERROR - UseAbsSignal set to %d\n",use);
      exit(EXIT_FAILURE);
    }
  }
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
  fBeamEventCount = 0;
  fEventPoTsTotal = 0.;
  fRunPoTsTotal = 0.;
  for (UChar_t i=0;i<32;i++) fStrip_charge[i] = 0.;
}

void TargetMonitor::StartOfEvent()
{
  // Check if event is a beam event
  if (fConfig->GetEventTrigMask() & 0x01) {
    fIsBeam = true;
  } else {
    fIsBeam = false;
  }
}

void TargetMonitor::EndOfEvent()
{
  // Do not analyze off-beam events
  if (! fIsBeam) return;

  // Compute number of positrons on target for this event
  ComputePoTs();

  // Add this event's Pots to totals
  fEventPoTsTotal += fEventPoTs;
  fRunPoTsTotal += fEventPoTs;

  // If we read enough events, dump PadmeMonitor file
  if (fBeamEventCount % fBeamOutputRate == 0) {

    // Update timelines
    fTL_RunPoTs[fTL_Current] = fRunPoTsTotal;
    fTL_EventPoTs[fTL_Current] = fEventPoTsTotal/(Double_t)fBeamOutputRate;
    fTL_Time[fTL_Current] = fConfig->GetEventAbsTime().GetSec();
    fTL_Current++;
    if (fTL_Current == TARGETMONITOR_TIMELINE_SIZE) fTL_Current = 0;

    OutputBeam();

    // Reset counters
    fEventPoTsTotal = 0.;
    for (UChar_t i=0;i<32;i++) fStrip_charge[i] = 0.;

  }

  // Count beam event
  fBeamEventCount++;
}

void TargetMonitor::Finalize()
{
  printf("TargetMonitor::Finalize - Total number of beam events: %d\n",fBeamEventCount);
  printf("TargetMonitor::Finalize - Total PoTs: %.0f\n",fRunPoTsTotal);
}

void TargetMonitor::Analyze(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Do not analyze off-beam events
  if (! fIsBeam) return;
  ComputeChannelCharge(board,channel,samples);
  fStrip_charge[fTarget_map[channel]-1] += fCharge[channel];
  // Save waveforms of last event. Center on pedestal to improve visibility
  //if (fEventCounter == fEventOutputScale) for(UInt_t i=0;i<1024;i++) fWaveform[channel][i] = samples[i]-(Short_t)fPedestal[channel];
  if (fBeamEventCount % fBeamOutputRate == 0) {
    for(UInt_t i=0;i<1024;i++) {
      if (fUseAbsSignal && samples[i] < 2048) {
	fWaveform[channel][i] = 4096-samples[i];
      } else {
	fWaveform[channel][i] = samples[i];
      }
    }
  }
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

void TargetMonitor::ComputePoTs()
{
  Double_t chargeX = 0.;
  Double_t chargeY = 0.;
  fEventPoTs = 0.;
  for (UChar_t i=0;i<32;i++) {
    if (i<16) {
      chargeX += fStrip_charge[i];
    } else {
      chargeY += fStrip_charge[i];
    }
  }
  fEventPoTs = fChargeToPoTs*(chargeX+chargeY)/2.;
}

Int_t TargetMonitor::OutputBeam()
{

  Bool_t first;

  if (fConfig->Verbose()>0) printf("TargetMonitor::OutputBeam - Writing beam output files\n");

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
    fprintf(outf,"%.3f",fStrip_charge[i]/fBeamOutputRate);
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
    fprintf(outf,"%.3f",fStrip_charge[i]/fBeamOutputRate);
  }
  fprintf(outf,"]]\n");

  // Waveforms
  for(UInt_t i=0; i<32; i++) {

    fprintf(outf,"\n");

    fprintf(outf,"PLOTID TargetMon_Waveform%2.2d\n",i);
    fprintf(outf,"PLOTTYPE scatter\n");
    fprintf(outf,"PLOTNAME Target ch%2.2d %d/%d %s\n",i,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime().GetSec()));
    fprintf(outf,"RANGE_X 0 1024\n");
    //fprintf(outf,"RANGE_Y 0 4096\n");
    fprintf(outf,"TITLE_X Sample\n");
    fprintf(outf,"TITLE_Y Counts\n");
    fprintf(outf,"MODE [ \"lines\" ]\n");
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
    fprintf(outf,"DATA [ [");
    Bool_t first = true;
    for(UInt_t s = 0; s<1024; s++) {
      if (first) { first = false; } else { fprintf(outf,","); }
      fprintf(outf,"[%d,%d]",s,fWaveform[i][s]);
    }
    fprintf(outf,"] ]\n\n");

  }

  // PoTs timeline
  fprintf(outf,"PLOTID TargetMon_PoTs_TL\n");
  fprintf(outf,"PLOTNAME Target PoTs - Run %d Event %d - %s\n",fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime().GetSec()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"TIME_FORMAT fine\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Pots (average)\n");
  //fprintf(outf,"RANGE_Y 0. 100.\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  //fprintf(outf,"LEGEND [ \"PoTs\" ]\n");
  fprintf(outf,"DATA [ [");
  first = true;
  for(UInt_t i = 0; i<TARGETMONITOR_TIMELINE_SIZE; i++) {
    UInt_t ii = (fTL_Current+i)%TARGETMONITOR_TIMELINE_SIZE;
    if (fTL_Time[ii] != 0) {
      if (first) { first = false; } else { fprintf(outf,",");	}
      fprintf(outf,"[%d,%.1f]",fTL_Time[ii],fTL_EventPoTs[ii]);
    }
  }
  fprintf(outf,"] ]\n\n");

  // Total Run PoTs timeline
  fprintf(outf,"PLOTID TargetMon_RunPoTs_TL\n");
  fprintf(outf,"PLOTNAME Target Total PoTs - Run %d Event %d - %s\n",fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime().GetSec()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"TIME_FORMAT fine\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Pots\n");
  //fprintf(outf,"RANGE_Y 0. 100.\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  //fprintf(outf,"LEGEND [ \"PoTs\" ]\n");
  fprintf(outf,"DATA [ [");
  first = true;
  for(UInt_t i = 0; i<TARGETMONITOR_TIMELINE_SIZE; i++) {
    UInt_t ii = (fTL_Current+i)%TARGETMONITOR_TIMELINE_SIZE;
    if (fTL_Time[ii] != 0) {
      if (first) { first = false; } else { fprintf(outf,",");	}
      fprintf(outf,"[%d,%.1f]",fTL_Time[ii],fTL_RunPoTs[ii]);
    }
  }
  fprintf(outf,"] ]\n\n");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("TargetMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}
