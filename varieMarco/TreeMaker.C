{
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

using namespace std;

// vector<double> EBeam;
// ifstream file("/home/mancinima/padme-fw/varieMarco/BeamEnergy_list.txt");
// double val;
// while (file >> val) {
//   EBeam.push_back(val);
//   cout<<val<<endl;
// }
// file.close();

// for(int i = 0; i<EBeam.size(); i++){
//   cout<<"beam energy from txt input "<<EBeam[i]<<endl;
// }
double EBeam[5] = {190.00, 220.00, 230.00, 240.00, 250.00};
for(int i = 0; i < 5; i++) {
  TString fileNameFull = Form("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaFull/BhabhaFull_%.2f.root", EBeam[i]);
  // TString fileNameSCh = Form("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaSCh/BhabhaSCh_%.2f.root", EBeam[i]);
  // TString fileNameAA = Form("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProduction/AAProduction_%.2f.root", EBeam[i]);
  TFile fileFull(fileNameFull, "recreate");
  // TFile fileSCh(fileNameSCh, "recreate");
  // TFile fileAA(fileNameAA, "recreate");
  TTree* treeFull = new TTree(Form("tBhabhaFull_%.2f", EBeam[i]), "data from .txt file");
  // TTree* treeSCh = new TTree(("tBhabhaSCh_" + TString::Format("%.2f", EBeam[i])), "data from .txt file");
  // TTree* treeAA = new TTree(("tAAProduction_" + TString::Format("%.2f", EBeam[i])), "data from .txt file");

  cout<< "EBeam x loop: "<<EBeam[i]<<endl;
  double EBeamValue = EBeam[i]; // Supponiamo che EBeam[i] sia un valore double
  cout<< "EBeam before formatting: "<< EBeamValue <<endl;
  stringstream ss;
  ss << fixed << std::setprecision(2) << EBeamValue; // Formatta EBeamValue con 2 cifre decimali

  string formattedEBeam = ss.str();
  cout<< formattedEBeam << endl;

  Long64_t nlinesFull = treeFull->ReadFile(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaFull/BhabhaFull_" + formattedEBeam + ".txt").c_str(), 
      "col1/D:InitialP3_positron:InitialP3_electron:FFinalP1_electron:FFinalP2_electron:FFinalP3_electron:FFinalP1_positron:FFinalP2_positron:FFinalP3_positron:rem1:rem2");
  treeFull->Write();
  // Long64_t nlinesSCh = treeSCh->ReadFile(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaSCh/BhabhaSCh_" + formattedEBeam +".txt").c_str(), 
      // "col1/D:InitialP3_positron:InitialP3_electron:SFinalP1_electron:SFinalP2_electron:SFinalP3_electron:SFinalP1_positron:SFinalP2_positron:SFinalP3_positron:rem1:rem2");
  // treeSCh->Write();    
  // Long64_t nlinesAA = treeAA->ReadFile(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProduction/AAProduction_" + formattedEBeam +".txt").c_str(), 
      // "col1/D:InitialP3_positron:InitialP3_electron:GFinalP1_G1:GFinalP2_G1:GFinalP3_G1:GFinalP1_G2:GFinalP2_G2:GFinalP3_G2:rem1:rem2");
  // treeAA->Write();    
}

}
