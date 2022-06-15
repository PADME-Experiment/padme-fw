#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <regex>
#include "TFile.h"
#include "TTree.h"

#include "TempCorr.hh"

TempCorr* TempCorr::fInstance = 0;
TempCorr* TempCorr::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TempCorr(); }
  return fInstance;
}

TempCorr::TempCorr()
  :fVerbose(0)
{
  fTimeRef = 0; // Will store first time in temperature file
  fTempRef = 36.195; // Reference temperature to compute energy corrections
  fTemperatureFileName = "config/ECal_Temperature.dat";

  // Populate channel id conversion map according to sensor id list
  for (Int_t i=0; i<TEMPCORR_NCHANNELS; i++) fChanNumber[fChanCodeList[i]] = i;

  // Initialize channel map, i.e. assign a sensor to each channel
  // ECal is seen from the back: X grows from left to right, Y grows from bottom to top
  Int_t chanMap[29*29] = {
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01
  };
  for(Int_t x=0; x<29; x++) {
    for(Int_t y=0; y<29; y++) {
      fChanMap[x][y] = chanMap[(28-y)*29+x];
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TempCorr::~TempCorr()
{;}

Int_t TempCorr::Initialize(TString filename)
{
  fTemperatureFileName = filename;
  return Initialize();
}

Int_t TempCorr::Initialize()
{

  if (fVerbose) printf("---> Reading temperature values from file %s\n",fTemperatureFileName.Data());

  fTempMap.clear();

  std::string line;
  std::smatch match;
  auto const retime = std::regex("^\\s*(\\d+)");
  auto const retemp = std::regex("(\\d+)\\s+([\\-\\.\\d]+)");

  Int_t lastTime = 0;

  // Check if temperature file exists
  struct stat buffer;   
  if (stat (fTemperatureFileName.Data(),&buffer) != 0) {
    printf("TempCorr::Initialize - ERROR - File %s does not exist\n",fTemperatureFileName.Data());
    return 1;
  }

  std::ifstream tempfile(fTemperatureFileName.Data());
  fEntries = 0;
  Short_t templist[TEMPCORR_NCHANNELS] = {0,0};
  while (std::getline(tempfile,line)) {

    // Skip comments and empty lines
    if (!regex_search(line,match,retime)) continue;

    // Extract temperature data from input line
    Long_t temptime = std::atoi(match[1].str().c_str());
    line = match.suffix();
    while (regex_search(line,match,retemp)) {
      Int_t chan = std::atoi(match[1].str().c_str());
      // Need to add 0.5 to avoid weird floating point rounding
      Short_t temp = Short_t(std::atof(match[2].str().c_str())/TEMPCORR_TEMPRESOLUTION+0.5);
      if (fChanNumber.find(chan) == fChanNumber.end()) {
	printf("TempCorr::Initialize - WARNING - Undefined channel %d found at time %ld\n",chan,temptime);
      } else {
	templist[fChanNumber[chan]] = temp;
      }
      line = match.suffix();
    }
    //std::cout << temptime << " " << templist[0] << " " << templist[1] << std::endl;

    // Store data into temperature map
    if (fEntries==0) {
      // Use first line as reference time
      fTimeRef = temptime;
      //printf("0");
      fTempMap.push_back({});
      for (Int_t c = 0; c<TEMPCORR_NCHANNELS; c++) {
	fTempMap[0].push_back(templist[c]);
	//printf(" %d",templist[c]);
      }
      //printf("\n");
      fEntries++;
      continue;
    }
    Int_t thisTime = (temptime-fTimeRef)/TEMPCORR_TIMERESOLUTION;
    if (thisTime > lastTime) {
      // If temperature is not read for a while, reuse last reading
      if (thisTime > lastTime+1) {
	for(Int_t t = lastTime+1; t<thisTime; t++) {
	  //printf("%d",t);
	  fTempMap.push_back({});
	  for (Int_t c = 0; c<TEMPCORR_NCHANNELS; c++) {
	    //printf(" %d",fTempMap[lastTime][c]);
	    fTempMap[t].push_back(fTempMap[lastTime][c]);
	  }
	  //printf("\n");
	}
      }
      //printf("%d",thisTime);
      fTempMap.push_back({});
      for (Int_t c = 0; c<TEMPCORR_NCHANNELS; c++) {
	//printf(" %d",templist[c]);
	fTempMap[thisTime].push_back(templist[c]);
      }
      //printf("\n");

      lastTime = thisTime;
    }
    fEntries++;

  }

  if(fVerbose>3) {
    // Dump temperature map
    for (UInt_t i=0; i<fTempMap.size(); i++) {
      printf("%d",i);
      for (UInt_t j=0; j<fTempMap[i].size(); j++) {
	printf(" %d=%d",j,fTempMap[i][j]);
      }
      printf("\n");
    }
  }

  return 0;
}

Double_t TempCorr::GetTemp(TTimeStamp eventtime)
{
  return GetTemp(eventtime,0);
}

Double_t TempCorr::GetTemp(TTimeStamp eventtime,Int_t x,Int_t y)
{
  return GetTemp(eventtime,fChanMap[x][y]);
}

Double_t TempCorr::GetTemp(TTimeStamp eventtime,Int_t ch)
{
  return Double_t(fTempMap[GetTimeIndex(eventtime)][ch])*TEMPCORR_TEMPRESOLUTION;
}

Double_t TempCorr::GetTempCorr(TTimeStamp eventtime)
{
  return ComputeCorrection(GetTemp(eventtime));
}

Double_t TempCorr::GetTempCorr(TTimeStamp eventtime,Int_t ch)
{
  return ComputeCorrection(GetTemp(eventtime,ch));
}

Double_t TempCorr::GetTempCorr(TTimeStamp eventtime,Int_t x,Int_t y)
{
  return ComputeCorrection(GetTemp(eventtime,x,y));
}

Double_t TempCorr::ComputeCorrection(Double_t temp)
{
  return 1.+(.01*(temp-fTempRef));
}

Int_t TempCorr::GetTimeIndex(TTimeStamp t)
{
  return (t.GetSec()-fTimeRef)/TEMPCORR_TIMERESOLUTION;
}
