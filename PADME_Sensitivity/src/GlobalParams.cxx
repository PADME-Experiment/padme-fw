#include "GlobalParams.h"

#include "TFile.h"
#include "TObjArray.h"
#include "TObjString.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace GlobalParams {
vector<TString> signal_sources = {"direct", "meson"};
} // namespace GlobalParams

void global_config::retrieve_input() {
  // extract signal yield
  // ee
  map<TString, TH2D *> ee_yield;
  TFile *dp_ee_yield = TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/DP_nexp_ee.root");
  ee_yield["direct"] = static_cast<TH2D *>(dp_ee_yield->FindObjectAny("hNexp_90CL_ee_direct")->Clone("hYield_ee_direct"));
  ee_yield["direct"]->SetDirectory(0);

  ee_yield["meson"] = static_cast<TH2D *>(dp_ee_yield->FindObjectAny("hNexp_90CL_ee_meson")->Clone("hYield_ee_meson"));
  ee_yield["meson"]->SetDirectory(0);
  dp_ee_yield->Close();
  hYield["ee"] = ee_yield;

  // mumu
  map<TString, TH2D *> mumu_yield;
  TFile *dp_mumu_yield = TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/DP_nexp_mumu.root");
  mumu_yield["direct"] = static_cast<TH2D *>(dp_mumu_yield->FindObjectAny("hNexp_90CL_mumu_direct")->Clone("hYield_mumu_direct"));
  mumu_yield["direct"]->SetDirectory(0);

  mumu_yield["meson"] = static_cast<TH2D *>(dp_mumu_yield->FindObjectAny("hNexp_90CL_mumu_meson")->Clone("hYield_mumu_meson"));
  mumu_yield["meson"]->SetDirectory(0);
  dp_mumu_yield->Close();
  hYield["mumu"] = mumu_yield;

  // extract invariant mass resolution
  // ee
  map<TString, TH2D *> ee_msigma;
  TFile *dp_ee_msigma_direct = TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/DP_mass_sigma_direct_ee_1402.root");
  ee_msigma["direct"] = static_cast<TH2D *>(dp_ee_msigma_direct->FindObjectAny("hMassResolutionSmoothed")->Clone("hMSigma_ee_direct"));
  ee_msigma["direct"]->SetDirectory(0);
  dp_ee_msigma_direct->Close();

  TFile *dp_ee_msigma_meson = TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/DP_mass_sigma_meson_ee_1402.root");
  ee_msigma["meson"] = static_cast<TH2D *>(dp_ee_msigma_meson->FindObjectAny("hMassResolutionSmoothed")->Clone("hMSigma_ee_meson")); //modificato nome TH2D clonato
  ee_msigma["meson"]->SetDirectory(0);
  dp_ee_msigma_meson->Close();
  hMSigma["ee"] = ee_msigma;

  // mumu
  map<TString, TH2D *> mumu_msigma;
  TFile *dp_mumu_msigma_direct =TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/DP_mass_sigma_direct_mumu_2101.root");
  mumu_msigma["direct"] = static_cast<TH2D *>(dp_mumu_msigma_direct->FindObjectAny("hMassResolutionSmoothed")->Clone("hMSigma_mumu_direct"));
  mumu_msigma["direct"]->SetDirectory(0);
  dp_mumu_msigma_direct->Close();

  TFile *dp_mumu_msigma_meson =TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/DP_mass_sigma_meson_mumu_2101.root");
  mumu_msigma["meson"] = static_cast<TH2D *>(dp_mumu_msigma_meson->FindObjectAny("hMassResolutionSmoothed")->Clone("hMSigma_mumu_meson")); //modificato nome TH2D clonato
  mumu_msigma["meson"]->SetDirectory(0);
  dp_mumu_msigma_meson->Close();
  hMSigma["mumu"] = mumu_msigma;
}

void global_config::configure_run(const int &argc, char **argv) { //sulla base di come è definita la funzione nel file .h, qui leggo da default (se non definisco input file in fase di esecuzione)
  double dlog_coupling = 0.1;                                     // o da file in input la configurazione globale di tutti i paramentri in entrata

  if (argc == 1) {
    // use default
    for (Int_t iM = 1; iM <= hMSigma["ee"]["direct"]->GetXaxis()->GetNbins(); iM++) { //modificato ma non sembra funzionare
      mass_points.push_back(hMSigma["ee"]["direct"]->GetXaxis()->GetBinCenter(iM));
    }
  } else if (argc == 2) {
    ifstream config_file(argv[1]);
    TString line;

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
      } else if (line.BeginsWith("UseEE")) {
        TObjArray *l = line.Tokenize(" ");
        useEE = static_cast<TObjString *>(l->At(1))->GetString().Atoi();
        delete l;
        continue;
      } else if (line.BeginsWith("UseMuMu")) {
        TObjArray *l = line.Tokenize(" ");
        useMuMu = static_cast<TObjString *>(l->At(1))->GetString().Atoi();
        delete l;
        continue;
      } else if (line.BeginsWith("RunType")) {
        TObjArray *l = line.Tokenize(" ");
        runType = static_cast<e_runType>(
            static_cast<TObjString *>(l->At(1))->GetString().Atoi());
        delete l;
        continue;
      } else if (line.BeginsWith("StatisticsType")) {
        TObjArray *l = line.Tokenize(" ");
        statType = static_cast<e_statistic_type>(
            static_cast<TObjString *>(l->At(1))->GetString().Atoi());
        delete l;
        continue;
      } else if (line.BeginsWith("TossingPolitics")) {
        TObjArray *l = line.Tokenize(" ");
        tossingType = static_cast<e_tossing_type>(
            static_cast<TObjString *>(l->At(1))->GetString().Atoi());
        delete l;
        continue;
      } else if (line.BeginsWith("ConfLevel")) {
        TObjArray *l = line.Tokenize(" ");
        cl = static_cast<TObjString *>(l->At(1))->GetString().Atof();
        delete l;
        continue;
      } else if (line.BeginsWith("MassPoints")) {
        TObjArray *l = line.Tokenize(" ");
        double m = 0;
        if (!static_cast<TObjString *>(l->At(1))->GetString().Atof()) {
          for (Int_t iM = 1; iM <= hMSigma["ee"]["direct"]->GetXaxis()->GetNbins(); iM++) { //modificato ma non sembra funzionare
            m = hMSigma["ee"]["direct"]->GetXaxis()->GetBinCenter(iM);
            mass_points.push_back(m);
          } 
        }
        else {
          m = static_cast<TObjString *>(l->At(1))->GetString().Atof();
          mass_points.push_back(m);
        } 
        delete l;
        continue;
      } else if (line.BeginsWith("OutputLog")) {
        TObjArray *l = line.Tokenize(" ");
        OutputFile = static_cast<TObjString *>(l->At(1))->GetString();
        OutputName = OutputFile.Data();
        delete l;
        continue;
      }
  }
  }

  double min_log_coupling = -18.;
  double max_log_coupling = -6.;
  Int_t n_log_coupling = floor((max_log_coupling - min_log_coupling) / dlog_coupling) + 1;
  for (Int_t iE = 0; iE < n_log_coupling; iE++) {
    log_coupling_points.push_back(min_log_coupling + iE * dlog_coupling);
  }
}

void global_config::print_config(){

  cout<<"Computing for: \n";
  cout<<"--- masses:";
  for (double m : mass_points){
    cout<<" "<<m<<"\n";
  }
  cout<<endl;

  cout<<"--- d(loge): "<<(log_coupling_points[1]-log_coupling_points[0])<<endl;
  cout<<"--- ee channel: "<<useEE<<endl;
  cout<<"--- mumu channel: "<<useMuMu<<endl;
  cout<<"--- run type: "<<runType<<endl;
  cout<<"--- stat type: "<<statType<<endl;
  cout<<"--- tossing type: "<<tossingType<<endl;
  cout<<"--- cl: "<<cl<<endl;
  cout<<"--- nb toys: "<<n_toys<<endl;
  cout<<"--- Output log Filename: "<<OutputFile<<endl;

}
