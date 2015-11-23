#include "TargetHisto.hh"

TargetHisto::TargetHisto() : VHisto("Target") {

  // Board map
  std::string name[32];
  //name[0] = "Strip Y6";
  name[0] = "";
  name[1] = "Strip Y7 RF";
  name[2] = "Strip Y8 RF";
  name[3] = "Strip Y9 CSA";
  //name[4] = "Strip Y10";
  name[4] = "";
  name[5] = "Strip Y11 RF";
  //name[6] = "Strip Y12";
  name[6] = "";
  name[7] = "Strip Y13 CSA";
  //name[8] = "Strip Y14";
  name[8] = "";
  name[9] = "Strip Y15 RF";
  name[10] = "Strip Y17 CSA";
  name[11] = "Scintillator";
  name[12] = "";
  name[13] = "";
  name[14] = "Strip X13 CSA";
  name[15] = "Strip X14 CSA";
  name[16] = "Strip Y1 CSA";
  name[17] = "Strip Y2 RF";
  //name[18] = "Strip Y3";
  name[18] = "";
  //name[19] = "Strip Y4";
  name[19] = "";
  name[20] = "Strip Y5 CSA";
  name[21] = "Calorimeter";
  name[22] = "Strip X1 CSA";
  name[23] = "Strip X2 CSA";
  name[24] = "Strip X3 CSA";
  name[25] = "Strip X4 CSA";
  name[26] = "Strip X5 CSA";
  name[27] = "Strip X7 CSA";
  name[28] = "Strip X8 CSA";
  name[29] = "Strip X9 CSA";
  name[30] = "Strip X10 CSA";
  name[31] = "Strip X11 CSA";

  // Create list to hold all histograms needed for Target
  fHistoList = new TList();

  fHistoList->Add(new TH1D("TargQTot","Target - Total Charge",400,0.,100.));
  fHistoList->Add(new TH2D("TargPos","Target - Position",120,-6.,6.,120,-6.,6.));

  for(int kk=0;kk<32;kk++){
    if (name[kk]!=""){
      fHistoList->Add(new TH1D(Form("TargPedCh%d",kk),Form("Target %s (Ch %d) Pedestals", name[kk].c_str(),kk), 400,0.,  40.));
      fHistoList->Add(new TH1D(Form("TargQCh%d",  kk),Form("Target %s (Ch %d) Charge",    name[kk].c_str(),kk), 400,0., 100.));
      fHistoList->Add(new TH1D(Form("TargRawCh%d",kk),Form("Target %s (Ch %d) Raw Counts",name[kk].c_str(),kk),1024,0.,1024.));
      fHistoList->Add(new TH1D(Form("TargSigCh%d",kk),Form("Target %s (Ch %d) Signal",    name[kk].c_str(),kk),1024,0.,1024.));
    }
  }

  fHistoList->Add(new TH1D("TargTR0","Trigger 0 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("TargTR1","Trigger 1 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("TargTR2","Trigger 2 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("TargTR3","Trigger 3 Signal",1024,0.,1024.));

}

TargetHisto::~TargetHisto()
{}
