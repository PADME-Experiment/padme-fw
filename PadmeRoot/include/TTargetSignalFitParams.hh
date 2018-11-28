// --------------------------------------------------------------
// History:
//
// Created by Lecce Team () 2017-6-27
//
// --------------------------------------------------------------
//

#include "TObject.h"
#define NfitPar 10
#ifndef TTargetSignalFitParams_H
#define TTargetSignalFitParams_H

class TTargetSignalFitParams : public TObject
{
public:

  TTargetSignalFitParams();
  TTargetSignalFitParams(TClass* hCls);
  virtual ~TTargetSignalFitParams();
  TTargetSignalFitParams(const TTargetSignalFitParams& T);


  int           getChannelId();
  int           getnParWFfit();
  Double_t      getminRange();
  Double_t      getmaxRange();
  Double_t      getWFfitPar(int i);
  Double_t      getWFfitParErrMatrix(int i, int j);
  int           getNdf();
  Double_t      getChi2(); 
  int           getIdWFfitFunction();


  void setChannelId          (int chid);
  void setnParWFfit          (int nParWFfit);
  void setminRange           (Double_t minRange);
  void setmaxRange           (Double_t maxRange);
  void setWFfitPar           (int i, Double_t FfitPar);
  void setWFfitParErrMatrix  (int i, int j, Double_t FfitParErrMatrix);
  void setNdf                (int Ndf);
  void setChi2               (Double_t Chi2);
  void setIdWFfitFuncion     (int IdWFfitFunction);

private:
    int fChannelId;
    int fnParWFfit;
    Double_t fminRange;
    Double_t fmaxRange;
    Double_t fWFfitPar[NfitPar];
    Double_t fWFfitParErrMatrix[NfitPar][NfitPar];
    int fNdf;
    Double_t fChi2; 
    int fIdWFfitFunction;
    
    ClassDef(TTargetSignalFitParams,1);

};
#endif
