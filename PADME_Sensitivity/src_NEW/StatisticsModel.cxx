#include "StatisticsModel.h"
#include "GlobalParams.h"

#include "RooAbsPdf.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooLognormal.h"
#include "RooNLLVar.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "TString.h"

using namespace std;

StatisticsModel::StatisticsModel(global_config &cfg)
    : SqrtS(new RooRealVar("SqrtS", "SqrtS", 10., 20.)),
      X17Mass(new RooRealVar("X17Mass", "X17Mass", 10., 20.)),
      Mu_test(new RooRealVar("Mu_test", "Mu_test", 17.0, 5.0, 25.0)),
      channelModels(), NPoints(),
      PID_categories(new RooCategory("PID_category", "PID_category")), //categoria creata senza scelte, le aggiunge di seguito
      NPoT_default(new RooRealVar("NPoT_default", "NPoT_default", 1.0E9, 1.0E6, 1.0E13)),
      NPoT_sigma(new RooRealVar("NPoT_sigma", "NPoT_sigma", 1.0E7, 1.0E4, 1.0E11)),
      NPoT_true(new RooRealVar("NPoT_true", "NPoT_true", 1.0, 0.0001, 3.0)),
      NPoT_constraint(new RooLognormal("NPoT_constraint", "NPoT_constraint", *NPoT_true, *NPoT_default, *NPoT_sigma)),
      // NPoT_constraint(new RooGaussian("NPoT_constraint", "NPoT_constraint",
      //                                 *NPoT_default, *NPoT_true, *NPoT_sigma)),
      SbModel_simultaneous(new RooSimultaneous("SbModel_simultaneous", "SbModel_simultaneous", *pid_categories)),
      BkgModel_simultaneous(new RooSimultaneous("BkgModel_simultaneous", "BkgModel_simultaneous", *pid_categories)),
      SbModel_full(nullptr), BkgModel_full(nullptr),
      SbModel_simultaneous_noConstraints(new RooSimultaneous("SbModel_simultaneous_noConstraints", "SbModel_simultaneous_noConstraints", *pid_categories)),
      BkgModel_simultaneous_noConstraints(new RooSimultaneous("BkgModel_simultaneous_noConstraints", "BkgModel_simultaneous_noConstraints", *pid_categories)),
      global_observables(new RooArgSet("global_observables")),
      fit_parameters(new RooArgSet("fit_parameters")),
      constrained_parameters(new RooArgSet("constrained_parameters")),
      constraints_map() {

  X17Mass->setConstant();
  NPoT_default->setConstant();
  NPoT_sigma->setConstant();

  global_observables->add(*NPoT_default);
  fit_parameters->add(*NPoT_true);
  fit_parameters->add(*mu_test);
  constrained_parameters->add(*NPoT_true);

  constraints_map[NPoT_true] = NPoT_constraint;
}

StatisticsModel::~StatisticsModel() {
  cout << "Destroing statistics model ..." << endl;
  // for (size_t ich = 0; ich < channelModels.size(); ich++){
  //   delete channelModels[ich];
  // }
  channelModels.clear();
  NPoints.clear();

  cout << "...done!" << endl;
}

void StatisticsModel::AddChannel(int NPoint, signal_types st, background_types bt, double Bkg_mean, double Sigma_bkg) {
          channelModels.push_back(new ChannelModel(NPoints, *SqrtS, *X17Mass, *NPoT_true, *Mu_test, st, bt, Bkg_mean, Sigma_bkg));
  channels.push_back(NPoints);
  pid_categories->defineType(NPoints);
  global_observables->add(*channelModels.back()->Bkg_yield_default);
  fit_parameters->add(*channelModels.back()->bkg_yield_scale);
  constrained_parameters->add(*channelModels.back()->bkg_yield_scale);
  default_bkgYield_MC.push_back(bkg_mean);
  constraints_map[channelModels.back()->bkg_yield_scale] = channelModels.back()->bkg_yield_constraint;
} //creo il canale del modello statistico, poi con la lettura dei file in input creerò quelli dell'esperimento, qui sto solo facendo "la classe"

void StatisticsModel::BuildModel() {
  for (size_t ich = 0; ich < channels.size(); ich++) {
    sbModel_simultaneous->addPdf(*channelModels[ich]->full_sbModel, channels[ich]);
    bkgModel_simultaneous->addPdf(*channelModels[ich]->full_bkgModel, channels[ich]);
    sbModel_simultaneous_noConstraints->addPdf(*channelModels[ich]->extended_sbModel, channels[ich]);
    bkgModel_simultaneous_noConstraints->addPdf(*channelModels[ich]->extended_bkgModel, channels[ich]);
  }

  sbModel_full = new RooProdPdf("sbModel_full", "sbModel_full", {*sbModel_simultaneous, *NPoT_constraint});
  bkgModel_full = new RooProdPdf("bkgModel_full", "bkgModel_full", {*bkgModel_simultaneous, *NPoT_constraint});
}

void StatisticsModel::Update(global_config &cfg, double mass, double log_coupling) { //aggiorna i valori della massa e dei coupling durante il fit
  dpMass->setVal(mass);

  for (size_t ich = 0; ich < channels.size(); ich++) {
    TString channelName = channels[ich];
    for (TString ss : GlobalParams::signal_sources) {
      double new_val = cfg.hYield[channelName][ss]->Interpolate(mass, log_coupling);
      channelModels[ich]->signal_yield[ss]->setVal(new_val);

      new_val = cfg.hMSigma[channelName][ss]->Interpolate(mass, log_coupling);
      channelModels[ich]->dpMass_resolution[ss]->setVal(new_val);
    }
  }
}

double StatisticsModel::GetTotalYield() {
  double total_yield = 0.0;
  for (size_t ich = 0; ich < channels.size(); ich++) {
    for (TString ss : GlobalParams::signal_sources) {
      total_yield += channelModels[ich]->signal_yield[ss]->getVal();
    }
  }
  return total_yield; //Valuta lo Yield totale di tutti i possibili canali, pesati, per quella coppia di massa e coupling
}

Bool_t StatisticsModel::IsLowYield() {
  double total_yield = GetTotalYield();
  if (total_yield < 1.0)
    return kTRUE;
  return kFALSE;
}

Bool_t StatisticsModel::IsHighYield() {
  for (size_t ich = 0; ich < channels.size(); ich++) {
    for (TString ss : GlobalParams::signal_sources) {
      if (channelModels[ich]->signal_yield[ss]->getVal() > 10.0)
        return kTRUE;
    }
  }
  return kFALSE;
}

double StatisticsModel::ComputeQstatistic(global_config &cfg, RooDataSet &ds, RooArgSet *paramsInit) {
  double q = 0.;
  if (cfg.tossingType == kFixToFit) { //algoritmi per fare il fit e calcolare il q value (QUI è TIPO UNA RICETTA)
    fit_parameters->assignFast(*paramsInit);
    // in all cases we need the s+b fit
    mu_test->setVal(1.0);
    mu_test->setConstant(true);
    unique_ptr<RooFitResult> fr_sb_toy{
        sbModel_full->fitTo(
          ds, /*RooFit::GlobalObservables(*global_observables),*/
          RooFit::Constrain(*constrained_parameters),
          RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
          RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
          RooFit::Verbose(false), RooFit::Save())
        };
    // fr_sb_toy->Print();
    double nll_test = fr_sb_toy->minNll();

    fit_parameters->assignFast(*paramsInit);

    if (cfg.statType == kTevatron) {
      // do b-only fit
      mu_test->setVal(0.0);
      mu_test->setConstant(true);
    } else if (cfg.statType == kLHC) {
      // do global fit for best mu
      mu_test->setVal(0.5);
      mu_test->setConstant(false);
    } else if (cfg.statType == kCLSB) {
      return nll_test;
    }

    unique_ptr<RooFitResult> fr{
          sbModel_full->fitTo(
          ds, /*RooFit::GlobalObservables(*global_observables),*/
          RooFit::Constrain(*constrained_parameters),
          RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
          RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
          RooFit::Verbose(false), RooFit::Save())
        };
    // fr_unconditional->Print();
    // cout << "Found best mu " << mu_test->getVal() << endl;
    double nll_uncond = fr->minNll();

    if (mu_test->getVal() > 1.0 && cfg.statType == kLHC)
      return 0.;
    q = 2.0 * (nll_test - nll_uncond);
  } else if (cfg.tossingType == kCousinsHighland) {
    mu_test->setVal(1.0);
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
  // cout << "q_toy = "  << q << endl;
  return q;
}
//classi per generare i q value attesi, quelli di confronto in qualche modo !
RooDataSet *StatisticsModel::Generate(global_config &cfg, Double_t mu, RooAbsPdf *pdf, RooArgList &fitParams) {
  mu_test->setVal(mu);
  if (cfg.tossingType == kCousinsHighland) {
    // first toss NPoT_true and bkg_scale
    for (auto par : constraints_map) {
      unique_ptr<RooDataSet> tmp_data(par.second->generate(*par.first, 1, RooFit::Verbose(true)));
      par.first->setVal(tmp_data->get(0)->getRealValue(par.first->GetName()));
    }
  } else {
    fit_parameters->assignFast(fitParams);
  }

  RooDataSet *toy{pdf->generate({*invariantMass, *pid_categories}, RooFit::Extended(), RooFit::Verbose(true))};
  return toy;
}

void StatisticsModel::ComputeQDistribution(global_config &cfg, RooAbsPdf *genPdf, RooArgList &paramsGen, RooArgSet *paramsInit, vector<double> &qDistribution) {
  qDistribution.clear();
  // Generate auxiliaries
  // fit_parameters->assignFast(paramsGen);
  // unique_ptr<RooDataSet> auxiliary_measurements{genPdf->generate(
  //     *global_observables, cfg.n_toys, RooFit::Verbose(false))};

  for (int iToy = 0; iToy < cfg.n_toys; ++iToy) {
    fit_parameters->assignFast(paramsGen);
    // global_observables->assignFast(*auxiliary_measurements->get(iToy));

    unique_ptr<RooDataSet> toys{genPdf->generate({*invariantMass, *pid_categories}, RooFit::Extended(), RooFit::Verbose(false))};
    // toys->setGlobalObservables(*auxiliary_measurements->get(iToy));

    qDistribution.push_back(ComputeQstatistic(cfg, *toys, paramsInit));
  }
}
