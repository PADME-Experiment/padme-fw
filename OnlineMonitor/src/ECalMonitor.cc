#include <stdio.h>
#include <sys/stat.h>

#include "ECalMonitor.hh"

#include "LeadGlassMonitor.hh" // Need to know LeadGlass board/channel to avoid warnings
#include "Configuration.hh"

#include "TMath.h"

ECalMonitor::ECalMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

ECalMonitor::~ECalMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }
}

void ECalMonitor::Initialize()
{

  // Get ECal map from configuration file
  if(fConfigParser->HasConfig("ADC","ID")){
    std::vector<std::string> bIDs = fConfigParser->GetConfig("ADC","ID");
    for(unsigned int ib = 0; ib < bIDs.size(); ib++) {
      std::string parName = "ADC"+bIDs[ib];
      if (fConfigParser->HasConfig("ADC",parName )) {
	std::vector<std::string> bMap = fConfigParser->GetConfig("ADC",parName);
	unsigned int bID = std::stoi(bIDs[ib]);
	for (unsigned int ic = 0; ic < bMap.size(); ic++) fECal_map[bID][ic] = std::stoi(bMap[ic]);
      } else {
	printf("ECalMonitor::Initialize - WARNING - No channel list for board %s in config file: using that from default channel map\n",parName.c_str());
      }
    }
  } else {
    printf("ECalMonitor::Initialize - WARNING - No board list in config file: using default channel map\n");
  }
  if (fConfig->Verbose() > 1) {
    printf("--- ECal readout map ---\n");
    for(unsigned int ib=0;ib<29;ib++) {
      printf("B%2.2d",ib);
      for (unsigned int ic=0;ic<32;ic++) {
	if (fECal_map[ib][ic] == -1) {
	  printf(" %4d",fECal_map[ib][ic]);
	} else {
	  printf(" %4.4d",fECal_map[ib][ic]);
	}
      }
      printf("\n");
    }
    printf("\n");
  }

  // Get output rates from config file
  fBeamOutputRate = fConfigParser->HasConfig("RECO","BeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate")):100;
  fOffBeamOutputRate = fConfigParser->HasConfig("RECO","OffBeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate")):100;
  fCosmicsOutputRate = fConfigParser->HasConfig("RECO","CosmicsOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate")):100;
  fRandomOutputRate = fConfigParser->HasConfig("RECO","RandomOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","RandomOutputRate")):100;

  // Get calibration constants from file
  for (UChar_t b=0; b<29; b++) {
    for (UChar_t c=0; c<32; c++) {
	fECal_CosmClb[b][c] = 0.;
    }
  }
  TString calibFile = "config/ECalCosmicsCalibration.dat";
  if ( fConfigParser->HasConfig("RECO","CosmicsCalibrationFile") )
    calibFile = fConfigParser->GetSingleArg("RECO","CosmicsCalibrationFile");
  struct stat buffer;   
  if (stat (calibFile.Data(),&buffer) != 0) {
    printf("ECalMonitor::Initialize - ERROR - file %s does not exist\n",calibFile.Data());
    exit(EXIT_FAILURE);
  }
  if (fConfig->Verbose() > 0) printf("ECalMonitor::Initialize - Reading cosmics calibration file %s\n",calibFile.Data());
  int x,y,b,c;
  float calib;
  FILE* cf = fopen(calibFile.Data(),"r");
  while(fscanf(cf,"%d %d %d %d %f\n",&x,&y,&b,&c,&calib) != EOF) {
    if (fConfig->Verbose() > 2) printf("Board %d Channel %d Calibration %f\n",b,c,calib);
    fECal_CosmClb[b][c] = calib;
  }
  fclose(cf);

  // Reset global counters
  ResetECalMap(fECal_CosmEvt);
  ResetECalMap(fECal_CosmSum);
  ResetECalMap(fECal_BeamESum);
  ResetECalMap(fECal_BeamEEvt);
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

}

void ECalMonitor::StartOfEvent()
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
    fNCosmWF = 0;
  } else {
    fIsCosmics = false;
  }
 
  // Check if event was a random trigger
  if (fConfig->GetEventTrigMask() & 0x40) {
    fIsRandom = true;
  } else {
    fIsRandom = false;
  }
  
  // Check if event was an off-beam trigger
  if (fConfig->GetEventTrigMask() & 0x80) {
    fIsOffBeam = true;
  } else {
    fIsOffBeam = false;
  }

}

void ECalMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset energy maps
      ResetECalMap(fECal_BeamESum);
      ResetECalMap(fECal_BeamEEvt);

    }

    // Count beam event
    fBeamEventCount++;

  } // End of beam output

  if (fIsOffBeam) {

    if (fOffBeamOutputRate && (fOffBeamEventCount % fOffBeamOutputRate == 0)) {

      // Write off-beam events data to output PadmeMonitor file
      OutputOffBeam();

    }

    // Count off-beam event
    fOffBeamEventCount++;

  } // End of off-beam output

  if (fIsCosmics) {

    if (fCosmicsOutputRate && (fCosmicsEventCount % fCosmicsOutputRate == 0)) {

      OutputCosmics();

      // Reset cosmics maps
      ResetECalMap(fECal_CosmEvt);
      ResetECalMap(fECal_CosmSum);

    }

    // Count cosmics event
    fCosmicsEventCount++;

  }

  if (fIsRandom) {

    if (fRandomOutputRate && (fRandomEventCount % fRandomOutputRate == 0)) {

      // Write random events data to output PadmeMonitor file
      OutputRandom();

    }

    // Count cosmics event
    fRandomEventCount++;

  } // End of random output

}

void ECalMonitor::Finalize()
{
  printf("ECalMonitor::Finalize - Total number of beam     events: %d\n",fBeamEventCount);
  printf("ECalMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  printf("ECalMonitor::Finalize - Total number of cosmics  events: %d\n",fCosmicsEventCount);
  printf("ECalMonitor::Finalize - Total number of random   events: %d\n",fRandomEventCount);
  /*
  if (fConfig->Verbose()>1) {

    // Show ECal occupation
    printf("--- ECal occupation ---\n");
    for(UChar_t y = 0;y<29;y++) {
      for(UChar_t x = 0;x<29;x++) {
	printf("%5d ",fECal_count[x][28-y]);
      }
      printf("\n");
    }

    printf("\n");

    // Show ECal signal map
    printf("--- ECal signal ---\n");
    for(UChar_t y = 0;y<29;y++) {
      for(UChar_t x = 0;x<29;x++) {
	printf("%5.0f ",fECal_signal[x][28-y]);
      }
      printf("\n");
    }

  }
  */

  /*
  TString ftname = fConfig->TmpDirectory()+"/ECAL.txt";
  TString ffname = fConfig->OutputDirectory()+"/ECAL.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  fprintf(outf,"\n");

  // ECal occupancy heatmap
  fprintf(outf,"PLOTID ECalMon_occupancy\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ECal Occupancy - %s\n",fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS 29 29\n");
  fprintf(outf,"RANGE_X 0 29\n");
  fprintf(outf,"RANGE_Y 0 29\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<29;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<29;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%d",fECal_count[x][28-y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");

  fprintf(outf,"\n");

  // ECal signal heatmap
  fprintf(outf,"PLOTID ECalMon_signal\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ECal Signal - %s\n",fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS 29 29\n");
  fprintf(outf,"RANGE_X 0 29\n");
  fprintf(outf,"RANGE_Y 0 29\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<29;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<29;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.2f",fECal_signal[x][28-y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");

  fclose(outf);

  // Move file to its final position
  if ( std::rename(ftname,ffname) != 0 ) perror("Error renaming file");
  */
}

void ECalMonitor::AnalyzeBoard(UChar_t board)
{;}

void ECalMonitor::AnalyzeChannel(UChar_t board,UChar_t channel,Short_t* samples)
{

  // Do not analyze the channel assigned to the LeadGlass
  if ((board == LEADGLASS_BOARD) && (channel == LEADGLASS_CHANNEL)) return;

  // Do not analyze channels used for the cosmics pads (last 4 channels of board 23)
  if ((board == 23) && (channel >= 28)) return;

  if ( fECal_map[board][channel] == -1 ) {
    if (! fConfig->IgnoreDisabledChannels()) printf("ECalMonitor::AnalyzeChannel - WARNING - board %d channel %d is disabled in the ECal map\n",board,channel);
    return;
  }

  // Get x,y coordinates from channel map
  UChar_t x = fECal_map[board][channel]/100;
  UChar_t y = fECal_map[board][channel]%100;
  //fECal_count[x][y]++;
  //fECal_signal[x][y] += TMath::RMS(994,samples);

  // Get calibrated channel energy
  Double_t chEnergy = GetChannelEnergy(board,channel,samples);
  //if (chEnergy>0.) printf("Board %d Channel %d Energy %f\n",board,channel,chEnergy);

  // Save information for Beam events
  if (fIsBeam) {

    // Add energy to cumulative map
    fECal_BeamESum[x][y] += chEnergy;

    // Save single event energy map once every few events
    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0))
      fECal_BeamEEvt[x][y] = chEnergy;

  }

  // Save information for Cosmics events
  if (fIsCosmics) {

    // Save energy in cosmics map
    fECal_CosmSum[x][y] += chEnergy;

    // Save waveform and event map once every few events
    if (fCosmicsOutputRate && (fCosmicsEventCount % fCosmicsOutputRate == 0)) {
      fECal_CosmEvt[x][y] = chEnergy;
      if (fNCosmWF<ECALMON_COSMWF_MAX) {
	fCosmWFX[fNCosmWF] = x;
	fCosmWFY[fNCosmWF] = y;
	for(UInt_t i = 0; i<1024; i++) fCosmWF[fNCosmWF][i] = samples[i];
	fNCosmWF++;
      }
    }

  }

}

Double_t ECalMonitor::GetChannelEnergy(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get total signal area using first 100 samples as pedestal and dropping last 30 samples
  Int_t sum = 0;
  Int_t sum_ped = 0;
  for(UInt_t s = 0; s<994; s++) {
    sum += samples[s];
    if (s<100) sum_ped += samples[s];
  }
  Double_t tot = 9.94*sum_ped-1.*sum;
  //printf("Board %d Channel %d Sum %d Sum_ped %d Tot %f\n",board,channel,sum,sum_ped,tot);
  // Convert to pC
  //tot = tot/(4096.*50.)*(1.E-9/1.E-12);
  tot *= 4.8828E-3;
  // Convert to MeV using calibration constant (18. MeV is the MPV from G4 simulation)
  if (fECal_CosmClb[board][channel]) {
    //printf("Board %d Channel %d Charge %f pC Calibration %f\n",board,channel,tot,fECal_CosmClb[board][channel]);
    tot *= 18./fECal_CosmClb[board][channel];
  } else {
    printf("ECalMonitor::GetChannelEnergy - ERROR - Calibration constant for board %d channel %d is 0\n",board,channel);
    tot = 0.;
  }
  return tot;
}

void ECalMonitor::ResetECalMap(Double_t map[29][29])
{
  // Set real crystal to 0. and fake crystals to -5.
  for (UChar_t x=0; x<29; x++) {
    for (UChar_t y=0; y<29; y++) {
      map[x][y] = -5.;
    }
  }
  for (UChar_t b=0; b<29; b++) {
    for (UChar_t c=0; c<32; c++) {
      if (fECal_map[b][c] != -1) {
	UChar_t x = fECal_map[b][c]/100;
	UChar_t y = fECal_map[b][c]%100;
	map[x][y] = 0.;
      }
    }
  }
}

Int_t ECalMonitor::OutputBeam()
{

  if (fConfig->Verbose()>0) printf("ECalMonitor::OutputBeam - Writing beam output files\n");

  // Write ECal energy maps for Beam events
  TString ftname = fConfig->TmpDirectory()+"/ECalMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ECalMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"w");
            
  fprintf(outf,"PLOTID ECalMon_beameventenergy\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ECal Beam - Event Energy - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 29 29\n");
  fprintf(outf,"RANGE_X 0 29\n");
  fprintf(outf,"RANGE_Y 0 29\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<29;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<29;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.3f",fECal_BeamEEvt[x][y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
            
  fprintf(outf,"PLOTID ECalMon_beammeanenergy\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ECal Beam - Average Energy - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 29 29\n");
  fprintf(outf,"RANGE_X 0 29\n");
  fprintf(outf,"RANGE_Y 0 29\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<29;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<29;x++) {
      if (x>0) fprintf(outf,",");
      if (fECal_BeamESum[x][y] > 0.) {
	fprintf(outf,"%.3f",fECal_BeamESum[x][y]/(Double_t)fBeamOutputRate);
      } else {
	fprintf(outf,"%.3f",fECal_BeamESum[x][y]); // Do not scale value for fake crystals
      }
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ECalMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t ECalMonitor::OutputOffBeam()
{
  return 0;
}

Int_t ECalMonitor::OutputCosmics()
{

  if (fConfig->Verbose()>0) printf("ECalMonitor::OutputCosmics - Writing cosmics output files\n");

  // Write ECal energy maps for Cosmics events
  TString ftname = fConfig->TmpDirectory()+"/ECalMon_Cosmics.txt";
  TString ffname = fConfig->OutputDirectory()+"/ECalMon_Cosmics.txt";
  FILE* outf = fopen(ftname.Data(),"w");

  // Show waveforms and event map only if at least one channel is active
  if (fNCosmWF>0) {
    for (UInt_t i = 0; i < ECALMON_COSMWF_MAX; i++) {
      fprintf(outf,"PLOTID ECalMon_cosmwf_%2.2d\n",i);
      fprintf(outf,"PLOTTYPE scatter\n");
      fprintf(outf,"PLOTNAME ECal Cosmics Waveform X %d Y %d - Run %d Event %d - %s\n",fCosmWFX[i],fCosmWFY[i],fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
      fprintf(outf,"RANGE_X 0 1024\n");
      //fprintf(outf,"RANGE_Y 0 4096\n");
      fprintf(outf,"TITLE_X Sample\n");
      fprintf(outf,"TITLE_Y Counts\n");
      fprintf(outf,"MODE [ \"lines\" ]\n");
      fprintf(outf,"COLOR [ \"ff0000\" ]\n");
      fprintf(outf,"DATA [ [");
      if (i<fNCosmWF) {
	for(UInt_t j = 0; j<1024; j++) {
	  if (j>0) { fprintf(outf,","); }
	  fprintf(outf,"[%d,%d]",j,fCosmWF[i][j]);
	}
      } else {
	fprintf(outf,"[0,0],[1023,0]");
      }
      fprintf(outf,"] ]\n\n");
    }
      
    fprintf(outf,"PLOTID ECalMon_cosmevt\n");
    fprintf(outf,"PLOTTYPE heatmap\n");
    fprintf(outf,"PLOTNAME ECal Cosmics - Run %d Event %d - %s\n",fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS 29 29\n");
    fprintf(outf,"RANGE_X 0 29\n");
    fprintf(outf,"RANGE_Y 0 29\n");
    fprintf(outf,"TITLE_X X\n");
    fprintf(outf,"TITLE_Y Y\n");
    fprintf(outf,"DATA [");
    for(UChar_t y = 0;y<29;y++) {
      if (y>0) fprintf(outf,",");
      fprintf(outf,"[");
      for(UChar_t x = 0;x<29;x++) {
	if (x>0) fprintf(outf,",");
	fprintf(outf,"%.3f",fECal_CosmEvt[x][y]);
      }
      fprintf(outf,"]");
    }
    fprintf(outf,"]\n");

  }
      
  fprintf(outf,"PLOTID ECalMon_cosmics\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ECal Cosmics - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(time(0)));
  fprintf(outf,"CHANNELS 29 29\n");
  fprintf(outf,"RANGE_X 0 29\n");
  fprintf(outf,"RANGE_Y 0 29\n");
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<29;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<29;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.3f",fECal_CosmSum[x][y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n");
      
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ECalMonitor::OutputCosmics - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t ECalMonitor::OutputRandom()
{
  return 0;
}
