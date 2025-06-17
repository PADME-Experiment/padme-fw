#include <stdio.h>
#include <sys/stat.h>
#include <fstream>

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
  if (fHECTotEnergyBM) { delete fHECTotEnergyBM; fHECTotEnergyBM = 0; }
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

  // Get pedestal and charge reconstruction parameters from config file
  fPedestalSamples = fConfigParser->HasConfig("RECO","PedestalSamples")?std::stoi(fConfigParser->GetSingleArg("RECO","PedestalSamples")):100;
  fSignalSamplesStart = fConfigParser->HasConfig("RECO","SignalSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesStart")):100;
  fSignalSamplesEnd = fConfigParser->HasConfig("RECO","SignalSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesEnd")):994;

  // Get calibration constants from file
  for (UChar_t b=0; b<29; b++) {
    for (UChar_t c=0; c<32; c++) {
	fECal_Calibration[b][c] = 0.;
    }
  }
  TString calibFile = "config/ECalCalibration.dat";
  if ( fConfigParser->HasConfig("RECO","CalibrationFile") )
    calibFile = fConfigParser->GetSingleArg("RECO","CalibrationFile");
  struct stat buffer;   
  if (stat (calibFile.Data(),&buffer) != 0) {
    printf("ECalMonitor::Initialize - ERROR - file %s does not exist\n",calibFile.Data());
    exit(EXIT_FAILURE);
  }
  if (fConfig->Verbose() > 0) printf("ECalMonitor::Initialize - Reading calibration file %s\n",calibFile.Data());
  int x,y,b,c;
  float calib;
  FILE* cf = fopen(calibFile.Data(),"r");
  while(fscanf(cf,"%d %d %d %d %f\n",&x,&y,&b,&c,&calib) != EOF) {
    if (fConfig->Verbose() > 2) printf("Board %d Channel %d X %d Y %d Calibration %f\n",b,c,x,y,calib);
    fECal_Calibration[b][c] = calib;
  }
  fclose(cf);

  // Define trend support file for this run
  fTFECTrendsBM = fConfig->TrendDirectory()+"/"+fConfig->RunName()+"_ECTrendsBM.trend";

  // If trend file exists, recover the data
  if (stat(fTFECTrendsBM.Data(),&buffer) == 0) {
    std::ifstream tf(fTFECTrendsBM.Data());
    Double_t abstime,totenergy;
    while (tf >> abstime >> totenergy) {
      //printf("%f %f\n",abstime,totenergy);
      fVECTimeBM.push_back(abstime);
      fVECTotEnergyBM.push_back(totenergy);
    }
  }

  // Create histograms
  fHECTotEnergyBM = new TH1D("ECal_TotalEnergyBM","ECal_TotalEnergyBM",1000,0.,50000.);

  // Reset energy maps
  ResetECalMap(fECal_CosmEvt);
  ResetECalMap(fECal_CosmSum);
  ResetECalMap(fECal_BeamESum);
  ResetECalMap(fECal_BeamEEvt);

  // Reset global counters
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

  // Reset event counters
  fECTotalEventEnergy = 0.;

}

void ECalMonitor::EndOfEvent()
{

  if (fIsBeam) {

    // Fill histogram with total ECal energy for this event
    fHECTotEnergyBM->Fill(fECTotalEventEnergy);

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Check if current data is new
      if ( (fVECTimeBM.size() == 0) || (fConfig->GetEventAbsTime().AsDouble() > fVECTimeBM.back()) ) {

	// Update trend vectors
	fVECTimeBM.push_back(fConfig->GetEventAbsTime().AsDouble());
	fVECTotEnergyBM.push_back(fHECTotEnergyBM->GetMean());

	// Update trends file
	FILE* tf = fopen(fTFECTrendsBM.Data(),"a");
	fprintf(tf,"%f %f\n",fVECTimeBM.back(),fVECTotEnergyBM.back());
	fclose(tf);

      }

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset energy maps
      ResetECalMap(fECal_BeamESum);
      ResetECalMap(fECal_BeamEEvt);

      // Reset beam histograms
      fHECTotEnergyBM->Reset();

    }

  } // End of beam output

  if (fIsOffBeam) {

    if (fOffBeamOutputRate && (fOffBeamEventCount % fOffBeamOutputRate == 0)) {

      // Write off-beam events data to output PadmeMonitor file
      OutputOffBeam();

    }

  } // End of off-beam output

  if (fIsCosmics) {

    if (fCosmicsOutputRate && (fCosmicsEventCount % fCosmicsOutputRate == 0)) {

      OutputCosmics();

      // Reset cosmics maps
      ResetECalMap(fECal_CosmEvt);
      ResetECalMap(fECal_CosmSum);

    }

  }

  if (fIsRandom) {

    if (fRandomOutputRate && (fRandomEventCount % fRandomOutputRate == 0)) {

      // Write random events data to output PadmeMonitor file
      OutputRandom();

    }

  } // End of random output

}

void ECalMonitor::Finalize()
{
  printf("ECalMonitor::Finalize - Total number of beam     events: %d\n",fBeamEventCount);
  printf("ECalMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  printf("ECalMonitor::Finalize - Total number of cosmics  events: %d\n",fCosmicsEventCount);
  printf("ECalMonitor::Finalize - Total number of random   events: %d\n",fRandomEventCount);
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

  // Get calibrated channel energy
  ComputeChannelCharge(samples);
  ComputeChannelEnergy(board,channel);

  //printf("Board %d Channel %d Pedestal %f PedRMS %f Charge %f Energy %f\n",board,channel,fChannelPedestal,fChannelPedRMS,fChannelCharge,fChannelEnergy);

  // Add channel energy to total event energy counter
  fECTotalEventEnergy += fChannelEnergy;

  // Save information for Beam events
  if (fIsBeam) {

    // Add energy to cumulative map
    fECal_BeamESum[x][y] += fChannelEnergy;

    // Save single event energy map once every few events
    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0))
      fECal_BeamEEvt[x][y] = fChannelEnergy;

  }

  // Save information for Cosmics events
  if (fIsCosmics) {

    // Save energy in cosmics map
    fECal_CosmSum[x][y] += fChannelEnergy;

    // Save single event energy map once every few events
    if (fCosmicsOutputRate && (fCosmicsEventCount % fCosmicsOutputRate == 0))
      fECal_CosmEvt[x][y] = fChannelEnergy;

  }

}

void ECalMonitor::ComputeChannelCharge(Short_t* samples)
{

  // Get total signal area using first fPedestalSamples samples as pedestal
  Int_t sum = 0;
  Int_t sum_ped = 0;
  ULong_t sum2_ped = 0;
  for(UInt_t s = 0; s<1024; s++) {
    if (s<fPedestalSamples) {
      sum_ped += samples[s];
      sum2_ped += samples[s]*samples[s];
    } else if (s >= fSignalSamplesStart) {
      if (s < fSignalSamplesEnd) {
	sum += samples[s];
      } else {
	break;
      }
    }
  }

  //printf("%d %d %d %d\n",sum,sum_ped,sum2_ped,fPedestalSamples);
  fChannelPedestal = (Double_t)sum_ped/(Double_t)fPedestalSamples;
  fChannelPedRMS = sqrt(((Double_t)sum2_ped - (Double_t)sum_ped*fChannelPedestal)/((Double_t)fPedestalSamples-1.));
  fChannelCharge = fChannelPedestal*(Double_t)(fSignalSamplesEnd-fSignalSamplesStart)-(Double_t)sum;
  // Convert counts to charge in pC
  //charge = counts/(4096.*50.)*(1.E-9/1.E-12);
  fChannelCharge *= 4.8828E-3;

}

void ECalMonitor::ComputeChannelEnergy(UChar_t board,UChar_t channel)
{

  // Convert charge to MeV using calibration constant (18. MeV is the MPV from G4 simulation)
  if (fECal_Calibration[board][channel]) {
    fChannelEnergy = fChannelCharge*18./fECal_Calibration[board][channel];
  } else {
    printf("ECalMonitor::GetChannelEnergy - ERROR - Calibration constant for board %d channel %d is 0\n",board,channel);
    fChannelEnergy = 0.;
  }
 
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
     
  // Total energy histogram
  fprintf(outf,"PLOTID ECalMon_beamtotalenergy\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME ECal Total Energy - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHECTotEnergyBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHECTotEnergyBM->GetXaxis()->GetXmin(),fHECTotEnergyBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X Energy (MeV)\n");
  fprintf(outf,"TITLE_Y Events\n");
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHECTotEnergyBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHECTotEnergyBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Total energy trend plot
  fprintf(outf,"PLOTID ECalMon_trendtotalenergy\n");
  fprintf(outf,"PLOTNAME ECal Total Energy - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Energy\n");
  fprintf(outf,"LEGEND [ \"Energy (MeV)\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVECTimeBM.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVECTimeBM[j],fVECTotEnergyBM[j]);
  }
  fprintf(outf,"] ]\n\n");
       
  fprintf(outf,"PLOTID ECalMon_beameventenergy\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ECal Event Energy (MeV)- Run %d Event %d - %s\n",fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
  fprintf(outf,"PLOTNAME ECal Energy (MeV)- Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
            
  fprintf(outf,"PLOTID ECalMon_beammeanenergy_log\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ECal Energy log(eV)- Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
	fprintf(outf,"%.3f",log10(1000000.*(fECal_BeamESum[x][y]/(Double_t)fBeamOutputRate)));
      } else {
	fprintf(outf,"%.3f",0.); // Set fake crystals to 0
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
