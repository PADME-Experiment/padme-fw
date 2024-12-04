#include <stdio.h>
#include <sys/stat.h>

#include "ETagMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

ETagMonitor::ETagMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

ETagMonitor::~ETagMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }
}

void ETagMonitor::Initialize()
{

  // Get ETag map from configuration file
  if(fConfigParser->HasConfig("ADC","ID")){
    std::vector<std::string> bIDs = fConfigParser->GetConfig("ADC","ID");
    for(unsigned int ib = 0; ib < bIDs.size(); ib++) {
      std::string parName = "ADC"+bIDs[ib];
      if (fConfigParser->HasConfig("ADC",parName )) {
	std::vector<std::string> bMap = fConfigParser->GetConfig("ADC",parName);
	unsigned int bID = std::stoi(bIDs[ib]);
	for (unsigned int ic = 0; ic < bMap.size(); ic++) fETag_map[bID][ic] = std::stoi(bMap[ic]);
      } else {
	printf("ETagMonitor::Initialize - WARNING - No channel list for board %s in config file: using that from default channel map\n",parName.c_str());
      }
    }
  } else {
    printf("ETagMonitor::Initialize - WARNING - No board list in config file: using default channel map\n");
  }
  if (fConfig->Verbose() > 1) {
    printf("--- ETag readout map ---\n");
    for(unsigned int ib=0;ib<29;ib++) {
      printf("B%2.2d",ib);
      for (unsigned int ic=0;ic<32;ic++) {
	if (fETag_map[ib][ic] == -1) {
	  printf(" %4d",fETag_map[ib][ic]);
	} else {
	  printf(" %4.4d",fETag_map[ib][ic]);
	}
      }
      printf("\n");
    }
    printf("\n");
  }

  // Get output rates from config file
  fBeamOutputRate = fConfigParser->HasConfig("RECO","BeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate")):200;
  fOffBeamOutputRate = fConfigParser->HasConfig("RECO","OffBeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate")):0;
  fCosmicsOutputRate = fConfigParser->HasConfig("RECO","CosmicsOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate")):0;
  fRandomOutputRate = fConfigParser->HasConfig("RECO","RandomOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","RandomOutputRate")):0;

  // Get pedestal and charge reconstruction parameters from config file
  fPedestalSamples = fConfigParser->HasConfig("RECO","PedestalSamples")?std::stoi(fConfigParser->GetSingleArg("RECO","PedestalSamples")):100;
  fSignalSamplesStart = fConfigParser->HasConfig("RECO","SignalSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesStart")):100;
  fSignalSamplesEnd = fConfigParser->HasConfig("RECO","SignalSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesEnd")):994;

  // Reset global counters
  ResetETagMap(fMChargeSumBM);
  ResetETagMap(fMChargeEvtBM);
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

}

void ETagMonitor::StartOfEvent()
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

void ETagMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset charge maps
      ResetETagMap(fMChargeSumBM);
      ResetETagMap(fMChargeEvtBM);

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

      // Write cosmics events data to output PadmeMonitor file
      OutputCosmics();

    }

  } // End of cosmics output

  if (fIsRandom) {

    if (fRandomOutputRate && (fRandomEventCount % fRandomOutputRate == 0)) {

      // Write random events data to output PadmeMonitor file
      OutputRandom();

    }

  } // End of random output

}

void ETagMonitor::Finalize()
{
  printf("ETagMonitor::Finalize - Total number of beam     events: %d\n",fBeamEventCount);
  printf("ETagMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  printf("ETagMonitor::Finalize - Total number of cosmics  events: %d\n",fCosmicsEventCount);
  printf("ETagMonitor::Finalize - Total number of random   events: %d\n",fRandomEventCount);
}

void ETagMonitor::AnalyzeBoard(UChar_t board)
{;}

void ETagMonitor::AnalyzeChannel(UChar_t board,UChar_t channel,Short_t* samples)
{

  // Check if board/channel belongs to ETag
  if ( board != 10 && board != 11 && board != 24 && board != 25 ) {
    printf("ETagMonitor::AnalyzeChannel - WARNING - board %d does not belong to ETag\n",board);
    return;
  } else if ( fETag_map[board][channel] == -1 ) {
    if (! fConfig->IgnoreDisabledChannels()) printf("ETagMonitor::AnalyzeChannel - WARNING - board %d channel %d is disabled in the ETag map\n",board,channel);
    return;
  }

  // Get SiPM channel id from channel map as llsc (level/side/channel)
  Short_t ch = fETag_map[board][channel];
  UChar_t level = ch/100;
  UChar_t side = (ch%100)/10;
  UChar_t sipmch = ch%10;
  //printf("ch %d level %d side %d sipch %d \n",ch,level,side,sipmch);

  // Compute channel pedestal and charge
  ComputeChannelPedestal(samples);
  ComputeChannelCharge(samples);

  if (fIsBeam) {

    // Add charge to cumulative map
    fMChargeSumBM[side][level*4+sipmch] += fChannelCharge;

    // Save single event charge map once every few events
    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0))
      fMChargeEvtBM[side][level*4+sipmch] = fChannelCharge;

  }

}

void ETagMonitor::ComputeChannelPedestal(Short_t* samples)
{
  // Get pedestal value and RMS
  Int_t sum = 0;
  ULong_t sum2 = 0;
  for(UInt_t s = 0; s<fPedestalSamples; s++) {
    sum += samples[s];
    sum2 += samples[s]*samples[s];
  }
  fChannelPedestal = (Double_t)sum/(Double_t)fPedestalSamples;
  fChannelPedRMS = sqrt( ((Double_t)sum2 - (Double_t)sum*fChannelPedestal)/(Double_t)(fPedestalSamples-1) );
}

void ETagMonitor::ComputeChannelCharge(Short_t* samples)
{
  // Get total signal area
  Int_t sum = 0;
  for(UInt_t s = fSignalSamplesStart; s<fSignalSamplesEnd; s++) sum += samples[s];
  fChannelCharge = (Double_t)(fSignalSamplesEnd-fSignalSamplesStart)*fChannelPedestal-(Double_t)sum;
  // Convert to pC
  //charge = counts/(4096.*50.)*(1.E-9/1.E-12);
  fChannelCharge *= 4.8828E-3;
}

Int_t ETagMonitor::OutputBeam()
{

  if (fConfig->Verbose()>0) printf("ETagMonitor::OutputBeam - Writing beam output files\n");

  TString ftname = fConfig->TmpDirectory()+"/ETagMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ETagMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"w");
      
  fprintf(outf,"PLOTID ETagMon_beameventcharge\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ETag Beam - Event Charge - Run %d Event %d - %s\n",fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 2 60\n");
  fprintf(outf,"RANGE_X 0 2\n");
  fprintf(outf,"RANGE_Y 0 60\n");
  fprintf(outf,"TITLE_X Side\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<60;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<2;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.3f",fMChargeEvtBM[x][59-y]);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
      
  fprintf(outf,"PLOTID ETagMon_beamaveragecharge\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME ETag Beam - Average Charge - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 2 60\n");
  fprintf(outf,"RANGE_X 0 2\n");
  fprintf(outf,"RANGE_Y 0 60\n");
  fprintf(outf,"TITLE_X Side\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<60;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<2;x++) {
      if (x>0) fprintf(outf,",");
      fprintf(outf,"%.3f",fMChargeSumBM[x][59-y]/(Double_t)fBeamOutputRate);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");

  // Linear histogram of channel charge
  fprintf(outf,"PLOTID ETagMon_beamlinearcharge\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME ETag Charge - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 120\n");
  fprintf(outf,"RANGE_X 0. 120.\n");
  fprintf(outf,"TITLE_X Channel\n");
  fprintf(outf,"TITLE_Y Charge\n");
  //fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  fprintf(outf,"DATA [[");
  for(UChar_t x = 0;x<2;x++) {
    for(UChar_t y = 0;y<60;y++) {
      if (x || y) fprintf(outf,",");
      fprintf(outf,"%.1f",fMChargeSumBM[x][y]/(Double_t)fBeamOutputRate);
    }
  }
  fprintf(outf,"]]\n\n");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ETagMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t ETagMonitor::OutputOffBeam()
{

  if (fConfig->Verbose()>0) printf("ETagMonitor::OutputOffBeam - Writing off-beam output files\n");

  TString ftname = fConfig->TmpDirectory()+"/ETagMon_OffBeam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ETagMon_OffBeam.txt";
  FILE* outf = fopen(ftname.Data(),"w");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ETagMonitor::OutputOffBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t ETagMonitor::OutputCosmics()
{

  if (fConfig->Verbose()>0) printf("ETagMonitor::OutputCosmics - Writing cosmics output files\n");

  TString ftname = fConfig->TmpDirectory()+"/ETagMon_Cosmics.txt";
  TString ffname = fConfig->OutputDirectory()+"/ETagMon_Cosmics.txt";
  FILE* outf = fopen(ftname.Data(),"w");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ETagMonitor::OutputCosmics - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t ETagMonitor::OutputRandom()
{

  if (fConfig->Verbose()>0) printf("ETagMonitor::OutputRandom - Writing random output files\n");

  TString ftname = fConfig->TmpDirectory()+"/ETagMon_Random.txt";
  TString ffname = fConfig->OutputDirectory()+"/ETagMon_Random.txt";
  FILE* outf = fopen(ftname.Data(),"w");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ETagMonitor::OutputRandom - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

void ETagMonitor::ResetETagMap(Double_t map[2][60])
{
  for (UChar_t s=0; s<2; s++) {
    for (UChar_t c=0; c<60; c++) {
      map[s][c] = 0.;
    }
  }
}
