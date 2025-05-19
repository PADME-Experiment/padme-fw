#ifndef ECalETagMatching_h
#define ECalETagMatching_h 1

#include "TH2D.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"

#include "TObject.h"
#include "TVector2.h"
#include "PadmeAnalysisEvent.hh"
#include "GeneralInfo.hh"
#include "MCTruthECal.hh"
#include "NPoTAnalysis.hh"
#include "ETagAn.hh"
#include "ECalSel.hh"
#include "HistoSvc.hh"


using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;
class ETagAn;
class ECalSel;

// enum eventFlag{
//   ev_gg  = 0,
//   ev_ee  = 1,
//   ev_ggg = 2,
//   ev_eeg = 3,
//   ev_single = 4,
//   ev_eg = 5 
// };

// enum algoFlag{
//   angles  = 0,
//   estars  = 1,
//   tagAndProbe = 2
// };


// struct Point {
//     double x, y;
// };


struct ECalETagMatchedEvent{
  eventFlag flagEv;
  algoFlag flagAlgo;
  int indexECal[3]; // indices of the ecal clusters selected (could become a std vector)
  double totalE;
  double avgT;
  double phi, theta;
  TVector2 cog, xyclu[2]; // cog and xy of the selected clusters
  int indexETagAss[3]; // indices of the etag association [pointer to the association array] (could become a std vector)
};



class ECalETagMatching : public TObject
{

protected:
  ECalETagMatching();

public:
  ~ECalETagMatching();
  static ECalETagMatching* GetInstance();


private:
  static ECalETagMatching* fInstance;
  virtual Bool_t InitHistos();

public:
  //virtual Bool_t Init(PadmeAnalysisEvent* event);
  virtual Bool_t Init(PadmeAnalysisEvent* event);

  Bool_t Finalize();  
  Int_t ECalETagMatch();

  virtual Bool_t Process();

  // int getNECalEvents(){return (int) fECalEvents.size();}
  // ECalETagMatchedEvent getECalETagMatchedEvent(int i){return fECalEvents.at(i);} //to be protected

protected:
  PadmeAnalysisEvent* fEvent;
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVClusCollection* fECal_clEvent    ;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  Int_t    fVerbose;

private:
  
  utl::ConfigParser* fCfgParser;
  std::vector<ECalSelEvent> fECalEvents; //occhio
  GeneralInfo* fGeneralInfo;
  NPoTAnalysis* fNPoTAnalysis;
  ETagAn* fETagAn;
  ECalSel* fECalSel;
  HistoSvc * fhSvcVal;
  // general setup
  

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif