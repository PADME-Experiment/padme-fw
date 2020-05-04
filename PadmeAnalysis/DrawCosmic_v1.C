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
TFile *file = new TFile("cosmic/an_t1_cosmic_160120_1600V.root");

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

	H1[g] = new TH1D(Form("Peak amplitude ch %d",g),Form("Peak amplitude ch %d",g),200,0.,800.);
	H2[g] = new TH1D(Form("Peak amplitude ch %d, time selection",g),Form("Peak amplitude ch %d, time selection",g),200,0.,600.);
	H3[g] = new TH1D(Form("Peak amplitude ch %d, delta time selection",g),Form("Peak amplitude ch %d, delta time selection",g),200,0.,800.);
	H4[g] = new TH1D(Form("Peak amplitude ch %d, verticality selection",g),Form("Peak amplitude ch %d, verticality selection",g),200,0.,800.);
	
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
int count=0;

Double_t Hits_Energy[200];
Double_t Hits_Time[200];
Double_t Hits_ChannelId_double[200];//<----questa dimensione non va bene, ancora non so su quale variabile

int Hits_ChannelId[200];
int column[200],row[200],ElCh[200],skip[30][2];

TBranch *b_NTSAC_Hits_Energy;   //!
TBranch *b_NTSAC_Hits_Time;   //!
TBranch *b_NTSAC_Hits_ChannelId;   //!

Double_t firstvalue,lastvalue,time_diff[200],col_diff[200],row_diff[200],MPV[25],CC[25];

tree->SetBranchAddress("NTNEvent",&NEvent);
tree->SetBranchAddress("NTNSAC_Hits",&NHits);

tree->SetBranchAddress("NTSAC_Hits_Energy",Hits_Energy,&b_NTSAC_Hits_Energy);
tree->SetBranchAddress("NTSAC_Hits_Time",Hits_Time,&b_NTSAC_Hits_Time);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",Hits_ChannelId_double,&b_NTSAC_Hits_ChannelId);

		for (Int_t i=0; i<entries; i++) //questo loop gira su  eventi, per ogni evento ho più hit!

		{

		//int count2=0;

		//cout<<"NHits = "<<NHits<<endl;

		tree->GetEntry(i);

			if (NHits>=2) //first request is having only events with a certain number of Hits
			  //rimuovere il taglio superiore perche' pericoloso
			{

			cout<<"=========================================================================="<<endl;
			cout<<"		Opening event n. "<<NEvent<<" : it has "<<NHits<<" Hits 	 "<<endl;
			cout<<"=========================================================================="<<endl;
			cout<<" "<<endl;
	

				for (Int_t g=0; g<NHits; g++) //qui riempio soltanto

				{
				  //  contare gli hit in tempo

				Hits_ChannelId[g] = static_cast<int>(Hits_ChannelId_double[g]);

				column[g]=Hits_ChannelId[g]/10;//column index is the first
				row[g]   =Hits_ChannelId[g]%10;//row index is the second
				ElCh[g]  =Hits_ChannelId[g]/10+Hits_ChannelId[g]%10*5;

				skip[g][0]=g;
				skip[g][1]=0; //il primo elemento di skip è la prima hit con indice g
				
					for (int d=0;d<2;d++)

					{

					//cout<<skip[g][d]<<endl;

					}

				//cout<<"$$$$$$$$$$$$$$$$$$$ Energy "<<Hits_Energy[g]<<" , time "<<Hits_Time[g]<<endl;

				}

				for (Int_t g=0; g<NHits; g++)

				{

				int count2=1,count2_vec[20]={0};

				H1[ElCh[g]]->Fill(Hits_Energy[g]);//senza tagli
				cout<<"Hit "<<g<<", column "<<column[g]<<" , row "<<row[g]<<endl;

				HitTimeEnergy[ElCh[g]]->Fill(Hits_Time[g],Hits_Energy[g]);

				hNHit->Fill(NHits);

				if (Hits_Time[g]<-195 || Hits_Time[g]>-167) {EmptyEvent[ElCh[g]]->Fill(Hits_Energy[g]);}

				if (Hits_Time[g]>-195 && Hits_Time[g]<-167) {H2[ElCh[g]]->Fill(Hits_Energy[g]);}//taglio in tempo


					for (Int_t h=g+1; h<NHits; h++)

					{

					if (skip[h][1]==1) continue;

					time_diff[h]=Hits_Time[g]-Hits_Time[h];

					col_diff[h]=abs(column[g]-column[h]);
					row_diff[h]=abs(row[g]-row[h]);
					cout<<"------->Hit "<<h<<" , column "<<column[h]<<" , row "<<row[h]<<" , skip "<<skip[h][1]<<endl;

					HitTimeDiff->Fill(time_diff[h]);

					//if ((col_diff[h]==0)||((col_diff[h]==1)&&(row_diff[h]==0))) {count2++;cout<<"+1 => stiamo a "<<count2<<endl;}
					
						if (col_diff[h]==0)

						{

						count2++;
						//count2_vec[h]=count2++;
						skip[h][1]=1;
						skip[h-1][1]=1;
						cout<<"One match for hit "<<h<<" => score "<<count2<<" , skip "<<skip[h][1]<<endl;

						}
							
					}//qui chiudo il for sulle hit h

					//a questo punto, count2 ha un certo valore, e alle hit in colonna è assegnato il valore di skip=1. Devo sommare le hit con skip=1!

						for (Int_t j=0;j<count2;j++)

						{

							if ((count2>=3)&&(skip[j][1]==1))

							{

							H4[ElCh[j]]->Fill(Hits_Energy[j]);
							cout<<"********************Sto riempiendo"<<endl;

							}

						}

				count2=1;

				if (time_diff[g]>-0.4 && time_diff[g]<1) H3[ElCh[g]]->Fill(Hits_Energy[g]);
				
				}//qui chiudo il for sulle hit g

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

	canvas->cd(25-e);
	//canvas->cd();
	H1[e]->Draw("");//senza tagli
	H2[e]->SetLineColor(kRed);
	//H2[e]->Draw("SAME"); //taglio in tempo
	H3[e]->SetLineColor(kGreen);
	//H3[e]->Draw("SAME");//taglio deltatime
	H4[e]->SetLineColor(kAzure);
	H4[e]->Draw("SAME");//taglio su verticalità
	EmptyEvent[e]->SetLineColor(kBlack);
	EmptyEvent[e]->Draw("SAME");

	EmptyEvent[e]->Fit("f2","0","",0,100);

	//Int_t firstbin = H3[e]->FindFirstBinAbove(1,1);
	Int_t lastbin = H2[e]->FindLastBinAbove(1,1);
	//firstvalue = H3[e]->GetBinContent(firstbin);
	//firstvalue = (f2->GetParameter(2))*5+f2->GetParameter(1);
	firstvalue =f2->GetParameter(1) + (f2->GetParameter(2))*5;
	lastvalue = H1[e]->GetBinContent(lastbin);
	//cout<<"First bin is "<<firstbin<<" and first value is "<<firstvalue<<endl;
	//f1->SetLineColor(kAzure);
	H4[e]->Fit("f1","Q","",firstvalue,1000);

	MPV[e]=f1->GetParameter(1);
	CC[e]=28.11/(MPV[e]);//attenzione, qui 28.11 è un'energia, nelle distribuzioni c'è l'ampiezza in mV
	cout<<"MPV for ch "<<e<<" is "<<MPV[e]<<endl;
	//cout<<e%5*10+e/5<<"	"<<CC[e]<<endl;

	canvas2->cd(25-e);
	HitTimeEnergy[e]->Draw("colz");

	//canvas5->cd(25-e)->SetLogy();
	//EmptyEvent[e]->Draw("");

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

