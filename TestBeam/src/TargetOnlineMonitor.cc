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

  int chposx1[9] = { 22,23,24,25,26,-1,27,28,29 };
  int chposx2[9] = { 30,31,-1,14,15,-1,-1,-1,-1 };
  int chposy1[9] = { 16,17,-1,-1,20,-1, 1, 2, 3 };
  int chposy2[9] = { -1, 5,-1, 7,-1, 9,-1,10,-1 };

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

  TCanvas* chChrgx1 = AddCanvasTab("Charge X 1-9");
  chChrgx1->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
      if (chposx1[pos-1]!=-1) {
	chChrgx1->cd(pos);
	targH->Get1DHisto(Form("TargQCh%d",chposx1[pos-1]))->Draw();
      }
  }

  TCanvas* chChrgx2 = AddCanvasTab("Charge X 10-18");
  chChrgx2->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
      if (chposx2[pos-1]!=-1) {
	chChrgx2->cd(pos);
	targH->Get1DHisto(Form("TargQCh%d",chposx2[pos-1]))->Draw();
      }
  }

  TCanvas* chChrgy1 = AddCanvasTab("Charge Y 1-9");
  chChrgy1->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
      if (chposy1[pos-1]!=-1) {
	chChrgy1->cd(pos);
	targH->Get1DHisto(Form("TargQCh%d",chposy1[pos-1]))->Draw();
      }
  }

  TCanvas* chChrgy2 = AddCanvasTab("Charge Y 10-18");
  chChrgy2->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
      if (chposy2[pos-1]!=-1) {
	chChrgy2->cd(pos);
	targH->Get1DHisto(Form("TargQCh%d",chposy2[pos-1]))->Draw();
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

  TCanvas* chSigx1 = AddCanvasTab("Signal X 1-9");
  chSigx1->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
      if (chposx1[pos-1]!=-1) {
	chSigx1->cd(pos);
	targH->Get1DHisto(Form("TargSigCh%d",chposx1[pos-1]))->Draw();
      }
  }

  TCanvas* chSigx2 = AddCanvasTab("Signal X 10-18");
  chSigx2->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
      if (chposx2[pos-1]!=-1) {
	chSigx2->cd(pos);
	targH->Get1DHisto(Form("TargSigCh%d",chposx2[pos-1]))->Draw();
      }
  }

  TCanvas* chSigy1 = AddCanvasTab("Signal Y 1-9");
  chSigy1->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
      if (chposy1[pos-1]!=-1) {
	chSigy1->cd(pos);
	targH->Get1DHisto(Form("TargSigCh%d",chposy1[pos-1]))->Draw();
      }
  }

  TCanvas* chSigy2 = AddCanvasTab("Signal Y 10-18");
  chSigy2->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
      if (chposy2[pos-1]!=-1) {
	chSigy2->cd(pos);
	targH->Get1DHisto(Form("TargSigCh%d",chposy2[pos-1]))->Draw();
      }
  }

  TCanvas* caltrg = AddCanvasTab("Triggers");
  caltrg->Divide(2,3);
  caltrg->cd(1);
  targH->Get1DHisto("TargTR0")->Draw();
  caltrg->cd(2);
  targH->Get1DHisto("TargTR1")->Draw();
  caltrg->cd(3);
  targH->Get1DHisto("TargTR2")->Draw();
  caltrg->cd(4);
  targH->Get1DHisto("TargTR3")->Draw();
  caltrg->cd(5);
  targH->Get1DHisto("TargSigCh21")->Draw();
  caltrg->cd(6);
  targH->Get1DHisto("TargSigCh11")->Draw();


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
