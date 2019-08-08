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

TFile *file = new TFile("an_t1_cosmic201907.root");
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

Double_t firstvalue,lastvalue;

tree->SetBranchAddress("NTSAC_Hits_Energy",&Hits_Energy[NHits]);
tree->SetBranchAddress("NTSAC_Hits_Time",&Hits_Time[NHits]);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",&Hits_ChannelId_double[NHits]);

//tree->Print();

//tree->Draw("NTNEvent","","goff");
//Int_t * NEvent = tree->GetV1();
/*tree->Draw("NTNSAC_Hits","","goff");
Int_t * NHits = tree->GetV1();
tree->Draw("NTSAC_Hits_Energy","","goff");
Double_t * Energy = tree->GetV1();
tree->Draw("NTSAC_Hits_Time","","goff");
Double_t * Time = tree->GetV1();
tree->Draw("NTSAC_Hits_ChannelId","","goff");
Double_t * ChannelId = tree->GetV1();*/

//Int_t n = tree->GetSelectedRows();

	/*for (int f=0;f<25;f++)

	{

	Hits_ChannelId[f] = static_cast<int>(Hits_ChannelId_double[f]);

	//int column[30];
	column[f]=Hits_ChannelId[f]/10;
	ElCh[f]=Hits_ChannelId[f]/4+Hits_ChannelId[f]%4;
	cout<<"Electronic channel is "<<ElCh<<endl;

	}*/


		for (Int_t i=0; i<entries; i++) //questo loop gira su  eventi, per ogni evento ho più hit!

		{

		tree->GetEntry(i);
		//cout<<"Event n. "<<NEvent<<" has "<<NHits<<endl;//<----fai controllare


			for (Int_t g=0; g<NHits; g++)

			{

				if (NHits>=2 && NHits<=5) //first request is having only events with a certain number of hits

				{

				Hits_ChannelId[g] = static_cast<int>(Hits_ChannelId_double[g]);

				int column[30],row[30],gref,colref,rowref,count2=0;
				Double_t timeref;

				column[g]=Hits_ChannelId[g]/10;//column index is the first
				row[g]=Hits_ChannelId[g]%10;//row index is the second

				ElCh[g]=Hits_ChannelId[g]/10+Hits_ChannelId[g]%10*5;

				canvas->cd(25-ElCh[g]);
				H2[ElCh[g]]->Fill(Hits_Energy[g]);
				//H2[ElCh[g]]->Draw();

				timeref=Hits_Time[0];
				//cout<<"Time reference is "<<timeref<<endl;

				//colref=column[0];rowref=row[0];

				//if (g==0){colref=column[0];rowref=row[0];gref=0;}//18:13
					if (g==0) gref=0;//in this way, you skip hit n.0!

					//if (column[g]==colref && (fabs(rowref-row[g])==1))
					if ((g==0) || ((column[g]==column[gref]) && (fabs(row[gref]-row[g])==1)))

					{
					//cout<<"Ciao, sono lo step g = "<<g<<endl;
					//cout<<"gref "<<gref<<endl;
					count2++;		

					canvas2->cd();
					HitTimeDiff->Fill(abs(timeref-Hits_Time[g]));
					HitTimeDiff->Draw();

					//if ((NHits>=2 && NHits<=5) && (fabs(timeref-Hits_Time[g])<0.5))
					//if ((fabs(timeref-Hits_Time[g])<0.5))

					//{			

					//cout<<"Event n. "<<NEvent<<" has: "<<g+1<<"° hit "<<" with energy "<<Hits_Energy[g]<<" at time "<<Hits_Time[g]<<" on ch "<<Hits_ChannelId[g]<<" distance between rows "<<fabs(rowref-row[g])<<" electronic channel "<<ElCh[g]<<" distance between columns is "<<fabs(colref-column[g])<<endl;

					cout<<"Event n. "<<NEvent<<" has: "<<g+1<<"° hit "<<" on ch "<<Hits_ChannelId[g]<<" column = "<<column[g]<<" row = "<<row[g]<<" distance between rows "<<fabs(row[gref]-row[g])<<" distance between columns is "<<fabs(column[gref]-column[g])<<endl;

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

					}

					//else {column[gref]=column[g];row[gref]=row[g];}
					else {gref=g;}

				}//closes if on hit number
	
			//cout<<"Count "<<count2++<<endl;

			}//closes for on hits

		}//closes for on events

cout<<"Number of cosmic rays is "<<count<<endl;

}

