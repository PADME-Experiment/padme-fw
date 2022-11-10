#ifndef ETagAn_h
#define ETagAn_h 1

#include "TObject.h"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "ECalSel.hh"

using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;

class ETagAn : public TObject
{

protected:
  ETagAn();
public:
  ~ETagAn();
  static ETagAn* GetInstance();
private:
  static ETagAn* fInstance;
public:
  virtual Bool_t Init(PadmeAnalysisEvent* event);
  Bool_t Finalize();
  virtual Bool_t InitHistos();
  virtual Bool_t Process(Bool_t isMC);  

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVClusCollection* fECal_clEvent    ;
  TRecoVObject*         fETag_hitEvent   ;
  TRecoVClusCollection* fETag_clEvent    ;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  Int_t    fVerbose;

private:
  Double_t fdistanceTarget;
  Int_t ETagMatch();

  std::vector<int> ECalClusInd1; // first cluster of the pair
  std::vector<int> ECalClusInd2; // second cluster of the pair

  int HitToClusterMapLeft[1500];
  int HitToClusterMapRight[1500];

  HistoSvc* fhSvcVal;
  ECalSel* fECalSel;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
