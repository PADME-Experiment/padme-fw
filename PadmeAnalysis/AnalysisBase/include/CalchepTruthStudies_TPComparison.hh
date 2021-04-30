#ifndef CalchepTruthStudies_TPComparison_h
#define CalchepTruthStudies_TPComparison_h 1

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TFile.h"
#include <fstream>
#include "TRandom2.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TVector3;
class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;


class CalchepTruthStudies_TPComparison : public TObject
{

public:
  CalchepTruthStudies_TPComparison();
  CalchepTruthStudies_TPComparison(Int_t processingMode, Int_t verbosityFlag );
  ~CalchepTruthStudies_TPComparison();
  virtual Bool_t Init(TRecoEvent* eventHeader, 
		      TRecoVObject* ECALev, TRecoVClusCollection* ECALcl);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos();
  virtual Bool_t Process();
  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi);
  void SortCluster_byEnergy();

  void getCalchepTruth();
  
  void CountersExtraction(Double_t rg1T,Double_t phig1, Double_t e1, Double_t rg2T, Double_t phig2, Double_t e2);
  void fillCountersTagInner(Double_t rg1T,Double_t phig1, Double_t rg2T, Double_t phig2);
  void fillCountersTagOuter(Double_t rg1T,Double_t phig1, Double_t rg2T, Double_t phig2);
  void fillCountersInnerNoProbe(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2,Double_t e2T, Double_t  cl1r, Double_t phi1, Double_t cl1e);
  void fillCountersOuterNoProbe(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2,Double_t e2T, Double_t  cl1r, Double_t phi1, Double_t cl1e);
  void fillCountersProbeIn25_TagInner(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2,Double_t e2T, Double_t  cl2r, Double_t phi2, Double_t cl2e);
  void fillCountersProbeIn25_TagOuter(Double_t rg1T,Double_t phig1, Double_t e1T, Double_t rg2T, Double_t phig2,Double_t e2T, Double_t  cl2r, Double_t phi2, Double_t cl2e);
  int returnChiBetterClIndex(int firstClIndex, Double_t constantE, Double_t slopeE, Double_t EnergyFuncExpSecondPhoton);

  void Terminate();
  void printCounters();

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;

  TRecoVClusCollection* fECal_ClColl      ;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  Int_t    fProcessingMode;
  
  Bool_t fInitToComplete;  

private:
  Double_t fdistanceTarget;
  Double_t fFRmin;
  Double_t fFRmid;
  Double_t fFRmax;
  Double_t fminPhiTag;
  Double_t fmaxPhiTag;
  Double_t fminPhiTagTruth;
  Double_t fmaxPhiTagTruth;
  Double_t fminPhiProbeTruth;
  Double_t fmaxPhiProbeTruth;

  Int_t fcountEvent;
  std::vector<int> fIdCl_SortByEnergy;

  ifstream fCalchep450;
  ifstream fCalchep430;

  
  //matrix of counters, 3->three main ranges in radius; 8->8 phi slice
  int nTag[2];
  int nTag_ifTruth[2];
  int nTag_ifTruthOutRIn[2];
  int nTag_ifTruthOutRout[2];

  int nTag_noProbe[2];
  int nTruth_noProbe[2];
  int nTruthOutRIn_noProbe[2];
  int nTruthOutRout_noProbe[2];

  int nProbe25[2];
  int noProbe25[2];
  int nProbe25inbin_ifTruth[2];
  int nProbe25inbin_ifTruthOutRIn[2];
  int nProbe25inbin_ifTruthOutRout[2];
  int nProbe25OutRIn_ifTruth[2];
  int nProbe25OutRIn_ifTruthOutRIn[2];
  int nProbe25OutRIn_ifTruthOutRout[2];
  int nProbe25OutROut_ifTruth[2];
  int nProbe25OutROut_ifTruthOutRIn[2];
  int nProbe25OutROut_ifTruthOutRout[2];
  int nProbe25_noTruthinPhi[2];

  Double_t radiusRangeMin[2];
  Double_t radiusRangeMax[2];
  Double_t phiRange[9];      


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
