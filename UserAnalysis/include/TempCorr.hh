#ifndef TempCorr_h
#define TempCorr_h 1

#include "TTimeStamp.h"

#define TEMPCORR_NCHANNELS 2
#define TEMPCORR_TIMERESOLUTION 60
#define TEMPCORR_TEMPRESOLUTION 0.01

class TempCorr
{

protected:

  TempCorr();

public:

  ~TempCorr();
  static TempCorr* GetInstance();

private:

  static TempCorr* fInstance;

public:

  Int_t Initialize();
  Int_t Initialize(TString);

  Double_t GetTemp(TTimeStamp);
  Double_t GetTemp(TTimeStamp,Int_t);
  Double_t GetTemp(TTimeStamp,Int_t,Int_t);

  Double_t GetTempCorr(TTimeStamp);
  Double_t GetTempCorr(TTimeStamp,Int_t);
  Double_t GetTempCorr(TTimeStamp,Int_t,Int_t);

  Int_t GetChannelCode(Int_t chan) { return fChanCodeList[chan]; }
  Int_t GetChannelNumber(Int_t code) { return fChanNumber[code]; }

private:

  Int_t GetTimeIndex(TTimeStamp);
  Double_t ComputeCorrection(Double_t);

  Int_t fVerbose;

  TString fTemperatureFileName;

  Long_t   fTimeRef;
  Double_t fTempRef;

  Int_t fEntries;
  Int_t fChanCodeList[TEMPCORR_NCHANNELS] = {106,211};
  std::map<Int_t,Int_t> fChanNumber;
  Int_t fChanMap[29][29];
  std::vector<std::vector<Short_t>> fTempMap;

};
#endif
