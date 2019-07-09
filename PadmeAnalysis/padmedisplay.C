#define padmedisplay_cxx
#include "padmedisplay.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

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

  int ChainEvent;
  ChainEvent=t->SearchChainEvent(event);
  t->GetEntry(ChainEvent);
  //t->Loop();
  if(ChainEvent==0) 
   { 
     cout << "The event number inserted may not exist, try another one! " <<endl; 
     return;                 
   }
  
  t->GetEntry(ChainEvent);
  t->Show(ChainEvent);   
  t->Draw(ChainEvent); 
  //t->Loop();
  //t->Draw(event); 
  
          
}

