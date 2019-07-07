#ifndef ECalTemplate_h
#define ECalTemplate_h

#include <iostream>
#include "cstdlib"

#include "TFile.h"
#include "TList.h"
#include "TF1.h"
#include "TH1D.h"
#include "TTree.h"
#include "TProfile.h"
#include "TSpline.h"

class ECalTemplate {

public :

  static ECalTemplate* GetInstance();
  ~ECalTemplate();

private:

  static ECalTemplate* fInstance;

protected:

  ECalTemplate();

public:
  void Init();
  void WriteHist();
  void PrepareTemplate(Short_t * v, Double_t time);
  void ReadTemplate();
  void BuildFitFunction();
  TSpline3 * GetTemplateSpline(){return fTemplateSpline;};
  TF1 * GetECalFitFunction(){return fECalFitFunction;};

  TSpline3 * fTemplateSpline;

private :
  std::vector <TH1D*> hVSig;
  std::vector <TH1D*> hVTemp;

  TTree* ECal;
  TList* hListTemplate;// single board related histograms 
  TFile * fileOut;
  
  TH1D * hTMax; 
  TH1D * HistoTemp;
  TH1D * hTemplate;
  //  TH1D * hTMax;    

  Double_t fVMax;
  TH1D * hSample;
  TProfile* hprof;
  Int_t NHist=0;
  double fTemplateVec[1000];
  TF1 * fECalFitFunction;

  Short_t * fSample;
};
#endif
