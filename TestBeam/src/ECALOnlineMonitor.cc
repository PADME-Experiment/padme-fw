#include "ECALOnlineMonitor.hh"

#include "HistoManager.hh"
#include "ECALHisto.hh"

ECALOnlineMonitor::ECALOnlineMonitor() : VOnlineMonitor("ECAL") {
  
  // Get pointer to ECAL histogram handler
  ECALHisto* ecalH = (ECALHisto*)HistoManager::GetInstance()->Histo("ECAL");

  //int chpos[12] = { 0,1,2,3,4,5,6,7,8,9,10,-1 };
  int chpos[9] = { 0,1,2,3,4,5,6,7,8 };

  TCanvas* totalCharge = AddCanvasTab("Total Charge");
  totalCharge->Divide(1,1);
  totalCharge->cd(1);
  ecalH->Get1DHisto("ECALQTot")->Draw();

  TCanvas* position = AddCanvasTab("Position");
  position->Divide(1,1);
  position->cd(1);
  ecalH->Get1DHisto("ECALPos")->Draw();

  TCanvas* chChrg = AddCanvasTab("Channel Charge");
  //chChrg->Divide(3,4);
  //for (Int_t pos=1; pos<=12; pos++){
  chChrg->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos[pos-1]!=-1) {
      chChrg->cd(pos);
      ecalH->Get1DHisto(Form("ECALQCh%d",chpos[pos-1]))->Draw();
    }
  }

  TCanvas* chPed = AddCanvasTab("Channel Pedestals");
  //chPed->Divide(3,4);
  //for (Int_t pos=1; pos<=12; pos++){
  chPed->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos[pos-1]!=-1) {
      chPed->cd(pos);
      ecalH->Get1DHisto(Form("ECALPedCh%d",chpos[pos-1]))->Draw();
    }
  }

  TCanvas* chSig = AddCanvasTab("Channel Signal");
  //chSig->Divide(3,4);
  //for (Int_t pos=1; pos<=12; pos++){
  chSig->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos[pos-1]!=-1) {
      chSig->cd(pos);
      ecalH->Get1DHisto(Form("ECALSigCh%d",chpos[pos-1]))->Draw();
    }
  }

  TCanvas* chRaw = AddCanvasTab("Channel Raw Counts");
  //chRaw->Divide(3,4);
  //for (Int_t pos=1; pos<=12; pos++){
  chRaw->Divide(3,3);
  for (Int_t pos=1; pos<=9; pos++){
    if (chpos[pos-1]!=-1) {
      chRaw->cd(pos);
      ecalH->Get1DHisto(Form("ECALRawCh%d",chpos[pos-1]))->Draw();
    }
  }

  VOnlineMonitor::CompleteTab();

}

ECALOnlineMonitor::~ECALOnlineMonitor()
{}
