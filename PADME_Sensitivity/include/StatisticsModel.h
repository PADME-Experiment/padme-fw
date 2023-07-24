#ifndef STATISTICSMODEL
#define STATISTICSMODEL

#include "ChannelModel.h"
#include "GlobalParams.h"

#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooLognormal.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "TString.h"

#include <map>

class global_config;
class RooAbsPdf;

class StatisticsModel {
public:
  StatisticsModel(global_config &cfg);
  ~StatisticsModel();

  void AddChannel(TString name, signal_types st, background_types bt,
                  double bkg_mean, double sigma_bkg);
  void BuildModel();
  void Update(global_config &cfg, double mass, double log_coupling);

  double GetTotalYield();
  Bool_t IsLowYield();
  Bool_t IsHighYield();

  RooDataSet *Generate(global_config &cfg, Double_t mu, RooAbsPdf *pdf,
                       RooArgList &fitParams);

  double ComputeQstatistic(global_config &cfg, RooDataSet &ds,
                           RooArgSet *paramsInit);

  void ComputeQDistribution(global_config &cfg, RooAbsPdf *genPdf,
                            RooArgList &paramsGen, RooArgSet *paramsInit,
                            std::vector<double> &qDistribution);

  void SetDefaultAuxiliaryObservation() {
    pot_default->setVal(1.0);
    for (std::size_t i = 0; i < channels.size(); i++) {
      channelModels[i]->bkg_yield_default->setVal(default_bkgYield_MC[i]);
    }
  }

public:
  RooRealVar *invariantMass;
  RooRealVar *dpMass;
  RooRealVar *mu_test;

  std::vector<ChannelModel *> channelModels;
  std::vector<TString> channels;
  std::vector<double> default_bkgYield_MC;

  RooCategory *pid_categories;

  RooRealVar *pot_default;
  RooRealVar *pot_sigma;
  RooRealVar *pot_true;
  RooAbsPdf *pot_constraint;

  RooSimultaneous *sbModel_simultaneous;
  RooSimultaneous *bkgModel_simultaneous;

  RooProdPdf *sbModel_full;
  RooProdPdf *bkgModel_full;

  RooSimultaneous *sbModel_simultaneous_noConstraints;
  RooSimultaneous *bkgModel_simultaneous_noConstraints;

  RooArgSet *global_observarbles;
  RooArgSet *fit_parameters;
  RooArgSet
      *constrained_parameters; // needed because bkg does not depend on pot_true

  std::map<RooRealVar *, RooAbsPdf *> constraints_map;
};

#endif