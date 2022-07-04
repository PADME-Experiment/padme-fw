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
  fBeamOutputRate = fConfigParser->HasConfig("RECO","BeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate")):100;
  fOffBeamOutputRate = fConfigParser->HasConfig("RECO","OffBeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate")):100;
  fCosmicsOutputRate = fConfigParser->HasConfig("RECO","CosmicsOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate")):100;
  fRandomOutputRate = fConfigParser->HasConfig("RECO","RandomOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","RandomOutputRate")):100;

  // Set number of samples to use to compute pedestals
  fPedestalSamples = 100;

  // Reset global counters
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
  } else {
    fIsBeam = false;
  }

  // Check if event was triggered by cosmics
  if (fConfig->GetEventTrigMask() & 0x02) {
    fIsCosmics = true;
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

void ETagMonitor::EndOfEvent()
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

    }

    // Count cosmics event
    fRandomEventCount++;

  } // End of random output

}

void ETagMonitor::Finalize()
{
  printf("ETagMonitor::Finalize - Total number of beam events: %d\n",fBeamEventCount);
  printf("ETagMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  printf("ETagMonitor::Finalize - Total number of cosmics events: %d\n",fCosmicsEventCount);
  printf("ETagMonitor::Finalize - Total number of random events: %d\n",fRandomEventCount);
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
  UChar_t sipmch = ch%1000;

}

void ETagMonitor::ComputeChannelEnergy(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get total signal area using first 100 samples as pedestal and dropping last 30 samples
  Int_t sum = 0;
  Int_t sum_ped = 0;
  for(UInt_t s = 0; s<994; s++) {
    sum += samples[s];
    if (s<fPedestalSamples) sum_ped += samples[s];
  }
  Double_t tot = ((Double_t)994/(Double_t)fPedestalSamples)*sum_ped-1.*sum;
  // Convert to pC
  //tot = tot/(4096.*50.)*(1.E-9/1.E-12);
  tot *= 4.8828E-3;
  fChannelEnergy = tot;
}

void ETagMonitor::ComputeChannelPedestal(UChar_t board,UChar_t channel,Short_t* samples)
{
  // Get pedestal value and RMS from first 100 samples
  Int_t sum = 0;
  ULong_t sum2 = 0;
  for(UInt_t s = 0; s<fPedestalSamples; s++) {
    sum += samples[s];
    sum2 += samples[s]*samples[s];
  }
  fChannelPedestal = (Double_t)sum/(Double_t)fPedestalSamples;
  fChannelPedRMS = sqrt( ((Double_t)sum2 - (Double_t)sum*fChannelPedestal)/(Double_t)(fPedestalSamples-1) );
}

Int_t ETagMonitor::OutputBeam()
{

  if (fConfig->Verbose()>0) printf("ETagMonitor::OutputBeam - Writing beam output files\n");

  TString ftname = fConfig->TmpDirectory()+"/ECALMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ECALMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

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

  TString ftname = fConfig->TmpDirectory()+"/ECALMon_OffBeam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ECALMon_OffBeam.txt";
  FILE* outf = fopen(ftname.Data(),"a");

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

  TString ftname = fConfig->TmpDirectory()+"/ECALMon_Cosmics.txt";
  TString ffname = fConfig->OutputDirectory()+"/ECALMon_Cosmics.txt";
  FILE* outf = fopen(ftname.Data(),"a");

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

  TString ftname = fConfig->TmpDirectory()+"/ECALMon_Random.txt";
  TString ffname = fConfig->OutputDirectory()+"/ECALMon_Random.txt";
  FILE* outf = fopen(ftname.Data(),"a");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ETagMonitor::OutputRandom - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}
