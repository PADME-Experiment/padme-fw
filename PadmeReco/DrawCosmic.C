#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TSpectrum.h"
//#include "TLegend.h"
//#include "TMultiGraph.h"
//#include "TPaveStats.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TROOT.h"
//#include <map>
#include <iostream>
#include <fstream>
//#include <string>
//#include <iterator>
//#include <vector>

void SACCosmic()

	{

	gROOT->SetStyle("Plain");
	gROOT->ForceStyle();
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	double Mean,Mean2N,sum,sum_c;
	int NSAC=25;
	char hisname[50],hisname2[50];
	double C[25],Q[25];
	int Nrow=5;
	int Ncol=5;

	TCanvas *histo=new TCanvas();
	histo->Divide(Ncol,Nrow);

	//TCanvas *histo2=new TCanvas();
	//histo2->Divide(2,1);

	//TCanvas *mean=new TCanvas();

	//TFile *fileMauro = new TFile("SACAn_prova.root","READ");
	TFile *fileMauro = new TFile("SACAn.root","READ");

	//TH1D *H2 = new TH1D("Charge of cluster","",10,0,50);

	TH1D ** H2;
	H2 = new TH1D*[2];
	for (int g=0;g<2;g++)

		{

		H2[g] = new TH1D("Charge of cluster","Charge of cluster",100,0.,50.);

		}


	//g1=new TF1("g1","gaus",100,600);
	//g1=new TF1("g1","gaus",120,160);
	//g2=new TF1("g2","gaus",40,100);

	for(int i=0;i<NSAC;i++)

		{

		//int j=i*(-1)+21;
		//histo->cd(25-i)->SetLogy();
		histo->cd(25-i);
		sprintf(hisname,Form("hVMax%d",i));
		TH1D* H1 = (TH1D*)fileMauro->Get(hisname);
		H1->SetFillColor(kBlue);
		H1->SetLineColor(kBlack);
		H1->GetXaxis()->SetTitle("Amplitude [mV]");
		
		H1->Rebin(5);
		H1->GetXaxis()->SetRange(0,200);

		H1->Draw("");

		TSpectrum* spec = new TSpectrum();
		//int n_peaks = spec->Search(H1,3,"",0.2);
		int n_peaks = spec->Search(H1,1.2,"goff",0.003);
		if (i==7) {n_peaks = spec->Search(H1,0.75,"goff",0.0024);}
		if (i==18||i==17) {n_peaks = spec->Search(H1,0.75,"goff",0.02);}

		double* peakpos = spec->GetPositionX();
		double* ampl =  spec->GetPositionY();

		Q[i]=peakpos[1]-peakpos[0];	

		if (i==18||i==12||i==13||i==17) 

			{

			//cout<<"Charge of crystal "<<i<<" is "<<Q[i]<<endl;

			sum = Q[18]+Q[17]+Q[12]+Q[13];

			//cout<<"Charge of cluster is "<<sum<<endl;

			//histo2->cd(1)->SetLogy();
			//H2[0]->Fill(sum);

			}

		/*g1->SetLineColor(kBlue);
		//g2->SetLineColor(kBlue);

		if (i==2 || i==7) 

			{

			H1->Fit("g1","","",200,400);

			}

		else

			{

			H1->Fit("g1","R");

			}

		Mean=g1->GetParameter(1);
		//cout<<"Mean value is "<<Mean[i]<<endl;
		//mean->cd();
		

		//Calibration constants

		//C[i]=Mean[12]/Mean[i];*/

		}

	for (int k;k<NSAC;k++)

		{

		/*histo2->cd(NSAC-k)->SetLogy();
		sprintf(hisname2,Form("hChTime%d",k));
		//TH2D *H3 = new TH2D("Energy vs time","Energy vs time",100,0,1000,100,0,10);
		TH1D* H3 = (TH1D*)fileMauro->Get(hisname2);
		H3->SetFillColor(kMagenta);
		H3->SetLineColor(kBlack);
		H3->GetXaxis()->SetTitle("Time");
		H3->Draw();*/

		}

	//H2->Draw();

	}
