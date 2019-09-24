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
TFile *file = new TFile("output/an_t1_AmplThrLow1AmplThrHigh1_nothres.root");
TTree *tree = new TTree();
tree = (TTree*) file->Get("PADME_FlatNT");
TCanvas *canvas  = new TCanvas ("SAC_Energy", "SAC_Energy", 100, 10, 700, 580);
TCanvas *canvas2  = new TCanvas ("Time", "Time", 100, 10, 700, 580);
TCanvas *canvas3  = new TCanvas ("NHit", "NHit", 100, 10, 700, 580);
TCanvas *canvas4  = new TCanvas ("Time diff", "Time diff", 100, 10, 700, 580);

TH1D *HitTime = new TH1D("Hit time","Hit time",200,-300,0);
TH1D *HitTimeDiff = new TH1D("Delta time between hits","Delta time between hits",400,-20,20);
TH1D *hNHit = new TH1D("Number of hit per event","Number of hit per event",100,0,100);

TH1D ** H1;
H1 = new TH1D*[25];
TH1D ** H2;
H2 = new TH1D*[25];
TH1D ** H3;
H3 = new TH1D*[25];

	for (int g=0;g<25;g++)

	{

	H1[g] = new TH1D(Form("Hit Energy%d with cuts",g),Form("Hit Energy%d with cuts",g),100,0.,100.);
	H2[g] = new TH1D(Form("Hit Energy%d",g),Form("Hit Energy%d",g),100,0.,100.);
	H3[g] = new TH1D(Form("Hit Energy%d with 2 cuts",g),Form("Hit Energy%d",g),100,0.,100.);

	}

canvas->Divide(5,5);

TF1 *f1 = new TF1("f1","landau",15,250);

//tree->Print();

Int_t NEvent;
Int_t NHits;

Int_t entries = tree->GetEntries();

//cout<<"Il numero di entries è "<<entries<<endl;

int totalhitnumber=0;
int count=0;

Double_t Hits_Energy[50];
Double_t Hits_Time[50];
Double_t Hits_ChannelId_double[50];//<----questa dimensione non va bene, ancora non so su quale variabile

int Hits_ChannelId[50];
int column[50],row[50],ElCh[25];

TBranch *b_NTSAC_Hits_Energy;   //!
TBranch *b_NTSAC_Hits_Time;   //!
TBranch *b_NTSAC_Hits_ChannelId;   //!

Double_t firstvalue,lastvalue,time_diff[50];

tree->SetBranchAddress("NTNEvent",&NEvent);
tree->SetBranchAddress("NTNSAC_Hits",&NHits);

tree->SetBranchAddress("NTSAC_Hits_Energy",Hits_Energy,&b_NTSAC_Hits_Energy);
tree->SetBranchAddress("NTSAC_Hits_Time",Hits_Time,&b_NTSAC_Hits_Time);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",Hits_ChannelId_double,&b_NTSAC_Hits_ChannelId);

//tree->SetBranchAddress("NTSAC_Hits_Energy",Hits_Energy[0]);
//tree->SetBranchAddress("NTSAC_Hits_Time",Hits_Time[0]);
//tree->SetBranchAddress("NTSAC_Hits_ChannelId",Hits_ChannelId_double[0]);
// controllare che ci siano meno di 50 hits e poi non processi 

		for (Int_t i=0; i<entries; i++) //questo loop gira su  eventi, per ogni evento ho più hit!

		{

		int count2=0;

		//cout<<"NHits = "<<NHits<<endl;

		tree->GetEntry(i);

			if (NHits>=2) //first request is having only events with a certain number of hits
			  //rimuovere il taglio superiore perche' pericoloso
			{

			cout<<"=========================================================================="<<endl;
			cout<<"		Opening event n. "<<NEvent<<" : it has "<<NHits<<" hits 	 "<<endl;
			cout<<"=========================================================================="<<endl;
			cout<<" "<<endl;
	

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

				cout<<" "<<endl;
				cout<<"	------> Hit on position "<<Hits_ChannelId[g]<<" with energy "<<Hits_Energy[g]<<endl;
				cout<<" "<<endl;

				//canvas->cd(25-ElCh[g]);
				H2[ElCh[g]]->Fill(Hits_Energy[g]);
				HitTime->Fill(Hits_Time[g]);

				hNHit->Fill(NHits);

				if (Hits_Time[g]<-190 && Hits_Time[g]>-170) continue;
				if (Hits_Energy[g]<5) continue;
				if (NHits>10) continue;

				H1[ElCh[g]]->Fill(Hits_Energy[g]);
				hNHit->Fill(NHits);


					for (Int_t h=g+1; h<NHits+1; h++)

					{

					time_diff[h]=Hits_Time[g]-Hits_Time[h];

					HitTimeDiff->Fill(time_diff[h]);
				
					}

				if (time_diff[g]>-1 && time_diff[g]<1) H3[ElCh[g]]->Fill(Hits_Energy[g]);



						/*if (Hits_Energy[g]!=Hits_Energy[h])

						{

						cout<<"		We compare it with hit on position "<<Hits_ChannelId[h]<<" with energy "<<Hits_Energy[h]<<endl;

						//H2[ElCh[g]]->Draw();

						timeref=Hits_Time[0];
						//cout<<"Time reference is "<<timeref<<endl;

							//if (Hits_Time>-190 && Hits_Time[h]<-170)

						//						if ((column[g]==column[h]) && (fabs(row[g]-row[h])==1)))//if on verticality
						//if (column[g]==column[h])//if on verticality

							{

							cout<<"		*°*°*°*°* We have a match, since distance is "<<fabs(row[g]-row[h])<<" *°*°*°*°* "<<endl;
							//cout<<"gref "<<gref<<endl;
							count2++;		

							
							//							HitTimeDiff->Fill(abs(timeref-Hits_Time[g]));
							if(g>0) HitTime->Fill(Hits_Time[g]);
							

							//if ((NHits>=2 && NHits<=5) && (fabs(timeref-Hits_Time[g])<0.5))

							count++;

							//canvas->cd(25-ElCh[g]);
							//H1[ElCh[g]]->SetLineColor(kRed);
							
							//H1[ElCh[g]]->Draw("");
							//H2[ElCh[g]]->Draw("SAME");
							Int_t firstbin = H1[ElCh[g]]->FindFirstBinAbove(2,1);
							Int_t lastbin = H1[ElCh[g]]->FindLastBinAbove(2,1);
							firstvalue = H1[ElCh[g]]->GetBinContent(firstbin);
							lastvalue = H1[ElCh[g]]->GetBinContent(lastbin);
							//cout<<"First bin is "<<firstbin<<" and first value is "<<firstvalue<<endl;
							//H1[ElCh[g]]->Fit("f1","Q","",firstvalue,lastvalue);

							//gref=g;

							//row[gref]=row[g];
							//column[gref]=column[g];//if column[g]==colref, second column becomes reference column

							}//closes if on verticality/time

						}//closes second for on hits

					}//closes if g!=h*/




				}//closes first for on hits

			cout<<" "<<endl;
			cout<<"Number of cosmics for the event is "<<count2<<endl;
			cout<<" "<<endl;

			}//closes if on hit number

		}//closes for on events

cout<<"Total number of cosmic rays is "<<count<<endl;

canvas2->cd();
HitTime->Draw();

canvas3->cd();
hNHit->Draw();

canvas4->cd();
HitTimeDiff->Draw();

	for (Int_t e=0;e<25;e++)

	{

	canvas->cd(25-e);
	H1[e]->Draw("");
	H2[e]->SetLineColor(kRed);
	H2[e]->Draw("SAME");
	H3[e]->SetLineColor(kGreen);
	H3[e]->Draw("SAME");
	
	}

}

