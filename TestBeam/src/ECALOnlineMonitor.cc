#include "ECALOnlineMonitor.hh"

#include "HistoManager.hh"
#include "ECALHisto.hh"

ECALOnlineMonitor::ECALOnlineMonitor() : VOnlineMonitor("ECAL") {
  
  // Get pointer to ECAL histogram handler
  ECALHisto* ecalH = (ECALHisto*)HistoManager::GetInstance()->Histo("ECAL");

  //int chpos[12] = { 0,1,2,3,4,5,6,7,8,9,10,-1 };
  int chpos1[16] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

  //TCanvas* totalCharge = AddCanvasTab("Tot Chrg");
  //totalCharge->Divide(1,1);
  //totalCharge->cd(1);
  //ecalH->Get1DHisto("ECALQTot")->Draw();

  //TCanvas* position = AddCanvasTab("Position");
  //position->Divide(1,1);
  //position->cd(1);
  //ecalH->Get1DHisto("ECALPos")->Draw();

  //TCanvas* chChrg1 = AddCanvasTab("Ch Charge");
  //chChrg1->Divide(4,4);
  //for (Int_t pos=1; pos<=16; pos++){
  //  if (chpos1[pos-1]!=-1) {
  //    chChrg1->cd(pos);
  //    ecalH->Get1DHisto(Form("ECALQCh%d",chpos1[pos-1]))->Draw();
  //  }
  //}
  TCanvas* chChrg1 = AddCanvasTab("Ch Charge");
  chChrg1->Divide(1,2);
  chChrg1->cd(1);
  ecalH->Get1DHisto("ECALQCh0")->Draw();
  chChrg1->cd(2);
  ecalH->Get1DHisto("ECALQCh1")->Draw();

  //TCanvas* chPed1 = AddCanvasTab("Ch Pedestal");
  //chPed1->Divide(4,4);
  //for (Int_t pos=1; pos<=16; pos++){
  //  if (chpos1[pos-1]!=-1) {
  //    chPed1->cd(pos);
  //    ecalH->Get1DHisto(Form("ECALPedCh%d",chpos1[pos-1]))->Draw();
  //  }
  //}
  TCanvas* chPed1 = AddCanvasTab("Ch Pedestal");
  chPed1->Divide(1,2);
  chPed1->cd(1);
  ecalH->Get1DHisto("ECALPedCh0")->Draw();
  chPed1->cd(2);
  ecalH->Get1DHisto("ECALPedCh1")->Draw();

  //TCanvas* chSig1 = AddCanvasTab("Ch Signal");
  //chSig1->Divide(4,4);
  //for (Int_t pos=1; pos<=16; pos++){
  //  if (chpos1[pos-1]!=-1) {
  //    chSig1->cd(pos);
  //    ecalH->Get1DHisto(Form("ECALSigCh%d",chpos1[pos-1]))->Draw();
  //  }
  //}
  TCanvas* chSig1 = AddCanvasTab("Ch Signal");
  chSig1->Divide(1,2);
  chSig1->cd(1);
  ecalH->Get1DHisto("ECALSigCh0")->Draw();
  chSig1->cd(2);
  ecalH->Get1DHisto("ECALSigCh1")->Draw();

  //TCanvas* chRaw1 = AddCanvasTab("Ch Raw");
  //chRaw1->Divide(4,4);
  //for (Int_t pos=1; pos<=16; pos++){
  //  if (chpos1[pos-1]!=-1) {
  //    chRaw1->cd(pos);
  //    ecalH->Get1DHisto(Form("ECALRawCh%d",chpos1[pos-1]))->Draw();
  //  }
  //}
  TCanvas* chRaw1 = AddCanvasTab("Ch Raw");
  chRaw1->Divide(1,3);
  chRaw1->cd(1);
  ecalH->Get1DHisto("ECALRawCh0")->Draw();
  chRaw1->cd(2);
  ecalH->Get1DHisto("ECALRawCh1")->Draw();
  chRaw1->cd(3);
  ecalH->Get1DHisto("ECALTR00")->Draw();

  //TCanvas* caltrg = AddCanvasTab("Triggers");
  //caltrg->Divide(1,2);
  //caltrg->cd(1);
  //ecalH->Get1DHisto("ECALTR00")->Draw();
  //caltrg->cd(2);
  //ecalH->Get1DHisto("ECALTR01")->Draw();
  //caltrg->cd(3);
  //ecalH->Get1DHisto("ECALTR10")->Draw();
  //caltrg->cd(4);
  //ecalH->Get1DHisto("ECALTR11")->Draw();

  VOnlineMonitor::CompleteTab();

}

ECALOnlineMonitor::~ECALOnlineMonitor()
{}
