#include <stdio.h>
#include <sys/stat.h>

#include "SACMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

SACMonitor::SACMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

SACMonitor::~SACMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }

  // Delete histograms
  for(UInt_t x=0; x<5; x++) {
    for(UInt_t y=0; y<5; y++) {
      if (fHNPeak[x][y]) { delete fHNPeak[x][y]; fHNPeak[x][y] = 0; }
      if (fHTPeak[x][y]) { delete fHTPeak[x][y]; fHTPeak[x][y] = 0; }
    }
  }

}

void SACMonitor::Initialize()
{

  // Get SAC map from configuration file
  if(fConfigParser->HasConfig("ADC","ID")){
    std::vector<std::string> bIDs = fConfigParser->GetConfig("ADC","ID");
    for(unsigned int ib = 0; ib < bIDs.size(); ib++) {
      unsigned int bID = std::stoi(bIDs[ib]);
      if (bID == 27) {
	std::string parName = "ADC"+bIDs[ib];
	if (fConfigParser->HasConfig("ADC",parName )) {
	  std::vector<std::string> bMap = fConfigParser->GetConfig("ADC",parName);
	  for (unsigned int ic = 0; ic < bMap.size(); ic++) fSAC_map[ic] = std::stoi(bMap[ic]);
	} else {
	  printf("SACMonitor::Initialize - WARNING - No channel list for board %s in config file: using that from default channel map\n",parName.c_str());
	}
      } else {
	printf("SACMonitor::Initialize - WARNING - Found map for board %d in configuration file: ignoring it",bID);
      }
    }
  } else {
    printf("SACMonitor::Initialize - WARNING - No board list in config file: using default channel map\n");
  }
  if (fConfig->Verbose() > 1) {
    printf("--- SAC readout map ---\n");
    printf("B27");
    for (unsigned int ic=0;ic<32;ic++) {
      if (fSAC_map[ic] == -1) {
	printf(" %2d",fSAC_map[ic]);
      } else {
	printf(" %2.2d",fSAC_map[ic]);
      }
    }
    printf("\n");
  }

  // Get beam output rate from config file
  fBeamOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","BeamOutputRate") ) {
    try {
      fBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate"));
    } catch (...) {
      printf("SACMonitor::Initialize - WARNING - 'BeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","BeamOutputRate").c_str(),fBeamOutputRate);
    }
  }

  // Get off-beam output rate from config file
  fOffBeamOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","OffBeamOutputRate") ) {
    try {
      fOffBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate"));
    } catch (...) {
      printf("SACMonitor::Initialize - WARNING - 'OffBeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","OffBeamOutputRate").c_str(),fOffBeamOutputRate);
    }
  }

  // Get cosmics output rate from config file
  fCosmicsOutputRate = 100;
  if ( fConfigParser->HasConfig("RECO","CosmicsOutputRate") ) {
    try {
      fCosmicsOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate"));
    } catch (...) {
      printf("SACMonitor::Initialize - WARNING - 'CosmicsOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","CosmicsOutputRate").c_str(),fCosmicsOutputRate);
    }
  }

  // Set number of samples to use to compute pedestals
  fPedestalSamples = 100;

  // Create histograms
  TString hname;
  for(UInt_t x=0; x<5; x++) {
    for(UInt_t y=0; y<5; y++) {
      // On-beam histograms
      hname.Form("SAC_NPeak_%d%d",y,x);
      fHNPeak[x][y] = new TH1D(hname.Data(),hname.Data(),100,0.,100.);
      hname.Form("SAC_TPeak_%d%d",y,x);
      fHTPeak[x][y] = new TH1D(hname.Data(),hname.Data(),100,0.,1000.);
    }
  }
 
  // Reset global counters
  ResetSACMap(fSAC_CosmEvt);
  ResetSACMap(fSAC_CosmSum);
  ResetSACMap(fSAC_BeamEvt);
  ResetSACMap(fSAC_BeamSum);
  ResetSACWaveforms(fCosmWF);
  ResetSACWaveforms(fBeamWF);
  ResetSACWaveforms(fOffBeamWF);
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

}

void SACMonitor::StartOfEvent()
{

  // Check if event was triggered by BTF beam
  if (fConfig->GetEventTrigMask() & 0x01) {
    fIsBeam = true;
    fBeamEventCount++;
  } else {
    fIsBeam = false;
  }

  // Check if event was triggered by cosmics
  if (fConfig->GetEventTrigMask() & 0x02) {
    fIsCosmics = true;
    fCosmicsEventCount++;
  } else {
    fIsCosmics = false;
  }
 
  // Check if event was a random trigger
  if (fConfig->GetEventTrigMask() & 0x40) {
    fIsRandom = true;
    fRandomEventCount++;
  } else {
    fIsRandom = false;
  }
  
  // Check if event was an off-beam trigger
  if (fConfig->GetEventTrigMask() & 0x80) {
    fIsOffBeam = true;
    fOffBeamEventCount++;
  } else {
    fIsOffBeam = false;
  }

}

void SACMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset beam histograms
      ResetSACMap(fHNPeak);
      ResetSACMap(fHTPeak);

      // Reset beam maps
      ResetSACMap(fSAC_BeamEvt);
      ResetSACMap(fSAC_BeamSum);

      // Reset beam waveforms
      ResetSACWaveforms(fBeamWF);

    }

  } // End of beam output

  if (fIsOffBeam) {

    if (fOffBeamOutputRate && (fOffBeamEventCount % fOffBeamOutputRate == 0)) {

      // Write off-beam events data to output PadmeMonitor file
      OutputOffBeam();

      // Reset off-beam waveforms
      ResetSACWaveforms(fOffBeamWF);

    }

  } // End of off-beam output

  if (fIsCosmics) {

    if (fCosmicsOutputRate && (fCosmicsEventCount % fCosmicsOutputRate == 0)) {

      // Write cosmics events data to output PadmeMonitor file
      OutputCosmics();

      // Reset cosmics maps
      ResetSACMap(fSAC_CosmEvt);
      ResetSACMap(fSAC_CosmSum);

      // Reset cosmics waveforms
      ResetSACWaveforms(fCosmWF);

    }

  } // End of cosmics output

  if (fIsRandom) {

    if (fRandomOutputRate && (fRandomEventCount % fRandomOutputRate == 0)) {

      // Write random events data to output PadmeMonitor file
      //OutputRandom();

    }

  } // End of random output

}

void SACMonitor::Finalize()
{
  printf("SACMonitor::Finalize - Total number of beam events: %d\n",fBeamEventCount);
  printf("SACMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  printf("SACMonitor::Finalize - Total number of cosmics events: %d\n",fCosmicsEventCount);
  printf("SACMonitor::Finalize - Total number of random events: %d\n",fRandomEventCount);
}

void SACMonitor::AnalyzeBoard(UChar_t board)
{;}

void SACMonitor::AnalyzeChannel(UChar_t board,UChar_t channel,Short_t* samples)
{

  if (board != 27) {
    printf("SACMonitor::Analyze - WARNING - ADC board %d does not belong to SAC\n",board);
    return;
  } else if (channel >= 25) {
    // For the moment we do not use cosmics pads samples
    return;
  } else if (fSAC_map[channel] == -1) {
    printf("SACMonitor::Analyze - WARNING - ADC board %d channel %d is disabled in the map\n",board,channel);
    return;
  }

  // Get x,y coordinates from channel map
  UChar_t x = fSAC_map[channel]/10;
  UChar_t y = fSAC_map[channel]%10;

  // Compute pedestal and RMS for this channel
  ComputeChannelPedestal(board,channel,samples);

  // Get energy in this channel
  ComputeChannelEnergy(board,channel,samples);

  // Find peaks in this channel
  FindChannelPeaks(board,channel,samples);

  // Save beam related information
  if (fIsBeam) {

    // Save energy in beam map
    fSAC_BeamSum[x][y] += fChannelEnergy;

    // Save peaks information
    fHNPeak[x][y]->Fill(fNPeaks);
    if (fNPeaks) for (UInt_t i = 0; i<fNPeaks; i++) fHTPeak[x][y]->Fill(fPeakTime[i]);

    // Save waveform once every few events
    if (fBeamEventCount % fBeamOutputRate == 0) {
      fSAC_BeamEvt[x][y]= fChannelEnergy;
      for(UInt_t i = 0; i<1024; i++) fBeamWF[x][y][i] = samples[i];
    }

  }

  // Save cosmics related information
  if (fIsCosmics) {

    // Save energy in cosmics map
    fSAC_CosmSum[x][y] += fChannelEnergy;

    // Save waveform and event map once every few events
    if (fCosmicsEventCount % fCosmicsOutputRate == 0) {
      fSAC_CosmEvt[x][y] = fChannelEnergy;
      for(UInt_t i = 0; i<1024; i++) fCosmWF[x][y][i] = samples[i];
    }

  }

}

void SACMonitor::ComputeChannelEnergy(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get total signal area using first 100 samples as pedestal and dropping last 30 samples
  Int_t sum = 0;
  Int_t sum_ped = 0;
  for(UInt_t s = 0; s<994; s++) {
    sum += samples[s];
    if (s<100) sum_ped += samples[s];
  }
  fChannelEnergy = 9.94*(Double_t)sum_ped-(Double_t)sum;
  // Convert to pC (check formula)
  //tot = tot/(4096.*50.)*(1.E-9/1.E-12);
  fChannelEnergy *= 4.8828E-3;
}

void SACMonitor::ComputeChannelPedestal(UChar_t board,UChar_t channel,Short_t* samples)
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

void SACMonitor::FindChannelPeaks(UChar_t board,UChar_t channel,Short_t* samples)
{

  fNPeaks = 0;

  // Count peaks defined as consecutive samples with value < pedestal - 5*RMS
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
    printf("SACMonitor::FindChannelPeaks - Event %d Channel %d NPeaks %d - Ped %f RMS %f Thr %f\n",fConfig->GetEventNumber(),channel,fNPeaks,fChannelPedestal,fChannelPedRMS,threshold);

}

void SACMonitor::ResetSACMap(Double_t map[5][5])
{
  // Set all crystals to 0
  for (UChar_t x=0; x<5; x++) {
    for (UChar_t y=0; y<5; y++) {
      map[x][y] = 0.;
    }
  }
}

void SACMonitor::ResetSACMap(TH1D* map[5][5])
{
  // Reset all histograms
  for (UChar_t x=0; x<5; x++) {
    for (UChar_t y=0; y<5; y++) {
      map[x][y]->Reset();
    }
  }
}

void SACMonitor::ResetSACWaveforms(Short_t map[5][5][1024])
{
  // Set all samples to 0
  for (UChar_t x=0; x<5; x++) {
    for (UChar_t y=0; y<5; y++) {
      for (UShort_t s=0; s<1024; s++) {
      	map[x][y][s] = 0;
      }
    }
  }
}

Int_t SACMonitor::OutputBeam()
{
  if (fConfig->Verbose()>0) printf("SACMonitor::OutputBeam - Writing beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/SACMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/SACMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Save waveforms and peak maps
  for (UInt_t x = 0; x < 5; x++) {
    for (UInt_t y = 0; y < 5; y++) {

      fprintf(outf,"PLOTID SACMon_beamwf_%d%d\n",y,x);
      fprintf(outf,"PLOTTYPE scatter\n");
      fprintf(outf,"PLOTNAME SAC Beam Waveform X %d Y %d - Run %d Event %d - %s\n",x,y,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
      fprintf(outf,"RANGE_X 0 1024\n");
      //fprintf(outf,"RANGE_Y 0 4096\n");
      fprintf(outf,"TITLE_X Sample\n");
      fprintf(outf,"TITLE_Y Counts\n");
      fprintf(outf,"MODE [ \"lines\" ]\n");
      fprintf(outf,"COLOR [ \"ff0000\" ]\n");
      fprintf(outf,"DATA [ [");
      for(UInt_t j = 0; j<1024; j++) {
	if (j>0) { fprintf(outf,","); }
	fprintf(outf,"[%d,%d]",j,fBeamWF[x][y][j]);
      }
      fprintf(outf,"] ]\n\n");

      fprintf(outf,"PLOTID SACMon_npeaks_%d%d\n",y,x);
      fprintf(outf,"PLOTTYPE histo1d\n");
      fprintf(outf,"PLOTNAME SAC Beam #Peaks X %d Y %d - Run %d - %s\n",x,y,fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
      fprintf(outf,"CHANNELS %d\n",fHNPeak[x][y]->GetNbinsX());
      fprintf(outf,"RANGE_X %.3f %.3f\n",fHNPeak[x][y]->GetXaxis()->GetXmin(),fHNPeak[x][y]->GetXaxis()->GetXmax());
      fprintf(outf,"TITLE_X N_Peaks\n");
      fprintf(outf,"DATA [ [");
      for(Int_t b = 1; b <= fHNPeak[x][y]->GetNbinsX(); b++) {
	if (b>1) fprintf(outf,",");
	fprintf(outf,"%.0f",fHNPeak[x][y]->GetBinContent(b));
      }
      fprintf(outf,"] ]\n\n");

      fprintf(outf,"PLOTID SACMon_tpeaks_%d%d\n",y,x);
      fprintf(outf,"PLOTTYPE histo1d\n");
      fprintf(outf,"PLOTNAME SAC Beam Peak Time X %d Y %d - Run %d - %s\n",x,y,fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
      fprintf(outf,"CHANNELS %d\n",fHTPeak[x][y]->GetNbinsX());
      fprintf(outf,"RANGE_X %.3f %.3f\n",fHTPeak[x][y]->GetXaxis()->GetXmin(),fHTPeak[x][y]->GetXaxis()->GetXmax());
      fprintf(outf,"TITLE_X Peak_Time\n");
      fprintf(outf,"DATA [ [");
      for(Int_t b = 1; b <= fHTPeak[x][y]->GetNbinsX(); b++) {
	if (b>1) fprintf(outf,",");
	fprintf(outf,"%.0f",fHTPeak[x][y]->GetBinContent(b));
      }
      fprintf(outf,"] ]\n\n");

    }
  }
       
  fprintf(outf,"PLOTID SACMon_beamevt\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME SAC Beam - Run %d Event %d - %s\n",fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 5 5\n");
  fprintf(outf,"RANGE_X 0 5\n");
  fprintf(outf,"RANGE_Y 0 5\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<5;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<5;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.3f",fSAC_BeamEvt[x][y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");

  fprintf(outf,"PLOTID SACMon_beam\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME SAC Beam - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS 5 5\n");
  fprintf(outf,"RANGE_X 0 5\n");
  fprintf(outf,"RANGE_Y 0 5\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<5;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<5;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.3f",fSAC_BeamSum[x][y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");

  // Close monitor file and move it to watchdir
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("SACMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}

Int_t SACMonitor::OutputOffBeam()
{

  if (fConfig->Verbose()>0) printf("SACMonitor::OutputOffBeam - Writing off-beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/SACMon_OffBeam.txt";
  TString ffname = fConfig->OutputDirectory()+"/SACMon_OffBeam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Show waveforms maps
  for (UInt_t x = 0; x < 5; x++) {
    for (UInt_t y = 0; y < 5; y++) {

      fprintf(outf,"PLOTID SACMon_offbeamwf_%d%d\n",y,x);
      fprintf(outf,"PLOTTYPE scatter\n");
      fprintf(outf,"PLOTNAME SAC Off-Beam Waveform X %d Y %d - Run %d Event %d - %s\n",x,y,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
      fprintf(outf,"RANGE_X 0 1024\n");
      //fprintf(outf,"RANGE_Y 0 4096\n");
      fprintf(outf,"TITLE_X Sample\n");
      fprintf(outf,"TITLE_Y Counts\n");
      fprintf(outf,"MODE [ \"lines\" ]\n");
      fprintf(outf,"COLOR [ \"ff0000\" ]\n");
      fprintf(outf,"DATA [ [");
      for(UInt_t j = 0; j<1024; j++) {
	if (j>0) { fprintf(outf,","); }
	fprintf(outf,"[%d,%d]",j,fBeamWF[x][y][j]);
      }
      fprintf(outf,"] ]\n\n");

    }
  }

  // Close monitor file
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("SACMonitor::OutputOffBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}

Int_t SACMonitor::OutputCosmics()
{
  if (fConfig->Verbose()>0) printf("SACMonitor::OutputCosmics - Writing cosmics output files\n");

  // Write cosmics-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/SACMon_Cosmics.txt";
  TString ffname = fConfig->OutputDirectory()+"/SACMon_Cosmics.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Show waveforms and event map
  for (UInt_t x = 0; x < 5; x++) {
    for (UInt_t y = 0; y < 5; y++) {
      fprintf(outf,"PLOTID SACMon_cosmwf_%d%d\n",y,x);
      fprintf(outf,"PLOTTYPE scatter\n");
      fprintf(outf,"PLOTNAME SAC Cosmics Waveform X %d Y %d - Run %d Event %d - %s\n",x,y,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
      fprintf(outf,"RANGE_X 0 1024\n");
      //fprintf(outf,"RANGE_Y 0 4096\n");
      fprintf(outf,"TITLE_X Sample\n");
      fprintf(outf,"TITLE_Y Counts\n");
      fprintf(outf,"MODE [ \"lines\" ]\n");
      fprintf(outf,"COLOR [ \"ff0000\" ]\n");
      fprintf(outf,"DATA [ [");
      for(UInt_t j = 0; j<1024; j++) {
	if (j>0) { fprintf(outf,","); }
	fprintf(outf,"[%d,%d]",j,fCosmWF[x][y][j]);
      }
      fprintf(outf,"] ]\n\n");
    }
  }
      
  fprintf(outf,"PLOTID SACMon_cosmevt\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME SAC Cosmics - Run %d Event %d - %s\n",fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 5 5\n");
  fprintf(outf,"RANGE_X 0 5\n");
  fprintf(outf,"RANGE_Y 0 5\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<5;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<5;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.3f",fSAC_CosmEvt[x][y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");

  fprintf(outf,"PLOTID SACMon_cosmics\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME SAC Cosmics - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS 5 5\n");
  fprintf(outf,"RANGE_X 0 5\n");
  fprintf(outf,"RANGE_Y 0 5\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<5;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<5;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.3f",fSAC_CosmSum[x][y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("SACMonitor::OutputCosmics - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}

Int_t SACMonitor::OutputRandom()
{

  if (fConfig->Verbose()>0) printf("SACMonitor::OutputRandom - Writing random output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/SACMon_Random.txt";
  TString ffname = fConfig->OutputDirectory()+"/SACMon_Random.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Close monitor file
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("SACMonitor::OutputRandom - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}
