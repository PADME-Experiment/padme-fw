#ifndef ECalSel_h
#define ECalSel_h 1

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
  ev_eeg = 3,
  ev_single = 4
};

enum algoFlag{
  angles  = 0,
  estars  = 1,
  tagAndProbe = 2
};


struct ECalSelEvent{
  eventFlag flagEv;
  algoFlag flagAlgo;
  int indexECal[3]; // indices of the ecal clusters selected (could become a std vector)
  double totalE;
  double avgT;
  TVector2 cog; // cog of the selected clusters
  int indexETagAss[3]; // indices of the etag association [pointer to the association array] (could become a std vector)
};



class ECalSel : public TObject
{

protected:
  ECalSel();

public:
  ~ECalSel();
  static ECalSel* GetInstance();
  // special process modes
  void ProcessForCalib();


private:
  static ECalSel* fInstance;
  virtual Bool_t InitHistos();

public:
  //virtual Bool_t Init(PadmeAnalysisEvent* event);
  virtual Bool_t Init(PadmeAnalysisEvent* event,Bool_t fHistoMode,TString InputHistofile);

  Bool_t Finalize();
  Bool_t TagProbeEff();
  Bool_t MCTagProbeEff();
  Bool_t FitTagProbeEff();
  Bool_t FitTagProbeEffvsPhi();
  //Bool_t FitTagProbeEff_notarg();
  Bool_t TagProbeEff_macro();
  Bool_t EvaluateResolutions();
  Bool_t EvaluateResolutions_macro();
  


  virtual Bool_t Process();

  int getNECalEvents(){return (int) fECalEvents.size();}
  ECalSelEvent getECalEvent(int i){return fECalEvents.at(i);} //to be protected

protected:
  PadmeAnalysisEvent* fEvent;
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVClusCollection* fECal_clEvent    ;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  Int_t    fVerbose;

private:
  Int_t TwoClusSel();
  Int_t OneClusSel();
  Int_t OneClusTagAndProbeSel();
  Double_t NPoTLGCorr();
  Bool_t NSignalBhabha();
  utl::ConfigParser* fCfgParser;
  std::vector<ECalSelEvent> fECalEvents;
  GeneralInfo* fGeneralInfo;
  MCTruthECal* fMCTruthECal;
  NPoTAnalysis* fNPoTAnalysis;
  // general setup
  Bool_t fApplyCorrection;
  bool fFillLocalHistograms;
  bool fFillCalibHistograms;
  Bool_t fHistoMode;
  TString InputHistofile;
  TString InputHistofileName;
  TString fNRun;
  // general cuts

  double fTimeSafeMin; // ns, margin in time wrt to start of beam (should do that for end, too?)
  double fMaxTimeDistance; // ns, two-cluster time distance
  double fMinGGDistance; // mm, two-cluster minimum distance
  double fSafeEnergyFactor; // safety factor allowing loosening of the min/max energy cuts used. If set to 1 -> default cuts are used
  double fSafeSpaceMargin;// mm, safety margin used for the radius min cut    
  

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
  Double_t tpLow=-15.;
  Double_t tpHigh=15.;
  Int_t fNYBins;
  Double_t fMeanDTheta = 3.113; //rad
  Double_t fMeanDPhi = 3.093; //rad
  Double_t fSigmaDTheta = 0.0932; //rad
  Double_t fSigmaDPhi = 0.1105; //rad
  Double_t fSigmaE = 12.64;
  Double_t fSigmaCut =3;

  TFile *fileIn;
  const double cellSize = 21+0.12;//mm + crystal gap                                                                                                                                                                        
  const int ncells = 29; // per row or column                                                                                                                                                            
  double ecalEdge = (ncells/2+0.5)*cellSize; // 304.5mm                                                                                                                                                  
  const int nhole = 5; // 5x5 matrix is not instrumented                  

  

  TH2D *EofTag;
  TH2D *EofeIoni;
  TH2D *EofAnnihil;
  TH2D *PhiofTag;
  TH2D *EofProbe;
  TH2D *PhiofProbe;
  TH2D *DeltaPhiofProbe;
  TH2D *EofTag_cut;
  TH2D *PhiofTag_cut;
  TH2D *EofProbe_cut;
  TH2D *PhiofProbe_cut;
  TH2D *DeltaPhiofProbe_cut;
  TH1D *PhiFullProbe;
  std::vector<TH1D*> PhiFullProbeSlice;

  TH2D *fhDTheta; 
  TH2D *fhDPhi;
  TH2D *fhDE_E1; 
  TH2D *fhDE_E2;
  double spacing = 5;
  Int_t NSlicesE;
  HistoSvc* fhSvcVal;   
  TGraphErrors* MCTPEff; 
  TGraphErrors* MCTrueDeno;
  TGraphErrors* DATATPEff; 
  TGraphErrors*  EffRatio; 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif