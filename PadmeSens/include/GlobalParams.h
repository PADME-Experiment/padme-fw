#ifndef GLOBALPARAMS
#define GLOBALPARAMS

#include "TH2D.h"

#include "RooRealVar.h"
#include <vector>

using namespace std;


namespace GlobalParams{
  // extern RooRealVar mInv;
  // extern RooRealVar dpMass;
  // extern RooRealVar pot_true;
  
  extern vector<TString> signal_sources; 
};

enum e_runType{
  kDebugRun = 0,
  kExclusionRun = 1,
  kExpectedBandRun = 2
};

enum e_statistic_type{
  kTevatron = 0,
  kLHC = 1,
  kCLSB = 2
};

enum e_tossing_type{
  kCousinsHighland = 0,
  kFixToFit = 1
};


class global_config {
public:
  global_config()
      : hYield(), hMSigma(),
        mass_points(0), n_toys(100), log_coupling_points(0),
        useEE(false), useMuMu(true), runType(kExclusionRun), statType(kTevatron),cl(0.9),OutputName("minimization.log"){}
  ~global_config() {
    for (auto p : hYield){
      for (auto q : p.second){
        delete q.second;
      }
      p.second.clear();
    }
    hYield.clear();
    for (auto p : hMSigma){
      for (auto q : p.second){
        delete q.second;
      }
      p.second.clear();
    }
    hMSigma.clear();
    
    mass_points.clear();
    log_coupling_points.clear();
  }

  void retrieve_input();
  void configure_run(const int &argc, char **argv);
  void print_config();

public:
  map<TString, map<TString, TH2D*>> hYield;
  map<TString, map<TString, TH2D*>> hMSigma;
  vector<double> mass_points;
  int n_toys;
  vector<double> log_coupling_points;
  bool useEE;
  bool useMuMu;
  e_runType runType;
  e_statistic_type statType;
  e_tossing_type tossingType;
  double cl;
  TString OutputFile;
  const char* OutputName;
};

#endif