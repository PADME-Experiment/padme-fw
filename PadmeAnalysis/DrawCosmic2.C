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
#include <map>
#include <iostream>
#include <fstream>
//#include <string>
//#include <iterator>
//#include <vector>

void DrawCosmic()

{

TFile *file = new TFile("an_t1_cosmic201907.root");
TTree *tree = new TTree();
tree = (TTree*) file->Get("PADME_FlatNT");
TCanvas *canvas  = new TCanvas ("SAC_Energy", "SAC_Energy", 100, 10, 700, 580);
//TCanvas *canvas2  = new TCanvas ("Time", "Time", 100, 10, 700, 580);
//TH1D *HitTimeDiff = new TH1D("Delta time between hits","Delta time between hits",10,0,10);

TH1D ** H1;
H1 = new TH1D*[25];
TH1D ** H2;
H2 = new TH1D*[25];

	for (int g=0;g<25;g++)

	{

	H1[g] = new TH1D(Form("Hit Energy%d with cuts",g),Form("Hit Energy%d with cuts",g),100,0.,300.);
	H2[g] = new TH1D(Form("Hit Energy%d",g),Form("Hit Energy%d",g),100,0.,300.);

	}

canvas->Divide(5,5);

TF1 *f1 = new TF1("f1","landau",15,250);

//tree->Print();

Int_t NEvent;
Int_t NHits;

tree->SetBranchAddress("NTNEvent",&NEvent);
tree->SetBranchAddress("NTNSAC_Hits",&NHits);

Int_t entries = tree->GetEntries();

int totalhitnumber=0;
int count=0,eff;

	/*for (Int_t hitnumber=0; hitnumber<entries; hitnumber++)

	{

	tree->GetEntry(hitnumber);
	totalhitnumber += NHits;

	}

cout<<"Total number of hits is "<<totalhitnumber<<endl;*/

Double_t Hits_Energy[30];
Double_t Hits_Time[30];
Double_t Hits_ChannelId_double[30];//<----questa dimensione non va bene, ancora non so su quale variabile

int Hits_ChannelId[30];
int column[30],ElCh[25];

Double_t firstvalue,lastvalue,MPV;

tree->SetBranchAddress("NTSAC_Hits_Energy",&Hits_Energy[NHits]);
tree->SetBranchAddress("NTSAC_Hits_Time",&Hits_Time[NHits]);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",&Hits_ChannelId_double[NHits]);

		for (Int_t i=0; i<entries; i++) //questo loop gira su  eventi, per ogni evento ho più hit!

		{

		tree->GetEntry(i);


			if (NHits>=2 && NHits<=5)

			{

			cout<<" "<<endl;
			cout<<" "<<endl;

			cout<<"=============================> Event n. "<<NEvent<<", total number of hits is "<<NHits<<endl;

				for (Int_t g=0; g<=NHits; g++)

				{

				//cout<<" "<<endl;
				//cout<<" "<<endl;

				//cout<<"=============================> Event n. "<<NEvent<<", total number of hits is "<<NHits<<" on ch "<<Hits_ChannelId[g]<<endl;

				Double_t timeref;
				timeref=Hits_Time[0];

				int column[30],row[30],gref,colref,rowref;

				Hits_ChannelId[g] = static_cast<int>(Hits_ChannelId_double[g]);
				
				column[g]=Hits_ChannelId[g]/10;//column index is the first
				row[g]=Hits_ChannelId[g]%10;//row index is the second
				ElCh[g]=Hits_ChannelId[g]/10+Hits_ChannelId[g]%10*5;

				cout<<g+1<<"° hit "<<" on ch "<<Hits_ChannelId[g]<<" column = "<<column[g]<<endl;

					if (fabs(timeref-Hits_Time[g])<0.5)
					//if ((NHits>=2 && NHits<=5)) //first request is having only events with a certain number of hits in a certain time
					//if ((fabs(timeref-Hits_Time[g])<0.5))
					{

					canvas->cd(25-ElCh[g]);
					H2[ElCh[g]]->Fill(Hits_Energy[g]);
					//H2[ElCh[g]]->Draw();

					//canvas2->cd();
					//HitTimeDiff->Fill(abs(timeref-Hits_Time[g]));
					//HitTimeDiff->Draw();

						for (Int_t h=0; h<NHits; h++)

						{

							if (g!=h && (column[g]==column[h]))

							{

							eff=0;

							count++;
							eff++;

							//cout<<"Event n. "<<NEvent<<" has: "<<g+1<<"° hit "<<" with energy "<<Hits_Energy[g]<<" at time "<<Hits_Time[g]<<" on ch "<<Hits_ChannelId[g]<<" distance between rows "<<fabs(rowref-row[g])<<" electronic channel "<<ElCh[g]<<" distance between columns is "<<fabs(colref-column[g])<<endl;

							cout<<"--------Here I found that hit "<<g+1<<" and hit "<<h+1<<" have the same column "<<endl;

							/*canvas->cd(25-ElCh[g]);
							//H1[ElCh[g1]]->SetLineColor(kRed);
							H1[ElCh[g]]->Fill(Hits_Energy[g]);
							H1[ElCh[g]]->Draw("");
							Int_t firstbin = H1[ElCh[g]]->FindFirstBinAbove(2,1);
							Int_t lastbin = H1[ElCh[g]]->FindLastBinAbove(2,1);
							firstvalue = H1[ElCh[g]]->GetBinContent(firstbin);
							lastvalue = H1[ElCh[g]]->GetBinContent(lastbin);*/

							}//closes the if on verticality

						}//closes the second for on hit number

					}//closes if on hit time
	
				//cout<<"Count "<<count<<endl;
				//cout<<"Event n. "<<NEvent<<" has "<<count<<" match(es)"<<endl;

				

						if (eff>=1)

						{

						canvas->cd(25-ElCh[g]);
						//H1[ElCh[g1]]->SetLineColor(kRed);
						H1[ElCh[g]]->Fill(Hits_Energy[g]);
						H1[ElCh[g]]->Draw("");
						Int_t firstbin = H1[ElCh[g]]->FindFirstBinAbove(2,1);
						Int_t lastbin = H1[ElCh[g]]->FindLastBinAbove(2,1);
						firstvalue = H1[ElCh[g]]->GetBinContent(firstbin);
						lastvalue = H1[ElCh[g]]->GetBinContent(lastbin);
						
						}

				}//closes the first for on hits

			cout<<"We have found "<<eff<<" match(es) in event n. "<<NEvent<<endl;

			}//closes if on hit number

		}//closes for on events

	for (Int_t k=0;k<25;k++)

	{

	H1[k]->Fit("f1","Q","",firstvalue,lastvalue);
	MPV=f1->GetParameter(1);
	cout<<"MPV for ch "<<k<<" is "<<MPV<<endl;

	}

cout<<"Number of cosmic rays is "<<count<<endl;

}

