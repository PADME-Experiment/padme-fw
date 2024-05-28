#ifndef CHANNELMODEL 
#define CHANNELMODEL

#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooGaussian.h"
#include "RooLognormal.h"
#include "RooProdPdf.h"
#include "RooProduct.h"
#include "RooRealVar.h"
#include "RooArgList.h"
#include "TString.h"

#include <map>

using namespace std;

enum signal_types{
  kConst = 0,
  kVoigt = 1,
  kCPfull = 2}; //Voigt function (Gaus + Lorentz) - Signal shape full
enum background_types{
  kFlat = 0,
  kPol = 1}; //linear Bkg from MC
// The PDF(\theta_scattering) actually must be a constant.  

class ChannelModel {
public:
  ChannelModel(TString channelName,
               RooRealVar &_sqrtS, RooRealVar &_xmass, RooRealVar &_theta,
               RooRealVar &_gve_test, RooRealVar &_kLumi_true, RooRealVar &_bes_true,
               RooRealVar &_pot_true, RooRealVar &_eff_true, RooRealVar &_bkg_yield_true,
               RooRealVar &_signal_yield_bare, signal_types sig_type, background_types bkg_type,
               double sqrts_mean, double err_sqrts,
               double bkg_mean, double sigma_bkg,
               double pot_mean, double sigma_pot,
               double eff_mean, double sigma_eff);
  ~ChannelModel();

public:
  TString fChannelName;
  RooRealVar *SqrtS; 
  RooRealVar *XMass;   
  RooRealVar *Theta;
  RooRealVar *gve_test;
  
  RooRealVar *kLumi_true;
  RooRealVar *Bes_true;

  RooRealVar *SqrtS_reso;

  RooRealVar *bkg_yield_default; // bkg_yield observed - from input TGraph
  RooRealVar *bkg_yield_true; // the actual bkg_yield value - to be fitted 
  RooRealVar *bkg_yield_sigma; // uncertainty on bkg_yield - from input TGraph (?)
  RooAbsPdf  *bkg_yield_constraint; // the pdf of bkg_yield  

  RooRealVar *pot_default; // pot observed - from input TGraph
  RooRealVar *pot_true; // the actual NPoT value - to be fitted 
  RooRealVar *pot_sigma; // uncertainty on pot - from input TGraph (?)
  RooAbsPdf  *pot_constraint; // the pdf of NPoT 

  RooRealVar *eff_default; // eff observed - from input TGraph
  RooRealVar *eff_true; // the actual Eff value - to be fitted 
  RooRealVar *eff_sigma; // uncertainty on eff - from input TGraph (?)
  RooAbsPdf  *eff_constraint; // the pdf of Eff 

  RooRealVar *signal_yield_bare;

  RooProduct *expected_signal_ev;
  RooProduct *expected_bkg_ev;

  RooAbsPdf    *bkg_shape;
  RooAbsPdf    *signal_shape;
  RooAddPdf    *extended_sbModel;
  RooExtendPdf *extended_bkgModel;
  RooProdPdf   *full_sbModel;
  RooProdPdf   *full_bkgModel;
};

#endif