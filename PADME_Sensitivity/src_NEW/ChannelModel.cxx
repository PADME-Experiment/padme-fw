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

ChannelModel::ChannelModel(double Nom_BeamEnergy, RooRealVar &_N2Cluster, RooRealVar &_NPoT, RooRealvar &_SigmaEB, RooRealVar &_EffSignal,
                          RooRealVar &_X17Mass, RooRealVar &_NPoT_true, RooRealVar &_Mu_test, 
                          signal_types sig_type, background_types bkg_type, double Bkg_mean, double Sigma_bkg)
    : fEBeam(Nom_BeamEnergy), N2Cluster(&_N2Cluster), SqrtS(&_SqrtS), NPoT(&_NPoT), SigmaEB(&_SigmaEB), EffSignal(&_EffSignal),
      NPoT_true(&_NPoT_true), Mu_test(&_Mu_test), X17Mass_resolution(), Signal_shape(), Signal_yield(), Bkg_shape(nullptr),
      Bkg_yield_default(new RooRealVar(
          Form("Bkg_yield_default_%2f", fEBeam.Data()),
          Form("Bkg_yield_default_%2f", fEBeam.Data()), Bkg_mean, 1.E1, 1.E10)),
      Bkg_yield_scale(new RooRealVar(
          Form("Bkg_yield_scale_%2f", fEBeam.Data()),
          Form("Bkg_yield_scale_%2f", fEBeam.Data()), 1.0E4, 1.E1, 1.E10)), //da capire che valori metterci
      Bkg_yield_true(new RooProduct(
          Form("BkgYieldTrue_%2f", fEBeam.Data()),
          Form("BkgYieldTrue_%2f", fEBeam.Data()), {*Bkg_yield_default, *Bkg_yield_scale})),
      Bkg_yield_sigma(new RooRealVar(
          Form("BkgYieldSigma_%2f", fEBeam.Data()),
          Form("BkgYieldSigma_%2f", fEBeam.Data()), Sigma_bkg, 1.0E4 + 1.E1, 1.E10)),
      Bkg_yield_constraint(new RooLognormal(
          Form("BkgYieldConstraint_%2f", fEBeam.Data()),
          Form("BkgYieldConstraint_%2f", fEBeam.Data()), *Bkg_yield_scale,  RooRealConstant::value(1.0E4), *Bkg_yield_sigma)),
      Extended_sbModel(nullptr), Extended_bkgModel(nullptr),
      Full_sbModel(nullptr), Full_bkgModel(nullptr)
// ChannelModel è una classe che rappresenta un modello completo che comprende il segnale, il background 
// comprende poi i relativi coefficienti e normalizzazioni.
// Durante l'esecuzione del fit, i parametri di questo modello verranno ottimizzati

{
  cout << "Constructing " << fEBeam.Data() << " points of the scan"<<endl;
  // Retrieve signal parameters: loop over direct and meson and create signal shape
  X17Mass_resolution = new RooRealVar(
      Form("X17MassResolution_%2f", fEBeam.Data()),
      Form("X17MassResolution_%2f", fEBeam.Data()),
      4., 1.0E-12, 100.);
  X17Mass_resolution->setConstant(); //setConstant fa si che questo parametro non venga modificato durante il fitting

  Signal_shape = new RooGaussian(
    Form("Signal_shape_%2f", fEBeam.Data()),
    Form("Signal_shape_%2f", fEBeam.Data()),
    *SqrtS, *SigmaEB, *X17Mass_resolution);

  // signal yields
  // this one is extracted from the input histograms
  Signal_yield = new RooRealVar(
      Form("Signal_yield_%2f", fEBeam.Data()),
      Form("Signal_yield_%2f", fEBeam.Data()), 2.5, 0., 1E6);
  Signal_yield->setConstant();
  Expected_signal_yield = new RooProduct(
      Form("Expected_signal_yield_%2f", fEBeam.Data()),
      Form("Expected_signal_yield_%2f", fEBeam.Data()),, {*Signal_yield, *NPoT_true, *Mu_test});
  }

  // build background shape according to input
  RooRealVar a0("a0", "a0", 1.88569e-02, 1e-3, 1); //coefficienti messi da me in questo modo perchè prima il codice non era compliant: FORSE BISOGNEREBBE TORNARE ALLA VERSIONE ORIGINALE (?)
  RooRealVar a1("a1", "a1", 1.94453e-01, 1e-3, 1);

  // DA MODIFICARE CON CORRETTI MODELLI
  if (bkg_type == kLandau) {
    Bkg_shape = new RooLandau(Form("BkgShape_%2f", fEBeam.Data()),
                              Form("BkgShape_%2f", fEBeam.Data()),
                              *SqrtS, RooRealConstant::value(14.86), RooRealConstant::value(3.482));
  } else if (bkg_type == kBernstein) {
    RooArgSet coeffs(a0, a1);
    Bkg_shape = new RooBernstein(Form("BkgShape_%2f", fEBeam.Data()),
                                 Form("BkgShape__%2f", fEBeam.Data()),
                                 *SqrtS, coeffs);
  } else {
    cout << "Wrong background type " << endl;
    exit(1);
  }

  Bkg_yield_default->setConstant();
  Bkg_yield_sigma->setConstant();

  // expected_bkg = new RooProduct(Form("expected_bkg_%s", fChannelName.Data()),
  // Form("expected_bkg_%s", fChannelName.Data()), {*bkg_yield_true,
  // *pot_true}); build exteded, unconstrained s+b and bonly models
  Extended_sbModel = new RooAddPdf(
      Form("ExtendedSB_%2f", fEBeam.Data()),
      Form("ExtendedSB_%2f", fEBeam.Data()),
      {*Signal_shape, *Signal_shape, *Bkg_shape},
      {*Expected_signal_yield, *Expected_signal_yield,
       *Bkg_yield_true});

  Extended_bkgModel = new RooExtendPdf(
      Form("ExtendedB_%2f", fEBeam.Data()),
      Form("ExtendedB_%2f", fEBeam.Data()), *Bkg_shape, *Bkg_yield_true);

  // add bkg constraint
  Full_sbModel = new RooProdPdf(Form("fullSB_%2f", fEBeam.Data()),
                                Form("fullSB_%2f", fEBeam.Data()),
                                {*Extended_sbModel, *Bkg_yield_constraint});
  Full_bkgModel = new RooProdPdf(Form("fullSB_%2f", fEBeam.Data()),
                                 Form("fullSB_%2f", fEBeam.Data()),
                                 {*Extended_bkgModel, *Bkg_yield_constraint});

  cout << "... done" << endl;
}

ChannelModel::~ChannelModel() {
  cout << "Destroying channel " << fEBeam.Data() << "..."<< endl;
  // cout << "... clearing resolution map" <<endl;
  // X17Mass_resolution.clear();
  cout << "... clearing signal shape map" << endl;
  Signal_shape.clear();
  cout << "... clearing signal yield map" << endl;
  Signal_yield.clear();

  cout << "... deleting pdfs" << endl;
  delete Bkg_shape;
  delete Extended_sbModel;
  delete Extended_bkgModel;
  delete Full_sbModel;
  delete Full_bkgModel;

  cout << "...done!" << endl;
}
