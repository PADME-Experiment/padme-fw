#include "EEShapes.h"
#include "RooAbsCategory.h"
#include "RooRealVar.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooRealConstant.h"
#include "RooAbsPdf.h"
#include "TMath.h"
#include <math.h>

EEShapes::EEShapes(TString name, TString title, RooRealVar &_sqrts, Double_t &_dsqrts,
                   RooRealVar &_norm_core_true, RooRealVar &_xmass, RooRealVar &_sigma_core_true,
                   RooRealVar &_norm_tail_true, RooRealVar &_sigma_tail_true, RooRealVar &_bes_true,
                   double _xMin, double _xMax)
    : /*RooAbsPdf(name, title),*/ SqrtS(&_sqrts), dsqrtS(&_dsqrts),
      normCore_true(&_norm_core_true), XMass(&_xmass), sigmaCore_true(&_sigma_core_true),
      normTail_true(&_norm_tail_true), sigmaTail_true(&_sigma_tail_true), Bes_true(&_bes_true) {
  // Create TF1 with a sum of two Gaussians
  tf1 = new TF1("tf1", EEShapes::TF1Function, xMin, xMax, 6);
  tf1->SetParameters(normCore_true->getVal(), XMass->getVal(), sigmaCore_true->getVal(),
                     normTail_true->getVal(), sigmaTail_true->getVal(), Bes_true->getVal());
}

// EEShapes::EEShapes(const EEShapes& other, TString name)
//     : RooAbsPdf(other, name), SqrtS(other._x),
//       normCore_true(other._norm_core_true), XMass(other._xmass), sigmaCore_true(other._sigma_core_true),
//       normTail_true(other._norm_tail_true), sigmaTail_true(other._sigma_tail_true), Bes_true(other._bes_true),
//       xMin(other._xMin), xMax(other._xMax), tf1(new TF1(*(other.tf1))) {}

Double_t EEShapes::TF1Function(Double_t* dsqrts, Double_t* par) {
  Double_t gauss1 = par[0] * 1/(sqrt((par[2]*par[2] + par[5]*par[5]))*2*(TMath::Pi())) * exp(-0.5 * pow((dsqrts[0] - par[1]) / sqrt(par[2]*par[2] + par[5]*par[5]), 2));
  Double_t gauss2 = par[3] * 1/(sqrt((par[4]*par[4] + par[5]*par[5]))*2*(TMath::Pi())) * exp(-0.5 * pow((dsqrts[0] - par[1]) / sqrt(par[4]*par[4] + par[5]*par[5]), 2));
  return gauss1 + gauss2;
}

Double_t EEShapes::evaluate() const {
  // Use TF1::Eval to evaluate the function at the current value of x
  return tf1->Eval(SqrtS->getVal());
}

// Int_t EEShapes::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const {
//   if (matchArgs(allVars, analVars, *SqrtS)) return 1;
//   return 0;
// }

Double_t EEShapes::analyticalIntegral(Int_t code, double rangeMin, double rangeMax) const {
  assert(code == 1);
  return tf1->Integral(rangeMin, rangeMax);
}

EEShapes::~EEShapes() { delete tf1;}



// #include "EEShapes.h"
// #include "RooAbsCategory.h"
// #include "RooRealVar.h"
// #include "RooAbsReal.h"
// #include "RooArgList.h"
// #include "RooRealConstant.h"
// #include "TMath.h"
// #include <math.h>

// EEShapes::EEShapes(const char* name, const char* title, RooRealVar &_sqrts, Double_t &_dsqrts,
//                    RooRealVar &_norm_core_true, RooRealVar &_xmass, RooRealVar &_sigma_core_true,
//                    RooRealVar &_norm_tail_true, RooRealVar &_sigma_tail_true, RooRealVar &_bes_true,
//                    double _xMin, double _xMax)
//     : RooAbsPdf(name, title), SqrtS(&_sqrts), dsqrtS(&_dsqrts),
//       normCore_true(&_norm_core_true), XMass(&_xmass), sigmaCore_true(&_sigma_core_true),
//       normTail_true(&_norm_tail_true), sigmaTail_true(&_sigma_tail_true), Bes_true(&_bes_true),
//       xMin(_xMin), xMax(_xMax) {
//   // Create TF1 with a sum of two Gaussians
//   tf1 = new TF1("tf1", EEShapes::TF1Function, xMin, xMax, 6);
//   tf1->SetParameters(normCore_true->getVal(), XMass->getVal(), sigmaCore_true->getVal(),
//                      normTail_true->getVal(), sigmaTail_true->getVal(), Bes_true->getVal());
// }

// EEShapes::EEShapes(const EEShapes& other, const char* name)
//     : RooAbsPdf(other, name), SqrtS(other.SqrtS), dsqrtS(other.dsqrtS),
//       normCore_true(other.normCore_true), XMass(other.XMass), sigmaCore_true(other.sigmaCore_true),
//       normTail_true(other.normTail_true), sigmaTail_true(other.sigmaTail_true), Bes_true(other.Bes_true),
//       xMin(other.xMin), xMax(other.xMax), tf1(new TF1(*(other.tf1))) {}

// EEShapes::~EEShapes() {
//   delete tf1;
// }

// Double_t EEShapes::TF1Function(Double_t* dsqrts, Double_t* par) {
//   Double_t gauss1 = par[0] * 1/(sqrt((par[2]*par[2] + par[5]*par[5]))*2*(TMath::Pi())) * exp(-0.5 * pow((dsqrts[0] - par[1]) / sqrt(par[2]*par[2] + par[5]*par[5]), 2));
//   Double_t gauss2 = par[3] * 1/(sqrt((par[4]*par[4] + par[5]*par[5]))*2*(TMath::Pi())) * exp(-0.5 * pow((dsqrts[0] - par[1]) / sqrt(par[4]*par[4] + par[5]*par[5]), 2));
//   return gauss1 + gauss2;
// }

// Double_t EEShapes::evaluate() const {
//   return tf1->Eval(SqrtS->getVal());
// }

// Int_t EEShapes::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const {
//   if (matchArgs(allVars, analVars, *SqrtS)) return 1;
//   return 0;
// }

// Double_t EEShapes::analyticalIntegral(Int_t code, double rangeMin, double rangeMax) const {
//   assert(code == 1);
//   return tf1->Integral(rangeMin, rangeMax);
// }

// ClassImp(EEShapes)
