#include "TargetOnlineMonitor.hh"

#include "HistoManager.hh"
#include "TargetHisto.hh"

TargetOnlineMonitor::TargetOnlineMonitor() : VOnlineMonitor("Target") {
  
  // Get pointer to Target histogram handler
  TargetHisto* ecalH = (TargetHisto*)HistoManager::GetInstance()->Histo("Target");

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
  ecalH->Get1DHisto("TargQTot")->Draw();

  TCanvas* position = AddCanvasTab("Position");
  position->cd(0);
  ecalH->Get1DHisto("TargPos")->Draw();

  TCanvas* chTotCharge = AddCanvasTab("Channel Charge");
  chTotCharge->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chTotCharge->cd(iCh+1);
    ecalH->Get1DHisto(Form("TargQCh%d",iCh))->Draw();
  }

  TCanvas* chPed = AddCanvasTab("Channel Pedestals");
  chPed->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chPed->cd(iCh+1);
    ecalH->Get1DHisto(Form("TargPed%d",iCh))->Draw();
  }
  
  VOnlineMonitor::CompleteTab();

}

TargetOnlineMonitor::~TargetOnlineMonitor()
{}
