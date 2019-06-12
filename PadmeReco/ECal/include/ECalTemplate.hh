#ifndef ECalTemplate_h
#define ECalTemplate_h

#include <iostream>
#include "cstdlib"

#include "TFile.h"
#include "TList.h"
#include "TH1D.h"
#include "TTree.h"
#include "TProfile.h"

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
private :

  TTree* ECal;
  TList* hListTemplate;// single board related histograms 
  TFile * fileOut;

  Double_t fVMax;

  TH1D * hSample;
  TProfile* hprof;
//  TH1F * hdxdtRMS;
//  TH1F * hTime;
//  TH1F * hTIntCorr;
//  TH1F * hTimeCut;
//  TH1F * hTimeOv;
//  TH1D * hSignal;
  TH1D * hSat;
  //  TH1F * hDiff;
  

  // Geometry parameters
//  Int_t fECalNRows;
//  Int_t fECalNCols;
  Short_t * fSample;
};
#endif
