/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef MUMUSIGNALSHAPE
#define MUMUSIGNALSHAPE

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

using namespace std;

 
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
  virtual TObject* clone(const char* newname) const { return new MuMuSignalShape(*this,newname); } //crea una copia uguale della pdf serve ad avere due pdf identiche e indipendenti a qualche putno
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

private:

  ClassDef(MuMuSignalShape,1) // Your description goes here...
};
// come definisce variabili e classi è uguale al MuMuBkgShape, solo che qui ha anche getGaussianIntegral perché approssima il segnale con una gaussiana
//il resto è getAnalyticalIntegral per vedere se si può calcolare l'integrale, Analitycalintegral lo calcola e evaluate restituisce il valore della pdf in un punto
#endif
