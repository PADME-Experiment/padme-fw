#include <stdio.h>

#include "ECalMonitor.hh"

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
  std::vector<Short_t> boards;
  if(fConfigParser->HasConfig("ADC","ID")){
    std::vector<std::string> bIDs = fConfigParser->GetConfig("ADC","ID");
    for(unsigned int ib = 0; ib < bIDs.size(); ib++) boards.push_back(std::stoi(bIDs[ib]));
  }
  for (unsigned int ib = 0;ib < boards.size();ib++){
    int bID = boards[ib];
    std::string parName = "ADC"+std::to_string(bID);
    if(fConfigParser->HasConfig("ADC",parName )){
      std::vector<std::string> bMap = fConfigParser->GetConfig("ADC",parName);
      for (unsigned int ic = 0; ic < bMap.size(); ic++) fECal_map[bID][ic] = std::stoi(bMap[ic]);
    }
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

  // Reset global counters
  for (UChar_t x=0; x<29; x++) {
    for (UChar_t y=0; y<29; y++) {
      fECal_count[x][y] = 0;
      fECal_signal[x][y] = 0.;
    }
  }

}

void ECalMonitor::StartOfEvent()
{;}

void ECalMonitor::EndOfEvent()
{;}

void ECalMonitor::Finalize()
{

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

}

void ECalMonitor::Analyze(UChar_t board,UChar_t channel,Short_t* samples)
{

  if ( fECal_map[board][channel] == -1 ) {
    printf("WARNING - ECal board %d channel %d is disabled in the map\n",board,channel);
  } else {
    UChar_t x = fECal_map[board][channel]/100;
    UChar_t y = fECal_map[board][channel]%100;
    fECal_count[x][y]++;
    fECal_signal[x][y] += TMath::RMS(994,samples);
  }

}
