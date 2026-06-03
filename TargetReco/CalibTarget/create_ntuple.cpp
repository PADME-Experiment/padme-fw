#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>

int create_ntuple() {

    // open ascii file
    std::ifstream infile("pulls.txt");
    if(!infile.is_open()) {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }

    // output ROOT file
    TFile *f = new TFile("ntuple.root", "RECREATE");
    TTree *t = new TTree("ntu", "pulls");

    // variables (one per column)
    double X, iq, pulls;

    // define branches
    t->Branch("X", &X, "X/D");
    t->Branch("iq", &iq, "iq/D");
    t->Branch("pulls", &pulls, "pulls/D");

    // read ASCII line by line
    string line;
    while (getline(infile, line)) {
        sscanf(line.c_str(), "%lf,%lf,%lf", &X, &iq, &pulls);
        t->Fill();
    }

    // save and exit
    f->Write();
    f->Close();
    return 0;
}
