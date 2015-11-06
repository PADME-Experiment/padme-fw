#include "ECALHisto.hh"

ECALHisto::ECALHisto() : VHisto("ECAL") {

  // Create list to hold all histograms needed for ECAL
  fHistoList = new TList();

  //TH1D* hCaloQTot = new TH1D("hQTotCalo","hQTotCalo",4000,0.,8000.);
  fHistoList->Add(new TH1D("hQTotCalo","hQTotCalo",4000,0.,8000.));

  //hCaloPos = new TH2D("hPosCalo","hPosCalo",120,-6.,6.,120,-6.,6.);
  fHistoList->Add(new TH2D("hPosCalo","hPosCalo",120,-6.,6.,120,-6.,6.));

  for(int kk=0;kk<9;kk++){

    //hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),400,3800.,4200.);
    fHistoList->Add(new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),400,3800.,4200.));

    //hQCh[kk] = new TH1D(Form("hQCh%d",kk),Form("hQCh%d",kk),4000,0.,8000.);
    fHistoList->Add(new TH1D(Form("hQCh%d",kk),Form("hQCh%d",kk),4000,0.,8000.));

  }

}

ECALHisto::~ECALHisto()
{}
