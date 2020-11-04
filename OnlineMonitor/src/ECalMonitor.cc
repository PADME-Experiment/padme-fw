#include <stdio.h>

#include "ECalMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

ECalMonitor::ECalMonitor()
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Initialize all counters
  Initialize();

}

ECalMonitor::~ECalMonitor()
{;}

void ECalMonitor::Initialize()
{

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

  if (fConfig->Verbose()>=1) {

    // Show ECal occupation
    printf("--- ECal occupation ---");
    for(UChar_t y = 0;y<29;y++) {
      for(UChar_t x = 0;x<29;x++) {
	printf("%5d ",fECal_count[x][28-y]);
      }
      printf("\n");
    }

    printf("\n");

    // Show ECal signal map
    printf("--- ECal signal ---");
    for(UChar_t y = 0;y<29;y++) {
      for(UChar_t x = 0;x<29;x++) {
	printf("%5.0f ",fECal_signal[x][28-y]);
      }
      printf("\n");
    }

  }

  FILE* outf = fopen(fConfig->GetOutputFile(),"a");

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
