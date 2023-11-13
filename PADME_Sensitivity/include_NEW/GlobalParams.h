#ifndef GLOBALPARAMS
#define GLOBALPARAMS

#include "TH2D.h"
#include "TH1D.h"

#include "RooRealVar.h"
#include <vector>

using namespace std;


namespace GlobalParams{
  // extern RooRealVar mInv;
  // extern RooRealVar dpMass;
  // extern RooRealVar pot_true;
  
  extern double Nominal_BeamEn; 
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
      : vN2Cluster(), vNPoT(), vSigmaEbeam(), vEffSignal(),
        X17mass(17), n_toys(100), log_coupling_points(0),
        nPoints(5), runType(kExclusionRun), statType(kTevatron),cl(0.9),OutputName("minimization.log"){}
  ~global_config() {
    // for (auto p : vN2Cluster){
    //   for (auto q : p.second){
    //     delete q.second;
    //   }
    //   p.second.clear();
    // }
    // vN2Cluster.clear();
    // for (auto p : vNPoT){
    //   for (auto q : p.second){
    //     delete q.second;
    //   }
    //   p.second.clear();
    // }
    // vNPoT.clear();
    // for (auto p : vSigmaEbeam){
    //   for (auto q : p.second){
    //     delete q.second;
    //   }
    //   p.second.clear();
    // }
    // vSigmaEbeam.clear();
    // for (auto p : vEffSignal){
    //   for (auto q : p.second){
    //     delete q.second;
    //   }
    //   p.second.clear();
    // }
    // vEffSignal.clear();
    X17mass.clear();
    log_coupling_points.clear();
  }

  void retrieve_input();
  void configure_run(const int &argc, char **argv);
  void print_config();

public:
  vector<double> vN2Cluster;
  vector<double> vNPoT;
  vector<double> vSigmaEbeam;
  vector<double> vEffSignal;
  vector<double> X17mass;
  int n_toys;
  vector<double> log_coupling_points;
  int nPoints;
  e_runType runType;
  e_statistic_type statType;
  e_tossing_type tossingType;
  double cl;
  TString OutputFile;
  const char* OutputName;
};

#endif