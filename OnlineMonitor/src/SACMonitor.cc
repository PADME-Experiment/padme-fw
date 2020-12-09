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

  // Get cosmics output rate from config file
  fCosmicsOutputRate = fConfigParser->HasConfig("RECO","CosmicsOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate")):100;

/*
  // Get calibration constants from file
  for (UChar_t b=0; b<29; b++) {
    for (UChar_t c=0; c<32; c++) {
	fSAC_CosmClb[b][c] = 0.;
    }
  }
  TString calibFile = "config/SACCosmicsCalibration.dat";
  if ( fConfigParser->HasConfig("RECO","CosmicsCalibrationFile") )
    calibFile = fConfigParser->GetSingleArg("RECO","CosmicsCalibrationFile");
  struct stat buffer;   
  if (stat (calibFile.Data(),&buffer) != 0) {
    printf("SACMonitor::Initialize - ERROR - calibration constants file %s does not exist\n",calibFile.Data());
    exit(EXIT_FAILURE);
  }
  if (fConfig->Verbose() > 0) printf("SACMonitor::Initialize - Reading cosmics calibration file %s\n",calibFile.Data());
  int x,y,b,c;
  float calib;
  FILE* cf = fopen(calibFile.Data(),"r");
  while(fscanf(cf,"%d %d %d %d %f\n",&x,&y,&b,&c,&calib) != EOF) {
    if (fConfig->Verbose() > 1) printf("Board %d Channel %d Calibration %f\n",b,c,calib);
    fECal_CosmClb[b][c] = calib;
  }
  fclose(cf);
*/

  // Reset global counters
  ResetSACMap(fSAC_CosmEvt);
  ResetSACMap(fSAC_CosmSum);
  ResetSACWaveforms(fCosmWF);
  fCosmicsEventCount = 0;

}

void SACMonitor::StartOfEvent()
{
  // Check if event is cosmics
  if (fConfig->GetEventTrigMask() & 0x02) {
    fIsCosmics = true;
  } else {
    fIsCosmics = false;
  }
  
}

void SACMonitor::EndOfEvent()
{
  if (fIsCosmics) {

    if (fCosmicsEventCount % fCosmicsOutputRate == 0) {

      if (fConfig->Verbose()>0) printf("SACMonitor::EndOfEvent - Writing output files\n");

      // Write current cosmics map
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
	  Bool_t first = true;
	  for(UInt_t j = 0; j<1024; j++) {
	    if (first) { first = false; } else { fprintf(outf,","); }
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
      if ( std::rename(ftname,ffname) != 0 ) perror("Error renaming file");

      // Reset cosmics maps and waveforms
      ResetSACMap(fSAC_CosmEvt);
      ResetSACMap(fSAC_CosmSum);
      ResetSACWaveforms(fCosmWF);
    }

    // Count cosmics event
    fCosmicsEventCount++;

  }

}

void SACMonitor::Finalize()
{
  printf("SACMonitor::Finalize - Total number of cosmics events: %d\n",fCosmicsEventCount);
}

void SACMonitor::Analyze(UChar_t board,UChar_t channel,Short_t* samples)
{

  if (board != 27) {
    printf("SACMonitor::Analyze - WARNING - ECal board %d does not belong to SAC\n",board);
    return;
  } else if (channel >= 25) {
    // For the moment we do not use cosmics pads samples
    return;
  } else if (fSAC_map[channel] == -1) {
    printf("SACMonitor::Analyze - WARNING - ECal board %d channel %d is disabled in the map\n",board,channel);
    return;
  }

  // Get x,y coordinates from channel map
  UChar_t x = fSAC_map[channel]/10;
  UChar_t y = fSAC_map[channel]%10;

  // Save cosmics related information
  if (fIsCosmics) {

    // Save energy in cosmics map
    Double_t chEnergy = GetChannelEnergy(board,channel,samples);
    fSAC_CosmSum[x][y] += chEnergy;

    // Find peaks
    GetChannelPeaks(board,channel,samples);

    //// Save waveform and event map once every few events
    //if (fCosmicsEventCount % fCosmicsOutputRate == 0) {
    //  fSAC_CosmEvt[x][y] = chEnergy;
    //  for(UInt_t i = 0; i<1024; i++) fCosmWF[x][y][i] = samples[i];
    //}
    // Only save channels with peaks
    if (fNPeaks) {
      printf("Found %d peaks\n",fNPeaks);
      for(UInt_t i = 0; i<1024; i++) fCosmWF[x][y][i] = samples[i];
    }
  }

}

Double_t SACMonitor::GetChannelEnergy(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get total signal area using first 100 samples as pedestal and dropping last 30 samples
  Int_t sum = 0;
  Int_t sum_ped = 0;
  for(UInt_t s = 0; s<994; s++) {
    sum += samples[s];
    if (s<100) sum_ped += samples[s];
  }
  Double_t tot = 9.94*sum_ped-1.*sum;
  // Convert to pC (check formula)
  //tot = tot/(4096.*50.)*(1.E-9/1.E-12);
  tot *= 4.8828E-3;
  return tot;
}

void SACMonitor::GetChannelPeaks(UChar_t board,UChar_t channel,Short_t* samples)
{

  fNPeaks = 0;

  // Get pedestal value and RMS from first 100 samples
  Int_t sum = 0;
  Int_t sum2 = 0;
  for(UInt_t s = 0; s<100; s++) {
    sum += samples[s];
    sum2 += samples[s]*samples[2];
  }
  Double_t ped = (Double_t)sum/100.;
  Double_t ped_rms = sqrt( ((Double_t)sum2 - (Double_t)sum*ped)/99. );

  // Count samples (above the 100th) with value < pedestal - 5.*RMS
  Double_t threshold = ped - 5.*ped_rms;
  for(UInt_t s = 100; s<994; s++) {
    if ((Double_t)samples[s] < threshold) {
      if (fNPeaks < SACMONITOR_PEAKS_MAX) fPeakTime[fNPeaks] = (Double_t)s;
      fNPeaks++;
    }
  }

  if (fNPeaks) printf("SACMonitor::GetChannelPeaks - Event %d Channel %d NPeaks %d\n",fConfig->GetEventNumber(),channel,fNPeaks);

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

void SACMonitor::ResetSACWaveforms(Short_t map[5][5][1024])
{
  // Set all crystals to 0
  for (UChar_t x=0; x<5; x++) {
    for (UChar_t y=0; y<5; y++) {
      for (UShort_t s=0; s<1024; s++) {
      	map[x][y][s] = 0;
      }
    }
  }
}
