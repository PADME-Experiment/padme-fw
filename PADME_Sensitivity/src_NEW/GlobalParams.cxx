#include "GlobalParams.h"

#include "TFile.h"
#include "TObjArray.h"
#include "TObjString.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace GlobalParams {
double Nominal_BeamEn[7] = {200, 202, 204, 206, 208, 210, 212}; //toy MC Marco below Res
// double Nominal_BeamEn[51] = {295.170, //following the order acquired during run III
// 293.600,
// 292.030,
// 290.460,
// 288.890,
// 284.490,
// 282.920,
// 281.030,
// 279.460,
// 277.890,
// 276.320,
// 275.220,
// 273.650,
// 272.080,
// 270.510,
// 268.940,
// 267.370,
// 265.960,
// 264.380,
// 262.810,
// 261.240,
// 211.510,
// 209.940,
// 208.370,
// 206.800,
// 205.230,
// 292.660,
// 291.090,
// 289.520,
// 287.950,
// 286.370,
// 284.800,
// 283.230,
// 281.660,
// 280.090,
// 278.520,
// 276.950,
// 275.380,
// 273.810,
// 272.240,
// 270.670,
// 269.100,
// 267.530,
// 265.960,
// 293.600,
// 295.170,
// 295.990,
// 296.650,
// 297.200,
// 298.000,
// 298.800};
} // namespace GlobalParams

void global_config::retrieve_input() {

  TFile *N2ClusterIn = TFile::Open("/home/mancinima/padme-fw/varieMarco/N2Cluster.root");
  TTree *TreeIn_N2Cluster = static_cast<TTree*>(N2ClusterIn->Get("Tree"));      
  vector<double> *VectorIn_N2Cluster = nullptr;
  TBranch *branch_N2Cluster = TreeIn_N2Cluster->GetBranch("value");
  VectorIn_N2Cluster = new vector<double>();
  branch_N2Cluster->SetAddress(&VectoIn_N2Cluster);
  
  // TFile *NPoTIn = TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/NPoT.root");
  // TTree *TreeIn_NPoT = static_cast<TTree*>(NPoTIn->Get("myTree"));      
  // vector<double> *VectorIn_NPoT = nullptr;
  // TBranch *branch_NPoT = TreeIn_NPoT->GetBranch("NPoT");
  // VectorIn_NPoT = new vector<double>();
  // branch_NPoT->SetAddress(&Vector_NPoT);

  // TFile *SigmaEbeamIn = TFile::Open("/home/mancinima/padme-fw/varieMarco/X17_prod/SigmaEbeam.root");
  // TTree *TreeIn_SigmaEbeam = static_cast<TTree*>(SigmaEbeamIn->Get("myTree"));      
  // vector<double> *VectorIn_SigmaEbeam = nullptr;
  // TBranch *branch_SigmaEbeam = TreeIn_SigmaEbeam->GetBranch("SigmaEbeam");
  // VectorIn_SigmaEbeam = new vector<double>();
  // branch_SigmaEbeam->SetAddress(&Vector_SigmaEbeam);
  
  TFile *EffSignalIn = TFile::Open("/home/mancinima/padme-fw/varieMarco/EffSignalBelow.root");
  TTree *TreeIn_EffSignal = static_cast<TTree*>(EffSignalIn->Get("Tree"));      
  vector<double> *VectorIn_EffSignal = nullptr;
  TBranch *branch_EffSignal = TreeIn_EffSignal->GetBranch("value");
  VectorIn_EffSignal = new vector<double>();
  branch_EffSignal->SetAddress(&Vector_EffSignal);

  int numEntries = TreeIn_N2Clustern->GetEntries();
  vN2Cluster.resize(numEntries);
  // vNPoT.resize(numEntries);
  // vSigmaEbeam.resize(numEntries);
  vEffSignal.resize(numEntries);

  for (int i = 0; i < numEntries; i++) {
    TreeIn_N2Cluster->GetEntry(i);
    vN2Cluster[i] = (*Vector_N2Cluster)[0];
    vNPoT.push_back(10E10);
    vSigmaEbeam.push_back(0.75); //MeV Beam Energy Spread
    // TreeIn_NPoT->GetEntry(i);
    // vNPoT[i] = (*Vector_NPoT)[0];
    // TreeIn_SigmaEbeam->GetEntry(i);
    // vSigmaEbeam[i] = (*Vector_SigmaEbeam)[0];
    TreeIn_EffSignal->GetEntry(i);
    vEffSignal[i] = (*Vector_EffSignal)[0];
  }

  delete VectorIn_N2Cluster;
  N2ClusterIn->Close();
  // delete Vector_NPoT;
  // NPoTIn->Close();
  // delete Vector_SigmaEbeam;
  SigmaEbeamIn->Close();
  delete Vector_EffSignal;
  EffSignalIn->Close();

  // vector<double> vNPoT;
  // TFile *NPoTIn = TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/NAMEFILE.root");
  // vNPoT = (NPoTIn->FindObjectAny("NAMEVECTOR")->Clone("NAMEVECTORNEW"));

  // vector<double> vSigmaEbeam;
  // TFile *SigmaEbeamIn = TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/NAMEFILE.root");
  // vSigmaEbeam = (SigmaEbeamIn->FindObjectAny("NAMEVECTOR")->Clone("NAMEVECTORNEW"));

  // vector<double> vEffSignal;
  // TFile *EffSignalIn = TFile::Open("/home/mancinima/padme-fw/PADME_Sensitivity/NAMEFILE.root");
  // vEffSignal = (EffSignalIn->FindObjectAny("NAMEVECTOR")->Clone("NAMEVECTORNEW"));
}

void global_config::configure_run(const int &argc, char **argv) { //sulla base di come è definita la funzione nel file .h, qui leggo da default (se non definisco input file in fase di esecuzione)
  double dlog_coupling = 0.1;                                     // o da file in input la configurazione globale di tutti i paramentri in entrata

  if (argc == 1) {
    // use default
    cout<<"missing second argument: Retry adding the configuration file"<<endl;
    return 0
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
      } else if (line.BeginsWith("nPoints")) {
        TObjArray *l = line.Tokenize(" ");
        useEE = static_cast<TObjString *>(l->At(1))->GetString().Atoi();
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
          for (Int_t iM = 1; iM <= vN2Cluster.size(); iM++) { 
          X17mass.push_back(vN2Cluster[i]);
          } 
        }
        else {
          m = static_cast<TObjString *>(l->At(1))->GetString().Atof();
          X17mass.push_back(m);
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

  double min_log_coupling = -10.;
  double max_log_coupling = -4.;
  Int_t n_log_coupling = floor((max_log_coupling - min_log_coupling) / dlog_coupling) + 1;
  for (Int_t iE = 0; iE < n_log_coupling; iE++) {
    log_coupling_points.push_back(min_log_coupling + iE * dlog_coupling);
  }
}

void global_config::print_config(){

  cout<<"Computing for: \n";
  cout<<"--- masses:";
  for (double m : X17mass){
    cout<<" "<<m<<"\n";
  }
  cout<<endl;

  cout<<"--- d(loge): "<<(log_coupling_points[1]-log_coupling_points[0])<<endl;
  cout<<"--- ee channel: "<<useEE<<endl;
  cout<<"--- run type: "<<runType<<endl;
  cout<<"--- stat type: "<<statType<<endl;
  cout<<"--- tossing type: "<<tossingType<<endl;
  cout<<"--- cl: "<<cl<<endl;
  cout<<"--- nb toys: "<<n_toys<<endl;
  cout<<"--- Output log Filename: "<<OutputFile<<endl;

}
