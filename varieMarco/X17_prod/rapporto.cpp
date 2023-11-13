#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
#include <TStyle.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TRandom.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <regex>

#include "analyzer.h"

#include <iostream>
#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

void rapporto(const char* H1, const char* H2, const char*nameH1, const char* nameH2) //const char* inputFile1, const char* inputFile2, const char* histoName1, const char* histoName2
{
    // Apri il file di input
    TFile* file1 = new TFile(H1, "READ");
    TFile* file2 = new TFile(H2, "READ");

    // Verifica se il file è stato aperto correttamente
    if (!file1 || file1->IsZombie()) {
        std::cout << "Errore nell'apertura del file1 di input." << std::endl;
        return;
    }
    if (!file2 || file2->IsZombie()) {
        std::cout << "Errore nell'apertura del file2 di input." << std::endl;
        return;
    }

    // Ottieni l'oggetto istogramma dal file di input
    TH1F* histogram1 = (TH1F*)file1->Get(nameH1);
    TH1F* histogram2 = (TH1F*)file2->Get(nameH2);
    // TH1F* hratio = new TH1F("hratio", "hratio", 7, 199, 213);
    // histogram1->Draw();
    // histogram2->Draw();    

    // Crea un nuovo istogramma per il rapporto
    TH1F* h1clone = (TH1F*)histogram1->Clone();
    h1clone->SetName("h1clone");
    h1clone->SetTitle("h1clone");
    // Calcola il rapporto bin per bin
    double a = 0;
    for (int bin = 0; bin < 10; ++bin) {
        a = 200 + bin*2;
        double content1 = histogram1->GetBinContent(bin);
        cout<<content1<<endl;
        double content2 = histogram2->GetBinContent(bin);
        cout<<content2<<endl;
        double error1 = histogram1->GetBinError(bin);
        double error2 = histogram2->GetBinError(bin);
        double ratio = content1 / content2;
        if(TMath::IsNaN(ratio)) ratio = 0;
        double error = (content2 != 0.0) ? (ratio * TMath::Sqrt(TMath::Power(error1 / content1, 2) + TMath::Power(error2 / content2, 2))) : 0.0;
        // hratio->Fill(a, ratio);
        h1clone->SetBinError(bin, error);
    }

    // Crea un canvas per disegnare gli istogrammi
    TCanvas* canvas = new TCanvas("canvas", "Histograms", 800, 600);

    // Disegna il primo istogramma
    // histogram1->Draw();
    histogram2->Draw();
    // histogram2->SetMarkerColor(kBlue);
    // hratio->Draw();

    // Disegna il rapporto come TGraphErrors
    TGraphErrors* ratioGraph = new TGraphErrors(h1clone);
    ratioGraph->SetMarkerStyle(20);
    ratioGraph->SetMarkerSize(0.8);
    ratioGraph->Draw("same p");

    // Visualizza il canvas
    canvas->Update();


}

