#ifndef MUMUSIGNALSHAPE
#define MUMUSIGNALSHAPE

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
 
class MuMuSignalShape : public RooAbsPdf {
public:
  MuMuSignalShape() {} ; 
  MuMuSignalShape(const char *name, const char *title,
	      RooAbsReal& _x,
	      RooAbsReal& _mean,
	      RooAbsReal& _sigma,
	      RooAbsReal& _xMin,
	      RooAbsReal& _xMax);
  MuMuSignalShape(const MuMuSignalShape& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new MuMuSignalShape(*this,newname); }
  inline virtual ~MuMuSignalShape() { }

  Int_t getAnalyticalIntegral(RooArgSet &allVars, RooArgSet& analVars, const char* /*rangeName*/) const;
  double analyticalIntegral(Int_t code, const char *rangeName) const;

  static double getGaussIntegral(double a);
protected:

  RooRealProxy x ;
  RooRealProxy mean ;
  RooRealProxy sigma ;
  RooRealProxy xMin ;
  RooRealProxy xMax ;
  
  Double_t evaluate() const ;

// private:

//   ClassDef(MuMuSignalShape,1) // Your description goes here...
};
 
#endif
