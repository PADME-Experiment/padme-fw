#include <stdio.h>
#include <sys/stat.h>

#include "ADCMonitor.hh"

#include "Configuration.hh"

ADCMonitor::ADCMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

ADCMonitor::~ADCMonitor()
{

  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }

  // Delete histograms

  if (fHChanMeanOB) { delete fHChanMeanOB; fHChanMeanOB = 0; }
  if (fHChanMeanRM) { delete fHChanMeanRM; fHChanMeanRM = 0; }

  if (fHChanRMSOB)  { delete fHChanRMSOB; fHChanRMSOB = 0; }
  if (fHChanRMSRM)  { delete fHChanRMSRM; fHChanRMSRM = 0; }

  if (fHGroupSICOB) { delete fHGroupSICOB; fHGroupSICOB = 0; }
  if (fHGroupSICRM) { delete fHGroupSICRM; fHGroupSICRM = 0; }

  if (fHPedestalBM) { delete fHPedestalBM; fHPedestalBM = 0; }
  if (fHPedestalCS) { delete fHPedestalCS; fHPedestalCS = 0; }
  if (fHPedestalOB) { delete fHPedestalOB; fHPedestalOB = 0; }
  if (fHPedestalRM) { delete fHPedestalRM; fHPedestalRM = 0; }

  if (fHPedRMSBM) { delete fHPedRMSBM; fHPedRMSBM = 0; }
  if (fHPedRMSCS) { delete fHPedRMSCS; fHPedRMSCS = 0; }
  if (fHPedRMSOB) { delete fHPedRMSOB; fHPedRMSOB = 0; }
  if (fHPedRMSRM) { delete fHPedRMSRM; fHPedRMSRM = 0; }

  for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
    for(UChar_t c=0; c<32; c++) {
      if (fHChPedestal[b][c]) { delete fHChPedestal[b][c]; fHChPedestal[b][c] = 0; }
      if (fHChPedRMS[b][c])   { delete fHChPedRMS[b][c];   fHChPedRMS[b][c] = 0;   }
    }
  }

}

void ADCMonitor::Initialize()
{

  // Get ADC map from configuration file
  if(fConfigParser->HasConfig("ADC","ID")) {
    std::vector<std::string> bIDs = fConfigParser->GetConfig("ADC","ID");
    for(unsigned int ib = 0; ib < bIDs.size(); ib++) {
      unsigned int bID = std::stoi(bIDs[ib]);
      fADC_map[bID] = 1;
    }
  } else {
    printf("ADCMonitor::Initialize - WARNING - No board list in config file: using all boards\n");
    for(unsigned int ib = 0; ib < ADCMONITOR_NBOARDS; ib++) fADC_map[ib] = 1;
  }
  if (fConfig->Verbose() > 1) {
    printf("ADCMonitor - List of ADC boards to analyze -\n");
    for(unsigned int ib = 0; ib < ADCMONITOR_NBOARDS; ib++) if (fADC_map[ib]) printf("%d ",ib);
    printf("\n");
  }

  // Get beam triggers output rate from config file
  fBeamOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","BeamOutputRate") ) {
    try {
      fBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate"));
    } catch (...) {
      printf("ADCMonitor::Initialize - WARNING - 'BeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","BeamOutputRate").c_str(),fBeamOutputRate);
    }
  }

  // Get off-beam triggers output rate from config file
  fOffBeamOutputRate = 100;
  if ( fConfigParser->HasConfig("RECO","OffBeamOutputRate") ) {
    try {
      fOffBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate"));
    } catch (...) {
      printf("ADCMonitor::Initialize - WARNING - 'OffBeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","OffBeamOutputRate").c_str(),fOffBeamOutputRate);
    }
  }

  // Get cosmics triggers output rate from config file
  fCosmicsOutputRate = 100;
  if ( fConfigParser->HasConfig("RECO","CosmicsOutputRate") ) {
    try {
      fCosmicsOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate"));
    } catch (...) {
      printf("ADCMonitor::Initialize - WARNING - 'CosmicsOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","CosmicsOutputRate").c_str(),fCosmicsOutputRate);
    }
  }

  // Get random triggers output rate from config file
  fRandomOutputRate = 100;
  if ( fConfigParser->HasConfig("RECO","RandomOutputRate") ) {
    try {
      fRandomOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","RandomOutputRate"));
    } catch (...) {
      printf("ADCMonitor::Initialize - WARNING - 'RandomOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","RandomOutputRate").c_str(),fRandomOutputRate);
    }
  }

  // Define number of samples to use for pedestal for each board (default to 100, will use datacard)
  for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
    fPedestalSamples[b] = 100;
  }

  // Choose which pedstals to show (Beam, Off-beam, Cosmics, Random)
  fShowPedBeam    = true;
  fShowPedOffBeam = false;
  fShowPedCosmics = false;
  fShowPedRandom  = false;
  if ( fConfigParser->HasConfig("RECO","ShowPedestals") ) {
    std::string showPed = fConfigParser->GetSingleArg("RECO","ShowPedestals");
    if (showPed.compare("Beam") == 0) {
      fShowPedBeam    = true;
      fShowPedOffBeam = false;
      fShowPedCosmics = false;
      fShowPedRandom  = false;
    } else if (showPed.compare("Off-Beam") == 0) {
      fShowPedBeam    = false;
      fShowPedOffBeam = true;
      fShowPedCosmics = false;
      fShowPedRandom  = false;
    } else if (showPed.compare("Cosmics") == 0) {
      fShowPedBeam    = false;
      fShowPedOffBeam = false;
      fShowPedCosmics = true;
      fShowPedRandom  = false;
    } else if (showPed.compare("Random") == 0) {
      fShowPedBeam    = false;
      fShowPedOffBeam = false;
      fShowPedCosmics = false;
      fShowPedRandom  = true;
    }
  }
  

  // Create histograms

  fHChanMeanOB = new TH2D("ADC_ChanMeanOB","ADC_ChanMeanOB",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,4096.);
  fHChanMeanRM = new TH2D("ADC_ChanMeanRM","ADC_ChanMeanRM",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,4096.);

  fHChanRMSOB  = new TH2D("ADC_ChanRMSOB", "ADC_ChanRMSOB", 32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,100.);
  fHChanRMSRM  = new TH2D("ADC_ChanRMSRM", "ADC_ChanRMSRM", 32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,100.);

  fHGroupSICOB = new TH2D("ADC_GroupSICOB","ADC_GroupSICOB",10*ADCMONITOR_NBOARDS,0.,10.*ADCMONITOR_NBOARDS,2,0.,2.);
  fHGroupSICRM = new TH2D("ADC_GroupSICRM","ADC_GroupSICRM",10*ADCMONITOR_NBOARDS,0.,10.*ADCMONITOR_NBOARDS,2,0.,2.);

  fHPedestalBM = new TH2D("ADC_PedestalBM","ADC_PedestalBM",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,4096.);
  fHPedestalCS = new TH2D("ADC_PedestalCS","ADC_PedestalCS",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,4096.);
  fHPedestalOB = new TH2D("ADC_PedestalOB","ADC_PedestalOB",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,4096.);
  fHPedestalRM = new TH2D("ADC_PedestalRM","ADC_PedestalRM",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,4096.);

  fHPedRMSBM = new TH2D("ADC_PedRMSBM","ADC_PedRMSBM",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,100.);
  fHPedRMSCS = new TH2D("ADC_PedRMSCS","ADC_PedRMSCS",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,100.);
  fHPedRMSOB = new TH2D("ADC_PedRMSOB","ADC_PedRMSOB",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,100.);
  fHPedRMSRM = new TH2D("ADC_PedRMSRM","ADC_PedRMSRM",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,100.);

  TString hname;
  for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
    for(UChar_t c=0; c<32; c++) {
      hname.Form("ADC_Pedestal_%2.2d_%2.2d",b,c);
      if (b==28) { // Target dynamics is centered
	fHChPedestal[b][c] = new TH1D(hname.Data(),hname.Data(),80,1800.,2200.);
      } else if ( (b==23) && (c>=28) ) { // Cosmic pads dynamics is positive
	fHChPedestal[b][c] = new TH1D(hname.Data(),hname.Data(),80,700.,1100.);
      } else { // Everybody else has negative dynamics
	fHChPedestal[b][c] = new TH1D(hname.Data(),hname.Data(),160,3300.,4100.);
      }
      hname.Form("ADC_PedRMS_%2.2d_%2.2d",b,c);
      fHChPedRMS[b][c] = new TH1D(hname.Data(),hname.Data(),100,0.,50.);
    }
  }

  // Reset global counters
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

  // Reset histograms

  fHChanMeanOB->Reset();
  fHChanMeanRM->Reset();

  fHChanRMSOB->Reset();
  fHChanRMSRM->Reset();

  fHGroupSICOB->Reset();
  fHGroupSICRM->Reset();

  fHPedestalBM->Reset();
  fHPedestalCS->Reset();
  fHPedestalOB->Reset();
  fHPedestalRM->Reset();

  fHPedRMSBM->Reset();
  fHPedRMSCS->Reset();
  fHPedRMSOB->Reset();
  fHPedRMSRM->Reset();

  for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
    for(UChar_t c=0; c<32; c++) {
      fHChPedestal[b][c]->Reset();
      fHChPedRMS[b][c]->Reset();
    }
  }

}

void ADCMonitor::StartOfEvent()
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
 
  // Check if event was an off-beam trigger
  if (fConfig->GetEventTrigMask() & 0x80) {
    fIsOffBeam = true;
    fOffBeamEventCount++;
  } else {
    fIsOffBeam = false;
  }
  
  // Check if event was a random trigger
  if (fConfig->GetEventTrigMask() & 0x40) {
    fIsRandom = true;
    fRandomEventCount++;
  } else {
    fIsRandom = false;
  }

}

void ADCMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset histograms
      fHPedestalBM->Reset();
      fHPedRMSBM->Reset();
      if (fShowPedBeam) {
	for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
	  for(UChar_t c=0; c<32; c++) {
	    fHChPedestal[b][c]->Reset();
	    fHChPedRMS[b][c]->Reset();
	  }
	}
      }

    }

  } // End of beam output

  if (fIsOffBeam) {

    if (fOffBeamOutputRate && (fOffBeamEventCount % fOffBeamOutputRate == 0)) {

      // Write off-beam events data to output PadmeMonitor file
      OutputOffBeam();

      // Reset histograms
      fHChanMeanOB->Reset();
      fHChanRMSOB->Reset();
      fHGroupSICOB->Reset();
      fHPedestalOB->Reset();
      fHPedRMSOB->Reset();
      if (fShowPedOffBeam) {
	for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
	  for(UChar_t c=0; c<32; c++) {
	    fHChPedestal[b][c]->Reset();
	    fHChPedRMS[b][c]->Reset();
	  }
	}
      }

    }

  } // End of off-beam output

  if (fIsCosmics) {

    if (fCosmicsOutputRate && (fCosmicsEventCount % fCosmicsOutputRate == 0)) {

      // Write cosmics events data to output PadmeMonitor file
      OutputCosmics();

      // Reset histograms
      fHPedestalCS->Reset();
      fHPedRMSCS->Reset();
      if (fShowPedCosmics) {
	for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
	  for(UChar_t c=0; c<32; c++) {
	    fHChPedestal[b][c]->Reset();
	    fHChPedRMS[b][c]->Reset();
	  }
	}
      }

    }

  } // End of cosmics output

  if (fIsRandom) {

    if (fRandomOutputRate && (fRandomEventCount % fRandomOutputRate == 0)) {

      // Write random events data to output PadmeMonitor file
      OutputRandom();

      // Reset histograms
      fHChanMeanRM->Reset();
      fHChanRMSRM->Reset();
      fHGroupSICRM->Reset();
      fHPedestalRM->Reset();
      fHPedRMSRM->Reset();
      if (fShowPedRandom) {
	for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
	  for(UChar_t c=0; c<32; c++) {
	    fHChPedestal[b][c]->Reset();
	    fHChPedRMS[b][c]->Reset();
	  }
	}
      }

      // Clean mean and ped counters
      for(UInt_t i=0; i<ADCMONITOR_NBOARDS*32; i++) {
	fCountMeanRM[i] = 0.;
	fCountPedRM[i] = 0.;
      }

    }

  } // End of cosmics output

}

void ADCMonitor::Finalize()
{
  printf("ADCMonitor::Finalize - Total number of analyzed events: %d\n",fBeamEventCount+fOffBeamEventCount+fCosmicsEventCount+fRandomEventCount);
  printf("ADCMonitor::Finalize - Total number of beam events:     %d\n",fBeamEventCount);
  printf("ADCMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  printf("ADCMonitor::Finalize - Total number of cosmics events:  %d\n",fCosmicsEventCount);
  printf("ADCMonitor::Finalize - Total number of random events:   %d\n",fRandomEventCount);
}

void ADCMonitor::AnalyzeBoard(UChar_t board)
{

  if (fADC_map[board] == 0) return; // Only analyze enabled boards

  // Check if each SIC is compatible with the other three
  for(UChar_t t=0;t<4;t++){
    //UShort_t address = board*4+t;
    UShort_t address = board*10+t;
    UChar_t nok = 0;
    for(UChar_t tt=0;tt<4;tt++){
      if (tt==t) continue;
      UShort_t diff = abs(fConfig->GetBoardGroupSIC(t)-fConfig->GetBoardGroupSIC(tt));
      if ( (diff < 20) || (1024-diff < 20) ) nok++; // Keep rollover into account
    }
    if (nok==0) {
      //printf("Board %d SIC:",board);
      //for(UChar_t tt=0;tt<4;tt++) printf(" %d",fConfig->GetBoardGroupSIC(tt));
      //printf("\n");
      if (fIsOffBeam) fHGroupSICOB->Fill(address,1);
      if (fIsRandom) fHGroupSICRM->Fill(address,1);
    } else {
      if (fIsOffBeam) fHGroupSICOB->Fill(address,0);
      if (fIsRandom) fHGroupSICRM->Fill(address,0);
    }
  }

}

void ADCMonitor::AnalyzeChannel(UChar_t board,UChar_t channel,Short_t* samples)
{

  if (fADC_map[board] == 0) return; // Only analyze enabled boards
  UShort_t address = board*32+channel;

  // Compute full signal RMS for off-beam and random events
  if (fIsOffBeam || fIsRandom) {
    ComputeChannelMeanRMS(board,channel,samples);
    if (fIsOffBeam) {
      fHChanMeanOB->Fill(address,fChannelMean);
      fHChanRMSOB->Fill(address,fChannelRMS);
    }
    if (fIsRandom) {
      fHChanMeanRM->Fill(address,fChannelMean);
      fHChanRMSRM->Fill(address,fChannelRMS);
      fCountMeanRM[address] += fChannelMean;
    }
  }

  // Compute pedestal for all events
  ComputeChannelPedestal(board,channel,samples);
  if (fIsBeam) {
    fHPedestalBM->Fill(address,fPedestal);
    fHPedRMSBM->Fill(address,fPedRMS);
    if (fShowPedBeam) {
      fHChPedestal[board][channel]->Fill(fPedestal);
      fHChPedRMS[board][channel]->Fill(fPedRMS);
    }
  }
  if (fIsCosmics) {
    fHPedestalCS->Fill(address,fPedestal);
    fHPedRMSCS->Fill(address,fPedRMS);
    if (fShowPedCosmics) {
      fHChPedestal[board][channel]->Fill(fPedestal);
      fHChPedRMS[board][channel]->Fill(fPedRMS);
    }
  }
  if (fIsOffBeam) {
    fHPedestalOB->Fill(address,fPedestal);
    fHPedRMSOB->Fill(address,fPedRMS);
    if (fShowPedOffBeam) {
      fHChPedestal[board][channel]->Fill(fPedestal);
      fHChPedRMS[board][channel]->Fill(fPedRMS);
    }
  }
  if (fIsRandom) {
    fHPedestalRM->Fill(address,fPedestal);
    fHPedRMSRM->Fill(address,fPedRMS);
    fCountPedRM[address] += fPedestal;
    if (fShowPedRandom) {
      fHChPedestal[board][channel]->Fill(fPedestal);
      fHChPedRMS[board][channel]->Fill(fPedRMS);
    }
  }

}

void ADCMonitor::ComputeChannelMeanRMS(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get channel mean and RMS from first 994 samples
  Long64_t sum = 0;
  Long64_t sum2 = 0;
  for(UInt_t s = 0; s<994; s++) {
    sum += samples[s];
    sum2 += samples[s]*samples[s];
  }
  fChannelMean = (Double_t)sum/994.;
  fChannelRMS = sqrt( ((Double_t)sum2 - (Double_t)sum*fChannelMean)/993. );
}

void ADCMonitor::ComputeChannelPedestal(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get channel pedestal and its RMS from first samples
  Long64_t sum = 0;
  Long64_t sum2 = 0;
  for(UInt_t s = 0; s<fPedestalSamples[board]; s++) {
    sum += samples[s];
    sum2 += samples[s]*samples[s];
  }
  fPedestal = (Double_t)sum/(Double_t)fPedestalSamples[board];
  fPedRMS = sqrt( ((Double_t)sum2 - (Double_t)sum*fPedestal)/((Double_t)fPedestalSamples[board]-1.) );
}

Int_t ADCMonitor::OutputBeam()
{

  if (fConfig->Verbose()>0) printf("ADCMonitor::OutputBeam - Writing beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/ADCMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ADCMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"a");
 
  fprintf(outf,"PLOTID ADCMon_beampedestal\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Beam Pedestal - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHPedestalBM->GetNbinsX(),fHPedestalBM->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedestalBM->GetXaxis()->GetXmin(),fHPedestalBM->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHPedestalBM->GetYaxis()->GetXmin(),fHPedestalBM->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Pedestal\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHPedestalBM->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHPedestalBM->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHPedestalBM->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
 
  fprintf(outf,"PLOTID ADCMon_beampedrms\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Beam Pedestal RMS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHPedRMSBM->GetNbinsX(),fHPedRMSBM->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedRMSBM->GetXaxis()->GetXmin(),fHPedRMSBM->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHPedRMSBM->GetYaxis()->GetXmin(),fHPedRMSBM->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Pedestal\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHPedRMSBM->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHPedRMSBM->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHPedRMSBM->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");

  if (fShowPedBeam) {

    for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
      for(UChar_t c=0; c<32; c++) {

	fprintf(outf,"PLOTID ADCMon_chpedestal_%2.2d_%2.2d\n",b,c);
	fprintf(outf,"PLOTTYPE histo1d\n");
	fprintf(outf,"PLOTNAME BM Ped B%2.2d C%2.2d R%d\n",b,c,fConfig->GetRunNumber());
	fprintf(outf,"CHANNELS %d\n",fHChPedestal[b][c]->GetNbinsX());
	fprintf(outf,"RANGE_X %.3f %.3f\n",fHChPedestal[b][c]->GetXaxis()->GetXmin(),fHChPedestal[b][c]->GetXaxis()->GetXmax());
	fprintf(outf,"TITLE_X Counts\n");
	fprintf(outf,"DATA [ [");
	for(Int_t i = 1; i <= fHChPedestal[b][c]->GetNbinsX(); i++) {
	  if (i>1) fprintf(outf,",");
	  fprintf(outf,"%.0f",fHChPedestal[b][c]->GetBinContent(i));
	}
	fprintf(outf,"] ]\n\n");

	fprintf(outf,"PLOTID ADCMon_chpedrms_%2.2d_%2.2d\n",b,c);
	fprintf(outf,"PLOTTYPE histo1d\n");
	fprintf(outf,"PLOTNAME BM RMS B%2.2d C%2.2d R%d\n",b,c,fConfig->GetRunNumber());
	fprintf(outf,"CHANNELS %d\n",fHChPedRMS[b][c]->GetNbinsX());
	fprintf(outf,"RANGE_X %.3f %.3f\n",fHChPedRMS[b][c]->GetXaxis()->GetXmin(),fHChPedRMS[b][c]->GetXaxis()->GetXmax());
	fprintf(outf,"TITLE_X Counts\n");
	fprintf(outf,"DATA [ [");
	for(Int_t i = 1; i <= fHChPedRMS[b][c]->GetNbinsX(); i++) {
	  if (i>1) fprintf(outf,",");
	  fprintf(outf,"%.0f",fHChPedRMS[b][c]->GetBinContent(i));
	}
	fprintf(outf,"] ]\n\n");

      }
    }

  }

  // Close monitor file and move it to watchdir
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ADCMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}

Int_t ADCMonitor::OutputOffBeam()
{

  if (fConfig->Verbose()>0) printf("ADCMonitor::OutputOffBeam - Writing off-beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/ADCMon_OffBeam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ADCMon_OffBeam.txt";
  FILE* outf = fopen(ftname.Data(),"a");
 
  fprintf(outf,"PLOTID ADCMon_offbeampedestal\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Off-Beam Pedestal - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHPedestalOB->GetNbinsX(),fHPedestalOB->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedestalOB->GetXaxis()->GetXmin(),fHPedestalOB->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHPedestalOB->GetYaxis()->GetXmin(),fHPedestalOB->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Pedestal\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHPedestalOB->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHPedestalOB->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHPedestalOB->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
 
  fprintf(outf,"PLOTID ADCMon_offbeampedrms\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Off-Beam Pedestal RMS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHPedRMSOB->GetNbinsX(),fHPedRMSOB->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedRMSOB->GetXaxis()->GetXmin(),fHPedRMSOB->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHPedRMSOB->GetYaxis()->GetXmin(),fHPedRMSOB->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Pedestal\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHPedRMSOB->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHPedRMSOB->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHPedRMSOB->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");

  fprintf(outf,"PLOTID ADCMon_offbeammean\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Off-Beam Mean - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHChanMeanOB->GetNbinsX(),fHChanMeanOB->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHChanMeanOB->GetXaxis()->GetXmin(),fHChanMeanOB->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHChanMeanOB->GetYaxis()->GetXmin(),fHChanMeanOB->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Mean\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHChanMeanOB->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHChanMeanOB->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHChanMeanOB->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
 
  fprintf(outf,"PLOTID ADCMon_offbeamrms\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Off-Beam RMS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d 100\n",ADCMONITOR_NBOARDS*32);
  fprintf(outf,"RANGE_X 0 %d\n",ADCMONITOR_NBOARDS*32);
  fprintf(outf,"RANGE_Y 0 100\n");
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y RMS\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHChanRMSOB->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHChanRMSOB->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHChanRMSOB->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
 
  fprintf(outf,"PLOTID ADCMon_offbeamsic\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME Off-beam Events ADC Group SIC Errors - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d\n",fHGroupSICOB->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHGroupSICOB->GetXaxis()->GetXmin(),fHGroupSICOB->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X Counts\n");
  fprintf(outf,"DATA [ [");
  for(Int_t b = 1; b <= fHGroupSICOB->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHGroupSICOB->GetBinContent(b,2));
  }
  fprintf(outf,"] ]\n\n");

  if (fShowPedOffBeam) {

    for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
      for(UChar_t c=0; c<32; c++) {

	fprintf(outf,"PLOTID ADCMon_chpedestal_%2.2d_%2.2d\n",b,c);
	fprintf(outf,"PLOTTYPE histo1d\n");
	fprintf(outf,"PLOTNAME OB Ped B%2.2d C%2.2d R%d\n",b,c,fConfig->GetRunNumber());
	fprintf(outf,"CHANNELS %d\n",fHChPedestal[b][c]->GetNbinsX());
	fprintf(outf,"RANGE_X %.3f %.3f\n",fHChPedestal[b][c]->GetXaxis()->GetXmin(),fHChPedestal[b][c]->GetXaxis()->GetXmax());
	fprintf(outf,"TITLE_X Counts\n");
	fprintf(outf,"DATA [ [");
	for(Int_t i = 1; i <= fHChPedestal[b][c]->GetNbinsX(); i++) {
	  if (i>1) fprintf(outf,",");
	  fprintf(outf,"%.0f",fHChPedestal[b][c]->GetBinContent(i));
	}
	fprintf(outf,"] ]\n\n");

	fprintf(outf,"PLOTID ADCMon_chpedrms_%2.2d_%2.2d\n",b,c);
	fprintf(outf,"PLOTTYPE histo1d\n");
	fprintf(outf,"PLOTNAME OB RMS B%2.2d C%2.2d R%d\n",b,c,fConfig->GetRunNumber());
	fprintf(outf,"CHANNELS %d\n",fHChPedRMS[b][c]->GetNbinsX());
	fprintf(outf,"RANGE_X %.3f %.3f\n",fHChPedRMS[b][c]->GetXaxis()->GetXmin(),fHChPedRMS[b][c]->GetXaxis()->GetXmax());
	fprintf(outf,"TITLE_X Counts\n");
	fprintf(outf,"DATA [ [");
	for(Int_t i = 1; i <= fHChPedRMS[b][c]->GetNbinsX(); i++) {
	  if (i>1) fprintf(outf,",");
	  fprintf(outf,"%.0f",fHChPedRMS[b][c]->GetBinContent(i));
	}
	fprintf(outf,"] ]\n\n");

      }
    }

  }

  // Close monitor file
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ADCMonitor::OutputOffBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}

Int_t ADCMonitor::OutputCosmics()
{
  if (fConfig->Verbose()>0) printf("ADCMonitor::OutputCosmics - Writing cosmics output files\n");

  // Write cosmics-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/ADCMon_Cosmics.txt";
  TString ffname = fConfig->OutputDirectory()+"/ADCMon_Cosmics.txt";
  FILE* outf = fopen(ftname.Data(),"a");
 
  fprintf(outf,"PLOTID ADCMon_cosmicspedestal\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Cosmics Pedestal - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHPedestalCS->GetNbinsX(),fHPedestalCS->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedestalCS->GetXaxis()->GetXmin(),fHPedestalCS->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHPedestalCS->GetYaxis()->GetXmin(),fHPedestalCS->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Pedestal\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHPedestalCS->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHPedestalCS->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHPedestalCS->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
 
  fprintf(outf,"PLOTID ADCMon_cosmicspedrms\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Cosmics Pedestal RMS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHPedRMSCS->GetNbinsX(),fHPedRMSCS->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedRMSCS->GetXaxis()->GetXmin(),fHPedRMSCS->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHPedRMSCS->GetYaxis()->GetXmin(),fHPedRMSCS->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Pedestal\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHPedRMSCS->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHPedRMSCS->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHPedRMSCS->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");

  if (fShowPedCosmics) {

    for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
      for(UChar_t c=0; c<32; c++) {

	fprintf(outf,"PLOTID ADCMon_chpedestal_%2.2d_%2.2d\n",b,c);
	fprintf(outf,"PLOTTYPE histo1d\n");
	fprintf(outf,"PLOTNAME CS Ped B%2.2d C%2.2d R%d\n",b,c,fConfig->GetRunNumber());
	fprintf(outf,"CHANNELS %d\n",fHChPedestal[b][c]->GetNbinsX());
	fprintf(outf,"RANGE_X %.3f %.3f\n",fHChPedestal[b][c]->GetXaxis()->GetXmin(),fHChPedestal[b][c]->GetXaxis()->GetXmax());
	fprintf(outf,"TITLE_X Counts\n");
	fprintf(outf,"DATA [ [");
	for(Int_t i = 1; i <= fHChPedestal[b][c]->GetNbinsX(); i++) {
	  if (i>1) fprintf(outf,",");
	  fprintf(outf,"%.0f",fHChPedestal[b][c]->GetBinContent(i));
	}
	fprintf(outf,"] ]\n\n");

	fprintf(outf,"PLOTID ADCMon_chpedrms_%2.2d_%2.2d\n",b,c);
	fprintf(outf,"PLOTTYPE histo1d\n");
	fprintf(outf,"PLOTNAME CS RMS B%2.2d C%2.2d R%d\n",b,c,fConfig->GetRunNumber());
	fprintf(outf,"CHANNELS %d\n",fHChPedRMS[b][c]->GetNbinsX());
	fprintf(outf,"RANGE_X %.3f %.3f\n",fHChPedRMS[b][c]->GetXaxis()->GetXmin(),fHChPedRMS[b][c]->GetXaxis()->GetXmax());
	fprintf(outf,"TITLE_X Counts\n");
	fprintf(outf,"DATA [ [");
	for(Int_t i = 1; i <= fHChPedRMS[b][c]->GetNbinsX(); i++) {
	  if (i>1) fprintf(outf,",");
	  fprintf(outf,"%.0f",fHChPedRMS[b][c]->GetBinContent(i));
	}
	fprintf(outf,"] ]\n\n");

      }
    }

  }

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ADCMonitor::OutputCosmics - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}

Int_t ADCMonitor::OutputRandom()
{

  if (fConfig->Verbose()>0) printf("ADCMonitor::OutputRandom - Writing random output files\n");

  // Write random events monitor file
  TString ftname = fConfig->TmpDirectory()+"/ADCMon_Random.txt";
  TString ffname = fConfig->OutputDirectory()+"/ADCMon_Random.txt";
  FILE* outf = fopen(ftname.Data(),"a");
 
  fprintf(outf,"PLOTID ADCMon_randompedestal\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Random Pedestal - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHPedestalRM->GetNbinsX(),fHPedestalRM->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedestalRM->GetXaxis()->GetXmin(),fHPedestalRM->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHPedestalRM->GetYaxis()->GetXmin(),fHPedestalRM->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Pedestal\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHPedestalRM->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHPedestalRM->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHPedestalRM->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
 
  fprintf(outf,"PLOTID ADCMon_randompedrms\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Random Pedestal RMS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHPedRMSRM->GetNbinsX(),fHPedRMSRM->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedRMSRM->GetXaxis()->GetXmin(),fHPedRMSRM->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHPedRMSRM->GetYaxis()->GetXmin(),fHPedRMSRM->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Pedestal\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHPedRMSRM->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHPedRMSRM->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHPedRMSRM->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");

  fprintf(outf,"PLOTID ADCMon_randommean\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Random Mean - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHChanMeanRM->GetNbinsX(),fHChanMeanRM->GetNbinsY());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHChanMeanRM->GetXaxis()->GetXmin(),fHChanMeanRM->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.3f %.3f\n",fHChanMeanRM->GetYaxis()->GetXmin(),fHChanMeanRM->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Mean\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHChanMeanRM->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHChanMeanRM->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHChanMeanRM->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
 
  fprintf(outf,"PLOTID ADCMon_randomrms\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Random RMS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d 100\n",ADCMONITOR_NBOARDS*32);
  fprintf(outf,"RANGE_X 0 %d\n",ADCMONITOR_NBOARDS*32);
  fprintf(outf,"RANGE_Y 0 100\n");
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y RMS\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHChanRMSRM->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHChanRMSRM->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHChanRMSRM->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");

  Double_t ratio = 0.;
  fprintf(outf,"PLOTID ADCMon_randomratio\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME ADC Random Ped-to-Mean ratio - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d\n",fHPedestalRM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHPedestalRM->GetXaxis()->GetXmin(),fHPedestalRM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Ratio\n");
  fprintf(outf,"DATA [ [");
  for(Int_t b = 0; b < ADCMONITOR_NBOARDS*32; b++) {
    if (fCountMeanRM[b] != 0.) {
      ratio = fCountPedRM[b]/fCountMeanRM[b];
    } else {
      ratio = 0.;
    }
    if (b>0) fprintf(outf,",");
    fprintf(outf,"%.4f",ratio);
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID ADCMon_randomsic\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME Random Events ADC Group SIC Errors - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d\n",fHGroupSICRM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHGroupSICRM->GetXaxis()->GetXmin(),fHGroupSICRM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X Group\n");
  fprintf(outf,"TITLE_Y Errors\n");
  fprintf(outf,"DATA [ [");
  for(Int_t b = 1; b <= fHGroupSICRM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHGroupSICRM->GetBinContent(b,2));
  }
  fprintf(outf,"] ]\n\n");

  if (fShowPedRandom) {

    for(UChar_t b=0; b<ADCMONITOR_NBOARDS; b++) {
      for(UChar_t c=0; c<32; c++) {

	fprintf(outf,"PLOTID ADCMon_chpedestal_%2.2d_%2.2d\n",b,c);
	fprintf(outf,"PLOTTYPE histo1d\n");
	fprintf(outf,"PLOTNAME RM Ped B%2.2d C%2.2d R%d\n",b,c,fConfig->GetRunNumber());
	fprintf(outf,"CHANNELS %d\n",fHChPedestal[b][c]->GetNbinsX());
	fprintf(outf,"RANGE_X %.3f %.3f\n",fHChPedestal[b][c]->GetXaxis()->GetXmin(),fHChPedestal[b][c]->GetXaxis()->GetXmax());
	fprintf(outf,"TITLE_X Counts\n");
	fprintf(outf,"DATA [ [");
	for(Int_t i = 1; i <= fHChPedestal[b][c]->GetNbinsX(); i++) {
	  if (i>1) fprintf(outf,",");
	  fprintf(outf,"%.0f",fHChPedestal[b][c]->GetBinContent(i));
	}
	fprintf(outf,"] ]\n\n");

	fprintf(outf,"PLOTID ADCMon_chpedrms_%2.2d_%2.2d\n",b,c);
	fprintf(outf,"PLOTTYPE histo1d\n");
	fprintf(outf,"PLOTNAME RM RMS B%2.2d C%2.2d R%d\n",b,c,fConfig->GetRunNumber());
	fprintf(outf,"CHANNELS %d\n",fHChPedRMS[b][c]->GetNbinsX());
	fprintf(outf,"RANGE_X %.3f %.3f\n",fHChPedRMS[b][c]->GetXaxis()->GetXmin(),fHChPedRMS[b][c]->GetXaxis()->GetXmax());
	fprintf(outf,"TITLE_X Counts\n");
	fprintf(outf,"DATA [ [");
	for(Int_t i = 1; i <= fHChPedRMS[b][c]->GetNbinsX(); i++) {
	  if (i>1) fprintf(outf,",");
	  fprintf(outf,"%.0f",fHChPedRMS[b][c]->GetBinContent(i));
	}
	fprintf(outf,"] ]\n\n");

      }
    }

  }

  // Close monitor file
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ADCMonitor::OutputRandom - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}
