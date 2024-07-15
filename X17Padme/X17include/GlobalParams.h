#ifndef GLOBALPARAMS
#define GLOBALPARAMS

#include "TH2D.h"

#include "RooRealVar.h"
#include "TGraphErrors.h"
#include <vector>

using namespace std;

namespace GlobalParams{
  // extern RooRealVar pot_true;
  
  // extern vector<TString> signal_sources; 
};

enum e_runType{
  kDebugRun = 0,
  kExclusionRun = 1,
  kExpectedBandRun = 2
};

enum e_statistic_type{
  kTevatron=0,
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
      : gPoT(nullptr), gEff(nullptr), gBkg(nullptr), gData(nullptr),
        mass_points(0), n_toys(100), log_coupling_points(0),
        nChannel(47), runType(kExclusionRun), statType(kTevatron),cl(0.9){}
  ~global_config() {
    
    mass_points.clear();
    log_coupling_points.clear();
  }

  void retrieve_input();
  void configure_run(const int &argc, char **argv);
  void print_config();

public:
  TGraphErrors* gPoT;
  TGraphErrors* gEff;
  TGraphErrors* gBkg;
  TGraphErrors* gData;
  
  vector<double> mass_points;
  int n_toys;
  vector<double> log_coupling_points;
  int nChannel;
  e_runType runType;
  e_statistic_type statType;
  e_tossing_type tossingType;
  double cl;
};

#endif