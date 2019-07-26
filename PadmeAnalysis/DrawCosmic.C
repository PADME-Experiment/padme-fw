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

TFile *file = new TFile("prova.root");
TTree *tree = new TTree();
tree = (TTree*) file->Get("PADME_FlatNT");
TCanvas *canvas  = new TCanvas ("SAC_Energy", "SAC_Energy", 100, 10, 700, 580);
TH1D *HitEnergy = new TH1D("Hit Energy","Hit Energy",10,0,1000);

/*TH1D ** H1;
H1 = new TH1D*[25];

	for (int g=0;g<25;g++)

	{

	H1[g] = new TH1D("Hit Energy","Hit Energy",10,0.,700.);

	}*/

//canvas->Divide(5,5);

//tree->Print();

Int_t NEvent;
Int_t NHits;

tree->SetBranchAddress("NTNEvent",&NEvent);
tree->SetBranchAddress("NTNSAC_Hits",&NHits);

Int_t entries = tree->GetEntries();

int totalhitnumber=0;

	/*for (Int_t hitnumber=0; hitnumber<entries; hitnumber++)

	{

	tree->GetEntry(hitnumber);
	totalhitnumber += NHits;

	}

cout<<"Total number of hits is "<<totalhitnumber<<endl;*/

Double_t Hits_Energy[30];
Double_t Hits_Time[30];
Double_t Hits_ChannelId[30];//<----questa dimensione non va bene, ancora non so su quale variabile

tree->SetBranchAddress("NTSAC_Hits_Energy",&Hits_Energy[NHits]);
tree->SetBranchAddress("NTSAC_Hits_Time",&Hits_Time[NHits]);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",&Hits_ChannelId[NHits]);

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


		for (Int_t i=0; i<entries; i++) //questo loop gira su  eventi, per ogni evento ho più hit!

		{

		tree->GetEntry(i);
		//cout<<"Event n. "<<NEvent<<" has "<<NHits<<endl;//<----fai controllare


			for (Int_t g=0; g<NHits; g++)

			{

				//if(Hits_Time[g]>=174.43 && Hits_Time[g]=<183.57)

				{

				//tree->GetEntry(g);
				//cout<<"Mortacci tua"<<endl;
				//cout<<"Hit energy is "<<Hits_Energy[g]<<endl;
				cout<<"Event n. "<<NEvent<<" has "<<g<<" hits "<<" with energy "<<Hits_Energy[g]<<" at time "<<Hits_Time[g]<<" on ch "<<Hits_ChannelId[g]<<endl;

				}

			}


		//cout<<"Number of events is "<<NEvent<<endl;
		//cout<<"Number of hits is "<<NHits<<endl;
		//cout<<"Dov'è il problema? "<<count<<endl;

		}

//cout<<"Number of cosmic rays is "<<count<<endl;

}

