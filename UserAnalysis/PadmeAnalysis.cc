#include "Riostream.h"
#include<iostream>
#include<string>
#include<fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TGraph.h"

#include <signal.h>
#include <fcntl.h>

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
#include "temp_corr.hh"
#include "utlConfigParser.hh"

#include "UserAnalysis.hh"

void usage(char* name){
  std::cout << "Usage: "<< name << " [-i <input file>] [-l <input file list>] [-n #MaxEvents] [-o <output file>] [-c <config file>] [-v] [-t] [-h]" 
	    << std::endl;
}

void sighandler(int sig){
    std::cerr << std::endl << "********************************************************************************" << std::endl;
    std::cerr << "Killed with Signal " << sig << std::endl << "Closing ROOT files ..." << std::endl;

    HistoSvc::GetInstance()->Finalize();

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

  long utc_time;

  signal(SIGXCPU,sighandler);
  signal(SIGINT,sighandler);
  signal(SIGTERM,sighandler);
  signal(127,sighandler);
  
  int opt;

  TObjArray InputFileNameList;

  TString CommentedLine = "#"; 
  TString ConfFileName("config/UserAnalysis.conf");
  TString OutputFileName = "OutputFileRoot.root";
  TString InputFileName;
  std::ifstream InputList;
  Int_t NInputFiles = 0;
  Int_t NSkippedFiles = 0;
  Int_t fVerbose = 0;
  Bool_t doNtuple = false;
  Int_t NEvt = 0; // Maximum number of events to process (0 = all)

  while ((opt = getopt(argc, argv, "c:i:l:n:o:vth")) != -1) {
    switch (opt) {
    case 'c':
      ConfFileName = TString(optarg);
      break;
    case 'i':
      InputFileNameList.Add(new TObjString(optarg));
      NInputFiles++;
      break;
    case 'l':
      InputList.open(optarg);
      while(InputFileName.ReadLine(InputList)){
	if (InputFileName.BeginsWith(CommentedLine)) { 
	  NSkippedFiles++;
	  continue;
	}
	InputFileNameList.Add(new TObjString(InputFileName.Data()));
	NInputFiles++;
      }
      InputList.close();
      break;
    case 'n':
      NEvt = TString(optarg).Atoi();
      break;
    case 'o':
      OutputFileName = TString(optarg);
      break;
    case 'v':
      fVerbose++;
      break;
    case 't':
      doNtuple = true;
      break;
    case 'h':
    case '?':
    default:
      usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  // Protection against potentially incorrect output filenames
  struct stat buffer;
  if ( (stat(OutputFileName.Data(), &buffer) == 0) && (! OutputFileName.EndsWith(".root")) ) {
    std::cout << " [PadmeReco] Output file " << OutputFileName << " exists and is not *.root: potentially a destructive call" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  if(InputFileNameList.GetEntries() == 0) {
    perror(Form("ERROR - No Input File specified"));
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  printf("Number of files to be processed: %d\n",NInputFiles);
  if (NSkippedFiles) printf("Number of files skipped: %d\n",NSkippedFiles);

  // Initialize histogram service
  printf("---> Initializing histogram service\n");
  HistoSvc* hSvc = HistoSvc::GetInstance();
  hSvc->Initialize(OutputFileName);
  
  // Create configuration file parser
  utl::ConfigParser* cfgParser = new utl::ConfigParser((const std::string)ConfFileName.Data());
  if (fVerbose) cfgParser->Print();

  // Define trigger pattern
  unsigned int trigMask = 0x00ff; // Default: enable all triggers
  if (cfgParser->HasConfig("GENERAL","Triggers")) {
    trigMask = 0x0000; // Start with empty trigger mask
    for (auto trig : cfgParser->GetConfig("GENERAL","Triggers")) {
      if (fVerbose) printf("Enabling trigger %s\n",trig.c_str());
      if (trig.compare("ALL") == 0) {
	trigMask = 0x00ff; // Enable all triggers
	break; // No need to check further
      }
      if (trig.compare("BEAM") == 0)         trigMask |= (1 << TRECOEVENT_TRIGMASKBIT_BEAM);
      if (trig.compare("COSMICS") == 0)      trigMask |= (1 << TRECOEVENT_TRIGMASKBIT_COSMICS);
      if (trig.compare("DUALTIMER") == 0)    trigMask |= (1 << TRECOEVENT_TRIGMASKBIT_DUALTIMER);
      if (trig.compare("UNCORRELATED") == 0) trigMask |= (1 << TRECOEVENT_TRIGMASKBIT_UNCORRELATED);
      if (trig.compare("CORRELATED") == 0)   trigMask |= (1 << TRECOEVENT_TRIGMASKBIT_CORRELATED);
    }
  }
  if (fVerbose) printf("Accepted trigger mask: 0x%2.2x\n",trigMask);

  // Create pointers to reconstructed event branches
  TRecoEvent*           fRecoEvent        =0;
  TTargetRecoEvent*     fTargetRecoEvent  =0;
  TEVetoRecoEvent*      fEVetoRecoEvent   =0;
  TPVetoRecoEvent*      fPVetoRecoEvent   =0;
  THEPVetoRecoEvent*    fHEPVetoRecoEvent =0;
  TECalRecoEvent*       fECalRecoEvent    =0;
  TSACRecoEvent*        fSACRecoEvent     =0;
  TTargetRecoBeam*      fTargetRecoBeam   =0;
  TRecoVClusCollection* fSACRecoCl        =0;
  TRecoVClusCollection* fECalRecoCl       =0;
  TRecoVClusCollection* fPVetoRecoCl      =0;
  TRecoVClusCollection* fEVetoRecoCl      =0;
  TRecoVClusCollection* fHEPVetoRecoCl    =0;

  TTree::SetMaxTreeSize(190000000000);

  TString recoTreeName = "Events";
  TChain* fRecoChain = BuildChain(recoTreeName,InputFileNameList);

  Int_t nevents = 0;
  if (fRecoChain) {
    nevents = fRecoChain->GetEntries();
    if (nevents<=0) {
      std::cout<<"No events found in the tree ... exiting "<<std::endl;
      exit(EXIT_FAILURE);
    }
  } else {
    printf("---> Tree %s not found in input chain ... exiting\n",recoTreeName.Data());
    exit(EXIT_FAILURE);
  }
  TObjArray* branches = fRecoChain->GetListOfBranches();
  printf("---> Tree '%s' found in input chain with %d branches and %d events\n",recoTreeName.Data(),branches->GetEntries(),nevents);
  for (Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++) {
    
    TBranch* branch = (TBranch*)(*branches)[iBranch];
    TString branchName = branch->GetName();
    TClass* branchObjectClass = TClass::GetClass(branch->GetClassName());
    printf("     Branch %-16s containing objects of class %-22s with size %6lld\n",branchName.Data(),branchObjectClass->GetName(),branch->GetTotalSize());
    
    if (branchName=="RecoEvent") {
      fRecoEvent = new TRecoEvent();
      fRecoChain->SetBranchAddress(branchName.Data(),&fRecoEvent);
    } else if (branchName=="Target_Hits") {
      fTargetRecoEvent = new TTargetRecoEvent();
      fRecoChain->SetBranchAddress(branchName.Data(),&fTargetRecoEvent);
    } else if (branchName=="EVeto_Hits") {
      fEVetoRecoEvent = new TEVetoRecoEvent();
      fRecoChain->SetBranchAddress(branchName.Data(),&fEVetoRecoEvent);
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
      //} else if (branchName=="TPix") {
      //  fTPixRecoEvent = new TTPixRecoEvent();
      //  fRecoChain->SetBranchAddress(branchName.Data(),&fTPixRecoEvent);
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
  
  // initialize temp corr
  printf("---> Initializing temperature corrections\n");
  Bool_t TempCorr = true;
  if(InitTemps() == 0) {
    printf("<--- Temperature corrections initialized\n");
  } else {
    printf("!--- Error while initializing temperature corrections\n");
    TempCorr = false;
  }
  
  // Book flat ntuple if required
  if (doNtuple) hSvc->BookNtuple();
  
  // Initialize input event structure
  PadmeAnalysisEvent* event = new PadmeAnalysisEvent();
  event->RecoEvent        = fRecoEvent       ;
  event->TargetRecoEvent  = fTargetRecoEvent ;
  event->EVetoRecoEvent   = fEVetoRecoEvent  ;
  event->PVetoRecoEvent   = fPVetoRecoEvent  ;
  event->HEPVetoRecoEvent = fHEPVetoRecoEvent;
  event->ECalRecoEvent    = fECalRecoEvent   ;
  event->SACRecoEvent     = fSACRecoEvent    ;
  event->TargetRecoBeam   = fTargetRecoBeam  ;
  event->SACRecoCl        = fSACRecoCl       ;
  event->ECalRecoCl       = fECalRecoCl      ;
  event->PVetoRecoCl      = fPVetoRecoCl     ;
  event->EVetoRecoCl      = fEVetoRecoCl     ;
  event->HEPVetoRecoCl    = fHEPVetoRecoCl   ;
  
  printf("---> Initializing user analysis\n");
  UserAnalysis* UserAn = new UserAnalysis(ConfFileName,fVerbose);
  UserAn->Init(event);
  
  Int_t nTargetHits =0;
  Int_t nECalHits   =0;   
  Int_t nPVetoHits  =0;  
  Int_t nEVetoHits  =0;
  Int_t nHEPVetoHits=0;
  Int_t nSACHits    =0;
  
  if (NEvt >0 && NEvt<nevents) nevents=NEvt;
  for (Int_t i=0; i<nevents; ++i) {
    
    jevent = fRecoChain->GetEntry(i);   
    
    // Check if event should be analyzed
    //printf("Event %d trigger mask: 0x%2.2x\n",i,fRecoEvent->GetTriggerMask());
    if ( !(fRecoEvent->GetTriggerMask() & trigMask) ) continue;
    //printf("Event %d accepted\n",i);
    
    // Debug printout
    if ( (i%1000==0) || (fVerbose>0 && (i%100==0)) || (fVerbose>1) ){
      std::cout<<"---> Event = "<<i<<"/"<<nevents<<" - Size "<< jevent << " Run " << fRecoEvent->GetRunNumber() <<" Event "<<fRecoEvent->GetEventNumber()<<std::endl;
      if (TempCorr) {
	TTimeStamp timevent=fRecoEvent->GetEventTime();
	utc_time=timevent.GetSec();
	float temp_event=GetTemp(utc_time);
	float temp_corr=GetEventTempCorr();
	std::cout<< "     Temperature corrections - UTC "<<utc_time<< " - Temperature "<<temp_event<<" - Correction "<<temp_corr<<std::endl;
      }
      if (fECalRecoEvent)    nECalHits   = fECalRecoEvent->GetNHits();
      if (fTargetRecoEvent)  nTargetHits = fTargetRecoEvent->GetNHits(); 
      if (fPVetoRecoEvent)   nPVetoHits  = fPVetoRecoEvent->GetNHits();
      if (fEVetoRecoEvent)   nEVetoHits  = fEVetoRecoEvent->GetNHits();
      if (fHEPVetoRecoEvent) nHEPVetoHits= fHEPVetoRecoEvent->GetNHits();
      if (fSACRecoEvent)     nSACHits    = fSACRecoEvent->GetNHits();
      std::cout<<"     Hits in Target "<<nTargetHits
	       <<" ECal "<<nECalHits
	       <<" PVeto "<<nPVetoHits
	       <<" EVeto "<<nEVetoHits
	       <<" HEPVeto "<<nHEPVetoHits
	       <<" SAC "<<nSACHits<<std::endl;
      std::cout<<"     TargetBeam X and Y  "<<fTargetRecoBeam->getX()<<" "<<fTargetRecoBeam->getY()<<std::endl;
    }
    if (fVerbose>2) {
      if (fTargetRecoEvent)  fTargetRecoEvent->Print();
      if (fECalRecoEvent)    fECalRecoEvent->Print();
      if (fPVetoRecoEvent)   fPVetoRecoEvent->Print();
      if (fEVetoRecoEvent)   fEVetoRecoEvent->Print();
      if (fHEPVetoRecoEvent) fHEPVetoRecoEvent->Print();
      if (fSACRecoEvent)     fSACRecoEvent->Print();
    }
    
    //Bool_t isMC = false;
    //if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) isMC=true;
    //if (isMC) printf("---> Analyzing simulated data");
    
    if (doNtuple) hSvc->FillNtuple(event);
    UserAn->Process();
    
  }
  
  printf("---> Finalizing user analysis\n");
  UserAn->Finalize();
  printf("---> Finalizing histogram service\n");
  hSvc->Finalize();
  
  delete UserAn;
  delete hSvc;
  
  return 0;   
}
