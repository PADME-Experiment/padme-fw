#include "ECalMonitor.hh"

#include "TMath.h"

ECalMonitor::ECalMonitor()
{
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

void ECalMonitor::Finalize()
{

  // Show ECal occupation
  for(UChar_t y = 0;y<29;y++) {
    for(UChar_t x = 0;x<29;x++) {
      printf("%5d ",fECal_count[x][28-y]);
    }
    printf("\n");
  }

  printf("\n");

  // Show ECal signal map
  for(UChar_t y = 0;y<29;y++) {
    for(UChar_t x = 0;x<29;x++) {
      printf("%5.0f ",fECal_signal[x][28-y]);
    }
    printf("\n");
  }

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
