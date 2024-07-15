// {
// #include <iostream>
// #include <fstream>
// #include <string>
// #include <vector>
// #include "TFile.h"
// #include "TTree.h"
// #include "TString.h"

// using namespace std;

// vector<double> EBeam;
// ifstream file("/home/mancinima/padme-fw/varieMarco/NPotE.txt");
// double val1, val2;
// while (file >> val1 >> val2){
//     cout<<val1<<endl;
//    EBeam.push_back(val1);
// }
// file.close();

// for(int i = 0; i<1; i++){
//   cout<<"beam energy from txt input "<<EBeam[i]<<endl;

//   TString fileNameFull = Form("/home/mancinima/padme-fw/varieMarco/BabaYaga/babayagaTOY/babayagaTOY%i/Full%i.root", i+1, (int)(EBeam[i]*10));
//   TFile fileFull(fileNameFull, "recreate");
//   TTree* treeFull = new TTree(Form("tBhabhaFull_%i", (int)(EBeam[i]*10)), " ");
  
//   cout << "EBeam x loop: " << EBeam[i] << endl;
//   int EBeamValue = static_cast<int>(10 * EBeam[i]);
//   cout << "EBeam before formatting: " << EBeamValue << endl;

//   // Convert EBeamValue to string
//   std::ostringstream ss, ii;
//   ss << EBeamValue;
//   ii << (i+1);
//   std::string EBeamValueStr = ss.str();
//   std::string istring = ii.str();

//   Long64_t nlinesFull = treeFull->ReadFile(("//home/mancinima/padme-fw/varieMarco/BabaYaga/babayagaTOY/babayagaTOY" + istring + "/calchep_like.csv").c_str(), 
//       "col1/D:InitialP3_positron:InitialP3_electron:FFinalP1_electron:FFinalP2_electron:FFinalP3_electron:FFinalP1_positron:FFinalP2_positron:FFinalP3_positron");
//   treeFull->Write();
     
// }

// }




#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

using namespace std;

// Function to replace commas with spaces in a file
void preprocessCSV(const string& inputFilePath, const string& outputFilePath) {
    ifstream infile(inputFilePath);
    ofstream outfile(outputFilePath);

    string line;
    while (getline(infile, line)) {
        replace(line.begin(), line.end(), ',', ' ');
        outfile << line << "\n";
    }

    infile.close();
    outfile.close();
}

void TreeMaker() {
    vector<double> EBeam;
    ifstream file("/home/mancinima/padme-fw/varieMarco/NPotE.txt");
    double val1, val2;
    while (file >> val1 >> val2) {
        cout << val1 << endl;
        EBeam.push_back(val1);
    }
    file.close();

    for (size_t i = 0; i < EBeam.size(); ++i) {
        cout << "beam energy from txt input " << EBeam[i] << endl;

        TString fileNameFull = Form("/home/mancinima/padme-fw/varieMarco/BabaYaga/babayagasamecut/BabayagaSameCut%i/Full%i.root", i + 1, static_cast<int>(EBeam[i] * 10));
        TFile fileFull(fileNameFull, "recreate");
        TTree* treeFull = new TTree(Form("tBhabhaFull_%i", static_cast<int>(EBeam[i] * 10)), " ");

        cout << "EBeam x loop: " << EBeam[i] << endl;
        int EBeamValue = static_cast<int>(10 * EBeam[i]);
        cout << "EBeam before formatting: " << EBeamValue << endl;

        // Convert EBeamValue and index to strings
        std::ostringstream ss, ii;
        ss << EBeamValue;
        ii << (i + 1);
        std::string EBeamValueStr = ss.str();
        std::string istring = ii.str();

        // Construct the CSV file path
        std::string csvFilePath = "/home/mancinima/padme-fw/varieMarco/BabaYaga/babayagasamecut/BabayagaSameCut" + istring + "/calchep_like.csv";
        // Create a temporary file path
        std::string tempFilePath = "/home/mancinima/padme-fw/varieMarco/BabaYaga/babayagasamecut/BabayagaSameCut" + istring + "/calchep_like_temp.txt";

        // Preprocess the CSV file to replace commas with spaces
        preprocessCSV(csvFilePath, tempFilePath);

        // Read the temporary file into TTree
        Long64_t nlinesFull = treeFull->ReadFile(tempFilePath.c_str(),
            "col1/D:InitialP3_positron/D:InitialP3_electron/D:FFinalP1_electron/D:FFinalP2_electron/D:FFinalP3_electron/D:FFinalP1_positron/D:FFinalP2_positron/D:FFinalP3_positron");

        treeFull->Write();
        fileFull.Close();
    }

    
}
