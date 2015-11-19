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
  chTotCharge->Divide(4,3);
  for (Int_t pos=1; pos<=11; pos++){
    Int_t iCh=0;
    if (pos<=5) iCh=pos+15;
    if (pos>=6 && pos<=10) iCh=pos-6;
    if (pos==11) iCh=21;
    chTotCharge->cd(pos);
    targH->Get1DHisto(Form("TargQCh%d",iCh))->Draw();
  }

  TCanvas* chPed = AddCanvasTab("Channel Pedestals");
  chPed->Divide(4,3);
  for (Int_t pos=1; pos<=11; pos++){
    Int_t iCh=0;
    if (pos<=5) iCh=pos+15;
    if (pos>=6 && pos<=10) iCh=pos-6;
    if (pos==11) iCh=21;
    chPed->cd(pos);
    targH->Get1DHisto(Form("TargPedCh%d",iCh))->Draw();
  }
  
  TCanvas* chSig = AddCanvasTab("Channel Signal");
  chSig->Divide(4,3);
  for (Int_t pos=1; pos<=11; pos++){
    Int_t iCh=0;
    if (pos<=5) iCh=pos+15;
    if (pos>=6 && pos<=10) iCh=pos-6;
    if (pos==11) iCh=21;
    chSig->cd(pos);
    targH->Get1DHisto(Form("TargSigCh%d",iCh))->Draw();
  }
  chSig->cd(12);
  targH->Get1DHisto("TargTR2")->Draw();

  TCanvas* chRaw = AddCanvasTab("Channel Raw Counts");
  chRaw->Divide(4,3);
  for (Int_t pos=1; pos<=11; pos++){
    Int_t iCh=0;
    if (pos<=5) iCh=pos+15;
    if (pos>=6 && pos<=10) iCh=pos-6;
    if (pos==11) iCh=21;
    chRaw->cd(pos);
    targH->Get1DHisto(Form("TargRawCh%d",iCh))->Draw();
  }

  VOnlineMonitor::CompleteTab();

}

TargetOnlineMonitor::~TargetOnlineMonitor()
{}
