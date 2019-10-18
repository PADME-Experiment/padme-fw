#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
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

void DrawCosmic_v1()

{

//TFile *file = new TFile("output/an_cosmiclong.root");//<-con questo crasha
//TFile *file = new TFile("output/an_t1_cosmic_AmplThrLowAmplThrHigh1.root");//con questo no
//TFile *file = new TFile("output/an_t1_AmplThrLow1AmplThrHigh1_nothres.root");
TFile *file = new TFile("an_t1_cosmic_HitEnergyyp_noconditions.root");
TTree *tree = new TTree();
tree = (TTree*) file->Get("PADME_FlatNT");
TCanvas *canvas  = new TCanvas ("SAC_Energy", "SAC_Energy", 100, 10, 700, 580);
TCanvas *canvas2  = new TCanvas ("Time", "Time", 100, 10, 700, 580);
TCanvas *canvas3  = new TCanvas ("NHit", "NHit", 100, 10, 700, 580);
TCanvas *canvas4  = new TCanvas ("Time diff", "Time diff", 100, 10, 700, 580);
TCanvas *canvas5  = new TCanvas ("Empty events", "Empty events", 100, 10, 700, 580);

//TH1D *HitTime = new TH1D("Hit time","Hit time",200,-500,500);
TH1D *HitTimeDiff = new TH1D("Delta time between Hits","Delta time between Hits",400,-20,20);
TH1D *hNHit = new TH1D("Number of hit per event","Number of hit per event",100,0,100);

TH1D ** H1;
H1 = new TH1D*[25];
TH1D ** H2;
H2 = new TH1D*[25];
TH1D ** H3;
H3 = new TH1D*[25];
TH1D ** H4;
H4 = new TH1D*[25];

	for (int g=0;g<25;g++)

	{

	H1[g] = new TH1D(Form("Hit Energy%d with cuts",g),Form("Hit Energy%d with cuts",g),200,0.,200.);
	H2[g] = new TH1D(Form("Hit Energy%d",g),Form("Hit Energy%d",g),200,0.,200.);
	H3[g] = new TH1D(Form("Hit Energy%d with 2 cuts",g),Form("Hit Energy%d",g),200,0.,200.);
	H4[g] = new TH1D(Form("Hit Energy%d with cut on verticality",g),Form("Hit Energy%d",g),200,0.,200.);
	
	}

canvas->Divide(5,5);

TH2D ** HitTime;
HitTime = new TH2D*[25];
TH1D ** EmptyEvent;
EmptyEvent = new TH1D*[25];

	for (int g=0;g<25;g++)

	{

	HitTime[g] = new TH2D(Form("Hit Time %d",g),Form("Hit Time %d",g),200,-500.,100.,200,0.,200.);
	EmptyEvent[g] = new TH1D(Form("Hit Energy %d of the empty events",g),Form("Hit Energy %d  of the empty events",g),200,0.,50.);

	}

canvas2->Divide(5,5);
canvas5->Divide(5,5);

TF1 *f1 = new TF1("f1","landau",15,250);
TF1 *f2 = new TF1("f2","gaus",0,15);

//tree->Print();

Int_t NEvent;
Int_t NHits;

Int_t entries = tree->GetEntries();

//cout<<"Il numero di entries è "<<entries<<endl;

int totalhitnumber=0;
int count=0;

Double_t Hits_Energy[200];
Double_t Hits_Time[200];
Double_t Hits_ChannelId_double[200];//<----questa dimensione non va bene, ancora non so su quale variabile

int Hits_ChannelId[200];
int column[200],row[200],ElCh[200];

TBranch *b_NTSAC_Hits_Energy;   //!
TBranch *b_NTSAC_Hits_Time;   //!
TBranch *b_NTSAC_Hits_ChannelId;   //!

Double_t firstvalue,lastvalue,time_diff[200],col_diff[200],MPV[25],CC[25];

tree->SetBranchAddress("NTNEvent",&NEvent);
tree->SetBranchAddress("NTNSAC_Hits",&NHits);

tree->SetBranchAddress("NTSAC_Hits_Energy",Hits_Energy,&b_NTSAC_Hits_Energy);
tree->SetBranchAddress("NTSAC_Hits_Time",Hits_Time,&b_NTSAC_Hits_Time);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",Hits_ChannelId_double,&b_NTSAC_Hits_ChannelId);

		for (Int_t i=0; i<entries; i++) //questo loop gira su  eventi, per ogni evento ho più hit!

		{

		int count2=0;

		//cout<<"NHits = "<<NHits<<endl;

		tree->GetEntry(i);

			if (NHits>=2) //first request is having only events with a certain number of Hits
			  //rimuovere il taglio superiore perche' pericoloso
			{

			/*cout<<"=========================================================================="<<endl;
			cout<<"		Opening event n. "<<NEvent<<" : it has "<<NHits<<" Hits 	 "<<endl;
			cout<<"=========================================================================="<<endl;
			cout<<" "<<endl;*/
	

				for (Int_t g=0; g<NHits; g++)

				{
				  //  contare gli hit in tempo

				Hits_ChannelId[g] = static_cast<int>(Hits_ChannelId_double[g]);

				//int column[30],row[30],gref,colref,rowref;
				Double_t timeref;

				timeref=Hits_Time[0];

				column[g]=Hits_ChannelId[g]/10;//column index is the first
				row[g]   =Hits_ChannelId[g]%10;//row index is the second
				ElCh[g]  =Hits_ChannelId[g]/10+Hits_ChannelId[g]%10*5;

				//cout<<" "<<endl;
				//cout<<"	------> Hit on position "<<Hits_ChannelId[g]<<" with energy "<<Hits_Energy[g]<<endl;
				//cout<<" "<<endl;

				//canvas->cd(25-ElCh[g]);
				H2[ElCh[g]]->Fill(Hits_Energy[g]); //questo è senza tagli
				HitTime[ElCh[g]]->Fill(Hits_Time[ElCh[g]],Hits_Energy[ElCh[g]]);

				hNHit->Fill(NHits);

				if (Hits_Time[g]<-220 || Hits_Time[g]>-140) EmptyEvent[ElCh[g]]->Fill(Hits_Energy[ElCh[g]]);

				if (Hits_Time[g]<-188 && Hits_Time[g]>-170) continue;
				
				//if (NHits>10) continue;

				H1[ElCh[g]]->Fill(Hits_Energy[g]);
				//hNHit->Fill(NHits);


					for (Int_t h=g+1; h<NHits+1; h++)

					{

					time_diff[h]=Hits_Time[g]-Hits_Time[h];

					col_diff[h]=column[g]-column[h];

					HitTimeDiff->Fill(time_diff[h]);

					//if ((time_diff[g]>-1 && time_diff[g]<1.5) && (column[g]==column[h])) H4[ElCh[g]]->Fill(Hits_Energy[g]);

				
					}

				if (time_diff[g]>-1 && time_diff[g]<1.5) H3[ElCh[g]]->Fill(Hits_Energy[g]);
				if ((time_diff[g]>-1 && time_diff[g]<1.5) && (col_diff[g]=0)) H4[ElCh[g]]->Fill(Hits_Energy[g]);
				

				}//closes first for on Hits

			/*cout<<" "<<endl;
			cout<<"Number of cosmics for the event is "<<count2<<endl;
			cout<<" "<<endl;*/

			}//closes if on hit number

		}//closes for on events

//cout<<"Total number of cosmic rays is "<<count<<endl;

//canvas2->cd();
//HitTime->Draw();

canvas3->cd();
hNHit->Draw();

canvas4->cd();
HitTimeDiff->Draw();

	for (Int_t e=0;e<25;e++)

	{

	canvas->cd(25-e)->SetLogy();
	//canvas->cd();
	H2[e]->Draw(""); //senza tagli
	H1[e]->SetLineColor(kRed);
	//H1[e]->Draw("SAME");//taglio in tempo
	H3[e]->SetLineColor(kGreen);
	//H3[e]->Draw("SAME");//taglio tempo, deltatime
	H4[e]->SetLineColor(kMagenta);
	//H4[e]->Draw("SAME");//taglio tempo, deltatime e verticalità

	//H2[e]->Fit("f2","Q","",0,10);

	//Int_t firstbin = H3[e]->FindFirstBinAbove(1,1);
	Int_t lastbin = H3[e]->FindLastBinAbove(1,1);
	//firstvalue = H3[e]->GetBinContent(firstbin);
	firstvalue = (f2->GetParameter(2))*5+f2->GetParameter(1);
	lastvalue = H3[e]->GetBinContent(lastbin);
	//cout<<"First bin is "<<firstbin<<" and first value is "<<firstvalue<<endl;
	//H4[e]->Fit("f1","Q","",firstvalue,lastvalue);

	MPV[e]=f1->GetParameter(1);
	CC[e]=28.11/(MPV[e]);
	//cout<<"MPV for ch "<<e<<" is "<<MPV[e]<<endl;
	//cout<<e%5*10+e/5<<"	"<<CC[e]<<endl;

	canvas2->cd(25-e);
	HitTime[e]->Draw("colz");

	canvas5->cd(25-e)->SetLogy();
	EmptyEvent[e]->Draw("");

	}

/*canvas->cd()->SetLogy();
H2[16]->Draw("");
	Int_t firstbin = H3[e]->FindFirstBinAbove(0,1);
	Int_t lastbin = H3[e]->FindLastBinAbove(1,1);
	firstvalue = H3[e]->GetBinContent(firstbin);
	lastvalue = H3[e]->GetBinContent(lastbin);
H3[16]->SetLineColor(kGreen);
H3[16]->Draw("SAME");*/

//H3[9]->Fit("f1","Q","",firstvalue,lastvalue);

}

