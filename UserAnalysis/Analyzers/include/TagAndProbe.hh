#ifndef TagAndProbe_h
#define TagAndProbe_h 1

#include "TH2D.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"

#include "TObject.h"
#include "TVector2.h"
#include "TLorentzVector.h"
#include "PadmeAnalysisEvent.hh"
#include "GeneralInfo.hh"
#include "MCTruthECal.hh"
#include "NPoTAnalysis.hh"
#include "ECalSel.hh"
//#include "MMTrackDevel.hh"
#include "MMFindBestTrack.hh"

#include "HistoSvc.hh"


using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;



struct TagAndProbeEvent{
  int indexECal[3]; // indices of the ecal clusters selected (could become a std vector)
  double totalE;
  double avgT;
  double phi, theta;
  TVector2 cog, xyclu[2]; // cog of the selected clusters
  TLorentzVector labP[2]; // lab momenta
  TLorentzVector cmP[2]; // cm momenta
  int TrackId[3]; // indices of the Track association [pointer to the association array] (could become a std vector)

};



class TagAndProbe : public TObject
{

protected:
  TagAndProbe();

public:
  ~TagAndProbe();
  static TagAndProbe* GetInstance();

private:
  static TagAndProbe* fInstance;
  virtual Bool_t InitHistos();
  utl::ConfigParser* fCfgParser;

  GeneralInfo* fGeneralInfo;
  MCTruthECal* fMCTruthECal;
  ECalSel* fECalSel;
  //MMTrackDevel* fMMTrackDevel;
  MMFindBestTrack* fMMFindBestTrack;
  NPoTAnalysis* fNPoTAnalysis;

public:
  //virtual Bool_t Init(PadmeAnalysisEvent* event);
  virtual Bool_t Init(PadmeAnalysisEvent* event,Bool_t fHistoMode,TString InputHistofile);
  Int_t TagAndProbeSelection();
  Bool_t Finalize();
  Bool_t TagProbeEff();
  Bool_t FitTagProbeEff();
  Bool_t FitTagProbeEffvsPhi();
  Bool_t TagProbeEff_macro();
  

  virtual Bool_t Process();

  
protected:
  PadmeAnalysisEvent*   fEvent;
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent;
  TRecoVClusCollection* fECal_clEvent;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  Int_t    fVerbose;

private:
  // std::vector<std::pair<Int_t, Int_t>> GetCluCouples();
  

  double fTimeSafeMin; // ns, margin in time wrt to start of beam (should do that for end, too?)
  double fMaxTimeDistance; // ns, two-cluster time distance
  double fMinGGDistance; // mm, two-cluster minimum distance
  double fSafeEnergyFactor; // safety factor allowing loosening of the min/max energy cuts used. If set to 1 -> default cuts are used
  double fSafeSpaceMargin;// mm, safety margin used for the radius min cut    
  TString fcfgPath;


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
  Double_t tpHigh=15.;

  Double_t tpLow=-tpHigh;
  Int_t fNYBins;
  Double_t fMeanDTheta = 3.143; //rad
  Double_t fMeanDPhi = 3.143; //rad
  Double_t fSigmaDTheta = 0.0932; //rad
  Double_t fSigmaDPhi = 0.1105; //rad
  Double_t fSigmaCut =3;
  Bool_t   fHistoMode;
  TString  fInputHistofile;
  TString  fNRun;

  TFile *fileIn;
  const double cellSize = 21+0.12;//mm + crystal gap                                                                                                                                                                        
  const int ncells = 29; // per row or column                                                                                                                                                            
  double ecalEdge = (ncells/2+0.5)*cellSize; // 304.5mm                                                                                                                                                  
  const int nhole = 5; // 5x5 matrix is not instrumented                  
  bool fUseMM;
  
  TH1D *fCutFlow;
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
  std::vector<TH2D*> TagSlicevsPhi;
  std::vector<TH2D*> ProbeSlicevsPhi;
  std::vector<TH2D*> DPhiSlicevsPhi;

  TH2D *fhDTheta; 
  TH2D *fhDPhi;
  TH2D *fhDE_E1; 
  TH2D *fhDE_E2;
  double spacing = 5;
  Int_t fNSlicesE;
  HistoSvc* fhSvcVal;   
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
