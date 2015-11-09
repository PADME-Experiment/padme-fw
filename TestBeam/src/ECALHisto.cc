#include "ECALHisto.hh"

ECALHisto::ECALHisto() : VHisto("ECAL") {

  // Create list to hold all histograms needed for ECAL
  fHistoList = new TList();

  fHistoList->Add(new TH1D("ECALQTot","ECAL - Total Charge",4000,0.,8000.));
  fHistoList->Add(new TH2D("ECALPos","ECAL - Position",120,-6.,6.,120,-6.,6.));

  for(int kk=0;kk<9;kk++){
    fHistoList->Add(new TH2D(Form("ECALRawCountsCh%d",kk),Form("ECAL - Raw Ch %d Counts",kk),1024.,0.,1024,4100,0.,4100.)); //molto poco efficiente rispetto al Graph

    fHistoList->Add(new TH2D(Form("ECALSignalCh%d",kk),Form("ECAL - Signal Ch %d",kk),1024.,0.,1024,1100,-1,0.1)); //molto poco efficiente rispetto al Graph

    fHistoList->Add(new TH1D(Form("ECALPed%d",kk),Form("ECAL - Ch %d Pedestals",kk),400,3800.,4200.));
    fHistoList->Add(new TH1D(Form("ECALQCh%d",kk),Form("ECAL - Ch %d Charge",kk),4000,0.,8000.));

  }

}

ECALHisto::~ECALHisto()
{}
