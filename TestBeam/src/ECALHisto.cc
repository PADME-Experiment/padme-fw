#include "ECALHisto.hh"

ECALHisto::ECALHisto() : VHisto("ECAL") {

  // Channel map
  std::string name[32];
  //name[0] = "APD 1 (0,0)";
  //name[1] = "APD 2 (0,1)";
  //name[2] = "APD 3 (0,2)";
  //name[3] = "APD 4 (0,3)";
  //name[4] = "APD 5 (1,0)";
  //name[5] = "APD 6 (1,1)";
  //name[6] = "APD 7 (1,2)";
  //name[7] = "APD 8 (1,3)";
  //name[8] = "APD 9 (2,0)";
  //name[9] = "APD 10 (2,1)";
  //name[10] = "APD 11 (2,2)";
  //name[11] = "APD 12 (2,3)";
  //name[12] = "APD 13 (3,0)";
  //name[13] = "APD 14 (3,1)";
  //name[14] = "APD 15 (3,2)";
  //name[15] = "APD 16 (3,3) G";
  name[0] = "PMT BGO 4cm (0,0)";
  name[1] = "PMT BGO 20cm (0,1)";
  name[2] = "";
  name[3] = "";
  name[4] = "";
  name[5] = "";
  name[6] = "";
  name[7] = "";
  name[8] = "";
  name[9] = "";
  name[10] = "";
  name[11] = "";
  name[12] = "";
  name[13] = "";
  name[14] = "";
  name[15] = "";
  name[16] = "";
  name[17] = "";
  name[18] = "";
  name[19] = "";
  name[20] = "";
  name[21] = "";
  name[22] = "";
  name[23] = "";
  name[24] = "";
  name[25] = "";
  name[26] = "";
  name[27] = "";
  name[28] = "";
  name[29] = "";
  name[30] = "";
  name[31] = "";

  // Create list to hold all histograms needed for ECAL
  fHistoList = new TList();

  //fHistoList->Add(new TH1D("ECALQTot","ECAL - APD Total Charge",6000,-1000.,17000.));
  //fHistoList->Add(new TH2D("ECALPos","ECAL - APD Position",120,0.,8.,120,0.,8.));

  for(int kk=0;kk<32;kk++){
    if (name[kk] != "") {
      fHistoList->Add(new TH1D(Form("ECALPedCh%d",kk),Form("ECAL %s (Ch %d) Pedestals", name[kk].c_str(),kk), 600,    3500., 4100.));
      fHistoList->Add(new TH1D(Form("ECALQCh%d",  kk),Form("ECAL %s (Ch %d) Charge",    name[kk].c_str(),kk),5000,-500.,2000.));
      fHistoList->Add(new TH1D(Form("ECALRawCh%d",kk),Form("ECAL %s (Ch %d) Raw Counts",name[kk].c_str(),kk),1024,    0.,1024.));
      fHistoList->Add(new TH1D(Form("ECALSigCh%d",kk),Form("ECAL %s (Ch %d) Signal",    name[kk].c_str(),kk),1024,    0.,1024.));
    }
  }

  fHistoList->Add(new TH1D("ECALTR00","Trigger 00 Signal",1024,0.,1024.));
  //fHistoList->Add(new TH1D("ECALTR01","Trigger 01 Signal",1024,0.,1024.));
  //fHistoList->Add(new TH1D("ECALTR10","Trigger 10 Signal",1024,0.,1024.));
  //fHistoList->Add(new TH1D("ECALTR11","Trigger 11 Signal",1024,0.,1024.));

}

ECALHisto::~ECALHisto()
{}
