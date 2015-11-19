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

  int chposx[12] = { 22,23,24,25,26,27,28,-1,-1,-1,-1,-1 };
  int chposy[12] = { 16,17,18,19,20, 0, 1, 2, 3, 4,21,-1 };

  TCanvas* totalCharge = AddCanvasTab("Total Charge");
  totalCharge->Divide(1,1);
  totalCharge->cd(1);
  targH->Get1DHisto("TargQTot")->Draw();

  /*
  TCanvas* position = AddCanvasTab("Position");
  position->Divide(1,1);
  position->cd(1);
  targH->Get1DHisto("TargPos")->Draw();
  */

  TCanvas* chChrgx = AddCanvasTab("Channel Charge X");
  chChrgx->Divide(4,3);
  for (Int_t pos=1; pos<=12; pos++){
      if (chposx[pos-1]!=-1) {
	chChrgx->cd(pos);
	targH->Get1DHisto(Form("TargQCh%d",chposx[pos-1]))->Draw();
      }
  }

  TCanvas* chChrgy = AddCanvasTab("Channel Charge Y");
  chChrgy->Divide(4,3);
  for (Int_t pos=1; pos<=12; pos++){
      if (chposy[pos-1]!=-1) {
	chChrgy->cd(pos);
	targH->Get1DHisto(Form("TargQCh%d",chposy[pos-1]))->Draw();
      }
  }

  /*
  TCanvas* chPed = AddCanvasTab("Channel Pedestals Y");
  chPed->Divide(4,3);
  for (Int_t pos=1; pos<=12; pos++){
      if (chposy[pos-1]!=-1) {
	chPed->cd(pos);
	targH->Get1DHisto(Form("TargPedCh%d",chposy[pos-1]))->Draw();
      }
  }
  */

  TCanvas* chSigx = AddCanvasTab("Channel Signal X");
  chSigx->Divide(4,3);
  for (Int_t pos=1; pos<=12; pos++){
      if (chposx[pos-1]!=-1) {
	chSigx->cd(pos);
	targH->Get1DHisto(Form("TargSigCh%d",chposx[pos-1]))->Draw();
      }
  }

  TCanvas* chSigy = AddCanvasTab("Channel Signal Y");
  chSigy->Divide(4,3);
  for (Int_t pos=1; pos<=12; pos++){
      if (chposy[pos-1]!=-1) {
	chSigy->cd(pos);
	targH->Get1DHisto(Form("TargSigCh%d",chposy[pos-1]))->Draw();
      }
  }
  chSigy->cd(12);
  targH->Get1DHisto("TargTR2")->Draw();

  /*
  TCanvas* chRaw = AddCanvasTab("Channel Raw Counts Y");
  chRaw->Divide(4,3);
  for (Int_t pos=1; pos<=12; pos++){
      if (chposy[pos-1]!=-1) {
	chRaw->cd(pos);
	targH->Get1DHisto(Form("TargRawCh%d",chposy[pos-1]))->Draw();
      }
  }
  */

  VOnlineMonitor::CompleteTab();

}

TargetOnlineMonitor::~TargetOnlineMonitor()
{}
