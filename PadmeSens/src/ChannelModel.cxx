#include "ChannelModel.h"
#include "GlobalParams.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooBernstein.h"
#include "RooExtendPdf.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooLognormal.h"
#include "RooProdPdf.h"
#include "RooProduct.h"
#include "RooRealConstant.h"
#include "RooRealVar.h"

#include <iostream>

using namespace std;

ChannelModel::ChannelModel(TString channelName, RooRealVar &_invMass,
                           RooRealVar &_dpMass, RooRealVar &_pot_true,
                           RooRealVar &_mu_test, signal_types sig_type,
                           background_types bkg_type, double bkg_mean,
                           double sigma_bkg)
    : fChannelName(channelName), invariantMass(&_invMass), dpMass(&_dpMass),
      pot_true(&_pot_true), mu_test(&_mu_test), dpMass_resolution(),
      signal_shape(), signal_yield(), bkg_shape(nullptr),
      bkg_yield_default(new RooRealVar(Form("bkg_yield_default_%s", fChannelName.Data()),
                        Form("bkg_yield_default_%s", fChannelName.Data()),
                        bkg_mean, 1E-10, 10.)),
      bkg_yield_scale(new RooRealVar(
          Form("bkg_yield_scale_%s", fChannelName.Data()),
          Form("bkg_yield_scale_%s", fChannelName.Data()), 1.0, 1E-10, 10.)),
      bkg_yield_true(new RooProduct(
          Form("bkgYieldTrue_%s", fChannelName.Data()),
          Form("bkgYieldTrue_%s", fChannelName.Data()), {*bkg_yield_default, *bkg_yield_scale})),
      bkg_yield_sigma(
          new RooRealVar(Form("bkgYieldSigma_%s", fChannelName.Data()),
                         Form("bkgYieldSigma_%s", fChannelName.Data()),
                         sigma_bkg, 1.0 + 1E-10, 10.)),
      bkg_yield_constraint(new RooLognormal(
          Form("bkgYieldConstraint_%s", fChannelName.Data()),
          Form("bkgYieldConstraint_%s", fChannelName.Data()), *bkg_yield_scale,
          RooRealConstant::value(1.0), *bkg_yield_sigma)),
      extended_sbModel(nullptr), extended_bkgModel(nullptr),
      full_sbModel(nullptr), full_bkgModel(nullptr)
// ChannelModel è una classe che rappresenta un modello completo che comprende il segnale, il background 
// comprende poi i relativi coefficienti e normalizzazioni.
// Durante l'esecuzione del fit, i parametri di questo modello verranno ottimizzati

{
  cout << "Constructing " << fChannelName.Data() << " channel..."<<endl;
  // Retrieve signal parameters: loop over direct and meson and create signal shape
  vector<TString> signal_sources({"direct", "meson"});
  for (TString ss : signal_sources) {
    dpMass_resolution[ss] = new RooRealVar(
        Form("dpMassResolution_%s_%s", fChannelName.Data(), ss.Data()),
        Form("dpMassResolution_%s_%s", fChannelName.Data(), ss.Data()), 4.,
        1.0E-12, 100.);
    dpMass_resolution[ss]->setConstant(); //setConstant fa si che questo parametro non venga modificato durante il fitting

    signal_shape[ss] = new RooGaussian(
        Form("signalShape_%s_%s", fChannelName.Data(), ss.Data()),
        Form("signalShape_%s_%s", fChannelName.Data(), ss.Data()),
        *invariantMass, *dpMass, *dpMass_resolution[ss]);

    // signal yields
    // this one is extracted from the input histograms
    signal_yield[ss] = new RooRealVar(
        Form("yield_%s_%s", fChannelName.Data(), ss.Data()),
        Form("yield_%s_%s", fChannelName.Data(), ss.Data()), 2.5, 0., 1E6);
    signal_yield[ss]->setConstant();
    expected_signal_yield[ss] = new RooProduct(
        Form("expectedYield_%s_%s", fChannelName.Data(), ss.Data()),
        Form("expectedYield_%s_%s", fChannelName.Data(), ss.Data()),
        {*signal_yield[ss], *pot_true, *mu_test});
  }

  // build background shape according to input
  RooRealVar a0("a0", "a0", 1.88569e-02, 1e-3, 1); //coefficienti messi da me in questo modo perchè prima il codice non era compliant: FORSE BISOGNEREBBE TORNARE ALLA VERSIONE ORIGINALE (?)
  RooRealVar a1("a1", "a1", 1.94453e-01, 1e-3, 1);
  RooRealVar a2("a2", "a2", 9.18272e-01, 1e-3, 1);
  RooRealVar a3("a3", "a3", 2.76931e-01, 1e-3, 1);
  RooRealVar a4("a4", "a4", 1.19633e-01, 1e-3, 1);
  RooRealVar a5("a5", "a5", 6.84498e-02, 1e-3, 1);
  if (bkg_type == kLandau) {
    bkg_shape = new RooLandau(Form("bkgShape_%s", fChannelName.Data()),
                              Form("bkgShape_%s", fChannelName.Data()),
                              *invariantMass, RooRealConstant::value(14.86),
                              RooRealConstant::value(3.482));
  } else if (bkg_type == kBernstein) {
    RooArgSet coeffs(a0, a1, a2, a3, a4, a5);
    bkg_shape = new RooBernstein(Form("bkgShape_%s", fChannelName.Data()),
                                 Form("bkgShape_%s", fChannelName.Data()),
                                 *invariantMass, coeffs);
  } else {
    cout << "Wrong background type " << endl;
    exit(1);
  }

  bkg_yield_default->setConstant();
  bkg_yield_sigma->setConstant();

  // expected_bkg = new RooProduct(Form("expected_bkg_%s", fChannelName.Data()),
  // Form("expected_bkg_%s", fChannelName.Data()), {*bkg_yield_true,
  // *pot_true}); build exteded, unconstrained s+b and bonly models
  extended_sbModel = new RooAddPdf(
      Form("extendedSB_%s", fChannelName.Data()),
      Form("extendedSB_%s", fChannelName.Data()),
      {*signal_shape["direct"], *signal_shape["meson"], *bkg_shape},
      {*expected_signal_yield["direct"], *expected_signal_yield["meson"],
       *bkg_yield_true});

  extended_bkgModel = new RooExtendPdf(
      Form("extendedB_%s", fChannelName.Data()),
      Form("extendedB_%s", fChannelName.Data()), *bkg_shape, *bkg_yield_true);

  // add bkg constraint
  full_sbModel = new RooProdPdf(Form("fullSB_%s", fChannelName.Data()),
                                Form("fullSB_%s", fChannelName.Data()),
                                {*extended_sbModel, *bkg_yield_constraint});
  full_bkgModel = new RooProdPdf(Form("fullBkg_%s", fChannelName.Data()),
                                 Form("fullBkg_%s", fChannelName.Data()),
                                 {*extended_bkgModel, *bkg_yield_constraint});

  cout << "... done" << endl;
}

ChannelModel::~ChannelModel() {
  cout << "Destroying channel " << fChannelName.Data() << "..."
            << endl;
  // cout << "... clearing resolution map" <<endl;
  // dpMass_resolution.clear();
  cout << "... clearing signal shape map" << endl;
  signal_shape.clear();
  cout << "... clearing signal yield map" << endl;
  signal_yield.clear();

  cout << "... deleting pdfs" << endl;
  delete bkg_shape;
  delete extended_sbModel;
  delete extended_bkgModel;
  delete full_sbModel;
  delete full_bkgModel;

  cout << "...done!" << endl;
}
