#include "TargetOnlineMonitor.hh"

#include "HistoManager.hh"
#include "TargetHisto.hh"

TargetOnlineMonitor::TargetOnlineMonitor() : VOnlineMonitor("Target") {
  
  // Get pointer to Target histogram handler
  TargetHisto* targH = (TargetHisto*)HistoManager::GetInstance()->Histo("Target");

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
  targH->Get1DHisto("TargQTot")->Draw();

  TCanvas* position = AddCanvasTab("Position");
  position->Divide(1,1);
  position->cd(1);
  targH->Get1DHisto("TargPos")->Draw();

  TCanvas* chTotCharge = AddCanvasTab("Channel Charge");
  chTotCharge->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chTotCharge->cd(iCh+1);
    targH->Get1DHisto(Form("TargQCh%d",iCh))->Draw();
  }

  TCanvas* chPed = AddCanvasTab("Channel Pedestals");
  chPed->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chPed->cd(iCh+1);
    targH->Get1DHisto(Form("TargPedCh%d",iCh))->Draw();
  }
  
  TCanvas* chSig = AddCanvasTab("Channel Signal");
  chSig->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chSig->cd(iCh+1);
    targH->Get1DHisto(Form("TargSigCh%d",iCh))->Draw();
  }

  TCanvas* chRaw = AddCanvasTab("Channel Raw Counts");
  chRaw->Divide(3,3);
  for (Int_t iCh=0; iCh<9; iCh++){
    chRaw->cd(iCh+1);
    targH->Get1DHisto(Form("TargRawCh%d",iCh))->Draw();
  }

  VOnlineMonitor::CompleteTab();

}

TargetOnlineMonitor::~TargetOnlineMonitor()
{}
