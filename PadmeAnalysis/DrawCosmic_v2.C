//Questa è la versione di DrawCosmic che ho corretto con Gabriele

#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TH2D.h"
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

void DrawCosmic_v2()

{

//TFile *file = new TFile("single_positron/an_t1_ch18_350MeV_single_pos_1250V.root");//<-con questo crasha
TFile *file = new TFile("cosmic/an_t1_cosmic_20200712_1500V.root");

TTree *tree = new TTree();
tree = (TTree*) file->Get("PADME_FlatNT");
TCanvas *canvas  = new TCanvas ("SAC_Amplitude", "SAC_Amplitude", 100, 10, 700, 580);
TCanvas *canvas2  = new TCanvas ("Time/Amplitude", "Time/Amplitude", 100, 10, 700, 580);
TCanvas *canvas3  = new TCanvas ("NHit", "NHit", 100, 10, 700, 580);
TCanvas *canvas4  = new TCanvas ("Time diff", "Time diff", 100, 10, 700, 580);
//TCanvas *canvas5  = new TCanvas ("Empty events", "Empty events", 100, 10, 700, 580);

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

	H1[g] = new TH1D(Form("Peak amplitude ch %d",g),Form("Peak amplitude ch %d",g),100,0.,1000.);
	H2[g] = new TH1D(Form("Peak amplitude ch %d, time selection",g),Form("Peak amplitude ch %d, time selection",g),200,0.,800.);
	H3[g] = new TH1D(Form("Peak amplitude ch %d, delta time selection",g),Form("Peak amplitude ch %d, delta time selection",g),200,0.,800.);
	H4[g] = new TH1D(Form("Peak amplitude ch %d, verticality selection",g),Form("Peak amplitude ch %d, verticality selection",g),100,0.,40.);
	
	}

canvas->Divide(5,5);

TH2D ** HitTimeEnergy;
HitTimeEnergy = new TH2D*[25];
TH1D ** EmptyEvent;
EmptyEvent = new TH1D*[25];

	for (int g=0;g<25;g++)

	{

	HitTimeEnergy[g] = new TH2D(Form("Hit Time vs amplitude for %d",g),Form("Hit Time vs hit amplitude for %d",g),200,-300.,250.,200,0.,1000.);
	EmptyEvent[g] = new TH1D(Form("Hit amplitude %d of the empty events",g),Form("Hit amplitude %d  of the empty events",g),200,0.,1000.);

	}

canvas2->Divide(5,5);
//canvas5->Divide(5,5);

TF1 *f1 = new TF1("f1","landau",15,1000);
TF1 *f2 = new TF1("f2","landau",0,15);

//tree->Print();

Int_t NEvent;
Int_t NHits;

Int_t entries = tree->GetEntries();

//cout<<"Il numero di entries è "<<entries<<endl;

int totalhitnumber=0;

Double_t Hits_Energy[200];
Double_t Hits_Time[200];
Double_t Hits_ChannelId_double[200];//<----questa dimensione non va bene, ancora non so su quale variabile

int Hits_ChannelId[200];
int column[200],row[200],ElCh[200],corr[10][3]; //corr: primo indice hit, secondo indice 0/1, terzo indice colonna

TBranch *b_NTSAC_Hits_Energy;   //!
TBranch *b_NTSAC_Hits_Time;   //!
TBranch *b_NTSAC_Hits_ChannelId;   //!

Double_t firstvalue,lastvalue,time_diff[200],col_diff[200],row_diff[200],MPV[25],err[25],CC[25];

tree->SetBranchAddress("NTNEvent",&NEvent);
tree->SetBranchAddress("NTNSAC_Hits",&NHits);

tree->SetBranchAddress("NTSAC_Hits_Energy",Hits_Energy,&b_NTSAC_Hits_Energy);
tree->SetBranchAddress("NTSAC_Hits_Time",Hits_Time,&b_NTSAC_Hits_Time);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",Hits_ChannelId_double,&b_NTSAC_Hits_ChannelId);

int colCount[5]={0};
int chCount[25]={0};
int chCountRep[25]={};

		for (Int_t i=0; i<entries; i++) //questo loop gira su  eventi, per ogni evento ho più hit!

		{

		//int count=0;

		//cout<<"NHits = "<<NHits<<endl;

		tree->GetEntry(i);

		std::map<int, double> ChAmp;
		int n_in_col[5]={};


			if (NHits>=2) //first request is having only events with a certain number of Hits
			  //rimuovere il taglio superiore perche' pericoloso
			{

			/*cout<<"=========================================================================="<<endl;
			cout<<"		Opening event n. "<<NEvent<<" : it has "<<NHits<<" Hits 	 "<<endl;
			cout<<"=========================================================================="<<endl;
			cout<<" "<<endl;*/

			//cout<<"Event "<<i<<" , hits energy "<<Hits_Energy[i]<<endl;
	

				for (Int_t g=0; g<NHits; g++) //qui riempio soltanto

				{
				  //  contare gli hit in tempo

				Hits_ChannelId[g] = static_cast<int>(Hits_ChannelId_double[g]);

				column[g] = Hits_ChannelId[g]/10;//column index is the first
				row[g]    = Hits_ChannelId[g]%10;//row index is the second
				ElCh[g]   = Hits_ChannelId[g]/10 + Hits_ChannelId[g]%10*5;

				std::map<int,double>::iterator it2;
				it2 = ChAmp.find(ElCh[g]);
				//if(it2!=ChAmp.end()){cout<<"Event "<<NEvent<<" ch "<<ElCh[g]<<endl;chCountRep[ElCh[g]]++;}
				if(it2!=ChAmp.end()){chCountRep[ElCh[g]]++;}
				else {n_in_col[column[g]]++;} //questo array incrementa di 1 quando compare una certa colonna
				ChAmp[ElCh[g]]=Hits_Energy[g];

				//cout<<"Hit "<<g<<" Electronic channel "<<ElCh[g]<<" , amplitude "<<ChAmp[ElCh[g]]<<" ("<<Hits_Energy[g]<<")"<<" col "<<column[g]<<"(n_in_col "<<n_in_col[column[g]]<<")"<<" row "<<row[g]<<" ch_id "<<Hits_ChannelId[g]<<endl;

				H1[ElCh[g]]->Fill(Hits_Energy[g]);
				
				}

				for (Int_t f=0; f<5; f++)

				{

					if (n_in_col[f]>4)

					{

						//cout<<"Amplitude "<<ChAmp[ElCh[f]]<<" on column "<<column[f]<<endl;

						//H4[ElCh[f]]->Fill(Hits_Energy[f]);

						//for (Int_t ff=0; ff<n_in_col[f]; ff++) {
						colCount[f]++;
						std::map<int, double>::iterator it;
						for(it=ChAmp.begin();it!=ChAmp.end();++it){
							if((it->first)%5 == f){
								//std::cout<<"\t"<<it->first<<" "<<endl;
								//cout<<"--------------------->I'm filling histogram with "<<it->second<<" on column "<<(it->first)%5<<" (electronic channel "<<it->first<<")"<<endl;
								H4[it->first]->Fill(it->second);
								chCount[it->first]++;
							}
						}

					}
				}

			}//closes if on hit number

		}//closes for on events

//cout<<"Total number of count rays is "<<count<<endl;

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
	H1[e]->Draw("");//senza tagli
	H2[e]->SetLineColor(kRed);
	//H2[e]->Draw("SAME"); //taglio in tempo
	H3[e]->SetLineColor(kGreen);
	//H3[e]->Draw("SAME");//taglio deltatime
	H4[e]->SetLineColor(kRed);
	H4[e]->Draw("SAME");//taglio su verticalità
	EmptyEvent[e]->SetLineColor(kBlack);
	//EmptyEvent[e]->Draw("SAME");

	//EmptyEvent[e]->Fit("f2","0","",0,100);

	//Int_t firstbin = H3[e]->FindFirstBinAbove(1,1);
	//Int_t lastbin = H2[e]->FindLastBinAbove(1,1);
	//firstvalue = H3[e]->GetBinContent(firstbin);
	//firstvalue = (f2->GetParameter(2))*5+f2->GetParameter(1);
	//firstvalue =f2->GetParameter(1) + (f2->GetParameter(2))*5;
	//lastvalue = H1[e]->GetBinContent(lastbin);
	//cout<<"First bin is "<<firstbin<<" and first value is "<<firstvalue<<endl;
	f1->SetLineColor(kAzure);
	H4[e]->Fit("f1","Q","",100,1000);

	MPV[e]=f1->GetParameter(1);
	err[e]=f1->GetParameter(2);
	//CC[e]=28.11/(MPV[e]);//attenzione, qui 28.11 è un'energia, nelle distribuzioni c'è l'ampiezza in mV
	cout<<"MPV for ch "<<e<<" is "<<MPV[e]<<" with an error of "<<err[e]<<endl;
	//cout<<e%5*10+e/5<<"	"<<CC[e]<<endl;

	canvas2->cd(25-e);
	HitTimeEnergy[e]->Draw("colz");

	//canvas5->cd(25-e)->SetLogy();
	//EmptyEvent[e]->Draw("");

	}

//for(int ij=0;ij<5;++ij){cout<<"col "<<ij<<": "<<colCount[ij]<<endl;}
//for(int jj=0;jj<25;jj++){cout<<"ch "<<jj<<" : "<<chCount[jj]<<" nRep "<<chCountRep[jj]<<endl;}

}

