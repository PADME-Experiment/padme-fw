#include "ECALOnlineMonitor.hh"

#include "HistoManager.hh"
#include "ECALHisto.hh"

ECALOnlineMonitor::ECALOnlineMonitor() : VOnlineMonitor("ECAL") {
  
  // Get pointer to ECAL histogram handler
  ECALHisto* ecalH = (ECALHisto*)HistoManager::GetInstance()->Histo("ECAL");

  //int chpos[12] = { 0,1,2,3,4,5,6,7,8,9,10,-1 };
  int chpos1[9] = {  0, 1, 2, 3, 4, 5, 6, 7, 8 };
  int chpos2[9] = { 16,17,18,19,20,21,22,23,24 };

  TCanvas* totalCharge = AddCanvasTab("Tot Chrg");
  totalCharge->Divide(1,2);
  totalCharge->cd(1);
  ecalH->Get1DHisto("ECALQTot1")->Draw();
  totalCharge->cd(2);
  ecalH->Get1DHisto("ECALQTot2")->Draw();

  TCanvas* position = AddCanvasTab("Position");
  position->Divide(1,2);
  position->cd(1);
  ecalH->Get1DHisto("ECALPos1")->Draw();
  position->cd(2);
  ecalH->Get1DHisto("ECALPos2")->Draw();

  TCanvas* chChrg1 = AddCanvasTab("PMT Ch Chrg");
  chChrg1->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos1[pos-1]!=-1) {
      chChrg1->cd(pos);
      ecalH->Get1DHisto(Form("ECALQCh%d",chpos1[pos-1]))->Draw();
    }
  }

  TCanvas* chChrg2 = AddCanvasTab("APD Ch Chrg");
  chChrg2->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos2[pos-1]!=-1) {
      chChrg2->cd(pos);
      ecalH->Get1DHisto(Form("ECALQCh%d",chpos2[pos-1]))->Draw();
    }
  }

  /*
  TCanvas* chPed = AddCanvasTab("Ch Ped");
  //chPed->Divide(3,4);
  //for (Int_t pos=1; pos<=12; pos++){
  chPed->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos[pos-1]!=-1) {
      chPed->cd(pos);
      ecalH->Get1DHisto(Form("ECALPedCh%d",chpos[pos-1]))->Draw();
    }
  }
  */

  TCanvas* chSig1 = AddCanvasTab("PMT Ch Sig");
  chSig1->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos1[pos-1]!=-1) {
      chSig1->cd(pos);
      ecalH->Get1DHisto(Form("ECALSigCh%d",chpos1[pos-1]))->Draw();
    }
  }

  TCanvas* chSig2 = AddCanvasTab("APD Ch Sig");
  chSig2->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos2[pos-1]!=-1) {
      chSig2->cd(pos);
      ecalH->Get1DHisto(Form("ECALSigCh%d",chpos2[pos-1]))->Draw();
    }
  }

  TCanvas* chRaw1 = AddCanvasTab("PMT Ch Raw");
  chRaw1->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos1[pos-1]!=-1) {
      chRaw1->cd(pos);
      ecalH->Get1DHisto(Form("ECALRawCh%d",chpos1[pos-1]))->Draw();
    }
  }

  TCanvas* chRaw2 = AddCanvasTab("APD Ch Raw");
  chRaw2->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos2[pos-1]!=-1) {
      chRaw2->cd(pos);
      ecalH->Get1DHisto(Form("ECALRawCh%d",chpos2[pos-1]))->Draw();
    }
  }

  TCanvas* caltrg = AddCanvasTab("Triggers");
  caltrg->Divide(2,2);
  caltrg->cd(1);
  //ecalH->Get1DHisto("ECALTR00")->Draw();
  ecalH->Get1DHisto("ECALRawCh25")->Draw();
  caltrg->cd(2);
  ecalH->Get1DHisto("ECALTR01")->Draw();
  caltrg->cd(3);
  ecalH->Get1DHisto("ECALTR10")->Draw();
  caltrg->cd(4);
  ecalH->Get1DHisto("ECALTR11")->Draw();

  VOnlineMonitor::CompleteTab();

}

ECALOnlineMonitor::~ECALOnlineMonitor()
{}
