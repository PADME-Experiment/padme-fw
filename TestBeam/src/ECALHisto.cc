#include "ECALHisto.hh"

ECALHisto::ECALHisto() : VHisto("ECAL") {

  // Create list to hold all histograms needed for ECAL
  fHistoList = new TList();

  fHistoList->Add(new TH1D("ECALQTot","ECAL - Total Charge",4000,0.,8000.));

  fHistoList->Add(new TH2D("ECALPos","ECAL - Position",120,-6.,6.,120,-6.,6.));

  for(int kk=0;kk<9;kk++){

    fHistoList->Add(new TH1D(Form("ECALPed%d",kk),Form("ECAL - Ch %d Pedestals",kk),400,3800.,4200.));

    fHistoList->Add(new TH1D(Form("ECALQCh%d",kk),Form("ECAL - Ch %d Charge",kk),4000,0.,8000.));

  }

}

ECALHisto::~ECALHisto()
{}
