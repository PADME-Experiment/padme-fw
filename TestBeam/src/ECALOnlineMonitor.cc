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
  totalCharge->Divide(1,1);
  totalCharge->cd(1);
  ecalH->Get1DHisto("ECALQTot")->Draw();

  TCanvas* position = AddCanvasTab("Position");
  position->Divide(1,1);
  position->cd(1);
  ecalH->Get1DHisto("ECALPos")->Draw();

  TCanvas* chTotCharge = AddCanvasTab("Channel Charge");
  chTotCharge->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chTotCharge->cd(iCh+1);
    ecalH->Get1DHisto(Form("ECALQCh%d",iCh))->Draw();
  }

  TCanvas* chPed = AddCanvasTab("Channel Pedestals");
  chPed->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chPed->cd(iCh+1);
    ecalH->Get1DHisto(Form("ECALPedCh%d",iCh))->Draw();
  }

  TCanvas* chSig = AddCanvasTab("Channel Signal");
  chSig->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chSig->cd(iCh+1);
    ecalH->Get1DHisto(Form("ECALSigCh%d",iCh))->Draw();
  }

  TCanvas* chRaw = AddCanvasTab("Channel Raw Counts");
  chRaw->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chRaw->cd(iCh+1);
    ecalH->Get1DHisto(Form("ECALRawCh%d",iCh))->Draw();
  }

  VOnlineMonitor::CompleteTab();

}

ECALOnlineMonitor::~ECALOnlineMonitor()
{}
