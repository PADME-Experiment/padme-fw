//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jun 24 09:33:48 2019 by ROOT version 5.34/36
// from TChain PADME_FlatNT/
//////////////////////////////////////////////////////////

#ifndef padmedisplay_h
#define padmedisplay_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class padmedisplay {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           NTEventStatus;
   Int_t           NTNTarget_Hits;
   Double_t        NTTarget_Hits_Energy[32];   //[NTNTarget_Hits]
   Double_t        NTTarget_Hits_Time[32];   //[NTNTarget_Hits]
   Double_t        NTTarget_Hits_ChannelId[32];   //[NTNTarget_Hits]
   Double_t        NTTarget_Hits_Xpos[32];   //[NTNTarget_Hits]
   Double_t        NTTarget_Hits_Ypos[32];   //[NTNTarget_Hits]
   Double_t        NTTarget_Hits_Zpos[32];   //[NTNTarget_Hits]
   Double_t        NTTargetBeam_X;
   Double_t        NTTargetBeam_XErr;
   Double_t        NTTargetBeam_XW;
   Double_t        NTTargetBeam_XWErr;
   Double_t        NTTargetBeam_Xfit;
   Double_t        NTTargetBeam_XfitErr;
   Double_t        NTTargetBeam_XWfit;
   Double_t        NTTargetBeam_XWfitErr;
   Double_t        NTTargetBeam_XChi2;
   Double_t        NTTargetBeam_XNdof;
   Double_t        NTTargetBeam_XCharge;
   Double_t        NTTargetBeam_XChargeErr;
   Double_t        NTTargetBeam_XTime;
   Double_t        NTTargetBeam_XTimeErr;
   Double_t        NTTargetBeam_Y;
   Double_t        NTTargetBeam_YErr;
   Double_t        NTTargetBeam_YW;
   Double_t        NTTargetBeam_YWErr;
   Double_t        NTTargetBeam_Yfit;
   Double_t        NTTargetBeam_YfitErr;
   Double_t        NTTargetBeam_YWfit;
   Double_t        NTTargetBeam_YWfitErr;
   Double_t        NTTargetBeam_YChi2;
   Double_t        NTTargetBeam_YNdof;
   Double_t        NTTargetBeam_YCharge;
   Double_t        NTTargetBeam_YChargeErr;
   Double_t        NTTargetBeam_YTime;
   Double_t        NTTargetBeam_YTimeErr;
   Double_t        NTTargetBeam_NPOT;
   Double_t        NTTargetBeam_NPOTErr;
   Int_t           NTNPVeto_Hits;
   Double_t        NTPVeto_Hits_Energy[1000];   //[NTNPVeto_Hits]
   Double_t        NTPVeto_Hits_Time[1000];   //[NTNPVeto_Hits]
   Double_t        NTPVeto_Hits_ChannelId[1000];   //[NTNPVeto_Hits]
   Double_t        NTPVeto_Hits_Xpos[1000];   //[NTNPVeto_Hits]
   Double_t        NTPVeto_Hits_Ypos[1000];   //[NTNPVeto_Hits]
   Double_t        NTPVeto_Hits_Zpos[1000];   //[NTNPVeto_Hits]
   Int_t           NTNPVeto_Clusters;
   Double_t        NTPVeto_Clusters_Energy[1000];   //[NTNPVeto_Clusters]
   Double_t        NTPVeto_Clusters_Time[1000];   //[NTNPVeto_Clusters]
   Double_t        NTPVeto_Clusters_ChannelId[1000];   //[NTNPVeto_Clusters]
   Double_t        NTPVeto_Clusters_Xpos[1000];   //[NTNPVeto_Clusters]
   Double_t        NTPVeto_Clusters_Ypos[1000];   //[NTNPVeto_Clusters]
   Double_t        NTPVeto_Clusters_Zpos[1000];   //[NTNPVeto_Clusters]
   Int_t           NTNEVeto_Hits;
   Double_t        NTEVeto_Hits_Energy[1000];   //[NTNEVeto_Hits]
   Double_t        NTEVeto_Hits_Time[1000];   //[NTNEVeto_Hits]
   Double_t        NTEVeto_Hits_ChannelId[1000];   //[NTNEVeto_Hits]
   Double_t        NTEVeto_Hits_Xpos[1000];   //[NTNEVeto_Hits]
   Double_t        NTEVeto_Hits_Ypos[1000];   //[NTNEVeto_Hits]
   Double_t        NTEVeto_Hits_Zpos[1000];   //[NTNEVeto_Hits]
   Int_t           NTNEVeto_Clusters;
   Double_t        NTEVeto_Clusters_Energy[1000];   //[NTNEVeto_Clusters]
   Double_t        NTEVeto_Clusters_Time[1000];   //[NTNEVeto_Clusters]
   Double_t        NTEVeto_Clusters_ChannelId[1000];   //[NTNEVeto_Clusters]
   Double_t        NTEVeto_Clusters_Xpos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTEVeto_Clusters_Ypos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTEVeto_Clusters_Zpos[1000];   //[NTNEVeto_Clusters]
   Int_t           NTNHEPVeto_Hits;
   Double_t        NTHEPVeto_Hits_Energy[1000];   //[NTNHEPVeto_Hits]
   Double_t        NTHEPVeto_Hits_Time[1000];   //[NTNHEPVeto_Hits]
   Double_t        NTHEPVeto_Hits_ChannelId[1000];   //[NTNHEPVeto_Hits]
   Double_t        NTHEPVeto_Hits_Xpos[1000];   //[NTNEVeto_Hits]
   Double_t        NTHEPVeto_Hits_Ypos[1000];   //[NTNEVeto_Hits]
   Double_t        NTHEPVeto_Hits_Zpos[1000];   //[NTNEVeto_Hits]
   Int_t           NTNHEPVeto_Clusters;
   Double_t        NTHEPVeto_Clusters_Energy[1000];   //[NTNHEPVeto_Clusters]
   Double_t        NTHEPVeto_Clusters_Time[1000];   //[NTNHEPVeto_Clusters]
   Double_t        NTHEPVeto_Clusters_ChannelId[1000];   //[NTNHEPVeto_Clusters]
   Double_t        NTHEPVeto_Clusters_Xpos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTHEPVeto_Clusters_Ypos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTHEPVeto_Clusters_Zpos[1000];   //[NTNEVeto_Clusters]
   Int_t           NTNECal_Hits;
   Double_t        NTECal_Hits_Energy[1000];   //[NTNECal_Hits]
   Double_t        NTECal_Hits_Time[1000];   //[NTNECal_Hits]
   Double_t        NTECal_Hits_ChannelId[1000];   //[NTNECal_Hits]
   Double_t        NTECal_Hits_Xpos[1000];   //[NTNPVeto_Hits]
   Double_t        NTECal_Hits_Ypos[1000];   //[NTNPVeto_Hits]
   Double_t        NTECal_Hits_Zpos[1000];   //[NTNPVeto_Hits]
   Int_t           NTNECal_Clusters;
   Double_t        NTECal_Clusters_Energy[1000];   //[NTNECal_Clusters]
   Double_t        NTECal_Clusters_Time[1000];   //[NTNECal_Clusters]
   Double_t        NTECal_Clusters_ChannelId[1000];   //[NTNECal_Clusters]
   Double_t        NTECal_Clusters_Xpos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTECal_Clusters_Ypos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTECal_Clusters_Zpos[1000];   //[NTNEVeto_Clusters]
   Int_t           NTNSAC_Hits;
   Double_t        NTSAC_Hits_Energy[1000];   //[NTNSAC_Hits]
   Double_t        NTSAC_Hits_Time[1000];   //[NTNSAC_Hits]
   Double_t        NTSAC_Hits_ChannelId[1000];   //[NTNSAC_Hits]
   Double_t        NTSAC_Hits_Xpos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTSAC_Hits_Ypos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTSAC_Hits_Zpos[1000];   //[NTNEVeto_Clusters]
   Int_t           NTNSAC_Clusters;
   Double_t        NTSAC_Clusters_Energy[51];   //[NTNSAC_Clusters]
   Double_t        NTSAC_Clusters_Time[51];   //[NTNSAC_Clusters]
   Double_t        NTSAC_Clusters_ChannelId[51];   //[NTNSAC_Clusters]
   Double_t        NTSAC_Clusters_Xpos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTSAC_Clusters_Ypos[1000];   //[NTNEVeto_Clusters]
   Double_t        NTSAC_Clusters_Zpos[1000];   //[NTNEVeto_Clusters]

   // List of branches
   TBranch        *b_NTEventStatus;
   TBranch        *b_NTNTarget_Hits;   //!
   TBranch        *b_NTTarget_Hits_Energy;   //!
   TBranch        *b_NTTarget_Hits_Time;   //!
   TBranch        *b_NTTarget_Hits_ChannelId;   //!
   TBranch        *b_NTTarget_Hits_Xpos;   //!
   TBranch        *b_NTTarget_Hits_Ypos;   //!
   TBranch        *b_NTTarget_Hits_Zpos;   //!
   TBranch        *b_NTTargetBeam_X;   //!
   TBranch        *b_NTTargetBeam_XErr;   //!
   TBranch        *b_NTTargetBeam_XW;   //!
   TBranch        *b_NTTargetBeam_XWErr;   //!
   TBranch        *b_NTTargetBeam_Xfit;   //!
   TBranch        *b_NTTargetBeam_XfitErr;   //!
   TBranch        *b_NTTargetBeam_XWfit;   //!
   TBranch        *b_NTTargetBeam_XWfitErr;   //!
   TBranch        *b_NTTargetBeam_XChi2;   //!
   TBranch        *b_NTTargetBeam_XNdof;   //!
   TBranch        *b_NTTargetBeam_XCharge;   //!
   TBranch        *b_NTTargetBeam_XChargeErr;   //!
   TBranch        *b_NTTargetBeam_XTime;   //!
   TBranch        *b_NTTargetBeam_XTimeErr;   //!
   TBranch        *b_NTTargetBeam_Y;   //!
   TBranch        *b_NTTargetBeam_YErr;   //!
   TBranch        *b_NTTargetBeam_YW;   //!
   TBranch        *b_NTTargetBeam_YWErr;   //!
   TBranch        *b_NTTargetBeam_Yfit;   //!
   TBranch        *b_NTTargetBeam_YfitErr;   //!
   TBranch        *b_NTTargetBeam_YWfit;   //!
   TBranch        *b_NTTargetBeam_YWfitErr;   //!
   TBranch        *b_NTTargetBeam_YChi2;   //!
   TBranch        *b_NTTargetBeam_YNdof;   //!
   TBranch        *b_NTTargetBeam_YCharge;   //!
   TBranch        *b_NTTargetBeam_YChargeErr;   //!
   TBranch        *b_NTTargetBeam_YTime;   //!
   TBranch        *b_NTTargetBeam_YTimeErr;   //!
   TBranch        *b_NTTargetBeam_NPOT;   //!
   TBranch        *b_NTTargetBeam_NPOTErr;   //!
   TBranch        *b_NTNPVeto_Hits;   //!
   TBranch        *b_NTPVeto_Hits_Energy;   //!
   TBranch        *b_NTPVeto_Hits_Time;   //!
   TBranch        *b_NTPVeto_Hits_ChannelId;   //!
   TBranch        *b_NTPVeto_Hits_Xpos;   //!
   TBranch        *b_NTPVeto_Hits_Ypos;   //!
   TBranch        *b_NTPVeto_Hits_Zpos;   //!
   TBranch        *b_NTNPVeto_Clusters;   //!
   TBranch        *b_NTPVeto_Clusters_Energy;   //!
   TBranch        *b_NTPVeto_Clusters_Time;   //!
   TBranch        *b_NTPVeto_Clusters_ChannelId;   //!
   TBranch        *b_NTPVeto_Clusters_Xpos;   //!
   TBranch        *b_NTPVeto_Clusters_Ypos;   //!
   TBranch        *b_NTPVeto_Clusters_Zpos;   //!
   TBranch        *b_NTNEVeto_Hits;   //!
   TBranch        *b_NTEVeto_Hits_Energy;   //!
   TBranch        *b_NTEVeto_Hits_Time;   //!
   TBranch        *b_NTEVeto_Hits_ChannelId;   //!
   TBranch        *b_NTEVeto_Hits_Xpos;   //!
   TBranch        *b_NTEVeto_Hits_Ypos;   //!
   TBranch        *b_NTEVeto_Hits_Zpos;   //!
   TBranch        *b_NTNEVeto_Clusters;   //!
   TBranch        *b_NTEVeto_Clusters_Energy;   //!
   TBranch        *b_NTEVeto_Clusters_Time;   //!
   TBranch        *b_NTEVeto_Clusters_ChannelId;   //!
   TBranch        *b_NTEVeto_Clusters_Xpos;   //!
   TBranch        *b_NTEVeto_Clusters_Ypos;   //!
   TBranch        *b_NTEVeto_Clusters_Zpos;   //!
   TBranch        *b_NTNHEPVeto_Hits;   //!
   TBranch        *b_NTHEPVeto_Hits_Energy;   //!
   TBranch        *b_NTHEPVeto_Hits_Time;   //!
   TBranch        *b_NTHEPVeto_Hits_ChannelId;   //!
   TBranch        *b_NTHEPVeto_Hits_Xpos;   //!
   TBranch        *b_NTHEPVeto_Hits_Ypos;   //!
   TBranch        *b_NTHEPVeto_Hits_Zpos;   //!
   TBranch        *b_NTNHEPVeto_Clusters;   //!
   TBranch        *b_NTHEPVeto_Clusters_Energy;   //!
   TBranch        *b_NTHEPVeto_Clusters_Time;   //!
   TBranch        *b_NTHEPVeto_Clusters_ChannelId;   //!
   TBranch        *b_NTHEPVeto_Clusters_Xpos;   //!
   TBranch        *b_NTHEPVeto_Clusters_Ypos;   //!
   TBranch        *b_NTHEPVeto_Clusters_Zpos;   //!
   TBranch        *b_NTNECal_Hits;   //!
   TBranch        *b_NTECal_Hits_Energy;   //!
   TBranch        *b_NTECal_Hits_Time;   //!
   TBranch        *b_NTECal_Hits_ChannelId;   //!
   TBranch        *b_NTECal_Hits_Xpos;   //!
   TBranch        *b_NTECal_Hits_Ypos;   //!
   TBranch        *b_NTECal_Hits_Zpos;   //!
   TBranch        *b_NTNECal_Clusters;   //!
   TBranch        *b_NTECal_Clusters_Energy;   //!
   TBranch        *b_NTECal_Clusters_Time;   //!
   TBranch        *b_NTECal_Clusters_ChannelId;   //!
   TBranch        *b_NTECal_Clusters_Xpos;   //!
   TBranch        *b_NTECal_Clusters_Ypos;   //!
   TBranch        *b_NTECal_Clusters_Zpos;   //!
   TBranch        *b_NTNSAC_Hits;   //!
   TBranch        *b_NTSAC_Hits_Energy;   //!
   TBranch        *b_NTSAC_Hits_Time;   //!
   TBranch        *b_NTSAC_Hits_ChannelId;   //!
   TBranch        *b_NTSAC_Hits_Xpos;   //!
   TBranch        *b_NTSAC_Hits_Ypos;   //!
   TBranch        *b_NTSAC_Hits_Zpos;   //!
   TBranch        *b_NTNSAC_Clusters;   //!
   TBranch        *b_NTSAC_Clusters_Energy;   //!
   TBranch        *b_NTSAC_Clusters_Time;   //!
   TBranch        *b_NTSAC_Clusters_ChannelId;   //!
   TBranch        *b_NTSAC_Clusters_Xpos;   //!
   TBranch        *b_NTSAC_Clusters_Ypos;   //!
   TBranch        *b_NTSAC_Clusters_Zpos;   //!

   padmedisplay(TTree *tree=0);
   virtual ~padmedisplay();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef padmedisplay_cxx
padmedisplay::padmedisplay(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f || !f->IsOpen()) {
         f = new TFile("Memory Directory");
      }
      f->GetObject("PADME_FlatNT",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
     // TChain * chain = new TChain("PADME_FlatNT","");
     // chain->Add("An_Reco-run_0000000_20181217_203029_100events.root/PADME_FlatNT");
     // tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

padmedisplay::~padmedisplay()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t padmedisplay::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t padmedisplay::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void padmedisplay::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("NTEventStatus", &NTEventStatus, &b_NTEventStatus);   
   fChain->SetBranchAddress("NTNTarget_Hits", &NTNTarget_Hits, &b_NTNTarget_Hits);
   fChain->SetBranchAddress("NTTarget_Hits_Energy", NTTarget_Hits_Energy, &b_NTTarget_Hits_Energy);
   fChain->SetBranchAddress("NTTarget_Hits_Time", NTTarget_Hits_Time, &b_NTTarget_Hits_Time);
   fChain->SetBranchAddress("NTTarget_Hits_ChannelId", NTTarget_Hits_ChannelId, &b_NTTarget_Hits_ChannelId);
   fChain->SetBranchAddress("NTTarget_Hits_Xpos", NTTarget_Hits_Xpos, &b_NTTarget_Hits_Xpos);
   fChain->SetBranchAddress("NTTarget_Hits_Ypos", NTTarget_Hits_Ypos, &b_NTTarget_Hits_Ypos);
   fChain->SetBranchAddress("NTTarget_Hits_Zpos", NTTarget_Hits_Zpos, &b_NTTarget_Hits_Zpos);
   fChain->SetBranchAddress("NTTargetBeam_X", &NTTargetBeam_X, &b_NTTargetBeam_X);
   fChain->SetBranchAddress("NTTargetBeam_XErr", &NTTargetBeam_XErr, &b_NTTargetBeam_XErr);
   fChain->SetBranchAddress("NTTargetBeam_XW", &NTTargetBeam_XW, &b_NTTargetBeam_XW);
   fChain->SetBranchAddress("NTTargetBeam_XWErr", &NTTargetBeam_XWErr, &b_NTTargetBeam_XWErr);
   fChain->SetBranchAddress("NTTargetBeam_Xfit", &NTTargetBeam_Xfit, &b_NTTargetBeam_Xfit);
   fChain->SetBranchAddress("NTTargetBeam_XfitErr", &NTTargetBeam_XfitErr, &b_NTTargetBeam_XfitErr);
   fChain->SetBranchAddress("NTTargetBeam_XWfit", &NTTargetBeam_XWfit, &b_NTTargetBeam_XWfit);
   fChain->SetBranchAddress("NTTargetBeam_XWfitErr", &NTTargetBeam_XWfitErr, &b_NTTargetBeam_XWfitErr);
   fChain->SetBranchAddress("NTTargetBeam_XChi2", &NTTargetBeam_XChi2, &b_NTTargetBeam_XChi2);
   fChain->SetBranchAddress("NTTargetBeam_XNdof", &NTTargetBeam_XNdof, &b_NTTargetBeam_XNdof);
   fChain->SetBranchAddress("NTTargetBeam_XCharge", &NTTargetBeam_XCharge, &b_NTTargetBeam_XCharge);
   fChain->SetBranchAddress("NTTargetBeam_XChargeErr", &NTTargetBeam_XChargeErr, &b_NTTargetBeam_XChargeErr);
   fChain->SetBranchAddress("NTTargetBeam_XTime", &NTTargetBeam_XTime, &b_NTTargetBeam_XTime);
   fChain->SetBranchAddress("NTTargetBeam_XTimeErr", &NTTargetBeam_XTimeErr, &b_NTTargetBeam_XTimeErr);
   fChain->SetBranchAddress("NTTargetBeam_Y", &NTTargetBeam_Y, &b_NTTargetBeam_Y);
   fChain->SetBranchAddress("NTTargetBeam_YErr", &NTTargetBeam_YErr, &b_NTTargetBeam_YErr);
   fChain->SetBranchAddress("NTTargetBeam_YW", &NTTargetBeam_YW, &b_NTTargetBeam_YW);
   fChain->SetBranchAddress("NTTargetBeam_YWErr", &NTTargetBeam_YWErr, &b_NTTargetBeam_YWErr);
   fChain->SetBranchAddress("NTTargetBeam_Yfit", &NTTargetBeam_Yfit, &b_NTTargetBeam_Yfit);
   fChain->SetBranchAddress("NTTargetBeam_YfitErr", &NTTargetBeam_YfitErr, &b_NTTargetBeam_YfitErr);
   fChain->SetBranchAddress("NTTargetBeam_YWfit", &NTTargetBeam_YWfit, &b_NTTargetBeam_YWfit);
   fChain->SetBranchAddress("NTTargetBeam_YWfitErr", &NTTargetBeam_YWfitErr, &b_NTTargetBeam_YWfitErr);
   fChain->SetBranchAddress("NTTargetBeam_YChi2", &NTTargetBeam_YChi2, &b_NTTargetBeam_YChi2);
   fChain->SetBranchAddress("NTTargetBeam_YNdof", &NTTargetBeam_YNdof, &b_NTTargetBeam_YNdof);
   fChain->SetBranchAddress("NTTargetBeam_YCharge", &NTTargetBeam_YCharge, &b_NTTargetBeam_YCharge);
   fChain->SetBranchAddress("NTTargetBeam_YChargeErr", &NTTargetBeam_YChargeErr, &b_NTTargetBeam_YChargeErr);
   fChain->SetBranchAddress("NTTargetBeam_YTime", &NTTargetBeam_YTime, &b_NTTargetBeam_YTime);
   fChain->SetBranchAddress("NTTargetBeam_YTimeErr", &NTTargetBeam_YTimeErr, &b_NTTargetBeam_YTimeErr);
   fChain->SetBranchAddress("NTTargetBeam_NPOT", &NTTargetBeam_NPOT, &b_NTTargetBeam_NPOT);
   fChain->SetBranchAddress("NTTargetBeam_NPOTErr", &NTTargetBeam_NPOTErr, &b_NTTargetBeam_NPOTErr);
   fChain->SetBranchAddress("NTNPVeto_Hits", &NTNPVeto_Hits, &b_NTNPVeto_Hits);
   fChain->SetBranchAddress("NTPVeto_Hits_Energy", NTPVeto_Hits_Energy, &b_NTPVeto_Hits_Energy);
   fChain->SetBranchAddress("NTPVeto_Hits_Time", NTPVeto_Hits_Time, &b_NTPVeto_Hits_Time);
   fChain->SetBranchAddress("NTPVeto_Hits_ChannelId", NTPVeto_Hits_ChannelId, &b_NTPVeto_Hits_ChannelId);
   fChain->SetBranchAddress("NTPVeto_Hits_Xpos", NTPVeto_Hits_Xpos, &b_NTPVeto_Hits_Xpos);
   fChain->SetBranchAddress("NTPVeto_Hits_Ypos", NTPVeto_Hits_Ypos, &b_NTPVeto_Hits_Ypos);
   fChain->SetBranchAddress("NTPVeto_Hits_Zpos", NTPVeto_Hits_Zpos, &b_NTPVeto_Hits_Zpos);
   fChain->SetBranchAddress("NTNPVeto_Clusters", &NTNPVeto_Clusters, &b_NTNPVeto_Clusters);
   fChain->SetBranchAddress("NTPVeto_Clusters_Energy", NTPVeto_Clusters_Energy, &b_NTPVeto_Clusters_Energy);
   fChain->SetBranchAddress("NTPVeto_Clusters_Time", NTPVeto_Clusters_Time, &b_NTPVeto_Clusters_Time);
   fChain->SetBranchAddress("NTPVeto_Clusters_ChannelId", NTPVeto_Clusters_ChannelId, &b_NTPVeto_Clusters_ChannelId);
   fChain->SetBranchAddress("NTPVeto_Clusters_Xpos", NTPVeto_Clusters_Xpos, &b_NTPVeto_Clusters_Xpos);
   fChain->SetBranchAddress("NTPVeto_Clusters_Ypos", NTPVeto_Clusters_Ypos, &b_NTPVeto_Clusters_Ypos);
   fChain->SetBranchAddress("NTPVeto_Clusters_Zpos", NTPVeto_Clusters_Zpos, &b_NTPVeto_Clusters_Zpos);
   fChain->SetBranchAddress("NTNEVeto_Hits", &NTNEVeto_Hits, &b_NTNEVeto_Hits);
   fChain->SetBranchAddress("NTEVeto_Hits_Energy", NTEVeto_Hits_Energy, &b_NTEVeto_Hits_Energy);
   fChain->SetBranchAddress("NTEVeto_Hits_Time", NTEVeto_Hits_Time, &b_NTEVeto_Hits_Time);
   fChain->SetBranchAddress("NTEVeto_Hits_ChannelId", NTEVeto_Hits_ChannelId, &b_NTEVeto_Hits_ChannelId);
   fChain->SetBranchAddress("NTEVeto_Hits_Xpos", NTEVeto_Hits_Xpos, &b_NTEVeto_Hits_Xpos);
   fChain->SetBranchAddress("NTEVeto_Hits_Ypos", NTEVeto_Hits_Ypos, &b_NTEVeto_Hits_Ypos);
   fChain->SetBranchAddress("NTEVeto_Hits_Zpos", NTEVeto_Hits_Zpos, &b_NTEVeto_Hits_Zpos);
   fChain->SetBranchAddress("NTNEVeto_Clusters", &NTNEVeto_Clusters, &b_NTNEVeto_Clusters);
   fChain->SetBranchAddress("NTEVeto_Clusters_Energy", NTEVeto_Clusters_Energy, &b_NTEVeto_Clusters_Energy);
   fChain->SetBranchAddress("NTEVeto_Clusters_Time", NTEVeto_Clusters_Time, &b_NTEVeto_Clusters_Time);
   fChain->SetBranchAddress("NTEVeto_Clusters_ChannelId", NTEVeto_Clusters_ChannelId, &b_NTEVeto_Clusters_ChannelId);
   fChain->SetBranchAddress("NTEVeto_Clusters_Xpos", NTEVeto_Clusters_Xpos, &b_NTEVeto_Clusters_Xpos);
   fChain->SetBranchAddress("NTEVeto_Clusters_Ypos", NTEVeto_Clusters_Ypos, &b_NTEVeto_Clusters_Ypos);
   fChain->SetBranchAddress("NTEVeto_Clusters_Zpos", NTEVeto_Clusters_Zpos, &b_NTEVeto_Clusters_Zpos);
   fChain->SetBranchAddress("NTNHEPVeto_Hits", &NTNHEPVeto_Hits, &b_NTNHEPVeto_Hits);
   fChain->SetBranchAddress("NTHEPVeto_Hits_Energy", NTHEPVeto_Hits_Energy, &b_NTHEPVeto_Hits_Energy);
   fChain->SetBranchAddress("NTHEPVeto_Hits_Time", NTHEPVeto_Hits_Time, &b_NTHEPVeto_Hits_Time);
   fChain->SetBranchAddress("NTHEPVeto_Hits_ChannelId", NTHEPVeto_Hits_ChannelId, &b_NTHEPVeto_Hits_ChannelId);
   fChain->SetBranchAddress("NTHEPVeto_Hits_Xpos", NTHEPVeto_Hits_Xpos, &b_NTHEPVeto_Hits_Xpos);
   fChain->SetBranchAddress("NTHEPVeto_Hits_Ypos", NTHEPVeto_Hits_Ypos, &b_NTHEPVeto_Hits_Ypos);
   fChain->SetBranchAddress("NTHEPVeto_Hits_Zpos", NTHEPVeto_Hits_Zpos, &b_NTHEPVeto_Hits_Zpos);
   fChain->SetBranchAddress("NTNHEPVeto_Clusters", &NTNHEPVeto_Clusters, &b_NTNHEPVeto_Clusters);
   fChain->SetBranchAddress("NTHEPVeto_Clusters_Energy", NTHEPVeto_Clusters_Energy, &b_NTHEPVeto_Clusters_Energy);
   fChain->SetBranchAddress("NTHEPVeto_Clusters_Time", NTHEPVeto_Clusters_Time, &b_NTHEPVeto_Clusters_Time);
   fChain->SetBranchAddress("NTHEPVeto_Clusters_ChannelId", NTHEPVeto_Clusters_ChannelId, &b_NTHEPVeto_Clusters_ChannelId);
   fChain->SetBranchAddress("NTHEPVeto_Clusters_Xpos", NTHEPVeto_Clusters_Xpos, &b_NTHEPVeto_Clusters_Xpos);
   fChain->SetBranchAddress("NTHEPVeto_Clusters_Ypos", NTHEPVeto_Clusters_Ypos, &b_NTHEPVeto_Clusters_Ypos);
   fChain->SetBranchAddress("NTHEPVeto_Clusters_Zpos", NTHEPVeto_Clusters_Zpos, &b_NTHEPVeto_Clusters_Zpos);
   fChain->SetBranchAddress("NTNECal_Hits", &NTNECal_Hits, &b_NTNECal_Hits);
   fChain->SetBranchAddress("NTECal_Hits_Energy", NTECal_Hits_Energy, &b_NTECal_Hits_Energy);
   fChain->SetBranchAddress("NTECal_Hits_Time", NTECal_Hits_Time, &b_NTECal_Hits_Time);
   fChain->SetBranchAddress("NTECal_Hits_ChannelId", NTECal_Hits_ChannelId, &b_NTECal_Hits_ChannelId);
   fChain->SetBranchAddress("NTECal_Hits_Xpos", NTECal_Hits_Xpos, &b_NTECal_Hits_Xpos);
   fChain->SetBranchAddress("NTECal_Hits_Ypos", NTECal_Hits_Ypos, &b_NTECal_Hits_Ypos);
   fChain->SetBranchAddress("NTECal_Hits_Zpos", NTECal_Hits_Zpos, &b_NTECal_Hits_Zpos);
   fChain->SetBranchAddress("NTNECal_Clusters", &NTNECal_Clusters, &b_NTNECal_Clusters);
   fChain->SetBranchAddress("NTECal_Clusters_Energy", NTECal_Clusters_Energy, &b_NTECal_Clusters_Energy);
   fChain->SetBranchAddress("NTECal_Clusters_Time", NTECal_Clusters_Time, &b_NTECal_Clusters_Time);
   fChain->SetBranchAddress("NTECal_Clusters_ChannelId", NTECal_Clusters_ChannelId, &b_NTECal_Clusters_ChannelId);
   fChain->SetBranchAddress("NTECal_Clusters_Xpos", NTECal_Clusters_Xpos, &b_NTECal_Clusters_Xpos);
   fChain->SetBranchAddress("NTECal_Clusters_Ypos", NTECal_Clusters_Ypos, &b_NTECal_Clusters_Ypos);
   fChain->SetBranchAddress("NTECal_Clusters_Zpos", NTECal_Clusters_Zpos, &b_NTECal_Clusters_Zpos);
   fChain->SetBranchAddress("NTNSAC_Hits", &NTNSAC_Hits, &b_NTNSAC_Hits);
   fChain->SetBranchAddress("NTSAC_Hits_Energy", NTSAC_Hits_Energy, &b_NTSAC_Hits_Energy);
   fChain->SetBranchAddress("NTSAC_Hits_Time", NTSAC_Hits_Time, &b_NTSAC_Hits_Time);
   fChain->SetBranchAddress("NTSAC_Hits_ChannelId", NTSAC_Hits_ChannelId, &b_NTSAC_Hits_ChannelId);
   fChain->SetBranchAddress("NTSAC_Hits_Xpos", NTSAC_Hits_Xpos, &b_NTSAC_Hits_Xpos);
   fChain->SetBranchAddress("NTSAC_Hits_Ypos", NTSAC_Hits_Ypos, &b_NTSAC_Hits_Ypos);
   fChain->SetBranchAddress("NTSAC_Hits_Zpos", NTSAC_Hits_Zpos, &b_NTSAC_Hits_Zpos);
   fChain->SetBranchAddress("NTNSAC_Clusters", &NTNSAC_Clusters, &b_NTNSAC_Clusters);
   fChain->SetBranchAddress("NTSAC_Clusters_Energy", NTSAC_Clusters_Energy, &b_NTSAC_Clusters_Energy);
   fChain->SetBranchAddress("NTSAC_Clusters_Time", NTSAC_Clusters_Time, &b_NTSAC_Clusters_Time);
   fChain->SetBranchAddress("NTSAC_Clusters_ChannelId", NTSAC_Clusters_ChannelId, &b_NTSAC_Clusters_ChannelId);
   fChain->SetBranchAddress("NTSAC_Clusters_Xpos", NTSAC_Clusters_Xpos, &b_NTSAC_Clusters_Xpos);
   fChain->SetBranchAddress("NTSAC_Clusters_Ypos", NTSAC_Clusters_Ypos, &b_NTSAC_Clusters_Ypos);
   fChain->SetBranchAddress("NTSAC_Clusters_Zpos", NTSAC_Clusters_Zpos, &b_NTSAC_Clusters_Zpos);
   Notify();
}

Bool_t padmedisplay::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void padmedisplay::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

void padmedisplay::Draw(int event)
{

   TCanvas *c1 = new TCanvas("c1", "c1", 600,800);
   TCanvas *c2 = new TCanvas("c2", "c2", 1000,800);
   TCanvas *c3 = new TCanvas("c3", "c3", 700,700);
   //TCanvas *c4 = new TCanvas("c4", "c4", 700,700);
   TCanvas *map = new TCanvas("map", "map", 800,1200);
   

   //TH1F *hProfileX   = new TH1F("hProfileX","ProfileX",16,0.5,16.5);
   //TH1F *hProfileY   = new TH1F("hProfileY","ProfileY",16,0.5,16.5);
   TH1F *hProfileX   = new TH1F("hProfileX","ProfileX",16,-8.5,7.5);
   TH1F *hProfileY   = new TH1F("hProfileY","ProfileY",16,-8.5,7.5);
   TH1F *hChargeX   = new TH1F("hChargeX","ChargeX",500,0,0.3);
   TH1F *hChargeY   = new TH1F("hChargeY","ChargeY",500,0,0.3);
   TH1F *hPVetoZPos   = new TH1F("hPVetoZPos","hPVetoZPos",100,-500,500);
   TH1F *hEVetoZPos   = new TH1F("hEVetoZPos","hEVetoZPos",100,-500,500);
   //TH2F *hBeamSpot   = new TH2F("hBeamSpot","BeamSpot",20,-10, 10, 20,-10,10);
  
   TH2F *hECalMap   = new TH2F("hECalMap","hECalMap",50,-500,500, 50,-500,500);

   map->cd();
   TH2F *hXZMap   = new TH2F("hXZMap","PADME map",500,-400, 400, 2000,-1300,4000);
   gStyle->SetOptStat("");
   hXZMap -> Draw( );

   TArc *arc0 = new TArc(0,0,100);
   arc0->SetLineColor(kRed);
   arc0->SetLineWidth(0);


   TArc *ECalClus = new TArc(0,0,100);
   ECalClus->SetLineColor(kRed);
   ECalClus->SetLineWidth(1); 
   

   TArc *PVetoClus= new TArc(0,0,100);
   PVetoClus->SetLineColor(kGreen);
   PVetoClus->SetLineWidth(1);

   TArc *EVetoClus = new TArc(0,0,100);
   EVetoClus->SetLineColor(kBlue);
   EVetoClus->SetLineWidth(1);

   TArc *SACClus = new TArc(0,0,100);
   SACClus->SetLineColor(kYellow);
   SACClus->SetLineWidth(1);

   
   TBox *box1 = new TBox(-70,-70,70,70);
   box1->SetLineColor(3);
   box1->SetFillColor(3);
   box1->SetLineWidth(1);


   c1->Divide(1,2);
   c2->Divide(2,1);

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=event; jentry<event+1;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   //}


    for(Long64_t i=0; i<NTNTarget_Hits;i++)   
     {
       if(NTTarget_Hits_ChannelId[i]<16)  {
          hChargeX->Fill(NTTarget_Hits_Energy[i]);
          //hProfileX->SetBinContent(NTTarget_Hits_ChannelId[i], NTTarget_Hits_Energy[i]);
          hProfileX->Fill(NTTarget_Hits_Xpos[i], NTTarget_Hits_Energy[i]);
         }
          else if(NTTarget_Hits_ChannelId[i]>16) {
          hChargeY->Fill(NTTarget_Hits_Energy[i]);
          //hProfileY->SetBinContent(NTTarget_Hits_ChannelId[i]-16, NTTarget_Hits_Energy[i]);
          hProfileY->Fill(NTTarget_Hits_Ypos[i], NTTarget_Hits_Energy[i]);
       
         }
      
     
     
      
      // if (Cut(ientry) < 0) continue;
   }
   //hBeamSpot->Fill(NTTargetBeam_X, NTTargetBeam_Y);
   for(Long64_t ipv=0; ipv<NTNPVeto_Clusters;ipv++)   
     {
       hPVetoZPos->Fill(NTPVeto_Clusters_Zpos[ipv], NTPVeto_Clusters_Energy[ipv]);
         
      // if (Cut(ientry) < 0) continue;
     }
   for(Long64_t iev=0; iev<NTNEVeto_Clusters;iev++)   
     {
       hEVetoZPos->Fill(NTEVeto_Clusters_Zpos[iev]);
         
      // if (Cut(ientry) < 0) continue;
     }

   float LeadingTime   = 0.;
   float LeadingEnergy = 0.;

   int kdrawcal, kdrawveto;

   if(NTEventStatus==1)  

   {  kdrawcal=50; 
      kdrawveto=100;
    }
   else
    { kdrawcal =5; 
      kdrawveto=1;
    }
   
      //if()>NTECalEnergy

   for(Long64_t iecal=0; iecal<NTNECal_Clusters;iecal++)   
     {
     
       hECalMap->Fill(int(NTECal_Clusters_Xpos[iecal]), int(NTECal_Clusters_Ypos[iecal]));
       //cout << " ChId   "<<   NTECal_Clusters_ChannelId[iecal] <<"X " <<NTECal_Clusters_Xpos[iecal] <<   "   Y   "<<NTECal_Clusters_Ypos[iecal]<< endl;
         
      // if (Cut(ientry) < 0) continue;
     }
   
      TBox *magnet = new TBox(-150,-500,150,500);
      //magnet->SetFillStyle(3017);
      magnet->SetFillColor(2);
      magnet->SetFillStyle(3017);
      magnet->Draw();

  /*    TBox *box3 = new TBox(170,-500,215, 500);   
      box3->SetLineWidth(20);
      box3->Draw();

      TBox *box4 = new TBox(-170,-500,-215,500);   
      //box4->SetFillColor(1);
      box4->SetLineWidth(20);
      box4->Draw();*/

      //TBox *target = new TBox(-20,-610,20,-600);
      TBox *target = new TBox(-20,-1030,20,-1030);   
      target->SetFillColor(1);
      target->SetLineWidth(11);
      target->Draw();
     //DrawBox(-70,-70,70,70);

      arc0->SetFillColor(1);
      arc0->SetLineColor(1);
      arc0->DrawArc(0, 0 ,1);
   /*for(Long64_t jecal=0; jecal<NTNECal_Hits;jecal++)   
     {
        float X =  NTECal_Hits_Xpos[jecal];
	//float Y =  NTECal_Hits_Ypos[jecal] + NTECal_Hits_Zpos[jecal]; 
        float Y =  NTECal_Hits_Ypos[jecal] + NTECal_Hits_Zpos[jecal];
	float E =  NTECal_Hits_Energy     [jecal] /100    ; 
	  
	ECalClus->DrawArc(X, Y ,E);
	
     }*/


   for(Long64_t jecal=0; jecal<NTNECal_Clusters;jecal++)   
     {
        float XECal =  NTECal_Clusters_Xpos[jecal];
        //float Y =  NTECal_Clusters_Ypos[jecal] + NTECal_Clusters_Zpos[jecal];
        float YECal =  NTECal_Clusters_Ypos[jecal] + NTECal_Clusters_Zpos[jecal];
	float EnECal =  NTECal_Clusters_Energy     [jecal] /kdrawcal    ; 
	  
	ECalClus->DrawArc(XECal, YECal ,EnECal);
	
     }

   for(Long64_t ipv=0; ipv<NTNPVeto_Clusters;ipv++)   
     {
	float XPVeto =  -NTPVeto_Clusters_Xpos[ipv];
        float YPVeto =   NTPVeto_Clusters_Zpos[ipv];
        float EnPVeto =  NTPVeto_Clusters_Energy[ipv]/kdrawveto; 
	//float Y =   (NTPVetoBarClZpos[ipv]-500)  / 10  + 0;
	PVetoClus->DrawArc(XPVeto, YPVeto , EnPVeto);
     
     }
     for(Long64_t iev=0; iev<NTNEVeto_Clusters;iev++)   
     {
        
	float XEVeto =  -NTEVeto_Clusters_Xpos[iev];
        float YEVeto =   NTEVeto_Clusters_Zpos[iev];
        float EnEVeto =  NTEVeto_Clusters_Energy[iev]/kdrawveto;
	//float Y =   (NTEVetoBarClZpos[iev]-500)  / 10  + 0; 
	EVetoClus->DrawArc(XEVeto, YEVeto , EnEVeto);
     }
     
      
     for(Long64_t iev=0; iev<NTNSAC_Clusters;iev++)   
     {
        
	float XSAC = NTSAC_Clusters_Xpos[iev];
        //float Y = NTSAC_Clusters_Ypos[iev]+1000;
        float YSAC = NTSAC_Clusters_Ypos[iev] + NTSAC_Hits_Zpos[iev];
        float EnSAC =  NTEVeto_Clusters_Energy[iev]/kdrawcal;
	//float Y =   (NTEVetoBarClZpos[iev]-500)  / 10  + 0; 
	SACClus->DrawArc(XSAC, YSAC , EnSAC);
      } 
  
   
   c1->cd(1);
   //hChargeX->Draw("");
   //hProfileX->SetFillColor(kBlue);
   hProfileX->SetFillColor(3);
   hProfileX->Draw("");
   c1->cd(2);
   hProfileY->SetFillColor(kRed);
   hProfileY->Draw("");
   //c1->cd(2);
   //hChargeY->Draw("");
   //TPad::SetVertical;
   c2->cd(1);
   hPVetoZPos->SetMarkerStyle(3);
   hPVetoZPos->SetMarkerColor(3);
   hPVetoZPos->Draw("P");
   c2->cd(2);
   hEVetoZPos->SetMarkerStyle(3);
   hEVetoZPos->SetMarkerColor(2);
   hEVetoZPos->Draw("P");
   c3->cd();
   hECalMap->Draw("COLZ");
   //c4->cd();
   //hBeamSpot->Draw("COLZ");

}
}
Int_t padmedisplay::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef padmedisplay_cxx
