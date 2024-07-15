#include "GlobalParams.h"

#include "TFile.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TGraphErrors.h"
#include "TGraph.h"


#include <fstream>
#include <iostream>

using namespace std;

namespace GlobalParams {
// vector<TString> signal_sources = {"direct", "meson"};
} // namespace GlobalParams

void global_config::retrieve_input() {
  TFile *InFile = new TFile("InputToy_LowFluctuation.root");  
  gPoT = (TGraphErrors*)(InFile->Get("gPoT")->Clone());
  gEff = (TGraphErrors*)(InFile->Get("gEff")->Clone());
  gBkg = (TGraphErrors*)(InFile->Get("gBkg")->Clone());
  gData = (TGraphErrors*)(InFile->Get("gData")->Clone());
  // Ensure the input file is closed properly
  InFile->Close();
  delete InFile;
}


void global_config::configure_run(const int &argc, char **argv) {
  double dlog_coupling = 0.1;

  if (argc == 1) {
    // use default
    for (Int_t iM = 1; iM <= gPoT->GetN(); iM++) {
      mass_points.push_back(gPoT->GetPointX(iM));
    }
  } else if (argc == 2) {
    ifstream config_file(argv[1]);
    TString line;

    //MM: using <static_cast *> instead of a regular cast as SG does. IDK why, but normal cast does not work.
    while (line.ReadLine(config_file)) {
      if (line.BeginsWith("#"))
        continue;
      else if (line.BeginsWith("DeltaLogCoupling")) {
        TObjArray *l = line.Tokenize(" ");
        dlog_coupling = static_cast<TObjString *>(l->At(1))->GetString().Atof();
        delete l;
        continue;
      } else if (line.BeginsWith("NbToys")) {
        TObjArray *l = line.Tokenize(" ");
        n_toys = static_cast<TObjString *>(l->At(1))->GetString().Atoi();
        delete l;
        continue;
      } else if (line.BeginsWith("nChannel")) {
        TObjArray *l = line.Tokenize(" ");
        nChannel = static_cast<TObjString *>(l->At(1))->GetString().Atoi();
        if(nChannel>gPoT->GetN()){
          cout << "The number of channels is too large: " << l << " > the whole scan (" << gPoT->GetN() << ")" << endl;
          nChannel = gPoT->GetN();
          cout << "The number of channels is set to the max value: " << gPoT->GetN() << endl;
          break;
        }
        delete l;
        continue;
      } else if (line.BeginsWith("RunType")) {
        TObjArray *l = line.Tokenize(" ");
        runType = static_cast<e_runType>(static_cast<TObjString *>(l->At(1))->GetString().Atoi());
        delete l;
        continue;
      } else if (line.BeginsWith("StatisticsType")) {
        TObjArray *l = line.Tokenize(" ");
        statType = static_cast<e_statistic_type>(static_cast<TObjString *>(l->At(1))->GetString().Atoi());
        delete l;
        continue;
      } else if (line.BeginsWith("TossingType")) {
        TObjArray *l = line.Tokenize(" ");
        tossingType = static_cast<e_tossing_type>(static_cast<TObjString *>(l->At(1))->GetString().Atoi());
        delete l;
        continue;
      } else if (line.BeginsWith("ConfLevel")) {
        TObjArray *l = line.Tokenize(" ");
        cl = static_cast<TObjString *>(l->At(1))->GetString().Atof();
        delete l;
        continue;
      } else if (line.BeginsWith("MassPoints")) {
        TObjArray *l = line.Tokenize(" ");
        double m = static_cast<TObjString *>(l->At(1))->GetString().Atof();
        mass_points.push_back(m);
        delete l;
        continue;
      }
    }
  }

  double min_log_coupling = -3.;
  double max_log_coupling = -2.;
  Int_t n_log_coupling = floor((max_log_coupling - min_log_coupling) / dlog_coupling) + 1;
  for (Int_t iE = 0; iE < n_log_coupling; iE++) {
    log_coupling_points.push_back(min_log_coupling + iE * dlog_coupling);
  }
}

void global_config::print_config(){
  cout << "Computing for: \n";
  cout << "--- masses:";
  for (double m : mass_points){
    cout << " " << m;
  }
  cout << endl;

  cout << "--- d(loge): " << log_coupling_points[1]-log_coupling_points[0] << endl;
  cout << "--- nChannel: " << nChannel << endl;
  cout << "--- run type: " << runType << endl;
  cout << "--- stat type: " << statType << endl;
  cout << "--- tossing type: " << tossingType << endl;
  cout << "--- cl: " << cl << endl;
  cout << "--- nb toys: " << n_toys << endl;
}