#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>

using namespace std;

//graph to vector
void Graph_to_Vector(const char* OutputName){

vector <double> vN2Cluster;
TGraph *gIn = new TGraph();
TFile *FileIn = TFile::Open("/home/mancinima/padme-fw/varieMarco/X17_prod/BelowResScan.root");
if (FileIn) {
    // Estrai l'istogramma dal file
    gIn = static_cast<TGraph *>(FileIn->Get("Graph_from_hSMYield"));
    if (gIn) {
        // Crea e riempi il vettore con i contenuti dell'istogramma
        for (int i = -1 ; i < gIn->GetN(); i++) {
            double x, y;
            gIn->GetPoint(i, x, y);
            vN2Cluster.push_back(y);
        }
        FileIn->Close();
    } else {
        std::cerr << "Errore: TGraph non trovato nel file." << std::endl;
        FileIn->Close();
        return;
    }
} else {
    std::cerr << "Errore nell'apertura del file di input." << std::endl;
    return;
}
TFile *FileOut = TFile::Open(OutputName, "RECREATE");
if (FileOut) {
    // Crea un nuovo albero (tree)
    TTree *Tree = new TTree("Tree", "Tree");
    // Dichiarazione della variabile per i dati da inserire nell'albero
    double value;
    // Collega la variabile all'albero
    Tree->Branch("value", &value);

    // Riempi l'albero con i dati dal vettore
    for (size_t i = 0; i < vN2Cluster.size(); i++) {
        value = vN2Cluster[i];
        Tree->Fill();
    }
    // Scrivi l'albero nel file di output
    FileOut->cd();
    Tree->Write();
    // Chiudi il file di output
    FileOut->Close();
} else {
    std::cerr << "Errore nell'apertura del file di output." << std::endl;
}
}

void Histo_to_Vector(const char* OutputName) {

    vector<double> vEffSignal;
    TH1D *hIn = nullptr;

    // Apertura del file ROOT
    TFile *FileIn = TFile::Open("/home/mancinima/padme-fw/varieMarco/X17_prod/BelowResScan.root", "READ");

    if (FileIn) {
        // Estrai l'istogramma dal file
        hIn = static_cast<TH1D *>(FileIn->Get("hSAcceptance"));

        if (hIn) {
            // Crea e riempi il vettore con i contenuti dell'istogramma
            for (int i = 1; i <= hIn->GetNbinsX(); i++) {
                vEffSignal.push_back(hIn->GetBinContent(i));
            }

            // Chiudi il file di input
            FileIn->Close();
        } else {
            std::cerr << "Errore: istogramma non trovato nel file." << std::endl;
            FileIn->Close();
            return;
        }
    } else {
        std::cerr << "Errore nell'apertura del file di input." << std::endl;
        return;
    }

    // Apertura del file di output per il tree
    TFile *FileOut = TFile::Open(OutputName, "RECREATE");

    if (FileOut) {
        // Crea un nuovo albero (tree)
        TTree *Tree = new TTree("Tree", "Tree");
        // Dichiarazione della variabile per i dati da inserire nell'albero
        double value;

        // Collega la variabile all'albero
        Tree->Branch("value", &value);

        // Riempi l'albero con i dati dal vettore
        for (size_t i = 0; i < vEffSignal.size(); i++) {
            value = vEffSignal[i];
            Tree->Fill();
        }

        // Scrivi l'albero nel file di output
        FileOut->cd();
        Tree->Write();

        // Chiudi il file di output
        FileOut->Close();
    } else {
        std::cerr << "Errore nell'apertura del file di output." << std::endl;
    }
}
