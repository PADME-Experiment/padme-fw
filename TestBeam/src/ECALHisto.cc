#include "ECALHisto.hh"

ECALHisto::ECALHisto() : VHisto("ECAL") {

  // Channel map
  std::string name[32];
  name[0] = "PMT Block 1 (-1,-1)";
  name[1] = "PMT Block 2 (-1,0)";
  name[2] = "PMT Block 3 (-1,1)";
  name[3] = "PMT Block 4 (0,-1)";
  name[4] = "PMT Block 5 (0,0)";
  name[5] = "PMT Block 6 (0,1)";
  name[6] = "PMT Block 7 (1,-1)";
  name[7] = "PMT Block 8 (1,0)";
  name[8] = "PMT Block 9 (1,1)";
  name[9] = "";
  name[10] = "";
  name[11] = "";
  name[12] = "";
  name[13] = "";
  name[14] = "";
  name[15] = "";
  name[16] = "APD Block 1 (-1,-1)";
  name[17] = "APD Block 2 (-1, 0)";
  name[18] = "APD Block 3 (-1, 1)";
  name[19] = "APD Block 4 ( 0,-1)";
  name[20] = "APD Block 5 ( 0, 0)";
  name[21] = "APD Block 6 ( 0, 1)";
  name[22] = "APD Block 7 ( 1,-1)";
  name[23] = "APD Block 8 ( 1, 0)";
  name[24] = "APD Block 9 ( 1, 1)";
  name[25] = "";
  name[26] = "";
  name[27] = "";
  name[28] = "";
  name[29] = "";
  name[30] = "";
  name[31] = "";

  // Create list to hold all histograms needed for ECAL
  fHistoList = new TList();

  fHistoList->Add(new TH1D("ECALQTot1","ECAL - PMT Total Charge",6000,-1000.,17000.));
  fHistoList->Add(new TH2D("ECALPos1","ECAL - PMT Position",120,-6.,6.,120,-6.,6.));
  fHistoList->Add(new TH1D("ECALQTot2","ECAL - APD Total Charge",6000,-1000.,17000.));
  fHistoList->Add(new TH2D("ECALPos2","ECAL - APD Position",120,-6.,6.,120,-6.,6.));

  for(int kk=0;kk<32;kk++){
    if (name[kk] != "") {
      fHistoList->Add(new TH1D(Form("ECALPedCh%d",kk),Form("ECAL %s (Ch %d) Pedestals", name[kk].c_str(),kk), 600,3500.,4100.));
      fHistoList->Add(new TH1D(Form("ECALQCh%d",kk),  Form("ECAL %s (Ch %d) Charge",    name[kk].c_str(),kk),4500,-1000.,8000.));
      fHistoList->Add(new TH1D(Form("ECALRawCh%d",kk),Form("ECAL %s (Ch %d) Raw Counts",name[kk].c_str(),kk),1024,   0.,1024.));
      fHistoList->Add(new TH1D(Form("ECALSigCh%d",kk),Form("ECAL %s (Ch %d) Signal",    name[kk].c_str(),kk),1024,   0.,1024.));
    }
  }

  fHistoList->Add(new TH1D("ECALTR00","Trigger 00 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("ECALTR01","Trigger 01 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("ECALTR10","Trigger 10 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("ECALTR11","Trigger 11 Signal",1024,0.,1024.));

}

ECALHisto::~ECALHisto()
{}
