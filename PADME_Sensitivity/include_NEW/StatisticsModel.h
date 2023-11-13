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

using namespace std;

class global_config;
class RooAbsPdf;

class StatisticsModel {
public:
  StatisticsModel(global_config &cfg);
  ~StatisticsModel();

  void AddChannel(int Nom_BeamEnergy, signal_types st, background_types bt, double Bkg_mean, double sigma_bkg);
  void BuildModel();
  void Update(global_config &cfg, double mass, double log_coupling); //modifica i valori in input di massa e coupling dopo aver ottimizzato il fit

  double GetTotalYield(); //con questa variabile poi è possibile fare un check se lo Yield sia troppo basso/alto 
  Bool_t IsLowYield();
  Bool_t IsHighYield();

  RooDataSet *Generate(global_config &cfg, Double_t mu, RooAbsPdf *pdf, RooArgList &fitParams);

  double ComputeQstatistic(global_config &cfg, RooDataSet &ds, RooArgSet *paramsInit); //è un metodo che chiamo con StatisticsModel:: e serve con i paramentri in input a calcolare il Qval

  void ComputeQDistribution(global_config &cfg, RooAbsPdf *genPdf, RooArgList &paramsGen, RooArgSet *paramsInit, vector<double> &qDistribution); // fa la stessa cosa di quello sopra ma per ricavare il valore atteso

  void SetDefaultAuxiliaryObservation() {
    pot_default->setVal(1.0);
    for (size_t i = 0; i < vNom_BE.size(); i++) {
      channelModels[i]->Bkg_yield_default->setVal(default_bkgYield_MC[i]);
    } //setta i valori di default di pot e yield del BKG, poi saremo sempre in grado di aggiornare questi valori
  }

public:
  RooRealVar *N2Cluster;  
  RooRealVar *NPoT;
  RooRealVar *SigmaEBeam;
  RooRealVar *EffSignal;
  RooRealVar *X17Mass;
  RooRealVar *Mu_test;

  vector<ChannelModel *> channelModels;
  vector<int> vNom_BE;
  vector<double> Default_bkgYield_MC;

  RooCategory *PID_categories;

  RooRealVar *NPoT_default;
  RooRealVar *NPoT_sigma;
  RooRealVar *NPoT_true;
  RooAbsPdf *NPoT_constraint;

  RooSimultaneous *SbModel_simultaneous;
  RooSimultaneous *BkgModel_simultaneous;

  RooProdPdf *SbModel_full;
  RooProdPdf *BkgModel_full;

  RooSimultaneous *SbModel_simultaneous_noConstraints;
  RooSimultaneous *BkgModel_simultaneous_noConstraints;

  RooArgSet *global_observables;
  RooArgSet *fit_parameters;
  RooArgSet *constrained_parameters; // needed because bkg does not depend on pot_true

  map<RooRealVar *, RooAbsPdf *> constraints_map;
};

#endif