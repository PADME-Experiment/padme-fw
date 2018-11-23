// --------------------------------------------------------------
// History:
//
// Created by Lecce Team () 2017-6-27
//
// --------------------------------------------------------------
//

#include "TTargetSignalFitParams.hh"

ClassImp(TTargetSignalFitParams)

TTargetSignalFitParams::TTargetSignalFitParams()
{
  fChannelId=0;
  fnParWFfit=0;
  fminRange=0.;
  fmaxRange=0.;
  for (int j=0; j<NfitPar; ++j)
    {
      fWFfitPar[j]=0.;
      for (int k=0; k<NfitPar; ++k) fWFfitParErrMatrix[k][j]=0.;
    }
  fNdf=0;
  fChi2=0.; 
  fIdWFfitFunction=0;
}

TTargetSignalFitParams::TTargetSignalFitParams(TClass* hCls)
{
  fChannelId=0;
  fnParWFfit=0;
  fminRange=0.;
  fmaxRange=0.;
  for (int j=0; j<NfitPar; ++j)
    {
      fWFfitPar[j]=0.;
      for (int k=0; k<NfitPar; ++k) fWFfitParErrMatrix[k][j]=0.;
    }
  fNdf=0;
  fChi2=0.; 
  fIdWFfitFunction=0;
}

TTargetSignalFitParams::~TTargetSignalFitParams()
{
}

TTargetSignalFitParams::TTargetSignalFitParams(const TTargetSignalFitParams& T)
{
  fChannelId=T.fChannelId;
  fnParWFfit=T.fnParWFfit;
  fminRange=T.fminRange;
  fmaxRange=T.fmaxRange;
  for (int j=0; j<NfitPar; ++j)
    {
      fWFfitPar[j]=T.fWFfitPar[j];
      for (int k=0; k<NfitPar; ++k) fWFfitParErrMatrix[k][j]=T.fWFfitParErrMatrix[k][j];
    }
  fNdf=T.fNdf;
  fChi2=T.fChi2; 
  fIdWFfitFunction=T.fIdWFfitFunction;
}


int    TTargetSignalFitParams::getChannelId(){return fChannelId;}
int    TTargetSignalFitParams::getnParWFfit(){return fnParWFfit;}
Double_t TTargetSignalFitParams::getminRange(){return fminRange;}
Double_t TTargetSignalFitParams::getmaxRange(){return fmaxRange;}
Double_t TTargetSignalFitParams::getWFfitPar(int i){return fWFfitPar[i];}
Double_t TTargetSignalFitParams::getWFfitParErrMatrix(int i,int j){return fWFfitParErrMatrix[i][j];}
int    TTargetSignalFitParams::getNdf(){return fNdf;}
Double_t   TTargetSignalFitParams::getChi2(){return fChi2;}
int    TTargetSignalFitParams::getIdWFfitFunction(){return fIdWFfitFunction;}

void TTargetSignalFitParams::setChannelId  (int chid) {fChannelId   = chid;}
void TTargetSignalFitParams::setnParWFfit          (int nParWFfit) {fnParWFfit=nParWFfit;}
void TTargetSignalFitParams::setminRange           (Double_t minRange){fminRange=minRange;}
void TTargetSignalFitParams::setmaxRange           (Double_t maxRange){fmaxRange=maxRange;}
void TTargetSignalFitParams::setWFfitPar           (int i, Double_t FfitPar){fWFfitPar[i]=FfitPar;}
void TTargetSignalFitParams::setWFfitParErrMatrix  (int i, int j, Double_t FfitParErrMatrix){fWFfitParErrMatrix[i][j]=FfitParErrMatrix;}
void TTargetSignalFitParams::setNdf                (int Ndf){fNdf=Ndf;}
void TTargetSignalFitParams::setChi2               (Double_t Chi2){fChi2=Chi2;}
void TTargetSignalFitParams::setIdWFfitFuncion     (int IdWFfitFunction){fIdWFfitFunction=IdWFfitFunction;}

