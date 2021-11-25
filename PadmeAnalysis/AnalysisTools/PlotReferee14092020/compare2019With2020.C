#include "Rtypes.h"

#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"

#include "TLegend.h"

void compare2019With2020() {
    
  //   TFile *oldT   = new TFile("AnalysisReco_MHVersion25May_Run_0000000_20190724_152634.root"  , "OLD");    // 2019 july run target in 
  TFile *oldT   = new TFile("AnalysisReco_MHVersion25May_Run_0000000_20190724_152634_CutOnNPOT.root"  , "OLD");    // 2019 july run target in 
  TFile *oldnoT   = new TFile("AnalysisReco_MHVersion25May_Run_0000000_20190724_181605.root"  , "OLD");    // 2019 july run target out
  TFile *newT   = new TFile("AnalysisHAdd_Recorun_0030203_20200722_031723_npotThr5000.root"  , "OLD");    // 2020 july run target in 
  TFile *newnoT   = new TFile("AnalysisHAdd_Recorun_0030204_20200722_054045_npotThr0.root"  , "OLD");    // 2020 july run target out

  TFile *fT5  =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030211_20200722_19182_npotThr1800.root");
  TFile *fT10 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030205_20200722_081640_npotThr5000.root"); 
  TFile *fT15 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030201_20200721_224335_npotThr5000.root");
  TFile *fT20 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030203_20200722_031723_npotThr5000.root"); 
  TFile *fT25 =new TFile("/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/Analysis/AnalysisHAdd_Recorun_0030209_20200722_143634_npotThr5000.root");


  
#ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
#endif
   
  SetPadmeStyle();
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  Double_t oldNoTScalef = TargetNoTargetScaleFactor(oldT,oldnoT); 
  Double_t newNoTScalef = TargetNoTargetScaleFactor(newT,newnoT); 
  Double_t scalef  =ScaleFTwoTargetSamples(oldT, newT, 20000);
  Double_t scalef5=0.;
  Double_t scalef10=0.;
  Double_t scalef15=0.;
  Double_t scalef20=0.;
  Double_t scalef25=0.;
  double nPOTfromTarget = ((TH1D*)fT15->Get("nPOT"))->GetBinContent(2);
  Double_t scalenPOT20= corrNPOT(fT20, 20000);
  Double_t scalenPOT25= corrNPOT(fT20, 25000);
  double nPOT5 = ((TH1D*)fT5->Get("nPOT"))->GetBinContent(2);
  double nPOT10 = ((TH1D*)fT10->Get("nPOT"))->GetBinContent(2);
  double nPOT15 = ((TH1D*)fT15->Get("nPOT"))->GetBinContent(2);
  double nPOT20 = ((TH1D*)fT20->Get("nPOT"))->GetBinContent(2);
  double nPOT25 = ((TH1D*)fT25->Get("nPOT"))->GetBinContent(2);
  scalef5=nPOT15/nPOT5;
  scalef10=nPOT15/nPOT10;
  scalef15=nPOT15/nPOT15;
  scalef20=nPOT15/(nPOT20*scalenPOT20);
  scalef25=nPOT15/(nPOT25*scalenPOT25);

  std::string hname;
  std::string xtitle, ytitle;
  double xmin=-999;
  double xmax= 999;
  Bool_t sameSample=true;
   xmin=0.;
  xmax=900;
  hname="ECAL_gravTwoPhoton10ns5cm";
  xtitle="E_{#gamma_{1}}+E_{#gamma_{2}} [MeV]";
  ytitle="MeV";
  //DrawPlot(hname, oldT, oldnoT, newT, newnoT, oldNoTScalef,newNoTScalef, scalef, xmin , xmax, xtitle, ytitle);
  xmin=0.;
  xmax=2500;
  hname="ECal_EHitTot";
  xtitle="ECal Total Energy [MeV]";
  ytitle="MeV";
  //DrawPlot(hname, oldT, oldnoT, newT, newnoT, oldNoTScalef,newNoTScalef, scalef, xmin , xmax, xtitle,ytitle, sameSample);
  
  xmin=-120;
  xmax=130;
  hname="ECal_ClusterTime";
  xtitle="ECal cluster time [ns]";
  ytitle="ns";
  //DrawPlotMultiplicity(hname,fT5, fT10, fT15, fT20, fT25, scalef5,scalef10, scalef15, scalef20, scalef25,xmin, xmax, xtitle, ytitle);
  xmin=0.;
  xmax=900;
  hname="ECAL_gravTwoPhoton10ns5cm";
  xtitle="E_{#gamma_{1}}+E_{#gamma_{2}} [MeV]";
  ytitle="MeV";
  DrawPlotMultiplicity(hname,fT5, fT10, fT15, fT20, fT25, scalef5,scalef10, scalef15, scalef20, scalef25,xmin, xmax, xtitle, ytitle);
}


Double_t ScaleFTwoTargetSamples( TFile* fTargetOld, TFile* fTargetNew, Double_t nPOTBunch){
  TH1D* hDN = (TH1D*)fTargetOld->Get("nPOT");
  TH1D* hMCN = (TH1D*)fTargetNew->Get("nPOT");
  double nPOTOld = hDN->GetBinContent(2);
  double nPOTNew = hMCN->GetBinContent(2);
  Double_t firstMean20=((TH1D*)fTargetNew->Get("TargetBeam_nPOT"))->GetMean(); 
  TH1D *h1=((TH1D*)fTargetNew->Get("TargetBeam_nPOT"));
  h1->GetXaxis()->SetRangeUser(firstMean20-7000, firstMean20+15000);
  Double_t MeanT = h1->GetMean();
  Double_t fcorr=nPOTBunch/MeanT;
  nPOTNew =nPOTNew*fcorr;
  Double_t scalef = nPOTOld/nPOTNew;
  return scalef;
}



Double_t TargetNoTargetScaleFactor( TFile* fTarget, TFile* fnoTarget, Double_t scalef){
  TH1D * DTime = (TH1D*)fTarget->Get("ECAL_dTime_10ns20DegreeDeltaTheta");

  Double_t bin1_5=DTime->FindBin(-10);
  Double_t bin2_5=DTime->FindBin(-6);
  Double_t bin4_5=DTime->FindBin(10);
  Double_t bin3_5=DTime->FindBin(6);
  DTime->Scale(scalef);
  Double_t IntegralLeft5=DTime->Integral(bin1_5,bin2_5);
  Double_t IntegralRight5=DTime->Integral(bin3_5,bin4_5);
      
  TH1D * DTime10 = (TH1D*)fnoTarget->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
  Double_t bin1_10=DTime10->FindBin(-10);
  Double_t bin2_10=DTime10->FindBin(-6);
  Double_t bin4_10=DTime10->FindBin(10);
  Double_t bin3_10=DTime10->FindBin(6);
  Double_t IntegralLeft10=DTime10->Integral(bin1_10,bin2_10);
  Double_t IntegralRight10=DTime10->Integral(bin3_10,bin4_10);
  Double_t scaleFactor = IntegralLeft5/IntegralLeft10;
  DTime->Scale(1./scalef);
  // std::cout<<"scalef " << scalef <<" integral left target " << IntegralLeft5 << " integral left Notarget " << IntegralLeft10  << " scale factor " << scaleFactor << std::endl;
  return scaleFactor;
}

Double_t TargetNoTargetScaleFactor( TFile* fTarget, TFile* fnoTarget){
  TH1D * DTime = (TH1D*)fTarget->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
  Double_t bin1_5=DTime->FindBin(-10);
  Double_t bin2_5=DTime->FindBin(-6);
  Double_t bin4_5=DTime->FindBin(10);
  Double_t bin3_5=DTime->FindBin(6);
  Double_t IntegralLeft5=DTime->Integral(bin1_5,bin2_5);
  Double_t IntegralRight5=DTime->Integral(bin3_5,bin4_5);
      
  TH1D * DTime10 = (TH1D*)fnoTarget->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
  Double_t bin1_10=DTime10->FindBin(-10);
  Double_t bin2_10=DTime10->FindBin(-6);
  Double_t bin4_10=DTime10->FindBin(10);
  Double_t bin3_10=DTime10->FindBin(6);
  Double_t IntegralLeft10=DTime10->Integral(bin1_10,bin2_10);
  Double_t IntegralRight10=DTime10->Integral(bin3_10,bin4_10);
  Double_t  scalef=IntegralLeft5/IntegralLeft10;
  //std::cout<<"integral left target " << IntegralLeft5 << " integral left Notarget " << IntegralLeft10  << " scale factor " << scaleFactor << std::endl;
  return scalef;
}


void DrawPlot(std::string hname, TFile *oldT, TFile *oldnoT, TFile *newT, TFile *newnoT, Double_t scaleOldNoT,Double_t scaleNewNoT, Double_t scalef,double xmin ,double xmax ,  std::string xtitle, std::string ytitle){    //scalef is to scale the new to old nPOT
  TH1F *hOldT       =  (TH1F*)oldT->Get(hname.c_str());
  TH1F *hOldnoT     =  (TH1F*)oldnoT->Get(hname.c_str());
  TH1F *hNewT       =  (TH1F*)newT->Get(hname.c_str());
  TH1F *hNewnoT     =  (TH1F*)newnoT->Get(hname.c_str());

  TH1D* hsOldnoT = hOldnoT->Clone();
  hsOldnoT->Scale(scaleOldNoT);
  TH1D* hsNewT = hNewT->Clone();
  hsNewT->Scale(scalef);
  TH1D* hsNewnoT = hNewnoT->Clone();
  hsNewnoT->Scale(scaleNewNoT*scalef);
  
  Double_t oldPot=((TH1F*)oldT->Get("nPOT"))->GetBinContent(2);
  Double_t newPot=((TH1F*)newT->Get("nPOT"))->GetBinContent(2);
  std::ostringstream s;
  //s<<"nPOT=" << oldPot;
  s<<"nPOT= 2.7E+09";
  std::string snpotold= s.str();
  std::ostringstream sN;
  //sN<<"nPOT=" << newPot ;
  sN<<"nPOT= 6.1E+09" ;
  std::string snpotnew= sN.str();
  std::ostringstream sN2;
  sN2<<"(rescaled to 2.7E+09)";
  std::string snpotnew2= sN2.str();
  

  auto t = new TLegend(0.65,0.65,0.85,0.80);
  //t->AddEntry(hOldT,"target in, 25k POT/bunch, 250 ns, E_{beam}=490 MeV, July'19","ep");
  t->AddEntry(hOldT,"target in","ep");
  t->AddEntry(hsOldnoT,"target out","ep");
  // t->SetHeader("July'19","C");
  t->SetTextAlign(12);
  t->SetBorderSize(0);
  t->SetFillStyle(0);
  t->SetFillColor(0);
  t->SetTextFont(43);
  t->SetTextSize(15);
  auto t1 = new TLegend(0.65,0.65,0.85,0.80);
  t1->AddEntry(hsNewT,"target in","ep");
  t1->AddEntry(hsNewnoT,"target out","ep");
  //t1->SetHeader("July'20", "C");
  t1->SetTextAlign(12);
  t1->SetBorderSize(0);
  t1->SetFillStyle(0);
  t1->SetFillColor(0);
  t1->SetTextFont(43);
  t1->SetTextSize(15);


  if (fabs(xmin+999)<0.0001 && fabs(xmax-999)<0.0001) {
    xmin = hOldT->GetXaxis()->GetXmin();
    xmax = hOldT->GetXaxis()->GetXmax();
  }
  double ymaxOldD    = hOldT->GetMaximum();
  double ymaxOldnoTD = hsOldnoT->GetMaximum();
  double ymaxNewD    = hsNewT->GetMaximum();
  double ymaxNewnoTD = hsNewnoT->GetMaximum();
  
  Double_t ymaxOld=0.;
  if(ymaxOldD>ymaxOldnoTD)ymaxOld=ymaxOldD;
  else ymaxOld=ymaxOldnoTD;
  Double_t ymaxNew=0.;
  if(ymaxNewD>ymaxNewnoTD)ymaxNew=ymaxNewD;
  else ymaxNew=ymaxNewnoTD;

  int binW= hOldT->GetBinWidth(10);
  std::ostringstream yTit;
  yTit<<"counts/"<<binW<<ytitle;
  std::string yTitle= yTit.str();
  
  TCanvas *c0 = new TCanvas("c0", "c0", 700,500);
  c0->cd();
  TH1D* hFrame = hOldT->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymaxOld*1.65);
  hFrame->GetXaxis()->SetRangeUser(xmin, xmax);
  hFrame->GetXaxis()->SetTitle(xtitle.c_str());
  hFrame->GetYaxis()->SetTitle(yTitle.c_str());
  hFrame->Draw();

  //  TCanvas *c1 = new TCanvas (hname.c_str(),hname.c_str(),1000,600 );
  //c1->Divide(2,1);
  //c1->cd(1);
  PADME_LABEL(0.2,0.87); myText( 0.33,0.87,1,"Preliminary");
  //  const char *a=hname.c_str();
  //myText( 0.25,0.91,1,a, 0.039);
  const char *b=snpotold.c_str();
  myText( 0.2, 0.80 , 1, b);
  myText( 0.2,0.75,1,"25k POT/bunch");
  myText( 0.2,0.72,1,"nominal bunch #Delta t 250 ns",0.038);
  myText( 0.2,0.69,1,"EBeam = 490 MeV", 0.038);
  myText( 0.2,0.66,1,"July '19", 0.038);
  // hOldT->GetXaxis()->SetRangeUser(xmin, xmax);
  hOldT->GetXaxis()->SetTitle(xtitle.c_str());
  hOldT->SetMarkerSize(0.6);
  hOldT->SetMarkerStyle(20);
  hOldT->SetMarkerColor(kRed);
  hOldT->SetMaximum(ymaxOld*1.3);
  hOldT->SetLineColor(kRed);
  hsOldnoT->SetLineColor(kBlack);
  hsOldnoT->SetMarkerSize(0.6);
  hsOldnoT->SetMarkerStyle(20);
  hsOldnoT->SetMarkerColor(kBlack);
  hOldT->Draw("SAMEPE");
  hsOldnoT->Draw("SAMEPE");
  t->Draw();
  TCanvas *c = new TCanvas("c", "c", 700,500);
  c->cd();
  TH1D* hFrame1 = hsNewT->Clone();
  hFrame1->Scale(0.);
  hFrame1->SetMaximum(ymaxNew*1.65);
  hFrame1->GetXaxis()->SetRangeUser(xmin, xmax);
  hFrame1->GetXaxis()->SetTitle(xtitle.c_str());
  hFrame1->GetYaxis()->SetTitle(yTitle.c_str());
  hFrame1->Draw();
  PADME_LABEL(0.2,0.87); myText( 0.33,0.87,1,"Preliminary");
  // const char *a=hname.c_str();
  //  myText( 0.25,0.91,1,a, 0.039); 
  const char *b=snpotnew.c_str();
  myText(       0.2, 0.80 , 1, b);
  const char *b2=snpotnew2.c_str();
  myText(       0.2, 0.76 , 1, b2, 0.045);
  myText( 0.2,0.69,1,"20k POT/bunch");
  myText( 0.2,0.64,1,"bunch #Delta t 150 ns",0.038);
  myText( 0.2,0.59,1,"EBeam = 450 MeV", 0.038);
  myText( 0.2,0.54,1,"July '20", 0.038);
  hsNewT->SetLineColor(kRed);
  hsNewT->SetMarkerSize(0.6);
  hsNewT->SetMarkerColor(kRed);
  hsNewT->SetMarkerStyle(20);
  hsNewT->SetMaximum(ymaxNew*1.3);
  hsNewT->SetMinimum(0.);
  hsNewT->GetXaxis()->SetRangeUser(xmin, xmax);
  hsNewT->GetXaxis()->SetTitle(xtitle.c_str());
  hsNewnoT->SetLineColor(kBlack);
  hsNewnoT->SetMarkerSize(0.6);
  hsNewnoT->SetMarkerColor(kBlack);
  hsNewnoT->SetMarkerStyle(20);
  hsNewT->Draw("SAMEPE");
  hsNewnoT->Draw("SAMEPE");
  t1->Draw();
  
  c0->SaveAs(("~/html/allow_listing/DataTakingJuly2020/png/"+hname+"July2019.png").c_str());
  c0->SaveAs(("CPlot/"+hname+"July2019.C").c_str());
  c->SaveAs(("~/html/allow_listing/DataTakingJuly2020/png/"+hname+"July2020.png").c_str());
  c->SaveAs(("CPlot/"+hname+"July2020.C").c_str());
  delete c0;
  delete c;
   
}




void DrawPlot(std::string hname, TFile *oldT, TFile *oldnoT, TFile *newT, TFile *newnoT, Double_t scaleOldNoT,Double_t scaleNewNoT, Double_t scalef,double xmin ,double xmax ,  std::string xtitle=, std::string ytitle, Bool_t sameSample){    //scalef is to scale the new to old nPOT
  TH1F *hOldT       =  (TH1F*)oldT->Get(hname.c_str());
  TH1F *hOldnoT     =  (TH1F*)oldnoT->Get(hname.c_str());
  TH1F *hNewT       =  (TH1F*)newT->Get(hname.c_str());
  TH1F *hNewnoT     =  (TH1F*)newnoT->Get(hname.c_str());

  TH1D* hsOldnoT = hOldnoT->Clone();
  hsOldnoT->Scale(scaleOldNoT);
  TH1D* hsNewT = hNewT->Clone();
  hsNewT->Scale(scalef);
  TH1D* hsNewnoT = hNewnoT->Clone();
  hsNewnoT->Scale(scaleNewNoT*scalef);
  
  Double_t oldPot=((TH1F*)oldT->Get("nPOT"))->GetBinContent(2);
  Double_t newPot=((TH1F*)newT->Get("nPOT"))->GetBinContent(2);
  std::ostringstream s;
  //s<<"nPOT=" << oldPot;
  s<<"nPOT= 2.7E+09";
  std::string snpotold= s.str();
  std::ostringstream sN;
  //sN<<"nPOT=" << newPot ;
  sN<<"nPOT= 6.1E+09" ;
  std::string snpotnew= sN.str();
  std::ostringstream sN2;
  //sN2<<"(rescaled to " << oldPot << " )";
  sN2<<"(rescaled to 2.7E+09 )";
  std::string snpotnew2= sN2.str();
  

  TLegend* t = new TLegend(0.40,0.65,0.80,0.85);
  t->AddEntry(hOldT,"25k POT/bunch, 250 ns, E_{beam}=490 MeV, July'19","ep");
  t->AddEntry(hsNewT,"20k POT/bunch, 150 ns, E_{beam}=450 MeV, July'20","ep");
  t->SetTextAlign(12);
  t->SetBorderSize(0);
  t->SetFillStyle(0);
  t->SetFillColor(0);
  t->SetTextFont(43);
  t->SetTextSize(15);

  TLegend* t1 = new TLegend(0.40,0.65,0.80,0.85);
  t1->AddEntry(hsOldnoT,"25k POT/bunch, 250 ns, E_{beam}=490 MeV, July'19","ep");
  t1->AddEntry(hsNewnoT,"20k POT/bunch, 150 ns, E_{beam}=450 MeV, July'20","ep");
  t1->SetTextAlign(12);
  t1->SetBorderSize(0);
  t1->SetFillStyle(0);
  t1->SetFillColor(0);
  t1->SetTextFont(43);
  t1->SetTextSize(15);


  if (fabs(xmin+999)<0.0001 && fabs(xmax-999)<0.0001) {
    xmin = hOldT->GetXaxis()->GetXmin();
    xmax = hOldT->GetXaxis()->GetXmax();
  }
  double ymaxOldD    = hOldT->GetMaximum();
  double ymaxOldnoTD = hsOldnoT->GetMaximum();
  double ymaxNewD    = hsNewT->GetMaximum();
  double ymaxNewnoTD = hsNewnoT->GetMaximum();
  
  Double_t ymaxIn=0.;
  if(ymaxOldD>ymaxNewD)ymaxIn=ymaxOldD;
  else ymaxIn=ymaxNewD;
  Double_t ymaxOut=0.;
  if(ymaxOldnoTD>ymaxNewnoTD)ymaxOut=ymaxOldnoTD;
  else ymaxOut=ymaxNewnoTD;

  TCanvas *c0 = new TCanvas("c0", "c0", 700,500);
  c0->cd();

  int binW= hOldT->GetBinWidth(10);
  std::ostringstream yTit;
  yTit<<"counts/"<<binW<<ytitle;
  std::string yTitle= yTit.str();
  

  TH1D* hFrame = hOldT->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymaxIn*1.60);
  hFrame->GetXaxis()->SetRangeUser(xmin, xmax);
  hFrame->GetXaxis()->SetTitle(xtitle.c_str());
  hFrame->GetYaxis()->SetTitle(yTitle.c_str());
  hFrame->Draw();

 
  PADME_LABEL(0.2,0.85); myText( 0.33,0.85,1,"Preliminary");
  //  const char *a=hname.c_str();
  //myText( 0.25,0.91,1,a, 0.039);
  const char *b=snpotold.c_str();
  myText(       0.2, 0.77 , 1, b);
  myText( 0.2,0.69,1,"Target in");
  hOldT->SetMarkerSize(0.6);
  hOldT->SetMarkerStyle(20);
  hOldT->SetMarkerColor(kRed);
  hsNewT->SetMarkerSize(0.6);
  hsNewT->SetMarkerColor(kBlue);
  hsNewT->SetMarkerStyle(20);
  hsNewT->SetMinimum(0.);
  hsNewT->Draw("SAMEPE"); 
  hOldT->Draw("SAMEPE");
  t->Draw();
  TCanvas *c = new TCanvas("c", "c", 700,500);
  c->cd();
  TH1D* hFrame1 = hsNewT->Clone();
  hFrame1->Scale(0.);
  hFrame1->SetMaximum(ymaxOut*1.60);
  hFrame1->GetXaxis()->SetRangeUser(xmin, xmax);
  hFrame1->GetXaxis()->SetTitle(xtitle.c_str());
  hFrame1->GetYaxis()->SetTitle(yTitle.c_str());
  hFrame1->Draw();
  PADME_LABEL(0.2,0.85); myText( 0.33,0.85,1,"Preliminary");
  // const char *a=hname.c_str();
  //  myText( 0.25,0.91,1,a, 0.039); 
  const char *b=snpotold.c_str();
  myText(       0.2, 0.77 , 1, b);
  //const char *b2=snpotnew2.c_str();
  //myText(       0.2, 0.69 , 1, b2, 0.045);
  myText( 0.2,0.69,1,"Target out");
  hsOldnoT->SetMarkerSize(0.6);
  hsOldnoT->SetMarkerStyle(20);
  hsOldnoT->SetMarkerColor(kRed);
  hsNewnoT->SetMarkerSize(0.6);
  hsNewnoT->SetMarkerColor(kBlue);
  hsNewnoT->SetMarkerStyle(20);

  hsOldnoT->Draw("SAMEPE");
  hsNewnoT->Draw("SAMEPE");
  t1->Draw();
  
  c0->SaveAs(("~/html/allow_listing/DataTakingJuly2020/png/"+hname+"TargetIn.png").c_str());
  c0->SaveAs(("CPlot/"+hname+"TargetIn.C").c_str());
  c->SaveAs(("~/html/allow_listing/DataTakingJuly2020/png/"+hname+"TargetOut.png").c_str());
  c0->SaveAs(("CPlot/"+hname+"TargetOut.C").c_str());
  delete c0;
  delete c;
  
}



void  DrawPlotMultiplicity(std::string hname,TFile *fT5,TFile * fT10,TFile * fT15, TFile *fT20,TFile * fT25,Double_t scalef5,Double_t scalef10,Double_t scalef15,Double_t scalef20,Double_t scalef25,double xmin, double xmax,std::string xtitle, std::string ytitle){

  TH1F *h5     =   (TH1F*)fT5->Get(hname.c_str());
  TH1F *h10     =  (TH1F*)fT10->Get(hname.c_str());
  TH1F *h15     =  (TH1F*)fT15->Get(hname.c_str());
  TH1F *h20     =  (TH1F*)fT20->Get(hname.c_str());
  TH1F *h25     =  (TH1F*)fT25->Get(hname.c_str());

  TH1D* hs5 = h5->Clone();
  hs5->Scale(scalef5);
  TH1D* hs10 = h10->Clone();
  hs10->Scale(scalef10);
  TH1D* hs15 = h15->Clone();
  hs15->Scale(scalef15);
  TH1D* hs20 = h20->Clone();
  hs20->Scale(scalef20);
  TH1D* hs25 = h25->Clone();
  hs25->Scale(scalef25);
  
  Double_t pot=((TH1F*)fT15->Get("nPOT"))->GetBinContent(2);
  std::ostringstream s;
  //s<<"nPOT=" << pot;
  s<<"nPOT= 5.6E+09";
  std::string snpot= s.str();

  TLegend* t = new TLegend(0.60,0.62,0.85,0.87);
  //  t->AddEntry(hs5, " 5k POT/bunch, 150 ns, E_{beam}=450 MeV, July'20","ep");
  t->AddEntry(hs5, " 5k POT/bunch","ep");
  t->AddEntry(hs10,"10k POT/bunch","ep");
  t->AddEntry(hs15,"15k POT/bunch","ep");
  t->AddEntry(hs20,"20k POT/bunch","ep");
  t->AddEntry(hs25,"25k POT/bunch","ep");
  t->SetTextAlign(12);
  t->SetBorderSize(0);
  t->SetFillStyle(0);
  t->SetFillColor(0);
  t->SetTextFont(43);
  t->SetTextSize(15);


  if (fabs(xmin+999)<0.0001 && fabs(xmax-999)<0.0001) {
    xmin = hs5->GetXaxis()->GetXmin();
    xmax = hs5->GetXaxis()->GetXmax();
  }
  double ymax5     = hs5->GetMaximum();
  double ymax10    = hs10->GetMaximum();
  double ymax15    = hs15->GetMaximum();
  double ymax20    = hs20->GetMaximum();
  double ymax25    = hs25->GetMaximum();

  
  Double_t ymax=0.;
  if(ymax5>ymax10)ymax=ymax5;
  else ymax=ymax10;
  if(ymax<ymax15)ymax=ymax15;
  if(ymax<ymax20)ymax=ymax20;
  if(ymax<ymax25)ymax=ymax25;

  TCanvas *c0 = new TCanvas("c0", "c0", 700,500);
  c0->cd(); 

  int binW= hs5->GetBinWidth(10);
  std::ostringstream yTit;
  yTit<<"counts/"<<binW<<ytitle;
  std::string yTitle= yTit.str();
  
  TH1D* hFrame = hs5->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymax*1.6);
  hFrame->GetXaxis()->SetRangeUser(xmin, xmax);
  hFrame->GetXaxis()->SetTitle(xtitle.c_str());
  hFrame->GetYaxis()->SetTitle(yTitle.c_str());
  hFrame->Draw();
 
  PADME_LABEL(0.2,0.83); myText( 0.33,0.83,1,"Preliminary");
  //  const char *a=hname.c_str();
  //myText( 0.25,0.91,1,a, 0.039);
  const char *b=snpot.c_str();
  myText(       0.2, 0.75 , 1, b);
  myText( 0.2,0.69,1,"bunch #Delta t 150 ns", 0.038);
  myText( 0.2,0.63,1,"Ebeam = 450 MeV", 0.038);
  myText( 0.2,0.57,1,"July '20", 0.038);
  hs5->SetMarkerSize(0.6);
  hs5->SetMarkerStyle(20);
  hs5->SetMarkerColor(kRed);
  hs10->SetMarkerSize(0.6);
  hs10->SetMarkerColor(kBlue);
  hs10->SetMarkerStyle(20);
  hs15->SetMarkerSize(0.6);
  hs15->SetMarkerStyle(20);
  hs15->SetMarkerColor(kYellow);
  hs20->SetMarkerSize(0.6);
  hs20->SetMarkerColor(kGreen);
  hs20->SetMarkerStyle(20);
  hs25->SetMarkerSize(0.6);
  hs25->SetMarkerStyle(20);
  hs25->SetMarkerColor(kBlack);
  hs5->Draw("SAMEPE"); 
  hs10->Draw("SAMEPE");
  hs15->Draw("SAMEPE"); 
  hs20->Draw("SAMEPE");
  hs25->Draw("SAMEPE"); 
  t->Draw();

  c0->SaveAs(("~/html/allow_listing/DataTakingJuly2020/png/"+hname+"MultiplicityJuly2020.png").c_str());
  c0->SaveAs(("CPlot/"+hname+"MultiplicityJuly2020.C").c_str());
  delete c0;
}



Double_t corrNPOT(TFile* fData, Double_t nPOTbtf){
  Double_t firstMean20=((TH1D*)fData->Get("TargetBeam_nPOT"))->GetMean(); 
  TH1D *h1=((TH1D*)fData->Get("TargetBeam_nPOT"));
  h1->GetXaxis()->SetRangeUser(firstMean20-7000, firstMean20+15000);
  Double_t Mean20 = h1->GetMean();
  Double_t fcorr=nPOTbtf/Mean20;
  return fcorr;
}
