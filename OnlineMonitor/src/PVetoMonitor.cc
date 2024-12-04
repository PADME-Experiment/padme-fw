#include <stdio.h>
#include <sys/stat.h>

#include "PVetoMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

PVetoMonitor::PVetoMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

PVetoMonitor::~PVetoMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }

  // Delete histograms
  for(UInt_t f=0; f<PVETO_FINGERS; f++) {
    if (fHNPeaks[f]) { delete fHNPeaks[f]; fHNPeaks[f] = 0; }
    if (fHNPeaksOB[f]) { delete fHNPeaksOB[f]; fHNPeaksOB[f] = 0; }
  }

}

void PVetoMonitor::Initialize()
{

  // Get PVeto map from configuration file
  if(fConfigParser->HasConfig("ADC","ID")){
    std::vector<std::string> bIDs = fConfigParser->GetConfig("ADC","ID");
    for(unsigned int ib = 0; ib < bIDs.size(); ib++) {
      unsigned int bID = std::stoi(bIDs[ib]);
      if (bID >= 10 && bID <= 12) {
	std::string parName = "ADC"+bIDs[ib];
	if (fConfigParser->HasConfig("ADC",parName )) {
	  std::vector<std::string> bMap = fConfigParser->GetConfig("ADC",parName);
	  for (unsigned int ic = 0; ic < bMap.size(); ic++)
	    fPVeto_map[bID-10][ic] = std::stoi(bMap[ic]);
	} else {
	  printf("PVetoMonitor::Initialize - WARNING - No channel list for board %s in config file: using that from default channel map\n",parName.c_str());
	}
      } else {
	printf("PVetoMonitor::Initialize - WARNING - Found map for board %d in configuration file: ignoring it",bID);
      }
    }
  } else {
    printf("PVetoMonitor::Initialize - WARNING - No board list in config file: using default channel map\n");
  }
  if (fConfig->Verbose() > 1) {
    printf("--- PVeto readout map ---\n");
    for (UChar_t ib = 0; ib<3; ib++) {
      printf("B%d",10+ib);
      for (unsigned int ic=0;ic<32;ic++) {
	if (fPVeto_map[ib][ic] == -1) {
	  printf(" %2d",fPVeto_map[ib][ic]);
	} else {
	  printf(" %2.2d",fPVeto_map[ib][ic]);
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
      printf("PVetoMonitor::Initialize - WARNING - 'BeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","BeamOutputRate").c_str(),fBeamOutputRate);
    }
  }

  // Get off-beam output rate from config file
  fOffBeamOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","OffBeamOutputRate") ) {
    try {
      fOffBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate"));
    } catch (...) {
      printf("PVetoMonitor::Initialize - WARNING - 'OffBeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","OffBeamOutputRate").c_str(),fOffBeamOutputRate);
    }
  }

  // Set number of samples to use to compute pedestals
  fPedestalSamples = 100;

  // Create histograms
  TString hname;
  for(UInt_t f=0; f<PVETO_FINGERS; f++) {
    // On-beam histograms
    hname.Form("PVeto_NPeaks_%2.2d",f);
    fHNPeaks[f] = new TH1D(hname.Data(),hname.Data(),100,0.,100.);
    // Off-beam histograms
    hname.Form("PVeto_NPeaksOB_%2.2d",f);
    fHNPeaksOB[f] = new TH1D(hname.Data(),hname.Data(),100,0.,100.);
  }
 
  // Reset global counters
  ResetPVetoWaveforms(fBeamWF);
  ResetPVetoWaveforms(fOffBeamWF);
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;

}

void PVetoMonitor::StartOfEvent()
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

void PVetoMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Write beam data to output PadmeMonitor file
      OutputBeam();

      // Reset beam histograms
      ResetPVetoMap(fHNPeaks);

      // Reset beam waveforms
      ResetPVetoWaveforms(fBeamWF);

    }

  } // End of beam output

  if (fIsOffBeam) {

    if (fOffBeamOutputRate && (fOffBeamEventCount % fOffBeamOutputRate == 0)) {

      // Write off-beam data to output PadmeMonitor file
      OutputOffBeam();

      // Reset off-beam histograms
      ResetPVetoMap(fHNPeaksOB);

      // Reset off-beam waveforms
      ResetPVetoWaveforms(fOffBeamWF);

    }

  } // End of off-beam output

}

void PVetoMonitor::Finalize()
{
  printf("PVetoMonitor::Finalize - Total number of beam events: %d\n",fBeamEventCount);
  printf("PVetoMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
}

void PVetoMonitor::AnalyzeBoard(UChar_t board)
{;}

void PVetoMonitor::AnalyzeChannel(UChar_t board,UChar_t channel,Short_t* samples)
{

  if (board < 10 || board > 12) {
    printf("PVetoMonitor::AnalyzeChannel - WARNING - board %d does not belong to PVeto\n",board);
    return;
  } else if (fPVeto_map[board-10][channel] == -1) {
    printf("PVetoMonitor::AnalyzeChannel - WARNING - board %d channel %d is disabled in the PVeto map\n",board,channel);
    return;
  }

  // Get finger number from channel map
  UChar_t f = fPVeto_map[board-10][channel];

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

void PVetoMonitor::ComputeChannelEnergy(UChar_t board,UChar_t channel,Short_t* samples)
{
  fChannelEnergy = 0.;
}

void PVetoMonitor::ComputeChannelPedestal(UChar_t board,UChar_t channel,Short_t* samples)
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

void PVetoMonitor::FindChannelPeaks(UChar_t board,UChar_t channel,Short_t* samples)
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
    printf("PVetoMonitor::FindChannelPeaks - Event %d Channel %d NPeaks %d - Ped %f RMS %f Thr %f\n",fConfig->GetEventNumber(),channel,fNPeaks,fChannelPedestal,fChannelPedRMS,threshold);

}

void PVetoMonitor::ResetPVetoMap(Double_t map[PVETO_FINGERS])
{
  // Set all crystals to 0
  for (UChar_t f=0; f<PVETO_FINGERS; f++) {
    map[f] = 0.;
  }
}

void PVetoMonitor::ResetPVetoMap(TH1D* map[PVETO_FINGERS])
{
  // Reset all histograms
  for (UChar_t f=0; f<PVETO_FINGERS; f++) {
    map[f]->Reset();
  }
}

void PVetoMonitor::ResetPVetoWaveforms(Short_t map[PVETO_FINGERS][1024])
{
  // Set all samples to 0
  for (UChar_t f=0; f<PVETO_FINGERS; f++) {
    for (UShort_t s=0; s<1024; s++) {
      map[f][s] = 0;
    }
  }
}

Int_t PVetoMonitor::OutputBeam()
{
  if (fConfig->Verbose()>0) printf("PVetoMonitor::OutputBeam - Writing beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/PVetoMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/PVetoMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Show waveforms and pedstal maps
  for (UInt_t f = 0; f < PVETO_FINGERS; f++) {

    fprintf(outf,"PLOTID PVetoMon_beamwf_%2.2d\n",f);
    fprintf(outf,"PLOTTYPE scatter\n");
    fprintf(outf,"PLOTNAME PVeto Beam Waveform F %2.2d - Run %d Event %d - %s\n",f,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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

    fprintf(outf,"PLOTID PVetoMon_beamnpeaks_%2.2d\n",f);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME PVeto Beam NPeaks F %2.2d - Run %d - %s\n",f,fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    printf("PVetoMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}

Int_t PVetoMonitor::OutputOffBeam()
{

  if (fConfig->Verbose()>0) printf("PVetoMonitor::OutputOffBeam - Writing off-beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/PVetoMon_OffBeam.txt";
  TString ffname = fConfig->OutputDirectory()+"/PVetoMon_OffBeam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  // Show waveforms and pedstal maps
  for (UInt_t f = 0; f < PVETO_FINGERS; f++) {

    fprintf(outf,"PLOTID PVetoMon_offbeamwf_%2.2d\n",f);
    fprintf(outf,"PLOTTYPE scatter\n");
    fprintf(outf,"PLOTNAME PVeto Off-Beam Waveform F %2.2d - Run %d Event %d - %s\n",f,fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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

    fprintf(outf,"PLOTID PVetoMon_offbeamnpeaks_%2.2d\n",f);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME PVeto Off-Beam NPeaks F %2.2d - Run %d - %s\n",f,fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    printf("PVetoMonitor::OutputOffBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}
