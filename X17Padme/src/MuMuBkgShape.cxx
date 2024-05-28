#include "Riostream.h"

#include "MuMuBkgShape.h"
#include "RooAbsCategory.h"
#include "RooAbsReal.h"
#include "TMath.h"
#include "RooArgList.h"

#include <math.h>

// ClassImp(MuMuBkgShape);

MuMuBkgShape::MuMuBkgShape(const char *name, const char *title, RooAbsReal &_x,
                           const RooArgList &_coeffs, RooAbsReal &_xMin,
                           RooAbsReal &_xMax)
    : RooAbsPdf(name, title), x("x", "x", this, _x),
      coeffs("coeffs", "coeffs", this), xMin("xMin", "xMin", this, _xMin), xMax("xMax", "xMax", this,_xMax) {
  for (auto *coef : _coeffs) {
    if (!dynamic_cast<RooAbsReal *>(coef)) {
      std::cout << "MuMuBkgShape::ctor(" << GetName() << ") ERROR: coefficient "
                << coef->GetName() << " is not of type RooAbsReal "
                << std::endl;
      R__ASSERT(0);
    }
    coeffs.add(*coef);
  }
}

MuMuBkgShape::MuMuBkgShape(const MuMuBkgShape &other, const char *name)
    : RooAbsPdf(other, name), x("x", this, other.x),
      coeffs("coeffs", this, other.coeffs), xMin("xMin", this, other.xMin),
      xMax("xMax", this, other.xMax) {}

Double_t MuMuBkgShape::evaluate() const {
  if (x < xMin)
    return 0.;
  if (x > xMax)
    return 0.;

  int degree = coeffs.getSize() - 1;
  double xx = (x - xMin) / (xMax - xMin);
  double t = xx;
  double s = 1.0 - xx;

  double result = static_cast<RooAbsReal &>(coeffs[0]).getVal() * s;

  for (int i = 1; i < degree; i++) {
    result = (result + t * TMath::Binomial(degree, i) *
                           static_cast<RooAbsReal &>(coeffs[i]).getVal()) *
             s;
    t = t * xx;
  }

  result = result + t * static_cast<RooAbsReal &>(coeffs[degree]).getVal();
  return result;
}

Int_t MuMuBkgShape::getAnalyticalIntegral(RooArgSet &allVars,
                                          RooArgSet &analVars,
                                          const char * /*rangeName*/) const {

  if (matchArgs(allVars, analVars, x))
    return 1;
  return 0;
}

double MuMuBkgShape::analyticalIntegral(Int_t code,
                                        const char *rangeName) const {
  R__ASSERT(code == 1);

  const double xlo = 0.;
  const double xhi = 1.;

  Int_t degree = coeffs.getSize() - 1; // n+1 polys of degree n
  double norm(0);

  double temp = 0;
  for (int i = 0; i <= degree; ++i) {
    // for each of the i Bernstein basis polynomials
    // represent it in the 'power basis' (the naive polynomial basis)
    // where the integral is straight forward.
    temp = 0;
    for (int j = i; j <= degree; ++j) { // power basis≈ß
      temp += pow(-1., j - i) * TMath::Binomial(degree, j) *
              TMath::Binomial(j, i) *
              (TMath::Power(xhi, j + 1) - TMath::Power(xlo, j + 1)) / (j + 1);
    }
    temp *= static_cast<RooAbsReal &>(coeffs[i]).getVal(); // include coeff
    norm += temp; // add this basis's contribution to total
  }

  norm *= xMax-xMin;
  return norm;
}

// void MuMuBkgShape::computeBatch(cudaStream_t* stream, double* output, size_t
// nEvents, RooFit::Detail::DataMap const& dataMap) const
// {
//   const int nCoef = _coefList.size();
//   std::vector<double> extraArgs(nCoef+2);
//   for (int i=0; i<nCoef; i++)
//     extraArgs[i] = static_cast<RooAbsReal&>(_coefList[i]).getVal();
//   extraArgs[nCoef] = _x.min();
//   extraArgs[nCoef+1] = _x.max();

//   auto dispatch = stream ? RooBatchCompute::dispatchCUDA :
//   RooBatchCompute::dispatchCPU; dispatch->compute(stream,
//   RooBatchCompute::Bernstein, output, nEvents, {dataMap.at(_x)}, extraArgs);
// }