#include "ECALOnlineMonitor.hh"

#include "HistoManager.hh"
#include "ECALHisto.hh"

ECALOnlineMonitor::ECALOnlineMonitor() : VOnlineMonitor("ECAL") {
  
  // Get pointer to ECAL histogram handler
  ECALHisto* ecalH = (ECALHisto*)HistoManager::GetInstance()->Histo("ECAL");

  /*
  TCanvas* adcReadout = AddCanvasTab("ADC Readout Map");
  adcReadout->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    adcReadout->cd(iCh);
    // Draw histogram
  }
  */

  TCanvas* totalCharge = AddCanvasTab("Total Charge");
  totalCharge->cd(0);
  ecalH->Get1DHisto("hQTotCalo")->Draw();

  TCanvas* position = AddCanvasTab("Position");
  position->cd(0);
  ecalH->Get1DHisto("hPosCalo")->Draw();

  TCanvas* chTotCharge = AddCanvasTab("Channel Charge");
  chTotCharge->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chTotCharge->cd(iCh+1);
    ecalH->Get1DHisto(Form("hQCh%d",iCh))->Draw();
  }

  TCanvas* chPed = AddCanvasTab("Channel Pedestals");
  chPed->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chPed->cd(iCh+1);
    ecalH->Get1DHisto(Form("hPedCalo%d",iCh))->Draw();
  }
  
  VOnlineMonitor::CompleteTab();

}

ECALOnlineMonitor::~ECALOnlineMonitor()
{}
