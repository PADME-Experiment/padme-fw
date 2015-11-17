#ifndef VHisto_h
#define VHisto_h

#include "TH1D.h"
#include "TH2D.h"
#include "TList.h"

class VHisto {

public :

  VHisto(TString);
  ~VHisto();

  void Reset();

  TString GetName() { return fName; };
  
  TH1D* Get1DHisto(TString name) {return (TH1D*)fHistoList->FindObject(name);}; 
  TH2D* Get2DHisto(TString name) {return (TH2D*)fHistoList->FindObject(name);};

  Int_t Fill1DHisto(TString,Float_t);
  Int_t Fill2DHisto(TString,Float_t,Float_t);

protected:

  TString fName;

  TList* fHistoList;

};
#endif
