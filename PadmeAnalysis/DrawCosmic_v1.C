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

void DrawCosmic()

{

TFile *file = new TFile("output/an_t1_cosmic201907.root");
TTree *tree = new TTree();
tree = (TTree*) file->Get("PADME_FlatNT");
TCanvas *canvas  = new TCanvas ("SAC_Energy", "SAC_Energy", 100, 10, 700, 580);
TCanvas *canvas2  = new TCanvas ("Time", "Time", 100, 10, 700, 580);
TH1D *HitTimeDiff = new TH1D("Delta time between hits","Delta time between hits",10,0,10);

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
int count=0;

Double_t Hits_Energy[30];
Double_t Hits_Time[30];
Double_t Hits_ChannelId_double[30];//<----questa dimensione non va bene, ancora non so su quale variabile

int Hits_ChannelId[30];
int column[30],ElCh[25];

Double_t firstvalue,lastvalue;

tree->SetBranchAddress("NTSAC_Hits_Energy",&Hits_Energy[NHits]);
tree->SetBranchAddress("NTSAC_Hits_Time",&Hits_Time[NHits]);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",&Hits_ChannelId_double[NHits]);

		for (Int_t i=0; i<entries; i++) //questo loop gira su  eventi, per ogni evento ho più hit!

		{

		int count2=0;

		tree->GetEntry(i);

			if (NHits>=2 && NHits<=5) //first request is having only events with a certain number of hits

			{

			cout<<"=========================================================================="<<endl;
			cout<<"		Opening event n. "<<NEvent<<" : it has "<<NHits<<" hits 	"<<endl;//<----fai controllare
			cout<<"=========================================================================="<<endl;
			cout<<" "<<endl;
	

				for (Int_t g=0; g<NHits; g++)

				{

				Hits_ChannelId[g] = static_cast<int>(Hits_ChannelId_double[g]);

				int column[30],row[30],gref,colref,rowref;
				Double_t timeref;

				column[g]=Hits_ChannelId[g]/10;//column index is the first
				row[g]=Hits_ChannelId[g]%10;//row index is the second
				ElCh[g]=Hits_ChannelId[g]/10+Hits_ChannelId[g]%10*5;

				cout<<" "<<endl;
				cout<<"	------> Hit on column "<<column[g]<<" with energy "<<Hits_Energy[g]<<endl;
				cout<<" "<<endl;


					for (Int_t h=0; h<NHits; h++)

					{

						if (Hits_Energy[g]!=Hits_Energy[h])

						{

						cout<<"		We compare it with hit on column "<<column[h]<<" with energy "<<Hits_Energy[h]<<endl;

						canvas->cd(25-ElCh[g]);
						H2[ElCh[g]]->Fill(Hits_Energy[g]);
						//H2[ElCh[g]]->Draw();

						timeref=Hits_Time[0];
						//cout<<"Time reference is "<<timeref<<endl;

						//if (column[g]==colref && (fabs(rowref-row[g])==1))
							if ((column[g]==column[h]) && (fabs(row[g]-row[h])==1)))

							{

							cout<<"		*°*°*°*°* We have a match! *°*°*°*°* "<<endl;
							//cout<<"gref "<<gref<<endl;
							count2++;		

							canvas2->cd();
							HitTimeDiff->Fill(abs(timeref-Hits_Time[g]));
							HitTimeDiff->Draw();

							//if ((NHits>=2 && NHits<=5) && (fabs(timeref-Hits_Time[g])<0.5))

							count++;

							canvas->cd(25-ElCh[g]);
							//H1[ElCh[g]]->SetLineColor(kRed);
							H1[ElCh[g]]->Fill(Hits_Energy[g]);
							H1[ElCh[g]]->Draw("");
							Int_t firstbin = H1[ElCh[g]]->FindFirstBinAbove(2,1);
							Int_t lastbin = H1[ElCh[g]]->FindLastBinAbove(2,1);
							firstvalue = H1[ElCh[g]]->GetBinContent(firstbin);
							lastvalue = H1[ElCh[g]]->GetBinContent(lastbin);
							//cout<<"First bin is "<<firstbin<<" and first value is "<<firstvalue<<endl;
							//H1[ElCh[g]]->Fit("f1","Q","",firstvalue,lastvalue);

							gref=g;

							//row[gref]=row[g];
							//column[gref]=column[g];//if column[g]==colref, second column becomes reference column

							}//closes if on verticality

						}//closes second for on hits

					}//closes if g!=h

				}//closes first for on hits

			cout<<" "<<endl;
			cout<<"Number of cosmics for the event is "<<count2<<endl;
			cout<<" "<<endl;

			}//closes if on hit number

		}//closes for on events

cout<<"Total number of cosmic rays is "<<count<<endl;

}

