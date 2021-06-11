#ifndef CalchepTruthStudies_h
#define CalchepTruthStudies_h 1

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


class CalchepTruthStudies : public TObject
{

public:
  CalchepTruthStudies();
  CalchepTruthStudies(Int_t processingMode, Int_t verbosityFlag );
  ~CalchepTruthStudies();
  virtual Bool_t Init(TRecoEvent* eventHeader, 
		      TRecoVObject* ECALev, TRecoVClusCollection* ECALcl);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos();
  virtual Bool_t Process();
  void SortCluster_byEnergy();

  void FillGeneralHisto(double R1, double E1,double R2, double E2, std::string sufix);
  void FillCalchepSmearedHisto(double R1,double R2);
  void FillComparisonHisto(double R1, double E1,double R2, double E2,double R1reco, double E1reco,double R2reco, double E2reco, std::string sufix);
  void FillHistoNCl3(double R1, double E1,double R2, double E2,double R3, double E3, std::string sufix);
  void getCalchepTruth();
  void extractEfficiency(Double_t rg1T, Double_t eg1T,Double_t phig1, Double_t rg2T, Double_t eg2T,Double_t phig2);
  void FillGeneralHistoEff(Double_t rg1, Double_t eg1, Double_t rg2, Double_t eg2, std::string sufix);
  void FillPhiRHistograms(Double_t rg1, Double_t eg1, Double_t phig1, std::string sufix);
  void FillPhiRHistograms_noFR(Double_t rg1, Double_t eg1, Double_t phig1, std::string sufix);
  void FillPhiRVariablesHistograms_noFR(Double_t rg1, Double_t phig1, std::string sufix);

  void CountersExtraction(Double_t rg1T,Double_t phig1, Double_t rg2T, Double_t phig2);
  void countsRminRmid(Double_t radius,Double_t phi, int p);
  void countsRmidRmax(Double_t radius,Double_t phi, int p);
  void countsRminRmax(Double_t radius,Double_t phi, int p);
  void countsReco(int p);

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

  Int_t fcountEvent;
  std::vector<int> fIdCl_SortByEnergy;

  ifstream fCalchep450;
  ifstream fCalchep430;

  TRandom2 * fr;
  Double_t fSigmaDTheta;

  Bool_t energyCut;
  //matrix of counters, 3->three main ranges in radius; 8->8 phi slice
  int nTruth[3][8];
  int nCluster_ifTruthOutRin[3][8];
  int nCluster_ifTruthOutRout[3][8];
  int nClusterOutRin_ifTruth[3][8];
  int nClusterOutRout_ifTruth[3][8];
  int nCluster_ifTRuth[3][8];
  int nCluster[3][8];
  int nCluster_ifnoTruth[3][8];      

  Double_t radiusRangeMin[3];
  Double_t radiusRangeMax[3];
  Double_t phiRange[9];      


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
