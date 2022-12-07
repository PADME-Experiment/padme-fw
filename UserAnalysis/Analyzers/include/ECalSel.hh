#ifndef ECalSel_h
#define ECalSel_h 1

#include "TObject.h"
#include "TVector2.h"
#include "PadmeAnalysisEvent.hh"
#include "GeneralInfo.hh"
#include "HistoSvc.hh"


using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;

enum eventFlag{
  ev_gg  = 0,
  ev_ee  = 1,
  ev_ggg = 2,
  ev_eeg = 3
};

struct ECalSelEvent{
  eventFlag flagEv;
  int indexECal[3]; // indices of the ecal clusters selected
  double totalE;
  double avgT;
  TVector2 cog; // cog of the selected clusters
  int indexETagAss[3]; // indices of the etag association [pointer to the association array]
};



class ECalSel : public TObject
{

protected:
  ECalSel();

public:
  ~ECalSel();
  static ECalSel* GetInstance();

private:
  static ECalSel* fInstance;
  virtual Bool_t InitHistos();

public:
  virtual Bool_t Init(PadmeAnalysisEvent* event);
  Bool_t Finalize();
  virtual Bool_t Process();

  int getNECalEvents(){return (int) fECalEvents.size();}
  ECalSelEvent getECalEvent(int i){return fECalEvents.at(i);} //to be protected

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVClusCollection* fECal_clEvent    ;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  Int_t    fVerbose;

private:
  Int_t TwoClusSel();
  std::vector<ECalSelEvent> fECalEvents;
  GeneralInfo* fGeneralInfo;

  int fNThetaBins;
  double fThetaWid;
  int fNPhiDirBins;
  double fPhiDirWid;
  Double_t fXMin;
  Double_t fXMax;
  Double_t fXW;
  Int_t fNXBins;
  Double_t fYMin;
  Double_t fYMax;
  Double_t fYW;
  Int_t fNYBins;

  HistoSvc* fhSvcVal;   
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
