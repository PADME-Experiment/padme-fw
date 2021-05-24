#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
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
{
  fTimeRef = 0; // Will store first time in temperature file
  fTempRef = 36.195; // Reference temperature to compute energy corrections
  fVerbose = 0;
  fTemperatureFileName = "ECAL_TR_tree.root";
  // Initialize channel map, i.e. assign a sensor to each channel
  // X grows from left to right, Y grows from bottom to top
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

  printf("---> Reading temperature values from file %s\n",fTemperatureFileName.Data());

  // Open the file containing the tree.
  auto myFile_r = TFile::Open(fTemperatureFileName);
  if (!myFile_r || myFile_r->IsZombie()) return 1;

  TTree *f1 = (TTree*)myFile_r->Get("T_R");
  f1->SetBranchAddress("ecalT_r",&fECalTR);

  fTempMap.clear();
  Int_t lastTime = 0;
  //Long_t oldT = 0;
  fEntries = (Int_t)f1->GetEntries();
  for (Int_t i=0;i<fEntries;i++) {

    f1->GetEntry(i);
    //printf("%12ld %12ld %3d %f\n",fECalTR.tstamp,fECalTR.tstamp-oldT,fECalTR.nch,fECalTR.temp_r);
    //oldT = fECalTR.tstamp;

    if (i==0) {
      fTimeRef = fECalTR.tstamp;
      fTempMap.push_back({});
      //printf("%8d",0);
      for (Int_t c = 0; c<TEMPCORR_NCHANNELS; c++) {
	fTempMap[0].push_back(Short_t(fECalTR.temp_r/TEMPCORR_TEMPRESOLUTION));
	//printf(" %4d",fTempMap[0][c]);
      }
      //printf("\n");
      continue;
    }
    Int_t thisTime = (fECalTR.tstamp-fTimeRef)/TEMPCORR_TIMERESOLUTION;
    if (thisTime > lastTime) {
      if (thisTime > lastTime+1) {
	for(Int_t t = lastTime+1; t<thisTime; t++) {
	  fTempMap.push_back({});
	  //printf("%8d",t);
	  for (Int_t c = 0; c<TEMPCORR_NCHANNELS; c++) {
	    fTempMap[t].push_back(fTempMap[lastTime][c]);
	    //printf(" %4d",fTempMap[t][c]);
	  }
	  //printf("\n");
	}
      }
      fTempMap.push_back({});
      //printf("%8d",thisTime);
      for (Int_t c = 0; c<TEMPCORR_NCHANNELS; c++) {
	fTempMap[thisTime].push_back(Short_t(fECalTR.temp_r/TEMPCORR_TEMPRESOLUTION));
	//printf(" %4d",fTempMap[thisTime][c]);
      }
      //printf("\n");
      lastTime = thisTime;
    }
    if(fVerbose && i<10) {
      printf(" i=%d tstamp=%ld ch=%d temp=%f\n",i,fECalTR.tstamp,fECalTR.nch,fECalTR.temp_r);
    }
  }
  myFile_r->Close();
  printf("---> Number of temperature values read = %d\n",fEntries);

  /*
  // Dump temperature map
  for (UInt_t i=0; i<fTempMap.size(); i++) {
    printf("%d",i);
    for (UInt_t j=0; j<fTempMap[i].size(); j++) {
      printf(" %d=%d",j,fTempMap[i][j]);
    }
    printf("\n");
  }
  */
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

