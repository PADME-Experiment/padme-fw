#include "StatisticsModel.h"
#include "GlobalParams.h"
#include "EEShapes.h"

#include "RooAbsPdf.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooArgList.h"
#include "RooFitResult.h"
#include "RooLognormal.h"
#include "RooNLLVar.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "TString.h"

using namespace std;

StatisticsModel::StatisticsModel(global_config &cfg)
    : SqrtS(new RooRealVar("SqrtS", "SqrtS", 16, 18.)),
      dsqrts(new Double_t(16.)),
      Theta(new RooRealVar("Theta", "Theta", 0., TMath::Pi())),
      XMass(new RooRealVar("XMass", "XMass", 16, 18.)),
      gve_test(new RooRealVar("gve_test", "gve_test", 1.e-4, 1.e-5, 1.e-2)),
      channelModels(), channels(),
      ScanPoint_categories(new RooCategory("ScanPoint_category", "ScanPoint_category")),
      kLumi_default(new RooRealVar("kLumi_default", "kLumi_default", 0.00176, 0.00167, 0.00185)), //kLumin = istantaneus luminosity global nuisance parameter 
      kLumi_true(new RooRealVar("kLumi_true", "kLumi_true", 0.00176, 0.00167, 0.00185)),
      kLumi_sigma(new RooRealVar("kLumi_sigma", "kLumi_sigma", 2.e-5, 1.e-6, 1.e-4)),
      kLumi_constraint(new RooLognormal("kLumi_constraint", "kLumi_constraint",
                                        *kLumi_true, *kLumi_default, *kLumi_sigma)),
      Bes_default(new RooRealVar("Bes_default", "Bes_default", 0.0025, 0.001, 0.01)), //Bes = Beam Energy spread  global nuisance parameter - to be written as a percentage of the SqrtS of that scan point
      Bes_true(new RooRealVar("Bes_true", "Bes_true", 0.0025, 0.001, 0.01)),
      Bes_sigma(new RooRealVar("Bes_sigma", "Bes_sigma", 0.0005, 0.0001, 0.001)),
      Bes_constraint(new RooLognormal("Bes_constraint", "Bes_constraint",
                                      *Bes_true, *Bes_default, *Bes_sigma)),
      NormCore_default(new RooRealVar("NormCore_default", "NormCore_default", 4.35e3, 4.22e3, 4.48e3)), //NormCore = normalization of the line shape core gaussian nuisance parameter
      NormCore_true(new RooRealVar("NormCore_true", "NormCore_true", 4.35e3, 4.22e3, 4.48e3)), // this is a cross section [b] 
      NormCore_sigma(new RooRealVar("NormCore_sigma", "NormCore_sigma", 0.13e3, 0.05e3, 0.25e3)), // production evaluated with gve = 1 -> added after.
      NormCore_constraint(new RooLognormal("NormCore_constraint", "NormCore_constraint",
                                           *NormCore_true, *NormCore_default, *NormCore_sigma)),
      sigmaCore_default(new RooRealVar("sigmaCore_default", "sigmaCore_default", 0.207, 0.201, 0.213)), //sigmaCore = sigma of the line shape core gaussian nuisance parameter
      sigmaCore_true(new RooRealVar("sigmaCore_true", "sigmaCore_true", 0.207, 0.201, 0.213)),
      sigmaCore_sigma(new RooRealVar("sigmaCore_sigma", "sigmaCore_sigma", 0.006, 0.001, 0.012)),
      sigmaCore_constraint(new RooLognormal("sigmaCore_constraint", "sigmaCore_constraint",
                                            *sigmaCore_true, *sigmaCore_default, *sigmaCore_sigma)),
      NormTail_default(new RooRealVar("NormTail_default", "NormTail_default", 2.52e3, 2.45e3, 2.59e3)), //NormTail = normalization of the line shape tail gaussian nuisance parameter
      NormTail_true(new RooRealVar("NormTail_true", "NormTail_true", 2.52e3, 2.45e3, 2.59e3)),
      NormTail_sigma(new RooRealVar("NormTail_sigma", "NormTail_sigma", 0.03e3, 0.01e3, 0.1e3)),
      NormTail_constraint(new RooLognormal("NormTail_constraint", "NormTail_constraint",
                                           *NormTail_true, *NormTail_default, *NormTail_sigma)),
      sigmaTail_default(new RooRealVar("sigmaTail_default", "sigmaTail_default", 0.630, 0.611, 0.649)), //sigmaTail = sigma of the line shape tail gaussian nuisance parameter
      sigmaTail_true(new RooRealVar("sigmaTail_true", "sigmaTail_true", 0.630, 0.611, 0.649)),
      sigmaTail_sigma(new RooRealVar("sigmaTail_sigma", "sigmaTail_sigma", 0.019, 0.001, 0.04)),
      sigmaTail_constraint(new RooLognormal("sigmaTail_constraint", "sigmaTail_constraint",
                                            *sigmaTail_true, *sigmaTail_default, *sigmaTail_sigma)),
      SignalShape(new EEShapes("SignalShape", "SignalShape", *SqrtS, *dsqrts,
                               *NormCore_true, *XMass, *sigmaCore_true,
                               *NormTail_true, *sigmaTail_true, *Bes_true,
                               SqrtSMin, SqrtSMax)),
      pot_scale_default(new RooRealVar("pot_scale_default", "pot_scale_default", 1., 0.9, 1.1)), //to scale the NEvents at the level of SMbkg 
      pot_scale_true(new RooRealVar("pot_scale_true", "pot_scale_true", 1., 0.9, 1.1)), //to scale the NEvents at the level of SMbkg 
      pot_scale_sigma(new RooRealVar("pot_scale_sigma", "pot_scale_sigma", 0.05, 0.001, 0.1)), //to scale the NEvents at the level of SMbkg 
      pot_scale_constraint(new RooLognormal("pot_scale_constraint", "pot_scale_constraint",
                                            *pot_scale_true, *pot_scale_default, *pot_scale_sigma)), //to scale the NEvents at the level of SMbkg 
      eff_scale_default(new RooRealVar("eff_scale_default", "eff_scale_default", 1., 0.95, 1.05)), //to scale the NEvents at the level of SMbkg 
      eff_scale_true(new RooRealVar("eff_scale_true", "eff_scale_true", 1., 0.95, 1.05)), //to scale the NEvents at the level of SMbkg 
      eff_scale_sigma(new RooRealVar("eff_scale_sigma", "eff_scale_sigma", 0.01, 0.001, 0.02)), //to scale the NEvents at the level of SMbkg 
      eff_scale_constraint(new RooLognormal("eff_scale_constraint", "eff_scale_constraint",
                                            *eff_scale_true, *eff_scale_default, *eff_scale_sigma)), //to scale the NEvents at the level of SMbkg 
      sbModel_simultaneous(new RooSimultaneous("sbModel_simultaneous", "sbModel_simultaneous", *ScanPoint_categories)),
      bkgModel_simultaneous(new RooSimultaneous("bkgModel_simultaneous", "bkgModel_simultaneous", *ScanPoint_categories)),
      sbModel_full(nullptr), bkgModel_full(nullptr),
      sbModel_simultaneous_noConstraints(new RooSimultaneous("sbModel_simultaneous_noConstraints", "sbModel_simultaneous_noConstraints", *ScanPoint_categories)),
      bkgModel_simultaneous_noConstraints(new RooSimultaneous("bkgModel_simultaneous_noConstraints", "bkgModel_simultaneous_noConstraints", *ScanPoint_categories)),
      Global_observables(new RooArgSet("Global_observables")),
      fitGlobal_parameters(new RooArgSet("fitGlobal_parameters")),
      constrainedGlobal_parameters(new RooArgSet("constrainedGlobal_parameters")),
      constraintsGlobal_map() {
        XMass->setConstant();
        gve_test->setConstant();

        kLumi_default->setConstant();
        kLumi_sigma->setConstant();
        Global_observables->add(*kLumi_default);
        fitGlobal_parameters->add(*kLumi_true);
        constrainedGlobal_parameters->add(*kLumi_true);

        Bes_default->setConstant();
        Bes_sigma->setConstant();
        Global_observables->add(*Bes_default);
        fitGlobal_parameters->add(*Bes_true);
        constrainedGlobal_parameters->add(*Bes_true);

        NormCore_default->setConstant();
        NormCore_sigma->setConstant();
        Global_observables->add(*NormCore_default);
        fitGlobal_parameters->add(*NormCore_true);
        constrainedGlobal_parameters->add(*NormCore_true);
        sigmaCore_default->setConstant();
        sigmaCore_sigma->setConstant();
        Global_observables->add(*sigmaCore_default);
        fitGlobal_parameters->add(*sigmaCore_true);
        constrainedGlobal_parameters->add(*sigmaCore_true);

        NormTail_default->setConstant();
        NormTail_sigma->setConstant();
        Global_observables->add(*NormTail_default);
        fitGlobal_parameters->add(*NormTail_true);
        constrainedGlobal_parameters->add(*NormTail_true);
        sigmaTail_default->setConstant();
        sigmaTail_sigma->setConstant();
        Global_observables->add(*sigmaTail_default);
        fitGlobal_parameters->add(*sigmaTail_true);
        constrainedGlobal_parameters->add(*sigmaTail_true);
        
        pot_scale_default->setConstant();
        pot_scale_sigma->setConstant();
        Global_observables->add(*pot_scale_default);
        fitGlobal_parameters->add(*pot_scale_true);
        constrainedGlobal_parameters->add(*pot_scale_true);

        eff_scale_default->setConstant();
        eff_scale_sigma->setConstant();
        Global_observables->add(*eff_scale_default);
        fitGlobal_parameters->add(*eff_scale_true);
        constrainedGlobal_parameters->add(*eff_scale_true);

        constraintsGlobal_map[kLumi_true] = kLumi_constraint;
        constraintsGlobal_map[Bes_true] = Bes_constraint;

        constraintsGlobal_map[sigmaCore_true] = sigmaCore_constraint;
        constraintsGlobal_map[sigmaTail_true] = sigmaTail_constraint;
        constraintsGlobal_map[NormCore_true] = NormCore_constraint;
        constraintsGlobal_map[NormTail_true] = NormTail_constraint;

        constraintsGlobal_map[pot_scale_true] = pot_scale_constraint;
        constraintsGlobal_map[eff_scale_true] = eff_scale_constraint;
      }

StatisticsModel::~StatisticsModel() {
  cout << "Destroing statistics model ..." << endl;
  // for (size_t ich = 0; ich < channelModels.size(); ich++){
  //   delete channelModels[ich];
  // }
  channelModels.clear();
  channels.clear();

  //let's try to understand if the memory leakage breaks the code:
  cout << "let's try to understand if the memory leakage breaks the code-> deleting a lot of 'new' pointers in StatisticsModel" << endl;

  delete kLumi_default;
  delete kLumi_true;
  delete kLumi_sigma;
  delete kLumi_constraint;

  delete Bes_default;
  delete Bes_true;
  delete Bes_sigma;
  delete Bes_constraint;

  delete NormCore_default;
  delete NormCore_true;
  delete NormCore_sigma;
  delete NormCore_constraint;

  delete sigmaCore_default;
  delete sigmaCore_true;
  delete sigmaCore_sigma;
  delete sigmaCore_constraint;

  delete NormTail_default;
  delete NormTail_true;
  delete NormTail_sigma;
  delete NormTail_constraint;
  
  delete sigmaTail_default;
  delete sigmaTail_true;
  delete sigmaTail_sigma;
  delete sigmaTail_constraint;

  delete pot_scale_default;
  delete pot_scale_true;
  delete pot_scale_sigma;
  delete pot_scale_constraint;

  delete eff_scale_default;
  delete eff_scale_true;
  delete eff_scale_sigma;
  delete eff_scale_constraint;

  delete sbModel_simultaneous;
  delete bkgModel_simultaneous;
  delete sbModel_simultaneous_noConstraints;
  delete bkgModel_simultaneous_noConstraints;


  cout << "...done!" << endl;
}

void StatisticsModel::AddChannel(TString fChannelModel, signal_types st, background_types bt,
                                 double sqrts_mean, double err_sqrts, 
                                 double bkg_mean, double sigma_bkg,
                                 double pot_mean, double sigma_pot,
                                 double eff_mean, double sigma_eff) {
  channelModels.push_back(new ChannelModel(fChannelModel,
                                           *SqrtS, *XMass, *Theta, *gve_test, *kLumi_true, *Bes_true,
                                           *pot_true, *bkg_yield_true, *eff_true, st, bt,
                                           sqrts_mean, err_sqrts,
                                           bkg_mean, sigma_bkg,
                                           pot_mean, sigma_pot,
                                           eff_mean, sigma_eff));
  channels.push_back(fChannelModel);
  ScanPoint_categories->defineType(fChannelModel.Data());
  channelModels.back()->signal_yield_bare->setVal(SignalShape->analyticalIntegral(1, sqrts_mean - err_sqrts, sqrts_mean + err_sqrts));
  }

void StatisticsModel::BuildModel() {
  for (size_t ich = 0; ich < channels.size(); ich++) {
    sbModel_simultaneous->addPdf(*channelModels[ich]->full_sbModel, channels[ich]);
    bkgModel_simultaneous->addPdf(*channelModels[ich]->full_bkgModel, channels[ich]);

    sbModel_simultaneous_noConstraints->addPdf(*channelModels[ich]->extended_sbModel, channels[ich]);
    bkgModel_simultaneous_noConstraints->addPdf(*channelModels[ich]->extended_bkgModel, channels[ich]);
  }

  sbModel_full = new RooProdPdf("sbModel_full", "sbModel_full", {*sbModel_simultaneous, *NormCore_constraint, *sigmaCore_constraint, *NormTail_constraint, *sigmaTail_constraint, *pot_scale_constraint, *eff_scale_constraint});
  bkgModel_full = new RooProdPdf("bkgModel_full", "bkgModel_full", {*bkgModel_simultaneous, *pot_scale_constraint});
}

void StatisticsModel::Update(global_config &cfg, double mass, double log_coupling) {
  XMass->setVal(mass);
  gve_test->setVal(pow(10,log_coupling));
  cout << "gve = " << gve_test->getVal() << "mass = " << XMass->getVal() << endl;
  // for (size_t ich = 0; ich < channels.size(); ich++) {
    // TString channelName = channels[ich];
    // double new_val = LineShape->analyticalIntegral(1, sqrts_mean - err_sqrts, sqrts_mean + err_sqrts);
    // channelModels[ich]->signal_yield_bare->setVal(new_val);
    // channelModels[ich]->signal_yield_bare->setVal(new_val);
  // }
}

double StatisticsModel::GetTotalYield() {
  double total_yield = 0.0;
  for (size_t ich = 0; ich < channels.size(); ich++) {
    total_yield += channelModels[ich]->expected_signal_ev->getVal();
  }
  return total_yield;
}

Bool_t StatisticsModel::IsLowYield() {
  double total_yield = GetTotalYield();
  // for (size_t ich = 0; ich < channels.size(); ich++) {
  if (total_yield < 10.0) return kTRUE;
  // }
  return kFALSE;
}

Bool_t StatisticsModel::IsHighYield() {
  double total_yield = GetTotalYield();
  //for (size_t ich = 0; ich < channels.size(); ich++) {
    if (total_yield > 15000.0) return kTRUE;
  // }
  return kFALSE;
}

double StatisticsModel::ComputeQstatistic(global_config &cfg, RooDataSet &ds, RooArgSet *paramsInit) {
  double q = 0.;
  if (cfg.tossingType == kFixToFit) {
    fitGlobal_parameters->assignFast(*paramsInit);
    // in all cases we need the s+b fit
    gve_test->setVal(1.0e-4);
    gve_test->setConstant(true);
    unique_ptr<RooFitResult> fr_sb_toy{sbModel_full->fitTo(ds,
                                       /*RooFit::GlobalObservables(*Global_observables),*/
                                       RooFit::Constrain(*constrainedGlobal_parameters),
                                       RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
                                       RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
                                       RooFit::Verbose(false), RooFit::Save())
    };
    // fr_sb_toy->Print();
    double nll_test = fr_sb_toy->minNll();

    fitGlobal_parameters->assignFast(*paramsInit);

    if (cfg.statType == kTevatron) {
      // do b-only fit
      gve_test->setVal(0.0);
      gve_test->setConstant(true);
    } else if (cfg.statType == kLHC) {
      // do Global fit for best gve
      gve_test->setVal(0.0);
      gve_test->setConstant(false);
    } else if (cfg.statType == kCLSB) {
      return nll_test;
    }

    unique_ptr<RooFitResult> fr{sbModel_full->fitTo(ds,
                                /*RooFit::GlobalObservables(*Global_observables),*/
                                RooFit::Constrain(*constrainedGlobal_parameters),
                                RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
                                RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
                                RooFit::Verbose(false), RooFit::Save())
    };
    // cout << "Found best mu " << gve_test->getVal() << endl;
    double nll_uncond = fr->minNll();

    if (gve_test->getVal() > 1.0e-4 && cfg.statType == kLHC)
      return 0.;
    q = 2.0 * (nll_test - nll_uncond);
  } else if (cfg.tossingType == kCousinsHighland) {
    gve_test->setVal(1.0e-4);
    RooNLLVar nll_sb_var("nll_sb_var", "nll_sb_var", *sbModel_simultaneous_noConstraints, ds, RooFit::Extended());
    q = nll_sb_var.getVal();

    RooNLLVar nll_bkg_var("nll_bkg_var", "nll_bkg_var", *bkgModel_simultaneous_noConstraints, ds, RooFit::Extended());

    q = q - nll_bkg_var.getVal();
    q = 2.0 * q;
    if (cfg.n_toys < 10) {
      cout << "nll_sb = " << nll_sb_var.getVal();
      cout << " nll_bkg = " << nll_bkg_var.getVal();
      cout << " q_toy = " << q << endl;
    }
  }
  return q;
}

RooDataSet *StatisticsModel::Generate(global_config &cfg, Double_t gve, RooAbsPdf *pdf, RooArgList &fitParams) {
  gve_test->setVal(gve);
  if (cfg.tossingType == kCousinsHighland) {
    // first toss pot_true, bkg_true, eff_true
    for (auto par : constraintsGlobal_map) {
      unique_ptr<RooDataSet> tmp_data(par.second->generate(*par.first, 1, RooFit::Verbose(false)));
      par.first->setVal(tmp_data->get(0)->getRealValue(par.first->GetName()));
    }
  } else {
    fitGlobal_parameters->assignFast(fitParams);
  }

  RooDataSet *toy{pdf->generate({*Theta, *ScanPoint_categories}, RooFit::Extended(), RooFit::Verbose(false))};
  return toy;
}

void StatisticsModel::ComputeQDistribution(global_config &cfg,
                                           RooAbsPdf *genPdf,
                                           RooArgList &paramsGen,
                                           RooArgSet *paramsInit,
                                           vector<double> &qDistribution) {
  qDistribution.clear();
  for (int iToy = 0; iToy < cfg.n_toys; ++iToy) {
    fitGlobal_parameters->assignFast(paramsGen);

    unique_ptr<RooDataSet> toys{genPdf->generate({*Theta, *ScanPoint_categories}, RooFit::Extended(), RooFit::Verbose(false))};
    qDistribution.push_back(ComputeQstatistic(cfg, *toys, paramsInit));
  }
}
