#include "ECALHisto.hh"

ECALHisto::ECALHisto() : VHisto("ECAL") {

  // Channel map
  std::string name[32];
  name[0] = "Block 0 (-1,-1)";
  name[1] = "Block 1 (-1,0)";
  name[2] = "Block 2 (-1,1)";
  name[3] = "Block 3 (0,-1)";
  name[4] = "Block 4 (0,0)";
  name[5] = "Block 5 (0,1)";
  name[6] = "Block 6 (1,-1)";
  name[7] = "Block 7 (1,0)";
  name[8] = "Block 8 (1,1)";
  name[9] = "Pad Up";
  name[10] = "Pad Down";
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

  fHistoList->Add(new TH1D("ECALQTot","ECAL - Total Charge",4000,0.,1000.));
  fHistoList->Add(new TH2D("ECALPos","ECAL - Position",120,-6.,6.,120,-6.,6.));

  for(int kk=0;kk<32;kk++){
    if (name[kk] != "") {
      fHistoList->Add(new TH1D(Form("ECALPedCh%d",kk),Form("ECAL %s (Ch %d) Pedestals", name[kk].c_str(),kk), 400,3800.,4200.));
      fHistoList->Add(new TH1D(Form("ECALQCh%d",kk),  Form("ECAL %s (Ch %d) Charge",    name[kk].c_str(),kk),4000,   0.,5000.));
      fHistoList->Add(new TH1D(Form("ECALRawCh%d",kk),Form("ECAL %s (Ch %d) Raw Counts",name[kk].c_str(),kk),1024,   0.,1024.));
      fHistoList->Add(new TH1D(Form("ECALSigCh%d",kk),Form("ECAL %s (Ch %d) Signal",    name[kk].c_str(),kk),1024,   0.,1024.));
    }
  }

}

ECALHisto::~ECALHisto()
{}
