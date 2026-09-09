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

  Double_t PFuncAtECal(Double_t x, Double_t y, Double_t *p);
  Int_t ChargeFinder(Double_t x, Double_t y, Double_t E);
  Double_t PurityFunc(Double_t x, Double_t y, Double_t *p);
  Int_t QualityBin(Double_t purity, Int_t type) {
    //        purity_TAG < 0      --> quality_bin_TAG = 0 (no track considered)
    //    0 < purity_TAG < 0.02   --> quality_bin_TAG = 1 (bad purity)
    // 0.02 < purity_TAG < 0.2    --> quality_bin_TAG = 2
    //  0.2 < purity_TAG < 0.6    --> quality_bin_TAG = 3
    //  0.6 < purity_TAG < 1    --> quality_bin_TAG = 4
    //        purity_PROBE < 0      --> quality_bin_PROBE = 0 (no track considered)
    //    0 < purity_PROBE < 0.02   --> quality_bin_PROBE = 1 (bad purity)
    // 0.02 < purity_PROBE < 0.2    --> quality_bin_PROBE = 2
    //  0.2 < purity_PROBE < 0.6    --> quality_bin_PROBE = 3
    //  0.6 < purity_PROBE < 1    --> quality_bin_PROBE = 4
    
    if(type == 0) { //TAG
      if(purity < 0)     return 0;
      if(purity < 0.02)  return 1;
      if(purity < 0.2)   return 2;
      if(purity < 0.6)   return 3;
      return 4;
    }
    else { //PROBE
      if(purity < 0)     return 0;
      if(purity < 0.02)  return 1;
      if(purity < 0.2)   return 2;
      if(purity < 0.6)   return 3;
      return 4;
    }
  };
  
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

  //array of parameters for B=100G pid
  Double_t par_ele_B100G[12] = {311.339, 22.8641, -1.38227, -399.42, 76.8035, 8.07501, 42.0509, -24.3585, -15.0576, 101.51, -39.3708, 8.6695};
  Double_t par_ele_err_B100G[12] = {0.940137, 1.19608, 0.911382, 5.51391, 7.02906, 5.80897, 10.0573, 13.0114, 11.0776, 5.76637, 7.59705, 6.45178};
  Double_t par_pos_B100G[12] = {287.734, -13.3566, -2.21919, -267.268, -112.543, 12.7678, -183.679, 62.481, -23.168, 221.518, 28.9116, 13.222};
  Double_t par_pos_err_B100G[12] = {3.24053, 4.13089, 3.21182, 19.1165, 24.4124, 20.5565, 35.0005, 45.3913, 39.2735, 20.104, 26.5749, 22.8867};
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
