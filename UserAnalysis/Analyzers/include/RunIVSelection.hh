#ifndef RunIVSelection_h
#define RunIVSelection_h 1

#include "TLorentzVector.h"
#include "TString.h"
#include "TObject.h"

#include "PadmeAnalysisEvent.hh"
#include "GeneralInfo.hh"
#include "MMFindBestTrack.hh"
#include "MCTruthECal.hh"
#include "MCTruthImprovedECal.hh"
#include "NPoTAnalysis.hh"

#include "HistoSvc.hh"
#include "utlConfigParser.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;

struct SelectedEvent{
  int indexECal[2]; // indices of the ecal clusters selected (could become a std vector)
  double energy[2];
  double time[2];
  double Ebeam, gamma;
  double ERange[2];
  double RRange[2];
  double phi[2], thetaLab[2], thetaCM[2], thetaCMinfo[2], phiinfo[2];
  int cluster_Nhits[2];
  TVector2 seedposition[2];
  double seedenergy[2];
  TVector2 cog, xyclu[2], truePos[2]; // cog of the selected clusters
  TLorentzVector labP[2]; // lab momenta
  TLorentzVector trueLabP[2]; // true lab momenta
  TLorentzVector trueCMP[2]; // true cm momenta
  TLorentzVector cmP[2]; // cm momenta
  Double32_t purity[2][2]; // indices of the etag association [pointer to the association array] (could become a std vector)
  int processID[2]; // indices of the etag association [pointer to the association array] (could become a std vector)
  Double32_t purity_comb[2]; // indices of the etag association [pointer to the association array] (could become a std vector)
    TVector3 trackPosAtECal[2][2]; // position of the track at the ecal
  TVector3 trackPosAtTarget[2][2]; // position of the track at the target
  Double_t trackSlope[2][2]; // slope of the track
  Double_t trackInter[2][2]; // intercept of the track
};

struct BadDQTimeInterval {
    long long tStart;
    long long tStop;
};

class RunIVSelection : public TObject
{

protected:
  RunIVSelection();

public:
  ~RunIVSelection();
  static RunIVSelection* GetInstance();

private:
  static RunIVSelection* fInstance;
  virtual Bool_t InitHistos();

public:
  virtual Bool_t Init(PadmeAnalysisEvent* event, Bool_t fHistoMode, TString InputHistofile);
  virtual Bool_t Process();
  Bool_t Finalize();

protected:
  PadmeAnalysisEvent*   fEvent;
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent;
  TRecoVClusCollection* fECal_clEvent;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  Int_t fVerbose;

private:
  std::vector<std::pair<Int_t, Int_t>> GetCluCouples();
  Bool_t SelectTwoClusters();
  Double_t NPoTLGCorr();
  Bool_t NSignalBhabha();


  utl::ConfigParser* fCfgParser;
  GeneralInfo*       fGeneralInfo;
  HistoSvc*          fhSvcVal;
  MCTruthECal*       fMCTruthECal;
  MCTruthImprovedECal* fMCTruthECalImproved;
  MMFindBestTrack*   fMMFindBestTrack;
  NPoTAnalysis*      fNPoTAnalysis;
  Double_t fSigmaCut;
  Double_t fSigmaDPhi;
  Double_t fSigmaDTheta;
  Double_t fMeanDPhi;
  Double_t fMeanDTheta;
  Double_t fSafeSpaceMargin;
  Double_t fMaxTimeDistance;
  Double_t fSafeEnergyFactor;
  Bool_t fisMC;
  Bool_t  fHistoMode;
  TString InputHistofile;
  std::vector<BadDQTimeInterval> fBadDQIntervals;
  Bool_t fApplyDataQuality;
  TTree* ftree;
  SelectedEvent* selev;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
