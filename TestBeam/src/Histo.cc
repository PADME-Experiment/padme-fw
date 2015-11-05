#define Histo_cxx
#include "Histo.hh"
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TList.h>
#include <iostream>
#include "cstdlib"

using namespace std;
Histo* Histo::fInstance = 0;
Histo* Histo::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new Histo(); }
  return fInstance;
}

void Histo::InitHisto()
{
  //  NCalo=9;
  //  NTrigCalo=2;
  printf("Initializing  Histograms\n");
  hListCal    = new TList();      // list of Ecal histograms to store
  //Histogram arrays for different data Type 
  hPedCalo = new TH1D*[32];
  hPedTarg = new TH1D*[32];
  hQCh     = new TH1D*[32];

  hCaloQTot = new TH1D("hQTotCalo","hQTotCalo",4000,0.,8000.);
  hTargQTot = new TH1D("hQTotTarg","hQTotTarg",4000,0.,8000.);

  for(int kk=0;kk<9;kk++){
    //  hInvThEi[kk]    = new TH2D(Form("hInvThEi%d",kk),Form("hInvThEi%d",kk),300,0.,600.,60,0.,12.);
    hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),400,3800.,4200.);
    hPedTarg[kk] = new TH1D(Form("hPedTarg%d",kk),Form("hPedTarg%d",kk),400,3800.,4200.);
    hQCh[kk] = new TH1D(Form("hQCh%d",kk),Form("hQCh%d",kk),4000,0.,8000.);

    hListCal->Add(hQCh[kk]);
    hListCal->Add(hPedCalo[kk]);
    hListCal->Add(hPedTarg[kk]);
  }
  hListCal->Add(hCaloQTot);
  hListCal->Add(hTargQTot);
}

void Histo::InitMonitor(){
//  fMainWindow = MainWindow;
//  if(fMainWindow) fMainWindow->StartEmbedding(1,-1);
//  fMainTabFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
//  fMainTabFrame->SetName(Name);
//  fMainTabFrame->SetWindowName(Name);
//  fMainTab = new TGTab(fMainTabFrame,300,300);
//  fMainTab->SetName(Name);
//  //  fMainTab->Draw();
}

void Histo::WriteHisto()
{
  printf("Writing  general Histograms\n");
  //  hListCal->Write();
  //  hListComp->Write();
}

void Histo::DrawCalo(Int_t NTrig, Int_t NCalo)
{
  printf("Drawing Calorimeter histograms\n");
  TCanvas * c1= new TCanvas("CaloMon","CaloMon",800,600);
  c1->Divide(3,3); 
  for(int kk=0;kk<NCalo;kk++){
    c1->cd(kk+1);
    if(gCaloRaw[kk]!=0) gCaloRaw[kk]->Draw("APL");
    c1->Update();
  }

  TCanvas * c3= new TCanvas("CaloRecMon","CaloRecMon",800,600);
  c3->Divide(3,3); 
  for(int kk=0;kk<NCalo;kk++){
    c3->cd(kk+1);
    if(gCaloReco[kk]!=0) gCaloReco[kk]->Draw("APL");
    c3->Update();
  }

  TCanvas * c4= new TCanvas("CaloPed","CaloPed",800,600);
  c4->Divide(3,3); 
  for(int kk=0;kk<NCalo;kk++){
    c4->cd(kk+1);
    if(hPedCalo!=0) hPedCalo[kk]->Draw();
    c4->Update();
  }

  TCanvas * c2=new TCanvas("TrigMon","TrigMon",800,600);
  c2->Divide(1,2); 
  for(int kk=0;kk<NTrig;kk++){
    c2->cd(kk+1);
    gCaloTrigRaw[kk]->Draw("APL");
    c2->Update();
  }
}

void Histo::DrawTarg(Int_t Ntrig, Int_t NCal)
{

}

Int_t Histo::Fill1D(const char* name,Float_t var)
{
  printf("Name %s e Var %f\n",name,var);
  TH1D* histo=(TH1D*)hListCal->FindObject(name);
  histo->Fill(var);
  return 0;
}

Int_t Histo::Fill2D(const char* name,Float_t var,Float_t var1)
{
  TH2D* histo=(TH2D*)hListCal->FindObject(name);
  histo->Fill(var,var1);
  return 0;
}

Int_t Histo::FillGraph(const char* name,Int_t Nch, Int_t Nsam,Float_t *var,Float_t *var1)
{
  char gName[10];
  sprintf(gName,"Ch%d",Nch);
  if (strcmp(name,"Calo")==0) {
    if(Nch<32){
      gCaloRaw[Nch] = new TGraphErrors(Nsam,var,var1);
      gCaloRaw[Nch]->SetTitle(gName);
    }else{
      printf("Index out of range %d",Nch);
      return -1;
    }
  }

  if (strcmp(name,"CaloReco")==0) {
    if(Nch<32){
      gCaloReco[Nch] = new TGraphErrors(Nsam,var,var1);
      gCaloReco[Nch]->SetTitle(gName);
    }else{
      printf("Index out of range %d",Nch);
      return -1;
    }
  }


  char gTrgName[10];
  sprintf(gTrgName,"Trg%d",Nch);
  if (strcmp(name,"CaloTrig")==0) {
    if(Nch<4){
      gCaloTrigRaw[Nch] = new TGraphErrors(Nsam,var,var1);
      gCaloTrigRaw[Nch]->SetTitle(gTrgName);
    }else{
      printf("Index out of range %d",Nch);
      return -1;
    }
  }
  return 0;
}



//Costruttore
//#ifdef Histo_cxx
Histo::Histo()
{
  //  printf("Calling Init histograms ");
  InitHisto();
}
//
Histo::~Histo()
{
//  printf("Closing Init histograms ");
}

