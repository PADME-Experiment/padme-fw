#include "TargetHisto.hh"

TargetHisto::TargetHisto() : VHisto("Target") {

  // Create list to hold all histograms needed for Target
  fHistoList = new TList();

  fHistoList->Add(new TH1D("TargQTot","Target - Total Charge",400,0.,100.));
  fHistoList->Add(new TH2D("TargPos","Target - Position",120,-6.,6.,120,-6.,6.));

  int  str[32] = {6,7,8,9,10,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,0,1,2,3,4,5,7,8,0,0,0};
  char coo[33] = "yyyyy-----------yyyyy-xxxxxxx---";

  //for(int kk=0;kk<9;kk++){
  for(int kk=0;kk<32;kk++){

    /*
    if ( (kk<=4) || (kk>=16 && kk<=20) ) {
      int str=0;
      if (kk<=4) str=kk+6;
      if (kk>=16 && kk<=20) str=kk-15;
      fHistoList->Add(new TH1D(Form("TargPedCh%d",kk),Form("Target - Ch %d Str %d Pedestals",kk,str),400,0.,40.));
      fHistoList->Add(new TH1D(Form("TargQCh%d",kk),Form("Target - Ch %d Str %d Charge",kk,str),400,0.,100.));
      fHistoList->Add(new TH1D(Form("TargRawCh%d",kk),Form("Target - Ch %d Str %d Raw Counts",kk,str),1024,0.,1024.));
      fHistoList->Add(new TH1D(Form("TargSigCh%d",kk),Form("Target - Ch %d Str %d Signal",kk,str),1024,0.,1024.));
    }
    */
    
    if (kk==21) {
      fHistoList->Add(new TH1D(Form("TargPedCh%d",kk),Form("Calorimeter - Ch %d Pedestals",21),400,0.,40.));
      fHistoList->Add(new TH1D(Form("TargQCh%d",kk),  Form("Calorimeter - Ch %d Charge",21),400,0.,100.));
      fHistoList->Add(new TH1D(Form("TargRawCh%d",kk),Form("Calorimeter - Ch %d Raw Counts",21),1024,0.,1024.));
      fHistoList->Add(new TH1D(Form("TargSigCh%d",kk),Form("Calorimeter - Ch %d Signal",21),1024,0.,1024.));
    } else {
      fHistoList->Add(new TH1D(Form("TargPedCh%d",kk),Form("Target - Ch %d Str %c%d Pedestals",kk,coo[kk],str[kk]),400,0.,40.));
      fHistoList->Add(new TH1D(Form("TargQCh%d",kk),  Form("Target - Ch %d Str %c%d Charge",kk,coo[kk],str[kk]),400,0.,100.));
      fHistoList->Add(new TH1D(Form("TargRawCh%d",kk),Form("Target - Ch %d Str %c%d Raw Counts",kk,coo[kk],str[kk]),1024,0.,1024.));
      fHistoList->Add(new TH1D(Form("TargSigCh%d",kk),Form("Target - Ch %d Str %c%d Signal",kk,coo[kk],str[kk]),1024,0.,1024.));
    }
  }

  fHistoList->Add(new TH1D("TargTR0","Trigger 0 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("TargTR1","Trigger 1 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("TargTR2","Trigger 2 Signal",1024,0.,1024.));
  fHistoList->Add(new TH1D("TargTR3","Trigger 3 Signal",1024,0.,1024.));

}

TargetHisto::~TargetHisto()
{}
