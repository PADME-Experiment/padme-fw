#ifndef TrigTimeSvc_h
#define TrigTimeSvc_h 1

#include <map>

#include <Rtypes.h>

class TRawEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TrigTimeSvc
{

protected:
  TrigTimeSvc();

public:

  ~TrigTimeSvc();
  static TrigTimeSvc* GetInstance();

private:

  static TrigTimeSvc* fInstance;

public:

  Bool_t Initialize();
  Bool_t Finalize();

  Bool_t ProcessEvent(TRawEvent*);

  Int_t GetMethod() { return fMethod; }
  void SetMethod(Int_t m) { fMethod = m; }

  Double_t GetCFShift() { return fCFShift; }
  void SetCFShift(Double_t m) { fCFShift = m; }

  Double_t GetCFRatio() { return fCFRatio; }
  void SetCFRatio(Double_t m) { fCFRatio = m; }

  Double_t GetCFThreshold() { return fCFThreshold; }
  void SetCFThreshold(Double_t m) { fCFThreshold = m; }

  UChar_t GetGroupFrequency(UChar_t,UChar_t);
  UShort_t GetStartIndexCell(UChar_t,UChar_t);
  Double_t GetReferencePoint(UChar_t,UChar_t);

  Double_t GetTimeDifference(UChar_t,UChar_t,Double_t);

  void SetVerbose(Int_t v){ fVerbose = v; }

private:

  Double_t ComputeReferencePoint(UChar_t, Short_t*);
  Double_t ComputeReferencePointCF(UChar_t, Short_t*);

  Int_t fVerbose;

 // Method to use for trigger time evaluation - 0: Level based - 1: Constant fraction
  Int_t fMethod;

  // Constant fraction parameters
  Int_t    fCFShift;     // Number of bins to shift
  Double_t fCFRatio;     // Multiplier for shifted waveform
  Double_t fCFThreshold; // Threshold value to define trigger time
  Int_t    fPedestalSamples; // Number of samples to use to compute channel pedestal

  //// Maps <board_id,group_id> to <frequency,reference_sample>
  //std::map<std::pair<UChar_t,UChar_t>,std::pair<UChar_t,Double_t>> fTrigTimeMap;
  // Maps <board_id,group_id> to <frequency,sic,reference_sample>
  std::map<std::pair<UChar_t,UChar_t>,std::tuple<UChar_t,UShort_t,Double_t>> fTrigTimeMap;

  Short_t fMaxAvgStart[3];
  Short_t fMaxAvgEnd[3];
  Short_t fMinAvgStart[3];
  Short_t fMinAvgEnd[3];

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
