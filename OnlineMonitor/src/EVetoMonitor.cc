#include <stdio.h>
#include <sys/stat.h>

#include "EVetoMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

EVetoMonitor::EVetoMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

EVetoMonitor::~EVetoMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }

  // Delete histograms
  for(UInt_t f=0; f<EVETO_FINGERS; f++) {
    if (fHNPeaks[f]) { delete fHNPeaks[f]; fHNPeaks[f] = 0; }
    if (fHNPeaksOB[f]) { delete fHNPeaksOB[f]; fHNPeaksOB[f] = 0; }
  }

}

void EVetoMonitor::Initialize()
{

  // Get EVeto map from configuration file
  if(fConfigParser->HasConfig("ADC","ID")){
    std::vector<std::string> bIDs = fConfigParser->GetConfig("ADC","ID");
    for(unsigned int ib = 0; ib < bIDs.size(); ib++) {
      unsigned int bID = std::stoi(bIDs[ib]);
      if (bID >= 24 && bID <= 26) {
	std::string parName = "ADC"+bIDs[ib];
	if (fConfigParser->HasConfig("ADC",parName )) {
	  std::vector<std::string> bMap = fConfigParser->GetConfig("ADC",parName);
	  for (unsigned int ic = 0; ic < bMap.size(); ic++)
	    fEVeto_map[bID-24][ic] = std::stoi(bMap[ic]);
	} else {
	  printf("EVetoMonitor::Initialize - WARNING - No channel list for board %s in config file: using that from default channel map\n",parName.c_str());
	}
      } else {
	printf("EVetoMonitor::Initialize - WARNING - Found map for board %d in configuration file: ignoring it",bID);
      }
    }
  } else {
    printf("EVetoMonitor::Initialize - WARNING - No board list in config file: using default channel map\n");
  }
  if (fConfig->Verbose() > 1) {
    printf("--- EVeto readout map ---\n");
    for (UChar_t ib = 0; ib<3; ib++) {
      printf("B%d",24+ib);
      for (unsigned int ic=0;ic<32;ic++) {
	if (fEVeto_map[ib][ic] == -1) {
	  printf(" %2d",fEVeto_map[ib][ic]);
	} else {
	  printf(" %2.2d",fEVeto_map[ib][ic]);
	}
      }
      printf("\n");
    }
  }

  // Get beam output rate from config file
  fBeamOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","BeamOutputRate") ) {
    try {
      fBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate"));
    } catch (...) {
      printf("EVetoMonitor::Initialize - WARNING - 'BeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","BeamOutputRate").c_str(),fBeamOutputRate);
    }
  }

  // Get off-beam output rate from config file
  fOffBeamOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","OffBeamOutputRate") ) {
    try {
      fOffBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate"));
    } catch (...) {
      printf("EVetoMonitor::Initialize - WARNING - 'OffBeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","OffBeamOutputRate").c_str(),fOffBeamOutputRate);
    }
  }

  // Set number of samples to use to compute pedestals
  fPedestalSamples = 100;

  // Create histograms
  TString hname;
  for(UInt_t f=0; f<EVETO_FINGERS; f++) {
    // On-beam histograms
    hname.Form("EVeto_NPeaks_%2.2d",f);
    fHNPeaks[f] = new TH1D(hname.Data(),hname.Data(),100,0.,100.);
    // Off-beam histograms
    hname.Form("EVeto_NPeaksOB_%2.2d",f);
    fHNPeaksOB[f] = new TH1D(hname.Data(),hname.Data(),100,0.,100.);
  }
 
  // Reset global counters
  ResetEVetoWaveforms(fBeamWF);
  ResetEVetoWaveforms(fOffBeamWF);
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;

}

void EVetoMonitor::StartOfEvent()
{

  // Check if event was triggered by BTF beam
  if (fConfig->GetEventTrigMask() & 0x01) {
    fIsBeam = true;
    fBeamEventCount++;
  } else {
    fIsBeam = false;
  }

  // Check if event was an off-beam trigger
  if (fConfig->GetEventTrigMask() & 0x80) {
    fIsOffBeam = true;
    fOffBeamEventCount++;
  } else {
    fIsOffBeam = false;
  }
 
}

void EVetoMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Write beam data to output PadmeMonitor file
      OutputBeam();

      // Reset beam histograms
      ResetEVetoMap(fHNPeaks);

      // Reset beam waveforms
      ResetEVetoWaveforms(fBeamWF);

    }

  } // End of beam output

  if (fIsOffBeam) {

    if (fOffBeamOutputRate && (fOffBeamEventCount % fOffBeamOutputRate == 0)) {

      // Write off-beam data to output PadmeMonitor file
      OutputOffBeam();

      // Reset off-beam histograms
      ResetEVetoMap(fHNPeaksOB);

      // Reset off-beam waveforms
      ResetEVetoWaveforms(fOffBeamWF);

    }

  } // End of off-beam output

}

void EVetoMonitor::Finalize()
{
  printf("EVetoMonitor::Finalize - Total number of beam events: %d\n",fBeamEventCount);
  printf("EVetoMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
}

void EVetoMonitor::AnalyzeBoard(UChar_t board)
{;}

void EVetoMonitor::AnalyzeChannel(UChar_t board,UChar_t channel,Short_t* samples)
{

  if (board < 24 || board > 26) {
    printf("EVetoMonitor::Analyze - WARNING - board %d does not belong to EVeto\n",board);
    return;
  } else if (fEVeto_map[board-24][channel] == -1) {
    printf("EVetoMonitor::Analyze - WARNING - board %d channel %d is disabled in the EVeto map\n",board,channel);
    return;
  }

  // Get finger number from channel map
  UChar_t f = fEVeto_map[board-24][channel];

  // Compute pedestal and RMS for this channel
  ComputeChannelPedestal(board,channel,samples);

  // Get energy in this channel
  ComputeChannelEnergy(board,channel,samples);

  // Find peaks in this channel
  FindChannelPeaks(board,channel,samples);
  if (fIsBeam) {
    fHNPeaks[f]->Fill(fNPeaks);
  }
  if (fIsOffBeam) {
    fHNPeaksOB[f]->Fill(fNPeaks);
  }

  // Save beam related information
  if (fIsBeam) {
    if (fBeamEventCount % fBeamOutputRate == 0) {
      for(UInt_t i = 0; i<1024; i++) fBeamWF[f][i] = samples[i];
    }
  }

  // Save off-beam related information
  if (fIsOffBeam) {
    if (fOffBeamEventCount % fOffBeamOutputRate == 0) {
      for(UInt_t i = 0; i<1024; i++) fOffBeamWF[f][i] = samples[i];
    }
  }

}

void EVetoMonitor::ComputeChannelEnergy(UChar_t board,UChar_t channel,Short_t* samples)
{
  fChannelEnergy = 0.;
}

void EVetoMonitor::ComputeChannelPedestal(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get pedestal value and RMS from first 100 samples
  Int_t sum = 0;
  Int_t sum2 = 0;
  for(UInt_t s = 0; s<fPedestalSamples; s++) {
    sum += samples[s];
    sum2 += samples[s]*samples[s];
  }
  fChannelPedestal = (Double_t)sum/(Double_t)fPedestalSamples;
  fChannelPedRMS = sqrt( ((Double_t)sum2 - (Double_t)sum*fChannelPedestal)/(Double_t)(fPedestalSamples-1) );
}

void EVetoMonitor::FindChannelPeaks(UChar_t board,UChar_t channel,Short_t* samples)
{

  fNPeaks = 0;

  // Count peaks defined as consecutive samples with value < pedestal - 10*RMS
  Double_t sample;
  Double_t threshold = fChannelPedestal - 5.*fChannelPedRMS;
  Bool_t onPeak = false;
  for(UInt_t s = 100; s<994; s++) {
    sample = (Double_t)samples[s];
    if (onPeak) {
      if (sample > threshold) onPeak = false;
    } else {
      if (sample <= threshold) {
	onPeak = true;
	fPeakTime[fNPeaks] = (Double_t)s;
	fNPeaks++;
      }
    }
  }

  if ( (fNPeaks > 0) && (fConfig->Verbose() > 2) )
    printf("EVetoMonitor::FindChannelPeaks - Event %d Channel %d NPeaks %d - Ped %f RMS %f Thr %f\n",fConfig->GetEventNumber(),channel,fNPeaks,fChannelPedestal,fChannelPedRMS,threshold);

}

void EVetoMonitor::ResetEVetoMap(Double_t map[EVETO_FINGERS])
{
  // Set all crystals to 0
  for (UChar_t f=0; f<EVETO_FINGERS; f++) {
    map[f] = 0.;
  }
}

void EVetoMonitor::ResetEVetoMap(TH1D* map[EVETO_FINGERS])
{
  // Reset all histograms
  for (UChar_t f=0; f<EVETO_FINGERS; f++) {
    map[f]->Reset();
  }
}

void EVetoMonitor::ResetEVetoWaveforms(Short_t map[EVETO_FINGERS][1024])
{
  // Set all samples to 0
  for (UChar_t f=0; f<EVETO_FINGERS; f++) {
    for (UShort_t s=0; s<1024; s++) {
      map[f][s] = 0;
    }
  }
}

Int_t EVetoMonitor::OutputBeam()
{
  if (fConfig->Verbose()>0) printf("EVetoMonitor::OutputBeam - Writing beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/EVetoMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/EVetoMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Show waveforms and pedstal maps
  for (UInt_t f = 0; f < EVETO_FINGERS; f++) {

    fprintf(outf,"PLOTID EVetoMon_beamwf_%2.2d\n",f);
    fprintf(outf,"PLOTTYPE scatter\n");
    fprintf(outf,"PLOTNAME EVeto Beam Waveform F %2.2d - Run %d Event %d - %s\n",f,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"RANGE_X 0 1024\n");
    //fprintf(outf,"RANGE_Y 0 4096\n");
    fprintf(outf,"TITLE_X Sample\n");
    fprintf(outf,"TITLE_Y Counts\n");
    fprintf(outf,"MODE [ \"lines\" ]\n");
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
    fprintf(outf,"DATA [ [");
    for(UInt_t j = 0; j<1024; j++) {
      if (j>0) { fprintf(outf,","); }
      fprintf(outf,"[%d,%d]",j,fBeamWF[f][j]);
    }
    fprintf(outf,"] ]\n\n");

    fprintf(outf,"PLOTID EVetoMon_beamnpeaks_%2.2d\n",f);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME EVeto Beam NPeaks F %2.2d - Run %d - %s\n",f,fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",fHNPeaks[f]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",fHNPeaks[f]->GetXaxis()->GetXmin(),fHNPeaks[f]->GetXaxis()->GetXmax());
    fprintf(outf,"TITLE_X Counts\n");
    fprintf(outf,"DATA [ [");
    for(Int_t b = 1; b <= fHNPeaks[f]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHNPeaks[f]->GetBinContent(b));
    }
    fprintf(outf,"] ]\n\n");

  }

  // Close monitor file and move it to watchdir
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("EVetoMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}

Int_t EVetoMonitor::OutputOffBeam()
{

  if (fConfig->Verbose()>0) printf("EVetoMonitor::OutputOffBeam - Writing off-beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/EVetoMon_OffBeam.txt";
  TString ffname = fConfig->OutputDirectory()+"/EVetoMon_OffBeam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Show waveforms and pedstal maps
  for (UInt_t f = 0; f < EVETO_FINGERS; f++) {

    fprintf(outf,"PLOTID EVetoMon_offbeamwf_%2.2d\n",f);
    fprintf(outf,"PLOTTYPE scatter\n");
    fprintf(outf,"PLOTNAME EVeto Off-Beam Waveform F %2.2d - Run %d Event %d - %s\n",f,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"RANGE_X 0 1024\n");
    //fprintf(outf,"RANGE_Y 0 4096\n");
    fprintf(outf,"TITLE_X Sample\n");
    fprintf(outf,"TITLE_Y Counts\n");
    fprintf(outf,"MODE [ \"lines\" ]\n");
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
    fprintf(outf,"DATA [ [");
    for(UInt_t j = 0; j<1024; j++) {
      if (j>0) { fprintf(outf,","); }
      fprintf(outf,"[%d,%d]",j,fOffBeamWF[f][j]);
    }
    fprintf(outf,"] ]\n\n");

    fprintf(outf,"PLOTID EVetoMon_offbeamnpeaks_%2.2d\n",f);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME EVeto Off-Beam NPeaks F %2.2d - Run %d - %s\n",f,fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",fHNPeaksOB[f]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",fHNPeaksOB[f]->GetXaxis()->GetXmin(),fHNPeaksOB[f]->GetXaxis()->GetXmax());
    fprintf(outf,"TITLE_X Counts\n");
    fprintf(outf,"DATA [ [");
    for(Int_t b = 1; b <= fHNPeaksOB[f]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHNPeaksOB[f]->GetBinContent(b));
    }
    fprintf(outf,"] ]\n\n");

  }
 
  // Close monitor file
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("EVetoMonitor::OutputOffBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}
