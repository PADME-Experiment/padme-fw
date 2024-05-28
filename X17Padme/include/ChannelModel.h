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
#include "TString.h"

#include <map>

enum signal_types {
  kGaussian = 0,
};
enum background_types { kLandau = 0, kBernstein };

class ChannelModel {
public:
  ChannelModel(TString channelName, RooRealVar &_invMass, RooRealVar &_dpMass,
               RooRealVar &_pot_true, RooRealVar &_mu_test, signal_types sig_type,
               background_types bkg_type, double bkg_mean, double sigma_bkg);
  ~ChannelModel();

public:
  TString fChannelName;
  std::shared_ptr<RooRealVar> invariantMass;
  std::shared_ptr<RooRealVar> dpMass;
  std::shared_ptr<RooRealVar> pot_true;
  std::shared_ptr<RooRealVar> mu_test;

  std::map<TString, RooRealVar*> dpMass_resolution;
  std::map<TString, RooAbsPdf*> signal_shape;
  std::map<TString, RooRealVar*> signal_yeild; // from histo
  std::map<TString, RooProduct*> expected_signal_yield;

  RooAbsPdf *bkg_shape;

  RooRealVar *bkg_yield_default;
  RooRealVar *bkg_yield_scale;
  RooProduct *bkg_yield_true;
  RooRealVar *bkg_yield_sigma;
  RooAbsPdf *bkg_yield_constraint;

  RooAddPdf *extended_sbModel;
  RooExtendPdf *extended_bkgModel;
  RooProdPdf *full_sbModel;
  RooProdPdf *full_bkgModel;

  // RooProduct *expected_bkg;

};

#endif