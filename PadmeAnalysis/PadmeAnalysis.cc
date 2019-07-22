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
//#include "PadmeReconstruction.hh"
//#include "PadmeVReconstruction.hh"

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
#include "PVetoAnalysis.hh"
#include "EVetoAnalysis.hh"
#include "HEPVetoAnalysis.hh"

void usage(char* name){
  std::cout << "Usage: "<< name << " [-h] [-b/-B #MaxFiles] [-i InputFile.root] [-l InputListFile.txt] [-n #MaxEvents] [-o OutputFile.root] [-s Seed] [-c ConfigFileName.conf] [-v verbose] [-m ProcessingMode] [-t ntuple] " 
	    << std::endl;
}

void sighandler(int sig){
    std::cerr << std::endl << "********************************************************************************" << std::endl;
    std::cerr << "Killed with Signal " << sig << std::endl << "Closing ROOT files ..." << std::endl;

    HistoSvc* hSvc = HistoSvc::GetInstance();
    hSvc->save();

    std::cerr << "... Histogram file Done" << std::endl;
    std::cerr << std::endl << "********************************************************************************" << std::endl;

    exit(0);
}
// Verify URL for each file
TString CheckProtocols(TString OldStr){
  TString NewStr=OldStr;
  if(NewStr.EndsWith("\r")) NewStr.Remove(NewStr.Last('\r'));
  return NewStr;
}

TChain* BuildChain(TString treeName, TObjArray fInputFileNameList){
  TChain* chain = new TChain(treeName.Data());
  for(Int_t iFile = 0; iFile < fInputFileNameList.GetEntries(); iFile++)
    chain->AddFile(CheckProtocols(((TObjString*)fInputFileNameList.At(iFile))->GetString()));
  if(chain->GetEntries() == 0){
    delete chain;
    chain = 0;
  }
  return chain;
}



int main(Int_t argc, char **argv)
{

  signal(SIGXCPU,sighandler);
  signal(SIGINT,sighandler);
  signal(SIGTERM,sighandler);
  signal(127,sighandler);
  
  extern char *optarg;
  int opt;
  TString CommentedLine("%");
  TString OutputFileName("OutputFileRoot.root");
  TString InputFileName("PadmeRecoOutputFile.root");
  TString InputListFileName("InputListFile.txt");
  Int_t fVerbose=0;
  Int_t fProcessingMode=0;
  Int_t fntuple=0;
  Int_t iFile = 0, NFiles = 100000, NEvt = 0;
  //UInt_t Seed = 4357;
  struct stat filestat;
  TString ConfFileName("config/PadmeReconstruction.conf");

  Int_t n_options_read = 0;
  Int_t nb=0, nc=0, ni=0, nl=0, nn=0, no=0, ns=0, nv=0, nval=0, nt=0;
  while ((opt = getopt(argc, argv, "b:B:c:h:i:l:n:o:s:v:m:t")) != -1) {
      n_options_read++;
      switch (opt) {
      case 'b':
      case 'B':
	nb++;
	NFiles = TString(optarg).Atoi();
	break;
      case 'c':
	nc++;
	ConfFileName = TString(optarg);
	break;
      case 'h':
	usage(argv[0]);
	return 0;
      case 'i':
	ni++;
	InputFileName = TString(optarg);
	break;
      case 'l':
	nl++;
	InputListFileName = TString(optarg);
	break;
      case 'n':
	nn++;
	NEvt = TString(optarg).Atoi();
	break;
      case 'o':
	no++;
	OutputFileName = TString(optarg);
	break;
      case 's':
       	ns++;
       	//Seed = (UInt_t)TString(optarg).Atoi();
      	break;
      case 'v':
	nv++;
	fVerbose = (Int_t)TString(optarg).Atoi();
	break;
      case 'm':
	nval++;
	fProcessingMode = (Int_t)TString(optarg).Atoi();
	break;
      case 't':
	nt++;
	fntuple = (Int_t)TString(optarg).Atoi();
      default:
      break;
	usage(argv[0]);
	return 0;
      }
    }

    // Sanity checks on the input
    if (!n_options_read || NEvt<0 || NFiles<=0) {
      usage(argv[0]);
      return 0;
    }
    if (nb>1 || nc>1 || ni>1 || nl>1 || nn>1 || no>1 || ns>0 ) {
      std::cerr << "[PadmeReco] Multiple arguments of the same type are not allowed" << std::endl;
      return 0;
    }

    // Protection against potentially incorrect output filenames
    struct stat buffer;
    if (!OutputFileName.EndsWith(".root") && !stat(OutputFileName.Data(), &buffer)) {
      std::cout << " [PadmeReco] Output file exists and is not *.root: potentially a destructive call" << std::endl;
      return 0;
    }

    Int_t nSkippedFiles=0;
    TObjArray InputFileNameList;
    if(stat(Form(InputListFileName.Data()), &filestat) == 0) { //-l option used
        std::ifstream InputList(InputListFileName.Data());
        while(InputFileName.ReadLine(InputList) && iFile < NFiles){
	  if (InputFileName.BeginsWith(CommentedLine)) 
	    { 
	      nSkippedFiles++;
	      continue;
	    }
//            if(stat(Form(InputFileName.Data()), &filestat) == 0)
          InputFileNameList.Add(new TObjString(InputFileName.Data()));
          iFile++;
        }
    } else if(InputFileName.CompareTo("")) //-i option used
//        if(stat(Form(InputFileName.Data()), &filestat) == 0)
      InputFileNameList.Add(new TObjString(InputFileName.Data()));

    if(InputFileNameList.GetEntries() == 0) {
        perror(Form("No Input File"));
        exit(1);
    }
    std::cout<<"Number of files to be processed "<<iFile<<" Number of files skipped "<<nSkippedFiles<<std::endl;


  HistoSvc* hSvc = HistoSvc::GetInstance();
  hSvc->setOutputFileName(OutputFileName);

  

  //Accssing input file initialization
  //TFile fileIn(InputFileName);
  //   TTree* theTree = nullptr;


  TRecoEvent*                     fRecoEvent            =0;
  TTargetRecoEvent*               fTargetRecoEvent      =0;
  TEVetoRecoEvent*                fEVetoRecoEvent       =0;
  TPVetoRecoEvent*                fPVetoRecoEvent       =0;
  THEPVetoRecoEvent*              fHEPVetoRecoEvent     =0;
  TECalRecoEvent*                 fECalRecoEvent        =0;
  TSACRecoEvent*                  fSACRecoEvent         =0;
  TTargetRecoBeam*                fTargetRecoBeam       =0;
  TRecoVClusCollection*           fSACRecoCl            =0;
  TRecoVClusCollection*           fECalRecoCl           =0;
  TRecoVClusCollection*           fPVetoRecoCl          =0;
  TRecoVClusCollection*           fEVetoRecoCl          =0;
  TRecoVClusCollection*           fHEPVetoRecoCl        =0;

   TTree::SetMaxTreeSize(190000000000);

   TString recoTreeName = "Events";
   TChain* fRecoChain = BuildChain(recoTreeName, InputFileNameList);

   //fileIn.GetObject("Events",theTree);

   Int_t nevents = 0;
   if (fRecoChain) 
     {
       std::cout<<"Tree found"<<std::endl; 
       nevents = fRecoChain->GetEntries();
       std::cout<<"Tree has "<<nevents<<" events ...... "<<std::endl; 
       if (nevents<=0) 
	 {
	   std::cout<<"No events found in the tree ... exiting "<<std::endl;
	   //fileIn.Close();
	   return(1);
	 }
     }
   else 
     {
       std::cout<<"Chain not found"<<std::endl; 
       std::cout<<"No tree in the input file(s)  ... exiting "<<std::endl;
       //fileIn.Close();
       return(1);
     }
   TObjArray* branches = fRecoChain->GetListOfBranches();
   std::cout << "Found Tree " << recoTreeName << "' with " << branches->GetEntries() << " branches and " << nevents << " entries" << std::endl;
   for(Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++){

      TString branchName = ((TBranch*)(*branches)[iBranch])->GetName();
      TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*branches)[iBranch])->GetClassName());
      std::cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << std::endl;

      Int_t branchSize = 0;
      if (branchName=="RecoEvent") {
	fRecoEvent = new TRecoEvent();
	fRecoChain->SetBranchAddress(branchName.Data(),&fRecoEvent);
	std::cout<<"RecoEvent branch size = "<< branchSize <<std::endl;
      } else if (branchName=="Target_Hits") {
	fTargetRecoEvent = new TTargetRecoEvent();
	fRecoChain->SetBranchAddress(branchName.Data(),&fTargetRecoEvent);
      } else if (branchName=="EVeto_Hits") {
	fEVetoRecoEvent = new TEVetoRecoEvent();
	branchSize = fRecoChain->SetBranchAddress(branchName.Data(),&fEVetoRecoEvent);
	std::cout<<"EvetoREcoEvent branch size = "<< branchSize <<std::endl;
      } else if (branchName=="PVeto_Hits") {
	fPVetoRecoEvent = new TPVetoRecoEvent();
	fRecoChain->SetBranchAddress(branchName.Data(),&fPVetoRecoEvent);
      } else if (branchName=="HEPVeto_Hits") {
	fHEPVetoRecoEvent = new THEPVetoRecoEvent();
	fRecoChain->SetBranchAddress(branchName.Data(),&fHEPVetoRecoEvent);
      } else if (branchName=="ECal_Hits") {
	fECalRecoEvent = new TECalRecoEvent();
	fRecoChain->SetBranchAddress(branchName.Data(),&fECalRecoEvent);
      } else if (branchName=="SAC_Hits") {
	fSACRecoEvent = new TSACRecoEvent();
	fRecoChain->SetBranchAddress(branchName.Data(),&fSACRecoEvent);
	//      } else if (branchName=="TPix") {
	//	fTPixRecoEvent = new TTPixRecoEvent();
	//	fRecoChain->SetBranchAddress(branchName.Data(),&fTPixRecoEvent);
      } else if (branchName=="TargetBeam") {
	fTargetRecoBeam = new TTargetRecoBeam();
	fRecoChain->SetBranchAddress(branchName.Data(),&fTargetRecoBeam);
      } else if (branchName=="SAC_Clusters") {
	fSACRecoCl = new TRecoVClusCollection();
	fRecoChain->SetBranchAddress(branchName.Data(),&fSACRecoCl);
      } else if (branchName=="ECal_Clusters") {
	fECalRecoCl = new TRecoVClusCollection();
	fRecoChain->SetBranchAddress(branchName.Data(),&fECalRecoCl);
      } else if (branchName=="PVeto_Clusters") {
	fPVetoRecoCl = new TRecoVClusCollection();
	fRecoChain->SetBranchAddress(branchName.Data(),&fPVetoRecoCl);
      } else if (branchName=="EVeto_Clusters") {
	fEVetoRecoCl = new TRecoVClusCollection();
	fRecoChain->SetBranchAddress(branchName.Data(),&fEVetoRecoCl);
      } else if (branchName=="HEPVeto_Clusters") {
	fHEPVetoRecoCl = new TRecoVClusCollection();
	fRecoChain->SetBranchAddress(branchName.Data(),&fHEPVetoRecoCl);
      }

   }



   //////////// You come here if a Chain with >=0 events has been found 
   Int_t jevent = 0;
   
   //int histoOutput
   //hSvc->book(fProcessingMode);
   hSvc->book(fProcessingMode, fntuple);

   std::vector<ValidationBase*> algoList;
   SACAnalysis*         sacAn  = new SACAnalysis(fProcessingMode, fVerbose);
   algoList.push_back(sacAn);
   ECalAnalysis*       ecalAn  = new ECalAnalysis(fProcessingMode, fVerbose); 
   algoList.push_back(ecalAn);
   TargetAnalysis*   targetAn  = new TargetAnalysis(fProcessingMode, fVerbose); 
   algoList.push_back(targetAn);
   PVetoAnalysis*     pvetoAn  = new PVetoAnalysis(fProcessingMode, fVerbose);
   algoList.push_back(pvetoAn);
   EVetoAnalysis*     evetoAn  = new EVetoAnalysis(fProcessingMode, fVerbose);
   algoList.push_back(evetoAn);
   HEPVetoAnalysis* hepvetoAn  = new HEPVetoAnalysis(fProcessingMode, fVerbose);
   algoList.push_back(hepvetoAn);
  
   Int_t nTargetHits =0;
   Int_t nECalHits   =0;   
   Int_t nPVetoHits  =0;  
   Int_t nEVetoHits  =0;
   Int_t nHEPVetoHits=0;
   Int_t nSACHits    =0;

   if (NEvt >0 && NEvt<nevents) nevents=NEvt;
   for (Int_t i=0; i<nevents; ++i)
     {
       //       jevent = theTree->GetEvent(i);   
       jevent = fRecoChain->GetEntry(i);   
       if ( (fVerbose>0 && (i%10==0)) || (i%1000==0) ){
	 std::cout<<"--------------------------------------------------------- jevent = "<<i<<"/"<<nevents<<" -- size of the event "<< jevent<<std::endl;
	 std::cout<<"----------------------------------------------------Run/Event n. = "<<fRecoEvent->GetRunNumber()<<" "<<fRecoEvent->GetEventNumber()<<std::endl;
       }
       
       if (fECalRecoEvent)   nECalHits   = fECalRecoEvent->GetNHits();
       if (fTargetRecoEvent) nTargetHits = fTargetRecoEvent->GetNHits(); 
       if (fPVetoRecoEvent)  nPVetoHits  = fPVetoRecoEvent->GetNHits();
       if (fEVetoRecoEvent)  nEVetoHits  = fEVetoRecoEvent->GetNHits();
       if (fHEPVetoRecoEvent)nHEPVetoHits= fHEPVetoRecoEvent->GetNHits();
       if (fSACRecoEvent)    nSACHits    = fSACRecoEvent->GetNHits();
      
       if ( (fVerbose>0 && (i%1==0)) || fVerbose>1) {
	   std::cout<<"---- Hits in Target "<<nTargetHits
		    <<" ECal "<<nECalHits
		    <<" PVeto "<<nPVetoHits
		    <<" EVeto "<<nEVetoHits
		    <<" HEPVeto "<<nHEPVetoHits
		    <<" SAC "<<nSACHits<<std::endl;
	   std::cout<<"---- TargetBeam X and Y  "<<fTargetRecoBeam->getX()<<" "<<fTargetRecoBeam->getY()<<std::endl;
	 }
       if (fVerbose>2){
	 if (fTargetRecoEvent) fTargetRecoEvent->Print();
	 if (fECalRecoEvent)   fECalRecoEvent->Print();
	 if (fPVetoRecoEvent)  fPVetoRecoEvent->Print();
	 if (fEVetoRecoEvent)  fEVetoRecoEvent->Print();
	 if (fHEPVetoRecoEvent)fHEPVetoRecoEvent->Print();
	 if (fSACRecoEvent)    fSACRecoEvent->Print();
       }
       //
      

       sacAn      ->Init(fRecoEvent, fSACRecoEvent,     fSACRecoCl            );
       ecalAn     ->Init(fRecoEvent, fECalRecoEvent,    fECalRecoCl           );
       targetAn   ->Init(fRecoEvent, fTargetRecoEvent,  fTargetRecoBeam       );
       pvetoAn    ->Init(fRecoEvent, fPVetoRecoEvent,   fPVetoRecoCl          );
       evetoAn    ->Init(fRecoEvent, fEVetoRecoEvent,   fEVetoRecoCl          );
       hepvetoAn  ->Init(fRecoEvent, fHEPVetoRecoEvent, fHEPVetoRecoCl        );

       //
       targetAn    ->Process();
       ecalAn      ->Process();
       sacAn       ->Process();
       pvetoAn     ->Process();
       evetoAn     ->Process();
       hepvetoAn   ->Process();
       
       //
       //
       hSvc->FillNtuple();
     }
   
   ecalAn   ->Finalize();
   targetAn ->Finalize();
   /*
   sacAn    ->Finalize();
   pvetoAn  ->Finalize();
   evetoAn  ->Finalize();
   hepvetoAn->Finalize();
   */

   /// end of job..........
   hSvc->save();

   // cleanup 
   delete sacAn;
   delete ecalAn;
   delete targetAn;
   delete pvetoAn;
   delete evetoAn;
   delete hepvetoAn;

   return 0;
   
}

