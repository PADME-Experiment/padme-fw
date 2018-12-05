#include "Riostream.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "PadmeReconstruction.hh"
#include "PadmeVReconstruction.hh"

#include <signal.h>
#include <fcntl.h>

//#include "RecoRootIOManager.hh"
#include "TRecoEvent.hh"
#include "TTargetRecoEvent.hh"
#include "TTargetRecoBeam.hh"
#include "TECalRecoEvent.hh"
#include "TPVetoRecoEvent.hh"
#include "TEVetoRecoEvent.hh"
#include "TSACRecoEvent.hh"
#include "THEPVetoRecoEvent.hh"
#include "TRecoVHit.hh"

#include "HistoSvc.hh"
#include "SACAnalysis.hh"
#include "ECalAnalysis.hh"
#include "TargetAnalysis.hh"


int main(Int_t argc, char **argv)
{

   Int_t fVerbose=0;
  //Accssing input file initialization
   TString InputFileName("PadmeRecoOutputFile.root");

   TFile fileIn(InputFileName);
   TTree* theTree = nullptr;
   fileIn.GetObject("Events",theTree);

   Int_t nevents = 0;
   if (theTree) 
     {
       std::cout<<"Tree found"<<std::endl; 
       nevents = theTree->GetEntries();
       std::cout<<"Tree has "<<nevents<<" events ...... "<<std::endl; 
       if (nevents<=0) 
	 {
	   std::cout<<"No events found in the tree ... exiting "<<std::endl;
	   fileIn.Close();
	   return(1);
	 }
     }
   else 
     {
       std::cout<<"Tree not found"<<std::endl; 
       std::cout<<"No tree in the input file "<<InputFileName<<"  ... exiting "<<std::endl;
       fileIn.Close();
       return(1);
     }

   //////////// You come here if a tree with >=0 events has been found 
   Int_t jevent = 0;
   TRecoEvent*        recoEv        = new TRecoEvent()       ;
   TTargetRecoEvent*  targetRecoEv  = new TTargetRecoEvent() ;
   TTargetRecoBeam*   targetRecoBeam= new TTargetRecoBeam()  ;
   TECalRecoEvent*    ecalRecoEv    = new TECalRecoEvent()   ;
   TPVetoRecoEvent*   pvetoRecoEv   = new TPVetoRecoEvent()  ;
   TEVetoRecoEvent*   evetoRecoEv   = new TEVetoRecoEvent()  ;
   THEPVetoRecoEvent* hepvetoRecoEv = new THEPVetoRecoEvent();
   TSACRecoEvent*     sacRecoEv     = new TSACRecoEvent()    ;

   theTree->SetBranchAddress("RecoEvent"  ,&recoEv)            ;
   theTree->SetBranchAddress("Target"     ,&targetRecoEv)      ;
   theTree->SetBranchAddress("TargetBeam" ,&targetRecoBeam)    ;
   theTree->SetBranchAddress("ECal"       ,&ecalRecoEv)        ;
   theTree->SetBranchAddress("PVeto"      ,&pvetoRecoEv)       ;
   theTree->SetBranchAddress("EVeto"      ,&evetoRecoEv)       ;
   theTree->SetBranchAddress("HEPVeto"    ,&hepvetoRecoEv)     ;
   theTree->SetBranchAddress("SAC"        ,&sacRecoEv)         ;

   
   //int histoOutput
   HistoSvc* hSvc = HistoSvc::GetInstance();
   hSvc->book();

   SACAnalysis*    sacAn    = new SACAnalysis(); 
   ECalAnalysis*   ecalAn   = new ECalAnalysis(); 
   TargetAnalysis* targetAn = new TargetAnalysis(); 

   
   for (Int_t i=0; i<nevents; ++i)
     {
       //       jevent = theTree->GetEvent(i);   
       jevent = theTree->GetEntry(i);   
       std::cout<<"------------------------------------------------------------ jevent = "<<i<<" size of the event "<< jevent<<std::endl;
       std::cout<<"-------------------------------------------------------Run/Event n. = "<<recoEv->GetRunNumber()<<" "<<recoEv->GetEventNumber()<<std::endl;

       Int_t nTargetHits = targetRecoEv->GetNHits();
       Int_t nECalHits = ecalRecoEv->GetNHits();
       Int_t nPVetoHits = pvetoRecoEv->GetNHits();
       Int_t nEVetoHits = evetoRecoEv->GetNHits();
       Int_t nHEPVetoHits = hepvetoRecoEv->GetNHits();
       Int_t nSACHits = sacRecoEv->GetNHits();
       std::cout<<"---- Hits in Target "<<nTargetHits
		<<" ECal "<<nECalHits
		<<" PVeto "<<nPVetoHits
		<<" EVeto "<<nEVetoHits
		<<" HEPVeto "<<nHEPVetoHits
		<<" SAC "<<nSACHits<<std::endl;
       std::cout<<"---- TargetBeam X and Y  "<<targetRecoBeam->getX()<<" "<<targetRecoBeam->getY()<<std::endl;
       if (fVerbose>1){
	 targetRecoEv->Print();
	 ecalRecoEv->Print();
	 pvetoRecoEv->Print();
	 evetoRecoEv->Print();
	 hepvetoRecoEv->Print();
	 sacRecoEv->Print();
       }
       //
       sacAn    ->Init(nSACHits,        sacRecoEv);
       ecalAn   ->Init(nECalHits,      ecalRecoEv);
       targetAn->Init(nTargetHits,  targetRecoEv);

       //
       sacAn     ->Process();
       ecalAn    ->Process();
       targetAn  ->Process();
       
       //
       //
       hSvc->FillNtuple();
     }


   

   /// end of job..........
   hSvc->save();

   // cleanup 
   delete sacAn;
   delete ecalAn;
   delete targetAn;

   return 0;
   
}

