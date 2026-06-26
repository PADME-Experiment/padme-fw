//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Aug 14 19:35:56 2014 by ROOT version 5.34/11
// from TTree apv_raw_ped/APVRawPedestals
// found on file: /Users/stefanos/Desktop/run11141.root
//////////////////////////////////////////////////////////

#ifndef APV_RAW_PED_h
#define APV_RAW_PED_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
using namespace std;

// Fixed size dimensions of array or collections stored in the TTree if any.

class APV_RAW_PED {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   ULong64_t       evt;
   UInt_t          error;
   Int_t           daqTimeSec;
   Int_t           daqTimeMicroSec;
   Int_t           srsTimeStamp;
   UInt_t          srsTrigger;
   vector<unsigned int> *srsFec;
   vector<unsigned int> *srsChip;
   vector<unsigned int> *srsChan;
   vector<string>  *mmChamber;
   vector<int>     *mmLayer;
   vector<char>    *mmReadout;
   vector<int>     *mmStrip;
   vector<double>  *ped_mean;
   vector<double>  *ped_stdev;
   vector<double>  *ped_sigma;

   // List of branches
   TBranch        *b_evt;   //!
   TBranch        *b_error;   //!
   TBranch        *b_daqTimeSec;   //!
   TBranch        *b_daqTimeMicroSec;   //!
   TBranch        *b_srsTimeStamp;   //!
   TBranch        *b_srsTrigger;   //!
   TBranch        *b_srsFec;   //!
   TBranch        *b_srsChip;   //!
   TBranch        *b_srsChan;   //!
   TBranch        *b_mmChamber;   //!
   TBranch        *b_mmLayer;   //!
   TBranch        *b_mmReadout;   //!
   TBranch        *b_mmStrip;   //!
   TBranch        *b_ped_mean;   //!
   TBranch        *b_ped_stdev;   //!
   TBranch        *b_ped_sigma;   //!

   APV_RAW_PED(TTree *tree=0);
   virtual ~APV_RAW_PED();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

