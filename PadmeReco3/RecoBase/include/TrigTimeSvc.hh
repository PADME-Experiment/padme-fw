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

  Double_t GetReferencePoint(UChar_t,UChar_t);
  UChar_t GetGroupFrequency(UChar_t,UChar_t);
  Double_t GetTimeDifference(UChar_t,UChar_t,Double_t);

  void SetVerbose(Int_t v){ fVerbose = v; }

private:

  Double_t ComputeReferencePoint(UChar_t, Short_t*);

  Int_t fVerbose;

  // Maps <board_id,group_id> to <frequency,reference_sample>
  std::map<std::pair<UChar_t,UChar_t>,std::pair<UChar_t,Double_t>> fTrigTimeMap;

  Short_t fMaxAvgStart[3];
  Short_t fMaxAvgEnd[3];
  Short_t fMinAvgStart[3];
  Short_t fMinAvgEnd[3];

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
