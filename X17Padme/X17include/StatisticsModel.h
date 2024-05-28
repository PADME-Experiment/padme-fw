#ifndef STATISTICSMODEL
#define STATISTICSMODEL

#include "ChannelModel.h"
#include "GlobalParams.h"
#include "EEShapes.h"

#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooLognormal.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooArgList.h"

#include "TString.h"

#include <map>

using namespace std;

class global_config;
class RooAbsPdf;

class StatisticsModel {
public:
  StatisticsModel(global_config &cfg);
  ~StatisticsModel();

  void AddChannel(TString name, signal_types st, background_types bt,
                  double sqrts_mean, double err_sqrts, 
                  double bkg_mean, double sigma_bkg,
                  double pot_mean, double sigma_pot,
                  double eff_mean, double sigma_eff);
  void BuildModel();
  void Update(global_config &cfg, double mass, double log_coupling);

  double GetTotalYield();
  Bool_t IsLowYield();
  Bool_t IsHighYield();

  RooDataSet *Generate(global_config &cfg, Double_t mu, RooAbsPdf *pdf, RooArgList &fitParams);

  double ComputeQstatistic(global_config &cfg, RooDataSet &ds, RooArgSet *paramsInit);

  void ComputeQDistribution(global_config &cfg, RooAbsPdf *genPdf,
                            RooArgList &paramsGen, RooArgSet *paramsInit,
                            vector<double> &qDistribution);

  void SetDefaultAuxiliaryObservation() {
    pot_scale_default->setVal(1);
    for (size_t i = 0; i < channels.size(); i++) {
      channelModels[i]->bkg_yield_default->setVal(default_bkgYield_MC[i]);
    }
  }

public:
  RooRealVar *SqrtS;
  Double_t* dsqrts;
  RooRealVar *Theta;
  RooRealVar *XMass;
  RooRealVar *gve_test;

  vector<ChannelModel *> channelModels;
  vector<TString> channels;
  RooCategory *ScanPoint_categories;

  RooRealVar *kLumi_default;
  RooRealVar *kLumi_true;
  RooRealVar *kLumi_sigma;
  RooAbsPdf  *kLumi_constraint;

  vector<double> default_bkgYield_MC; //mi serve??

  RooRealVar *pot_true;
  RooRealVar *bkg_yield_true;
  RooRealVar *eff_true;
  RooRealVar *signal_yield_bare;
  RooRealVar *signal_expected_ev;

  //Lineshape parameters
  RooRealVar *Bes_default;
  RooRealVar *Bes_true;
  RooRealVar *Bes_sigma;
  RooAbsPdf  *Bes_constraint;
  RooRealVar *NormCore_default;
  RooRealVar *NormCore_true;
  RooRealVar *NormCore_sigma;
  RooAbsPdf  *NormCore_constraint;
  RooRealVar *sigmaCore_default;
  RooRealVar *sigmaCore_true;
  RooRealVar *sigmaCore_sigma;
  RooAbsPdf  *sigmaCore_constraint;
  RooRealVar *NormTail_default;
  RooRealVar *NormTail_true;
  RooRealVar *NormTail_sigma;
  RooAbsPdf  *NormTail_constraint;
  RooRealVar *sigmaTail_default;
  RooRealVar *sigmaTail_true;
  RooRealVar *sigmaTail_sigma;
  RooAbsPdf  *sigmaTail_constraint;

  Double_t SqrtSMin;
  Double_t SqrtSMax;
  EEShapes *SignalShape;

  RooRealVar *pot_scale_default;
  RooRealVar *pot_scale_true;
  RooRealVar *pot_scale_sigma;
  RooAbsPdf  *pot_scale_constraint;
  
  RooRealVar *eff_scale_default;
  RooRealVar *eff_scale_true;
  RooRealVar *eff_scale_sigma;
  RooAbsPdf  *eff_scale_constraint;

  RooSimultaneous *sbModel_simultaneous;
  RooSimultaneous *bkgModel_simultaneous;

  RooProdPdf *sbModel_full;
  RooProdPdf *bkgModel_full;

  RooSimultaneous *sbModel_simultaneous_noConstraints;
  RooSimultaneous *bkgModel_simultaneous_noConstraints;

  RooArgSet *Global_observables;
  RooArgSet *fitGlobal_parameters;
  RooArgSet *constrainedGlobal_parameters; // needed because bkg does not depend on pot_true
  map<RooRealVar *, RooAbsPdf *> constraintsGlobal_map;

};

#endif