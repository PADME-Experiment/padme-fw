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
  if (fHChanRMSOB) delete fHChanRMSOB;
  if (fHChanRMSRM) delete fHChanRMSRM;
  if (fHGroupSICOB) delete fHGroupSICOB;
  if (fHGroupSICRM) delete fHGroupSICRM;

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
  fBeamOutputRate = 0;
  if ( fConfigParser->HasConfig("RECO","BeamOutputRate") ) {
    try {
      fBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate"));
    } catch (...) {
      printf("ADCMonitor::Initialize - WARNING - 'BeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","BeamOutputRate").c_str(),fBeamOutputRate);
    }
  }

  // Get off-beam triggers output rate from config file
  fOffBeamOutputRate = 500;
  if ( fConfigParser->HasConfig("RECO","OffBeamOutputRate") ) {
    try {
      fOffBeamOutputRate = std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate"));
    } catch (...) {
      printf("ADCMonitor::Initialize - WARNING - 'OffBeamOutputRate' parameter has non-integer value '%s': using default %d\n",fConfigParser->GetSingleArg("RECO","OffBeamOutputRate").c_str(),fOffBeamOutputRate);
    }
  }

  // Get cosmics triggers output rate from config file
  fCosmicsOutputRate = 0;
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

  // Create histograms
  fHChanRMSOB = new TH2D("ADC_ChanRMSOB","ADC_ChanRMSOB",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,100.);
  fHChanRMSRM = new TH2D("ADC_ChanRMSRM","ADC_ChanRMSRM",32*ADCMONITOR_NBOARDS,0.,32.*ADCMONITOR_NBOARDS,100,0.,100.);
  //fHGroupSICOB = new TH2D("ADC_GroupSICOB","ADC_GroupSICOB",4*ADCMONITOR_NBOARDS,0.,4.*ADCMONITOR_NBOARDS,2,0.,2.);
  //fHGroupSICRM = new TH2D("ADC_GroupSICRM","ADC_GroupSICRM",4*ADCMONITOR_NBOARDS,0.,4.*ADCMONITOR_NBOARDS,2,0.,2.);
  fHGroupSICOB = new TH2D("ADC_GroupSICOB","ADC_GroupSICOB",10*ADCMONITOR_NBOARDS,0.,10.*ADCMONITOR_NBOARDS,2,0.,2.);
  fHGroupSICRM = new TH2D("ADC_GroupSICRM","ADC_GroupSICRM",10*ADCMONITOR_NBOARDS,0.,10.*ADCMONITOR_NBOARDS,2,0.,2.);

  // Reset global counters
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

  // Reset histograms
  fHChanRMSOB->Reset();
  fHChanRMSRM->Reset();
  fHGroupSICOB->Reset();
  fHGroupSICRM->Reset();

}

void ADCMonitor::StartOfEvent()
{

  // Check if event was triggered by BTF beam
  if (fConfig->GetEventTrigMask() & 0x01) {
    fIsBeam = true;
  } else {
    fIsBeam = false;
  }

  // Check if event was triggered by cosmics
  if (fConfig->GetEventTrigMask() & 0x02) {
    fIsCosmics = true;
  } else {
    fIsCosmics = false;
  }
 
  // Check if event was an off-beam trigger
  if (fConfig->GetEventTrigMask() & 0x80) {
    fIsOffBeam = true;
  } else {
    fIsOffBeam = false;
  }
  
  // Check if event was a random trigger
  if (fConfig->GetEventTrigMask() & 0x40) {
    fIsRandom = true;
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

    }

    // Count beam event
    fBeamEventCount++;

  } // End of beam output

  if (fIsOffBeam) {

    if (fOffBeamOutputRate && (fOffBeamEventCount % fOffBeamOutputRate == 0)) {

      // Write off-beam events data to output PadmeMonitor file
      OutputOffBeam();

      // Reset histograms
      fHChanRMSOB->Reset();

    }

    // Count off-beam event
    fOffBeamEventCount++;

  } // End of off-beam output

  if (fIsCosmics) {

    if (fCosmicsOutputRate && (fCosmicsEventCount % fCosmicsOutputRate == 0)) {

      // Write cosmics events data to output PadmeMonitor file
      OutputCosmics();

    }

    // Count cosmics event
    fCosmicsEventCount++;

  } // End of cosmics output

  if (fIsRandom) {

    if (fRandomOutputRate && (fRandomEventCount % fRandomOutputRate == 0)) {

      // Write random events data to output PadmeMonitor file
      OutputRandom();

      // Reset histograms
      fHChanRMSRM->Reset();

    }

    // Count random event
    fRandomEventCount++;

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

void ADCMonitor::Analyze(UChar_t board)
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
    ComputeChannelRMS(board,channel,samples);
    if (fIsOffBeam) fHChanRMSOB->Fill(address,fChannelRMS);
    if (fIsRandom) fHChanRMSRM->Fill(address,fChannelRMS);
  }

}

void ADCMonitor::ComputeChannelRMS(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get full channel RMS from first 994 samples
  Long64_t sum = 0;
  Long64_t sum2 = 0;
  for(UInt_t s = 0; s<994; s++) {
    sum += samples[s];
    sum2 += samples[s]*samples[s];
  }
  Double_t avg = (Double_t)sum/994.;
  fChannelRMS = sqrt( ((Double_t)sum2 - (Double_t)sum*avg)/993. );
}

Int_t ADCMonitor::OutputBeam()
{

  if (fConfig->Verbose()>0) printf("ADCMonitor::OutputBeam - Writing beam output files\n");

  // Write beam-related monitor file
  TString ftname = fConfig->TmpDirectory()+"/ADCMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ADCMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

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
  fprintf(outf,"]\n");
 
  fprintf(outf,"PLOTID ADCMon_offbeamsic\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Off-Beam Group SIC - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS %d %d\n",fHGroupSICOB->GetNbinsX(),fHGroupSICOB->GetNbinsY());
  fprintf(outf,"RANGE_X %f %f\n",fHGroupSICOB->GetXaxis()->GetXmin(),fHGroupSICOB->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %f %f\n",fHGroupSICOB->GetYaxis()->GetXmin(),fHGroupSICOB->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Group\n");
  fprintf(outf,"TITLE_Y SIC\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHGroupSICOB->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHGroupSICOB->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHGroupSICOB->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");

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
  fprintf(outf,"]\n");
 
  fprintf(outf,"PLOTID ADCMon_randomsic\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ADC Random Group SIC - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  //fprintf(outf,"CHANNELS %d 2\n",ADCMONITOR_NBOARDS*4);
  //fprintf(outf,"RANGE_X 0 %d\n",ADCMONITOR_NBOARDS*4);
  //fprintf(outf,"RANGE_Y 0 2\n");
  fprintf(outf,"CHANNELS %d %d\n",fHGroupSICRM->GetNbinsX(),fHGroupSICRM->GetNbinsY());
  fprintf(outf,"RANGE_X %f %f\n",fHGroupSICRM->GetXaxis()->GetXmin(),fHGroupSICRM->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %f %f\n",fHGroupSICRM->GetYaxis()->GetXmin(),fHGroupSICRM->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X Group\n");
  fprintf(outf,"TITLE_Y SIC\n");
  fprintf(outf,"DATA [");
  for(Int_t y = 1; y <= fHGroupSICRM->GetNbinsY(); y++) {
    if (y>1) fprintf(outf,",");
    fprintf(outf,"[");
    for(Int_t x = 1; x <= fHGroupSICRM->GetNbinsX(); x++) {
      if (x>1) fprintf(outf,",");
      fprintf(outf,"%.0f",fHGroupSICRM->GetBinContent(x,y));
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");

  // Close monitor file
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ADCMonitor::OutputRandom - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;

}
