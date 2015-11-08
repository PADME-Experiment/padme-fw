#include "TargetHisto.hh"

TargetHisto::TargetHisto() : VHisto("Target") {

  // Create list to hold all histograms needed for Target
  fHistoList = new TList();

  fHistoList->Add(new TH1D("TargQTot","Target - Total Charge",4000,0.,8000.));

  fHistoList->Add(new TH2D("TargPos","Target - Position",120,-6.,6.,120,-6.,6.));

  for(int kk=0;kk<9;kk++){

    fHistoList->Add(new TH1D(Form("TargPed%d",kk),Form("Target - Ch %d Pedestals",kk),400,3800.,4200.));

    fHistoList->Add(new TH1D(Form("TargQCh%d",kk),Form("Target - Ch %d Charge",kk),4000,0.,8000.));

  }

}

TargetHisto::~TargetHisto()
{}
