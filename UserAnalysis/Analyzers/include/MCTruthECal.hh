#ifndef MCTruthECal_h
#define MCTruthECal_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "GeneralInfo.hh"

#include "HistoSvc.hh"
#include <map>

class MCTruthECal 
{

protected:
  //  MCTruthECal();//??
  MCTruthECal(TString c = "config/UserAnalysis.conf", Int_t v = 0);
public:
  ~MCTruthECal();
  static MCTruthECal* GetInstance();//??


  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();
  Bool_t CorrelateVtxClu();
  Bool_t TagAndProbeFromVertex();

  
  Int_t GetVtxFromCluID(Int_t CluId);//{
  //std::vector<Int_t> GetCluFromVtxID(Int_t VtxId);//{//controllare se esiste
  //Int_t* GetCluPcleCorr(Int_t VtxId);
  std::pair<Int_t,Int_t> GetCluPcleCorr(Int_t VtxId);
  //std::map<Int_t, Int_t*> GetCluPcleCorr_all();
  std::map<Int_t, std::pair<Int_t,Int_t>> GetCluPcleCorr_all();;

  //void GetCluPcleCorr(Int_t VtxId, Int_t &Val1, Int_t &Val2);
private:
  static MCTruthECal* fInstance; 
  Bool_t InitHistos();
  Int_t fVerbose;

  GeneralInfo* fGeneralInfo;
  Int_t fCellMap[4000]={-1};
  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;
  TMCVertex* mcVtx;
  std::map<Int_t, Int_t> CluVtxCorr;
  std::map<Int_t, std::vector<Int_t>> VtxCluCorr;
  //std::map<Int_t, Int_t*> VtxPcleCluCorr;
  std::map<Int_t, std::pair<Int_t,Int_t>> VtxPcleCluCorr;

  Double_t fXMin;
  Double_t fXMax;
  Double_t fXW;
  Int_t fNXBins;
  Double_t fYMin;
  Double_t fYMax;
  Double_t fYW;
  Int_t fNYBins;
  TH2D *hBrem, *heIoni, *hAnnihil;
  const double cellSize = 21+0.12;//mm + crystal gap                                                                                                                                                                        
  const int ncells = 29; // per row or column                                                                                                                                                            
  double ecalEdge = (ncells/2+0.5)*cellSize; // 304.5mm                                                                                                                                                  
  const int nhole = 5; // 5x5 matrix is not instrumented                  

  TRecoVClusCollection* fECal_clEvent;
  TRecoEvent*           fRecoEvent;




};
#endif
