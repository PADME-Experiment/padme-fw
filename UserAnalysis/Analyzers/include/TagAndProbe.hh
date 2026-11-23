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
#include "MCTruthImprovedECal.hh"
#include "NPoTAnalysis.hh"
#include "ECalSel.hh"
//#include "MMTrackDevel.hh"
#include "MMFindBestTrack.hh"



using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;



struct TagAndProbeEvent{
  double cluEnergy[2];
  double Eexp[2];
  double cluTime[2];
  double phiCM[2], thetaCM[2];
  double Ebeam;
  Int_t cluProcess[2];
  Int_t RunID;
  Int_t isMC;
  Int_t HasProbe = -1;
  Double_t EnergyRange[2];
  Double_t RadiusRange[2];
  TVector3 cog, targ, cluPos[2], cluPosExp; // cog of the selected clusters
  Int_t Hit_ix[25], Hit_iy[25];
  Double_t Hit_E[25];
  TVector3 truePos[2];
  TLorentzVector labP[2]; // lab momenta
  TLorentzVector cmP[2]; // cm momenta
  TLorentzVector trueLabP[2]; //mctruth
  TLorentzVector trueCMP[2]; //mctruth
  Double_t purity[2][2]; // indices of the Track association [pointer to the association array] (could become a std vector)
  Double_t purity_combined[2];
  TVector3 trackPosAtECal[2][2]; // position of the track at the ecal
  TVector3 trackPosAtTarget[2][2]; // position of the track at the target
  Double_t trackSlope[2][2]; // slope of the track
  Double_t trackInter[2][2]; // intercept of the track
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
  MCTruthImprovedECal* fMCTruthImprovedECal;
  ECalSel* fECalSel;
  //MMTrackDevel* fMMTrackDevel;
  MMFindBestTrack* fMMFindBestTrack;
  NPoTAnalysis* fNPoTAnalysis;

  
public:
  //virtual Bool_t Init(PadmeAnalysisEvent* event);
  virtual Bool_t Init(PadmeAnalysisEvent* event,Bool_t fHistoMode,TString InputHistofile);
  Int_t TagAndProbeSelection();
  Bool_t Finalize();

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

  Int_t fNYBins;
  Double_t fMeanDTheta;
  Double_t fMeanDPhi;
  Double_t fSigmaDTheta;
  Double_t fSigmaDPhi;
  Double_t fSigmaCut;
  Bool_t   fHistoMode;
  TString  fInputHistofile;
  TString  fNRun;
  Bool_t fisMC;
  TFile *fileIn;
              
  bool fUseMM;
  TTree *ftree;
  TagAndProbeEvent *tpEvent;

  HistoSvc* fhSvcVal;   
  
  
 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
