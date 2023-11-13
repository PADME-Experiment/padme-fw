#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TStyle.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TRandom.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <regex>

using namespace std;

void rapporto(const char*nameH1, const char* nameH2){ //const char* inputFile1, const char* inputFile2, const char* histoName1, const char* histoName2
    // Apri il file di input
    TFile* file1 = new TFile("/home/mancinima/padme-fw/varieMarco/NoAngleBelow.root", "READ");
    TFile* file2 = new TFile("/home/mancinima/padme-fw/varieMarco/AngleBelow4_1mRad.root", "READ");

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

void CreateRatioGraph() {
    // Check if the input graphs are valid
    
    TFile* File1 = new TFile("/home/mancinima/padme-fw/varieMarco/NoAngleBelow.root", "READ");
    TFile* File2 = new TFile("/home/mancinima/padme-fw/varieMarco/AngleBelow4_1mRad.root", "READ");

    TGraph* graph1 = (TGraph*)File1->Get("Graph_from_hSMYield");
    TGraph* graph2 = (TGraph*)File2->Get("Graph_from_hSMYield");

    // Get the number of points in the input graphs
    static const int numPoints = graph1->GetN();
    // if (numPoints != graph2->GetN()) {
    //     std::cerr << "Graphs have different number of points!" << std::endl;
    //     return;
    // }

    // Create arrays to store x and y values of the ratio
    double xValues[numPoints];
    double yValues[numPoints];
    double exValues[numPoints];
    double eyValues[numPoints];

    // Calculate the ratio of y-values and store in yValues array
    for (int i = 0; i < numPoints; ++i) {
        double x1, y1, x2, y2;
        graph1->GetPoint(i, x1, y1);
        graph2->GetPoint(i, x2, y2);
        
        // Calculate ratio (avoid division by zero)
        if (y2 != 0) {
            xValues[i] = x1; // Assuming x-values are the same for both graphs
            yValues[i] = y1 / y2;
            exValues[i] = 0;
            eyValues[i] = y1 / y2 * 0.03;
        } else {
            xValues[i] = x1;
            yValues[i] = 0; // Set ratio to 0 if denominator is 0
            exValues[i] = 0;
            eyValues[i] = 0;
        }
    }

    // Create a new TGraph for the ratio
    TGraphErrors* ratioGraph = new TGraphErrors(7, xValues, yValues, exValues, eyValues);

    // Plot the ratio graph
    TCanvas* c1 = new TCanvas("c1", "Ratio Graph", 800, 600);
    ratioGraph->SetMarkerStyle(20);
    ratioGraph->SetMarkerColor(kBlue);
    ratioGraph->SetTitle("Ratio Graph");
    ratioGraph->GetXaxis()->SetTitle("X-Axis Title");
    ratioGraph->GetYaxis()->SetTitle("Y-Axis Title");
    ratioGraph->Draw("AP"); // "AP" option for markers and connecting lines

    // Optionally, you can save the plot to a file
    // c1->SaveAs("RatioGraph.png");

    // Clean up
    // delete c1;
    // delete ratioGraph;
}

void diffHistogram() {
    // Read data from the first file
    // std::ifstream file1("/home/mancinima/padme-fw/varieMarco/EnergyPoints.txt");
    std::ifstream file1("/home/mancinima/padme-fw/varieMarco/ExcelBeamEnergies.txt");
    std::vector<double> V1, V2, V3, V4, V5, V5a, V6, V7;
    double value1, value2, value3, value4, value5, value6, value7;
    while (file1 >> value1 >> value2 >> value3 >> value4 >> value5 >> value6 >> value7) {
        // if(value1 == char("#")) continue;
        cout<< "value1 "<< value1 << endl;
        V1.push_back(value1);
        cout<< "value2 "<< value2 << endl;
        V2.push_back(value2);
        cout<< "value3 "<< value3 << endl;
        V3.push_back(value3);
        cout<< "value4 "<< value4 << endl;
        V4.push_back(value4);
        cout<< "value5 "<< value5 << endl;
        V5.push_back((30+28.42*value4)*0.0551318);
        // V5a.push_back((16.22+28.42*value4)*0.0551318);
        cout<< "value6 "<< value6 << endl;
        V6.push_back(value6);
        cout<< "value7 "<< value7 << endl;
        V7.push_back(value7);
    }
    file1.close();

    // Read data from the second file (first column only)
    // std::ifstream file2("/home/mancinima/padme-fw/varieMarco/NPotE.txt");
    // std::vector<double> V2;
    // double value2, value3;
    // while (file2 >> value2 >> value3) {
    //     V2.push_back(value2);
    // }
    // file2.close();

    // Calculate differences
    std::vector<double> differences3, differences5, differences6;
    for (size_t i = 0; i < V1.size() && i < V7.size(); ++i) {
        differences3.push_back(V7[i] - V3[i]);
        differences5.push_back(V7[i] - V5[i]);
        differences6.push_back(V3[i] - V5[i]);
    }

    // Create an histogram or a TGraph
    TGraph *g3 = new TGraph(V1.size(), &V1[0], &V3[0]);
    g3->SetTitle(" E_{Beam}[B]");
    g3->SetMarkerStyle(20); 
    g3->SetMarkerSize(0.8);
    g3->SetMarkerColor(kBlue);
    TGraph *g5 = new TGraph(V1.size(), &V1[0], &V5[0]);
    g5->SetTitle(" E_{Beam}[I]");
    g5->SetMarkerStyle(20); 
    g5->SetMarkerSize(0.8);
    g5->SetMarkerColor(kRed);
    TGraph *g6 = new TGraph(V1.size(), &V1[0], &V6[0]);
    g6->SetTitle(" E_{Beam} DB");
    g6->SetMarkerStyle(20); 
    g6->SetMarkerSize(0.8);
    g6->SetMarkerColor(kGreen);
    TGraph *g7 = new TGraph(V1.size(), &V1[0], &V7[0]);
    g7->SetTitle(" E_{Beam} nominal");
    g7->SetMarkerStyle(20); 
    g7->SetMarkerSize(0.8);
    g7->SetMarkerColor(kBlack);

    TMultiGraph *mg0 = new TMultiGraph();
    mg0->Add(g3);
    mg0->Add(g5);
    // mg0->Add(g6);
    // mg0->Add(g7);

    TGraph* gEB0 = new TGraph(V1.size(), &V2[0], &V7[0]);
    gEB0->SetTitle(" E_{Beam}[MeV] vs B[G]; run ; E_{Beam} [MeV] ");
    gEB0->SetMarkerStyle(20); 
    gEB0->SetMarkerSize(0.8);
    gEB0->SetMarkerColor(kBlack);
    TGraph* gEB1 = new TGraph(23, &V2[0], &V7[0]);
    gEB1->SetTitle(" E_{Beam}[MeV] vs B[G] - first SCAN; run ; E_{Beam} [MeV] ");
    gEB1->SetMarkerStyle(20); 
    gEB1->SetMarkerSize(0.8);
    gEB1->SetMarkerColor(kBlue);
    TGraph* gEB2 = new TGraph(24, &V2[29], &V7[29]);
    gEB2->SetTitle(" E_{Beam}[MeV] vs B[G] - first SCAN; run ; E_{Beam} [MeV] ");
    gEB2->SetMarkerStyle(20); 
    gEB2->SetMarkerSize(0.8);
    gEB2->SetMarkerColor(kRed);

    TMultiGraph *mg1 = new TMultiGraph();
    mg1->Add(gEB1);
    mg1->Add(gEB2);

    TGraph* gr3 = new TGraph(V1.size(), &V1[0], &differences3[0]);
    gr3->SetTitle("E^{PV}_{Beam} - E^{B-field}_{Beam}"); //(" Differences between V7 and V3 vs run; run ; E_{Beam} [MeV] ");
    gr3->SetMarkerStyle(20); 
    gr3->SetMarkerSize(0.8);
    gr3->SetMarkerColor(kBlue);
    TGraph* gr5 = new TGraph(V1.size(), &V1[0], &differences5[0]);
    gr5->SetTitle("E^{PV}_{Beam} - E^{I-current}_{Beam}"); //(" Differences between V7 and V5 vs run; run ; E_{Beam} [MeV] ");
    gr5->SetMarkerStyle(20); 
    gr5->SetMarkerSize(0.8);
    gr5->SetMarkerColor(kRed);
    TGraph* gr6 = new TGraph(V1.size(), &V1[0], &differences6[0]);
    gr6->SetTitle("E^{PV}_{Beam} - E^{DB}_{Beam}"); //(" Differences between V7 and V6 vs run; run ; E_{Beam} [MeV] ");
    gr6->SetMarkerStyle(20); 
    gr6->SetMarkerSize(0.8);
    gr6->SetMarkerColor(kGreen);

    TMultiGraph *mg2 = new TMultiGraph();
    // mg2->Add(gr3);
    // mg2->Add(gr5);
    mg2->Add(gr6);

    TH1F *h3 = new TH1F("h3", "Differences between V7 and V3", 90, -4.5, 4.5);
    TH1F *h5 = new TH1F("h5", "Differences between V7 and V5", 90, -4.5, 4.5);
    TH1F *h6 = new TH1F("h6", "Differences between V7 and V6", 90, -4.5, 4.5);
    TH1F *histogram3 = new TH1F("histogram3", "Differences V3[i] and V3[i]+1", 90, -4.5, 4.5);
    TH1F *histogram5 = new TH1F("histogram5", "Differences V5[i] and V5[i]+1", 90, -4.5, 4.5);
    TH1F *histogram6 = new TH1F("histogram6", "Differences V6[i] and V6[i]+1", 90, -4.5, 4.5);
    TH1F *histogram7 = new TH1F("histogram7", "Differences V7[i] and V7[i]+1", 90, -4.5, 4.5);

    // Fill the histogram with differences
    for (int i = 0; i< differences3.size(); i++) {
        h3->Fill(differences3[i]);
        h5->Fill(differences5[i]);
        h6->Fill(differences6[i]);
        if (i!=V3.size()) histogram3->Fill(V3[i]-V3[i+1]);
        if (i!=V5.size()) histogram5->Fill(V5[i]-V5[i+1]);
        if (i!=V6.size()) histogram6->Fill(V6[i]-V6[i+1]);
        if (i!=V7.size()) histogram7->Fill(V7[i]-V7[i+1]);
    }


    // Draw histogram on a canvas
    TCanvas *canvas0 = new TCanvas("canvas0", "Canvas0", 800, 600);
    mg0->Draw("AP");
    
    TCanvas *canvas1 = new TCanvas("canvas1", "Canvas1", 800, 600);
    // h3->Draw();
    // h5->Draw("same");
    h6->Draw("same");

    TCanvas *canvas2 = new TCanvas("canvas2", "Canvas2", 800, 600);
    canvas2->Divide(2,2);
    canvas2->cd(1);
    histogram3->Draw();
    canvas2->cd(2);
    histogram5->Draw();
    canvas2->cd(3);
    histogram6->Draw();
    canvas2->cd(4);
    histogram7->Draw();

    TCanvas *canvas3 = new TCanvas("canvas3", "Canvas3", 800, 600);
    mg1->Draw("AP");
    
    TCanvas *canvas4 = new TCanvas("canvas4", "Canvas4", 800, 600);
    gEB0->Draw("AP");

    TCanvas *canvas5 = new TCanvas("canvas5", "Canvas5", 800, 600);
    mg2->Draw("AP");
}