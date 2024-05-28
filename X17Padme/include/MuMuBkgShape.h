#ifndef MUMUBKGSHAPE
#define MUMUBKGSHAPE

#include "RooAbsCategory.h"
#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooCategoryProxy.h"
#include "RooListProxy.h"
#include "RooRealProxy.h"

class MuMuBkgShape : public RooAbsPdf {
public:
  MuMuBkgShape(){};
  MuMuBkgShape(const char *name, const char *title, RooAbsReal &_x,
               const RooArgList &_coeffs, RooAbsReal &_xMin, RooAbsReal &_xMax);
  MuMuBkgShape(const MuMuBkgShape &other, const char *name = 0);
  virtual TObject *clone(const char *newname) const {
    return new MuMuBkgShape(*this, newname);
  }
  inline virtual ~MuMuBkgShape() {}

  Int_t getAnalyticalIntegral(RooArgSet &allVars, RooArgSet &analVars,
                              const char * /*rangeName*/) const;
  double analyticalIntegral(Int_t code, const char *rangeName) const;

protected:
  RooRealProxy x;
  RooListProxy coeffs;
  RooRealProxy xMin;
  RooRealProxy xMax;

  Double_t evaluate() const;

// private:
//   ClassDef(MuMuBkgShape, 1) // Your description goes here...
};

#endif
