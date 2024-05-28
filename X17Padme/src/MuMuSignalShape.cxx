#include "Riostream.h"

#include "MuMuSignalShape.h"
#include "RooAbsCategory.h"
#include "RooAbsReal.h"
#include "TMath.h"
#include <math.h>

// ClassImp(MuMuSignalShape);

MuMuSignalShape::MuMuSignalShape(const char *name, const char *title, RooAbsReal &_x,
                       RooAbsReal &_mean, RooAbsReal &_sigma, RooAbsReal &_xMin,
                       RooAbsReal &_xMax)
    : RooAbsPdf(name, title), x("x", "x", this, _x),
      mean("mean", "mean", this, _mean), sigma("sigma", "sigma", this, _sigma),
      xMin("xMin", "xMin", this, _xMin), xMax("xMax", "xMax", this, _xMax) {}

MuMuSignalShape::MuMuSignalShape(const MuMuSignalShape &other, const char *name)
    : RooAbsPdf(other, name), x("x", this, other.x),
      mean("mean", this, other.mean), sigma("sigma", this, other.sigma),
      xMin("xMin", this, other.xMin), xMax("xMax", this, other.xMax) {}

Double_t MuMuSignalShape::evaluate() const {
  if (x < xMin) return 0.;
  if (x > xMax) return 0.;

  return TMath::Exp(-0.5*(x-mean)*(x-mean)/(sigma*sigma));
}

Int_t MuMuSignalShape::getAnalyticalIntegral(RooArgSet &allVars, RooArgSet& analVars, const char* /*rangeName*/) const{
  if (matchArgs(allVars, analVars, x)) return 1;
  return 0;
}

double MuMuSignalShape::analyticalIntegral(Int_t code, const char *rangeName) const
{
  R__ASSERT(code==1) ;
  
  
  double alpha = (xMin-mean)/sigma;
  double beta = (xMax-mean)/sigma;

  double phi_alpha = getGaussIntegral(alpha);
  double phi_beta = getGaussIntegral(beta);
  
  double sqrt_pi_over_2 = TMath::Sqrt(TMath::Pi()/2.);
  
  double result = sqrt_pi_over_2*sigma*(phi_beta-phi_alpha);
  return 2.0*result;
}

double MuMuSignalShape::getGaussIntegral(double a){
  return 0.5*(1.0+TMath::Erf(a/TMath::Sqrt(2.0)));
}