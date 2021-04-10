#include <iostream>
#include <cmath>

#include "Rtypes.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include <fstream>
#include "TString.h"
#include "Riostream.h"
#include "TMath.h"


void timeCalibration()
{
    
   //TFile *f1 = TFile::Open(filename);
   TFile *f1 = TFile::Open("TimeCal.root");
   
    
   TCanvas *c0 = new TCanvas("c1", "c1", 600,800);
   

   TF1 *gaus = new TF1("gaus","gaus", 0, 100);
   gaus->SetLineColor(2);


   TCanvas *c1 = new TCanvas("c1", "c1", 600,800);
   c1->Divide(2,3);

   TCanvas *c2 = new TCanvas("c2", "c2", 600,800);
   c2->Divide(2,3);


   TCanvas *c3 = new TCanvas("c3", "c3", 600,800);
   c3->Divide(2,5);

   TH2D* DiffTimeHitsPVetoSAC22VsChIdPVeto = new TH2D("DiffTimeHitsPVetoSAC22VsChIdPVeto", "DiffTimeHitsPVetoSAC22VsChIdPVeto", 90, 0, 90, 100, -5, 5);
   TH2D* DiffTimeClusterVsChIdPVeto = new TH2D("DiffTimeClusterVsChIdPVeto", "DiffTimeClusterVsChIdPVeto", 90, 0, 90, 100, -5, 5);

   TH2D* DiffTimeHitsEVetoSAC22VsChIdEVeto = new TH2D("DiffTimeHitsEVetoSAC22VsChIdEVeto", "DiffTimeHitsEVetoSAC22VsChIdEVeto", 90, 0, 90, 100, -5, 5);
   TH2D* DiffTimeHitsHEPVetoSAC22VsChIdHEPVeto = new TH2D("DiffTimeHitsHEPVetoSAC22VsChIdHEPVeto", "DiffTimeHitsHEPVetoSAC22VsChIdHEPVeto", 90, 0, 90, 100, -5, 5);
    

   Double_t mean;
   Double_t meanAllHits;
   Double_t sumMean=0;
   Double_t MeanVec[90];
   Double_t FixedAmpli = 65.;
   Double_t Mean;
   Double_t ESAC, EHEPVeto;
   Double_t Ebeam = 490;
   Double_t Zvalue, back;

   TFile *RootFile = new TFile("Time.root","RECREATE");
   
   char fname[30];

   float ich, thit;
   Double_t tPV[90], tEV[90], tHEPV[32];
   char name[10];

   ifstream t0filePV;
   t0filePV.open ("TimePVeto0.txt");
   for(int n=0; n<90; n++)

   {
   t0filePV>>name>>thit;
   std::cout<<"  name  "<<name<<"    time hit   "<<thit<<std::endl;
   tPV[n] = thit;
   }
   t0filePV.close();

   /*ifstream t0fileEV;
   t0fileEV.open ("TimeEVeto0.txt");
   for(int n=0; n<90; n++)

   {
   t0fileEV>>name>>thit;
   //std::cout<<"  name  "<<name<<"    time hit   "<<thit<<std::endl;
   tEV[n] = thit;
   }
   t0fileEV.close();*/

   ifstream t0fileHEPV;
   t0fileHEPV.open ("TimeHEPVeto0.txt");
   for(int n=0; n<32; n++)

   {
   t0fileHEPV>>name>>thit;
   std::cout<<"  name  "<<name<<"    time hit   "<<thit<<std::endl;
   tHEPV[n] = thit;
   }
   t0fileHEPV.close();

   Double_t tECal;

   ifstream t0fileECal;
   t0fileECal.open ("TimeECal0.txt");
   t0fileECal>>name>>thit;
   tECal=thit;

   Double_t tEVeto;

   ifstream t0fileEVeto;
   t0fileEVeto.open ("TimeEVeto0.txt");
   t0fileEVeto>>name>>thit;
   tEVeto=thit;
/*for(int k=0; k<90; k++)
  
  { 
   sprintf(fname,"DiffSAC22_PVeto%d", k);
   TH1D* tmp = new TH1D(fname, fname, 500, 0, 1000);
   sprintf(fname,"DiffClSACVsPVeto_ChId%d", k);
   TH1D* tmp = new TH1D(fname, fname, 500, 0, 1000);
   sprintf(fname,"DiffClSAC22_EVeto%d", k);
   TH1D* tmp = new TH1D(fname, fname, 500, 0, 1000);
   sprintf(fname,"DiffSAC22_EVeto%d", k);
   TH1D* tmp = new TH1D(fname, fname, 500, 0, 1000);
  }*/

for(int k=0; k<32; k++)
  
  { 
   sprintf(fname,"DiffSAC22_HEPVeto%d", k);
   TH1D* tmp = new TH1D(fname, fname, 500, 0, 1000);
   sprintf(fname,"DiffClSAC22_HEPVeto%d", k);
   TH1D* tmp = new TH1D(fname, fname, 500, 0, 1000);
   
  }

   
   ofstream file;
   file.open ("TimePVeto.txt");
   ofstream filecfg;
   filecfg.open ("Time_config.txt"); //file after time config addiction


   int binmin=0;
   int chID=0;
   double deltat;
   
   file<<"Hit time difference\n";
   filecfg<<"Positron Veto\n";
 
  for(int i=0; i<15; i++)
  
  {
   for(int j=0;j<6;j++)
    {
     c1->cd(j+1);
     
     sprintf(fname,"DiffSAC22_PVeto%d", chID);
     TH1D *tmp = (TH1D*)f1->Get(fname);
     int binmax = tmp->GetMaximumBin(); 
     double x = tmp->GetXaxis()->GetBinCenter(binmax);
     
     //tmp->Fit(gaus, "","", x-1, x+1);
     tmp->Fit(gaus, "","", x-2, x+2);
     deltat = gaus->GetParameter(1);
    
     file<<"t0_"<<chID<<"\t"<<deltat<<"\n";
    
     
     //file<<chID<<"\t"<<deltat<<"\n";
     filecfg<<"t0_"<<chID<<" "<<deltat+tPV[chID]<<"\n";
     DiffTimeHitsPVetoSAC22VsChIdPVeto->Fill(chID, deltat);

     tmp->Write("");
     
     chID++;
    }
}
    DiffTimeHitsPVetoSAC22VsChIdPVeto->Write("");

    chID=0;


    //file2.close();
    ofstream file3;
    file3.open ("TimeHEPVeto.txt");
    chID=0;
    filecfg<<"HEP Veto\n";


    for(i=0; i<8; i++)
  
  {
   for(j=0;j<4;j++)
    {
     c1->cd(j+1);
     
     
     sprintf(fname,"DiffSAC22_HEPVeto%d", chID);
     TH1D *tmp = (TH1D*)f1->Get(fname);
     binmax = tmp->GetMaximumBin(); 
     x = tmp->GetXaxis()->GetBinCenter(binmax);
     
     tmp->Fit(gaus, "","", x-1, x+1);
     deltat = gaus->GetParameter(1);
    
     file3<<"t0_"<<chID<<"\t"<<deltat<<"\n";
     filecfg<<"t0_"<<chID<<" "<<deltat+tHEPV[chID]<<"\n";
     DiffTimeHitsHEPVetoSAC22VsChIdHEPVeto->Fill(chID, deltat);
    
     tmp->Write("");
    
     chID++;
    }
}
    DiffTimeHitsHEPVetoSAC22VsChIdHEPVeto->Write("");

    ofstream file4;
    file4.open ("TimeECal.txt");
   
    file4<<"ECal\n";
    filecfg<<"ECal\n";
    sprintf(fname,"DtimeECalVsPVeto41_Hits");
    TH1D *tmp = (TH1D*)f1->Get(fname);
    binmax = tmp->GetMaximumBin(); 
    x = tmp->GetXaxis()->GetBinCenter(binmax);
     
    tmp->Fit(gaus, "","", x-2, x+2);
    deltat = gaus->GetParameter(1);
    file4<<"Common t0  "<<deltat<<"\n";
    filecfg<<"Common t0\t"<<deltat+tECal<<"\n";
    tmp->Write("");

    ofstream file5;
    file5.open ("TimeEVeto.txt");
   
    file5<<"EVeto\n";
    filecfg<<"EVeto\n";
    sprintf(fname,"DtimeEVetoVsSAC22_Hits");
    TH1D *tmp = (TH1D*)f1->Get(fname);
    binmax = tmp->GetMaximumBin(); 
    x = tmp->GetXaxis()->GetBinCenter(binmax);
     
    tmp->Fit(gaus, "","", x-1, x+1);
    deltat = gaus->GetParameter(1);
    file5<<"Common t0  "<<deltat<<"\n";
    filecfg<<"Common t0\t"<<deltat+tEVeto<<"\n";
    tmp->Write("");
    
   
return;
}
