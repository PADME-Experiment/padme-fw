#ifndef EESHAPES_H
#define EESHAPES_H

#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "TF1.h"

class EEShapes{
public:

  EEShapes(TString name, TString title, RooRealVar &_sqrts, Double_t &_dsqrts,
           RooRealVar &_norm_core_true, RooRealVar &_xmass, RooRealVar &_sigma_core_true,
           RooRealVar &_norm_tail_true, RooRealVar &_sigma_tail_true, RooRealVar &_bes_true,
           double _xMin, double _xMax);
  ~EEShapes();

  Double_t evaluate() const;
  // Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const;
  Double_t analyticalIntegral(Int_t code, double rangeMin, double rangeMax) const;

  static Double_t TF1Function(Double_t* dsqrtS, Double_t* par);

public:
  RooRealVar *SqrtS;
  Double_t *dsqrtS;
  RooRealVar *normCore_true;
  RooRealVar *XMass;
  RooRealVar *sigmaCore_true;
  RooRealVar *normTail_true;
  RooRealVar *sigmaTail_true;
  RooRealVar *Bes_true;
  Double_t xMin;
  Double_t xMax;
  TF1 *tf1;

};

#endif

// #ifndef EESHAPES_H
// #define EESHAPES_H

// #include "RooAbsPdf.h"
// #include "RooRealVar.h"
// #include "RooArgSet.h"
// #include "TF1.h"

// class EEShapes : public RooAbsPdf {
// public:
//   EEShapes(const char* name, const char* title, RooRealVar &_sqrts, Double_t &_dsqrts,
//            RooRealVar &_norm_core_true, RooRealVar &_xmass, RooRealVar &_sigma_core_true,
//            RooRealVar &_norm_tail_true, RooRealVar &_sigma_tail_true, RooRealVar &_bes_true,
//            double _xMin, double _xMax);
//   EEShapes(const EEShapes& other, const char* name = 0);
//   virtual ~EEShapes();

//   TObject* clone(const char* newname) const { return new EEShapes(*this, newname); }

//   Double_t evaluate() const;
//   Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const;
//   Double_t analyticalIntegral(Int_t code, double rangeMin, double rangeMax) const;

//   static Double_t TF1Function(Double_t* dsqrtS, Double_t* par);

// private:
//   RooRealVar *SqrtS;
//   Double_t *dsqrtS;
//   RooRealVar *normCore_true;
//   RooRealVar *XMass;
//   RooRealVar *sigmaCore_true;
//   RooRealVar *normTail_true;
//   RooRealVar *sigmaTail_true;
//   RooRealVar *Bes_true;
//   Double_t xMin;
//   Double_t xMax;
//   TF1 *tf1;

//   ClassDef(EEShapes, 1)
// };

// #endif


