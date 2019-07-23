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

TFile *file = new TFile("/home/taruggi/padme-fw_220719/PadmeAnalysis/an_cosmic20190721_171912.root");
TCanvas *canvas  = new TCanvas ("SAC_Energy", "SAC_Energy", 100, 10, 700, 580);
//TH1D *HitEnergy = new TH1D("Hit Energy","Hit Energy",10,0,1000);

TH1D ** H1;
H1 = new TH1D*[25];

	for (int g=0;g<25;g++)

	{

	H1[g] = new TH1D("Hit Energy","Hit Energy",10,0.,700.);

	}


TTree *tree = new TTree();
tree = (TTree*) file->Get("PADME_FlatNT");
canvas->Divide(5,5);

//tree->Print();

Int_t NTNSAC_Hits;
Double_t NTSAC_Hits_Energy;
Double_t NTSAC_Hits_Time;
Double_t NTSAC_Hits_ChannelId;

tree->SetBranchAddress("NTNSAC_Hits",&NTNSAC_Hits);
tree->SetBranchAddress("NTSAC_Hits_Energy",NTSAC_Hits_Energy);
tree->SetBranchAddress("NTSAC_Hits_Time",NTSAC_Hits_Time);
tree->SetBranchAddress("NTSAC_Hits_ChannelId",NTSAC_Hits_ChannelId);

//tree->Print();

Int_t entries = tree->GetEntries();
//cout<<"Number of entries is "<<entries<<endl;


		for (Int_t i=0; i<entries; i++)

		{

		tree->GetEntry(i);

		}


}

