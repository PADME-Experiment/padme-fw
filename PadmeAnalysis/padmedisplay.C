#define padmedisplay_cxx
#include "padmedisplay.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

//void padmedisplay::Loop()
//void padmedisplay::Draw(int event)
//{
//   In a ROOT session, you can do:
//      Root > .L padmedisplay.C
//      Root > padmedisplay t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch


void padmedisplay(char *filename, int event)
{
    
  //read files input , runnumber and set .gif output
  int NFiles ;
  int runnumber = 0 ;
  ifstream FilesInput;
  char InputFile[1000];
  //FilesInput.open("InputFiles",ifstream::in);
  //FilesInput >> NFiles;
  //cout << "NFiles: " <<NFiles << endl;
  
  //Load the ntuples
  TChain *mychain = new TChain("PADME_FlatNT");
  //for (int jfile=0; jfile!=NFiles; jfile++){
  //FilesInput>>InputFile;
  //cout << "InputFile: " <<InputFile<<endl;
  //mychain->Add(InputFile);
  //}
  
  mychain->Add(filename);
  
  //Macro * t = new Macro (mychain);
  padmedisplay * t = new padmedisplay (mychain);
  t->GetEntry(event);
  t->Show(event);    
  //t->Loop();
  t->Draw(event); 
          
}

