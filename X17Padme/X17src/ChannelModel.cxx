#include "ChannelModel.h"
#include "GlobalParams.h"
#include "EEShapes.h"

#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooBernstein.h"
#include "RooChebychev.h"
#include "RooExtendPdf.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooLognormal.h"
#include "RooProdPdf.h"
#include "RooProduct.h"
#include "RooRealConstant.h"
#include "RooRealVar.h"
#include "RooVoigtian.h"

#include <iostream>

using namespace std;

ChannelModel::ChannelModel(TString channelName,
                           RooRealVar &_sqrtS, RooRealVar &_xmass, RooRealVar &_theta,
                           RooRealVar &_gve_test, RooRealVar &_kLumi_true, RooRealVar &_bes_true,
                           RooRealVar &_pot_true, RooRealVar &_eff_true, RooRealVar &_bkg_yield_true,
                           signal_types sig_type, background_types bkg_type,
                           double sqrts_mean, double err_sqrts,
                           double bkg_mean, double sigma_bkg,
                           double pot_mean, double sigma_pot,
                           double eff_mean, double sigma_eff)
    : fChannelName(channelName),
      SqrtS(&_sqrtS), XMass(&_xmass), Theta(&_theta), 
      gve_test(&_gve_test), kLumi_true(&_kLumi_true), Bes_true(&_bes_true),
      bkg_yield_default(new RooRealVar(Form("bkg_yield_default_%s", fChannelName.Data()),
                                       Form("bkg_yield_default_%s", fChannelName.Data()),
                                       bkg_mean, 1.e-6, 1.e-4)), //averages acquired by the input TGraphs
      bkg_yield_true(new RooRealVar(Form("bkg_yield_true_%s", fChannelName.Data()),
                                    Form("bkg_yield_true_%s", fChannelName.Data()),
                                    bkg_mean, 1.e-6, 1.e-4)),
      bkg_yield_sigma(new RooRealVar(Form("bkg_yield_sigma_%s", fChannelName.Data()),
                                     Form("bkg_yield_sigma_%s", fChannelName.Data()),
                                     sigma_bkg, 5.e-9, 5.e-6)),      
      bkg_yield_constraint(new RooLognormal(Form("bkg_yield_constraint_%s", fChannelName.Data()),
                                            Form("bkg_yield_constraint_%s", fChannelName.Data()),
                                            *bkg_yield_true, *bkg_yield_default, *bkg_yield_sigma)),
      pot_default(new RooRealVar(Form("pot_default_%s", fChannelName.Data()),
                                 Form("pot_default_%s", fChannelName.Data()),
                                 pot_mean, 5.e7, 1.e11)), //averages to be acquired by the input TGraphs
      pot_true(new RooRealVar(Form("pot_default_%s", fChannelName.Data()),
                              Form("pot_default_%s", fChannelName.Data()),
                              pot_mean, 5.e7, 1.e11)),
      pot_sigma(new RooRealVar(Form("pot_sigma_%s", fChannelName.Data()),
                               Form("pot_sigma_%s", fChannelName.Data()),
                               sigma_pot, 1.e7, 1.e9)),
      pot_constraint(new RooLognormal(Form("pot_constraint_%s", fChannelName.Data()),
                                      Form("pot_constraint_%s", fChannelName.Data()),
                                      *pot_true, *pot_default, *pot_sigma)),
      eff_default(new RooRealVar(Form("eff_default_%s", fChannelName.Data()),
                                 Form("eff_default_%s", fChannelName.Data()),
                                 eff_mean, 0.05, 0.30)), //averages to be acquired by the input TGraphs
      eff_true(new RooRealVar(Form("eff_default_%s", fChannelName.Data()),
                                 Form("eff_default_%s", fChannelName.Data()),
                                 eff_mean, 0.05, 0.30)),
      eff_sigma(new RooRealVar(Form("eff_sigma_%s", fChannelName.Data()),
                                  Form("eff_sigma_%s", fChannelName.Data()),
                                  sigma_eff, 0.001, 0.1)),
      eff_constraint(new RooLognormal(Form("eff_constraint_%s", fChannelName.Data()),
                                      Form("eff_constraint_%s", fChannelName.Data()),
                                      *eff_true, *eff_default, *eff_sigma)),
      signal_yield_bare(new RooRealVar(Form("signal_yield_bare_%s", fChannelName.Data()),
                                       Form("signal_yield_bare_%s", fChannelName.Data()),
                                       1. , 0., 1E6)), 
      expected_signal_ev(nullptr), expected_bkg_ev(nullptr),
      extended_sbModel(nullptr), extended_bkgModel(nullptr),
      full_sbModel(nullptr), full_bkgModel(nullptr)
{

  cout << "Constructing " << fChannelName.Data() << " period..." << endl;
  // Retrieve signal parameters

//   SqrtS_reso = new RooRealVar(Form("SqrtS_reso_%s", fChannelName.Data()),
//                               Form("SqrtS_reso_%s", fChannelName.Data()),
//                               1.0E-12, 100.);
  // signal yields
  // this one is extracted from TGraph
  if(sig_type == kConst){
      signal_shape = new RooChebychev(Form("signalShape_%s", fChannelName.Data()),
                                      Form("signalShape_%s", fChannelName.Data()),
                                      *Theta, RooArgList(1.0));
  // } elseif(sig_type == kVoigt){
  //     cout << "signal type under developing" << endl;
  //     exit(1);
  //     // signal_shape = new RooVoigtian(
  //     //     Form("signalShape_%s", fChannelName.Data()),
  //     //     Form("signalShape_%s", fChannelName.Data()),
  //     //     *SqrtS, *XMass, *dpMass_resolution[ss]);
  //     // MM: Voigt function doesn't must be implemented here ->statisticsModel (?)
  // } elseif(sig_type == kCPFull){
  //     cout << "signal type under developing" << endl;
  //     exit(1);
  //     // MM: full signal shape to be implemented!
  } else {
      cout << "Signal type not already developed" << endl;
      exit(1);
  }

  // MM sigma and Default are fixed because we measure them every time with the same technique
  pot_default->setConstant();
  pot_sigma->setConstant();
  eff_default->setConstant();
  eff_sigma->setConstant();

  expected_signal_ev = new RooProduct(Form("expected_signal_ev%s", fChannelName.Data()),
                                      Form("expected_signal_ev%s", fChannelName.Data()),
                                      {*signal_yield_bare, *eff_true, *pot_true, *gve_test, *gve_test, *kLumi_true});  

  // build background shape according to input
  if(bkg_type == kFlat){
      bkg_shape = new RooChebychev(Form("bkgShape_%s", fChannelName.Data()),
                                   Form("bkgShape_%s", fChannelName.Data()),
                                   *Theta, RooArgList(1.0));
  // } elseif (bkg_type == kPol){
  //     cout << "background type under developing" << endl;
  //     exit(1);
  //     // bkg_shape = new RooChebychev(Form("bkgShape_%s", fChannelName.Data()),
  //     //                              Form("bkgShape_%s", fChannelName.Data()),
  //     //                              *theta, RooArgList(*c0, *c1));
  //     // MM: bkg shape as a function of SqrtS must be implemented elsewhere -> StasticsModel (?)
  } else{
      cout << "Background type not already developed" << endl;
      exit(1);
  }
  bkg_yield_default->setConstant();
  bkg_yield_sigma->setConstant();

  expected_bkg_ev = new RooProduct(Form("expected_bkg_ev%s", fChannelName.Data()),
                                   Form("expected_bkg_ev%s", fChannelName.Data()),
                                   RooArgList(*bkg_yield_true, *pot_true));

  extended_sbModel = new RooAddPdf(Form("extendedSB_%s", fChannelName.Data()),
                                   Form("extendedSB_%s", fChannelName.Data()),
                                   RooArgList(*signal_shape, *bkg_shape),
                                   RooArgList(*expected_signal_ev, *expected_bkg_ev));

  extended_bkgModel = new RooExtendPdf(Form("extendedB_%s", fChannelName.Data()),
                                       Form("extendedB_%s", fChannelName.Data()),
                                       *bkg_shape, *expected_bkg_ev);

  full_sbModel = new RooProdPdf(Form("fullSB_%s", fChannelName.Data()),
                                Form("fullSB_%s", fChannelName.Data()),
                                RooArgList(*extended_sbModel, *bkg_yield_constraint, *pot_constraint, *eff_constraint));
  full_bkgModel = new RooProdPdf(Form("fullBkg_%s", fChannelName.Data()),
                                 Form("fullBkg_%s", fChannelName.Data()),
                                 RooArgList(*extended_bkgModel, *bkg_yield_constraint, *pot_constraint));

  cout << "... done" << endl;
}

ChannelModel::~ChannelModel() {
  cout << "Destroying Period " << fChannelName.Data() << "..." << endl;

  //let's try to understand if the memory leakage breaks the code:
  cout << "let's try to understand if the memory leakage breaks the code-> deleting a lot of 'new' pointers in ChannelModels" << endl;

  delete bkg_yield_default;
  delete bkg_yield_true;
  delete bkg_yield_sigma;
  delete bkg_yield_constraint;

  delete pot_default;
  delete pot_true;
  delete pot_sigma;
  delete pot_constraint;

  delete eff_default;
  delete eff_true;
  delete eff_sigma;
  delete eff_constraint;

  delete signal_yield_bare;

  cout << "... deleting pdfs" << endl;
  delete signal_shape;
  delete bkg_shape;
  delete extended_sbModel;
  delete extended_bkgModel;
  delete full_sbModel;
  delete full_bkgModel;

  cout << "...done!" << endl;
}