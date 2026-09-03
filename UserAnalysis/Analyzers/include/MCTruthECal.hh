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

  
  Int_t GetVtxFromCluID(Int_t CluId);//{ //returns a sigle vertex associated to a cluster
  std::vector<Int_t> GetPcleFromCluID(Int_t CluId);//{  //returns the list of particles associated to a cluster (numerated following the vertex->GetNParticleOut() order)
  std::vector<Int_t> GetClusFromVtx(Int_t VtxId); //returns the list of clusters associated to a vertex
  Int_t GetCluFromPcle(Int_t VtxId, Int_t PcleId); //returns the cluster associated to a vertex and a particle out from that vertex

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
  std::map<Int_t, Int_t> CluVtxCorr; //cluster-vertex couple
  std::map<Int_t, std::vector<Int_t>> CluPcleCorr; //cluster-list of particles candidates (particle out from the vertex) couple
  std::map<Int_t, std::vector<Int_t>> VtxCluCorr; //vertex-list of clusters associated to the vertex

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

  const int NPcles = 10;


};
#endif
