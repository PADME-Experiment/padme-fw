#include "Riostream.h"
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "TRecoEvent.hh"
#include "TTargetRecoEvent.hh"
#include "TETagRecoEvent.hh"
#include "TTargetRecoBeam.hh"
#include "TECalRecoEvent.hh"
#include "TPVetoRecoEvent.hh"
#include "TEVetoRecoEvent.hh"
#include "TSACRecoEvent.hh"
#include "THEPVetoRecoEvent.hh"
#include "TLeadGlassRecoEvent.hh"
#include "TMCTruthEvent.hh"

#include "HistoSvc.hh"
#include "StdNtuple.hh"
#include "temp_corr.hh"
#include "TempCorr.hh"
#include "utlConfigParser.hh"
#include "UserAnalysis.hh"

void usage(char* name){
  std::cout << "Usage: "<< name << " [-i <input file>] [-l <input file list>] [-n #MaxEvents] [-o <output file>] [-c <config file>] [-t] [-v] [-h]" 
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

  //long utc_time;
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
  if (fVerbose) {
    printf("Number of files to be processed: %d\n",NInputFiles);
    if (NSkippedFiles) printf("Number of files skipped: %d\n",NSkippedFiles);
  }

  // Initialize histogram service
  if (fVerbose) printf("---> Initializing histogram service\n");
  HistoSvc* hSvc = HistoSvc::GetInstance();
  hSvc->SetVerbose(fVerbose);
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
      if (trig.compare("BEAM") == 0)         trigMask |= (1U << TRECOEVENT_TRIGMASKBIT_BEAM);
      if (trig.compare("COSMICS") == 0)      trigMask |= (1U << TRECOEVENT_TRIGMASKBIT_COSMICS);
      if (trig.compare("DUALTIMER") == 0)    trigMask |= (1U << TRECOEVENT_TRIGMASKBIT_DUALTIMER);
      if (trig.compare("UNCORRELATED") == 0) trigMask |= (1U << TRECOEVENT_TRIGMASKBIT_UNCORRELATED);
      if (trig.compare("CORRELATED") == 0)   trigMask |= (1U << TRECOEVENT_TRIGMASKBIT_CORRELATED);
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
  TETagRecoEvent*       fETagRecoEvent    =0;
  TSACRecoEvent*        fSACRecoEvent     =0;
  TLeadGlassRecoEvent*  fLeadGlassRecoEvent =0;
  TTargetRecoBeam*      fTargetRecoBeam   =0;
  TRecoVClusCollection* fSACRecoCl        =0;
  TRecoVClusCollection* fECalRecoCl       =0;
  TRecoVClusCollection* fETagRecoCl       =0;
  TRecoVClusCollection* fPVetoRecoCl      =0;
  TRecoVClusCollection* fEVetoRecoCl      =0;
  TRecoVClusCollection* fHEPVetoRecoCl    =0;
  TMCTruthEvent*        fMCTruthEvent     =0;

  TTree::SetMaxTreeSize(190000000000);

  TString recoTreeName = "Events";
  cout<<"Building chain "<<endl;
  //  if(InputFileNameList.size()>200) cout<<"List too big!!!" <<endl;
  TChain* fRecoChain = BuildChain(recoTreeName,InputFileNameList);
  cout<<"chain  Built "<<endl;

  Int_t nevents = 0;
  if (fRecoChain) {
    nevents = fRecoChain->GetEntries();
    if (nevents<=0) {
      printf("---> Tree %s contains no events ... exiting\n",recoTreeName.Data());
      exit(EXIT_FAILURE);
    }
  } else {
    printf("---> Tree %s not found in input chain ... exiting\n",recoTreeName.Data());
    exit(EXIT_FAILURE);
  }
  TObjArray* branches = fRecoChain->GetListOfBranches();
  if (fVerbose) printf("---> Tree '%s' found in input chain with %d branches and %d events\n",recoTreeName.Data(),branches->GetEntries(),nevents);
  for (Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++) {
    
    TBranch* branch = (TBranch*)(*branches)[iBranch];
    TString branchName = branch->GetName();
    TClass* branchObjectClass = TClass::GetClass(branch->GetClassName());
    if (fVerbose>1) printf("     Branch %-16s containing objects of class %-22s with size %6lld\n",branchName.Data(),branchObjectClass->GetName(),branch->GetTotalSize());
    
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
    } else if (branchName=="ETag_Hits") {
      fETagRecoEvent = new TETagRecoEvent();
      fRecoChain->SetBranchAddress(branchName.Data(),&fETagRecoEvent);
    } else if (branchName=="ECal_Hits") {
      fECalRecoEvent = new TECalRecoEvent();
      fRecoChain->SetBranchAddress(branchName.Data(),&fECalRecoEvent);
    } else if (branchName=="SAC_Hits") {
      fSACRecoEvent = new TSACRecoEvent();
      fRecoChain->SetBranchAddress(branchName.Data(),&fSACRecoEvent);
    } else if (branchName=="LeadGlass_Hits") {
      fLeadGlassRecoEvent = new TLeadGlassRecoEvent();
      fRecoChain->SetBranchAddress(branchName.Data(),&fLeadGlassRecoEvent);
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
    } else if (branchName=="ETag_Clusters") {  //MR 06/22
      fETagRecoCl = new TRecoVClusCollection();
      fRecoChain->SetBranchAddress(branchName.Data(),&fETagRecoCl);
    } else if (branchName=="HEPVeto_Clusters") {
      fHEPVetoRecoCl = new TRecoVClusCollection();
      fRecoChain->SetBranchAddress(branchName.Data(),&fHEPVetoRecoCl);
    } else if (branchName=="MCTruth") {
      fMCTruthEvent = new TMCTruthEvent();
      fRecoChain->SetBranchAddress(branchName.Data(),&fMCTruthEvent);
    }
    
  }
  
  //////////// You come here if a Chain with >=0 events has been found 
  Int_t jevent = 0;
  
  // Initialize temperature correction service
  if (fVerbose) printf("---> Initializing temperature correction service\n");
  TempCorr* TempCorr = TempCorr::GetInstance();
  TempCorr->SetVerbose(fVerbose);
  Bool_t tCorrOK = true;
  Int_t rc;
  if (cfgParser->HasConfig("ECAL","Temp_File")) {
    rc = TempCorr->Initialize(TString(cfgParser->GetSingleArg("ECAL","Temp_File")));
  } else {
    rc = TempCorr->Initialize();
  }
  if (rc == 0) {
    if (fVerbose) printf("<--- Temperature corrections initialized\n");
  } else {
    printf("!--- Error while initializing temperature corrections\n");
    tCorrOK = false;
  }
  //InitTemps(); // Old system

  // Book flat ntuple if required
  StdNtuple* stdNtuple = 0;
  if (doNtuple) {
    if (fVerbose) printf("---> Initializing standard ntuple\n");
    stdNtuple = new StdNtuple();
  }
  
  // Initialize input event structure
  PadmeAnalysisEvent* event = new PadmeAnalysisEvent();
  event->RecoEvent        = fRecoEvent       ;
  event->TargetRecoEvent  = fTargetRecoEvent ;
  event->EVetoRecoEvent   = fEVetoRecoEvent  ;
  event->PVetoRecoEvent   = fPVetoRecoEvent  ;
  event->HEPVetoRecoEvent = fHEPVetoRecoEvent;
  event->ETagRecoEvent    = fETagRecoEvent   ; //MR 06/22 
  event->ECalRecoEvent    = fECalRecoEvent   ;
  event->SACRecoEvent     = fSACRecoEvent    ;
  event->LeadGlassRecoEvent = fLeadGlassRecoEvent;
  event->TargetRecoBeam   = fTargetRecoBeam  ;
  event->SACRecoCl        = fSACRecoCl       ;
  event->ECalRecoCl       = fECalRecoCl      ;
  event->PVetoRecoCl      = fPVetoRecoCl     ;
  event->ETagRecoCl       = fETagRecoCl      ;
  event->EVetoRecoCl      = fEVetoRecoCl     ;
  event->HEPVetoRecoCl    = fHEPVetoRecoCl   ;
  event->MCTruthEvent     = fMCTruthEvent    ;

  if (fVerbose) printf("---> Initializing user analysis\n");
  UserAnalysis* UserAn = new UserAnalysis(ConfFileName,fVerbose);
  UserAn->Init(event);
  
  Int_t nTargetHits =0;
  Int_t nECalHits   =0;   
  Int_t nPVetoHits  =0;  
  Int_t nEVetoHits  =0;
  Int_t nHEPVetoHits=0;
  Int_t nSACHits    =0;
  Int_t nETagHits   =0;
  Int_t nLeadGlassHits=0;
  
  UInt_t mcEvent = (1U << TRECOEVENT_STATUSBIT_SIMULATED); // Mask to check if event is MC

  if (NEvt >0 && NEvt<nevents) nevents=NEvt;
  for (Int_t i=0; i<nevents; ++i) {
    
    jevent = fRecoChain->GetEntry(i);   
    
    // Check if event should be analyzed
    //printf("Event %d trigger mask: 0x%2.2x\n",i,fRecoEvent->GetTriggerMask());
    if ( !(fRecoEvent->GetEventStatus() & mcEvent) && !(fRecoEvent->GetTriggerMask() & trigMask) ) continue;
    //printf("Event %d accepted\n",i);
    //printf("DEBUG 0x%02x %d\n",fRecoEvent->GetTriggerMask(),fECalRecoEvent->GetNHits());
    
    // Debug printout
    if ( (fVerbose>0 && (i%1000==0)) || (fVerbose>1 && (i%100==0)) || (fVerbose>2) ){
      std::cout<<"---> Event = "<<i<<"/"<<nevents<<" - Size "<< jevent << " Run " << fRecoEvent->GetRunNumber() <<" Event "<<fRecoEvent->GetEventNumber()<<std::endl;
      if (!(fRecoEvent->GetEventStatus() & mcEvent) && tCorrOK) { // Show ECal tempereature corrections only for real data
	TTimeStamp timevent=fRecoEvent->GetEventTime();
	//utc_time=timevent.GetSec();
	//float temp_event=GetTemp(utc_time);
	//float temp_corr=GetEventTempCorr();
	//printf("     Time %ld Old Temp %5.2f Old Corr %7.5f New Temp %6.4f %6.4f New Corr %7.5f %7.5f\n",timevent.GetSec(),temp_event,temp_corr,TempCorr->GetTemp(timevent,0),TempCorr->GetTemp(timevent,1),TempCorr->GetTempCorr(timevent,0),TempCorr->GetTempCorr(timevent,1));
	printf("     Time %ld Temp %6.4f %6.4f Corr %7.5f %7.5f\n",timevent.GetSec(),TempCorr->GetTemp(timevent,0),TempCorr->GetTemp(timevent,1),TempCorr->GetTempCorr(timevent,0),TempCorr->GetTempCorr(timevent,1));
      }
      if (fECalRecoEvent)    nECalHits   = fECalRecoEvent->GetNHits();
      if (fTargetRecoEvent)  nTargetHits = fTargetRecoEvent->GetNHits(); 
      if (fPVetoRecoEvent)   nPVetoHits  = fPVetoRecoEvent->GetNHits();
      if (fEVetoRecoEvent)   nEVetoHits  = fEVetoRecoEvent->GetNHits();
      if (fHEPVetoRecoEvent) nHEPVetoHits= fHEPVetoRecoEvent->GetNHits();
      if (fSACRecoEvent)     nSACHits    = fSACRecoEvent->GetNHits();
      if (fETagRecoEvent)    nETagHits   = fETagRecoEvent->GetNHits();
      if (fLeadGlassRecoEvent) nLeadGlassHits= fLeadGlassRecoEvent->GetNHits();
      std::cout<<"     Hits in Target "<<nTargetHits
	       <<" ECal "<<nECalHits
	       <<" PVeto "<<nPVetoHits
	       <<" EVeto "<<nEVetoHits
	       <<" HEPVeto "<<nHEPVetoHits
	       <<" ETag "<<nETagHits
	       <<" LeadGlass "<<nLeadGlassHits
	       <<" SAC "<<nSACHits<<std::endl;
      if(fTargetRecoBeam)      std::cout<<"     TargetBeam X and Y  "<<fTargetRecoBeam->getX()<<" "<<fTargetRecoBeam->getY()<<std::endl;

      // Show MCTruth information (example)
      if (fMCTruthEvent) {
	//	printf("     MCTruthEvent - Run %d Event %d Weight %5.3f Vertices %d\n",fMCTruthEvent->GetRunNumber(),fMCTruthEvent->GetEventNumber(),fMCTruthEvent->GetEventWeight(),fMCTruthEvent->GetNVertices());
	for(Int_t ii=0;ii<fMCTruthEvent->GetNVertices();ii++) {
	  TMCVertex* vtx = fMCTruthEvent->Vertex(ii);
	  //printf("       Vertex %d Type %s Time %8.3f ns Position (%8.3f,%8.3f,%8.3f) mm Particles in %d out %d\n",ii,vtx->GetProcess().Data(),vtx->GetTime(),vtx->GetPosition().X(),vtx->GetPosition().Y(),vtx->GetPosition().Z(),vtx->GetNParticleIn(),vtx->GetNParticleOut());
	  for(Int_t j=0;j<vtx->GetNParticleIn();j++) {
	    TMCParticle* p = vtx->ParticleIn(j);
	    //printf("         Particle In  %2d PDGCode %4d Energy %8.3f MeV Momentum (%8.3f,%8.3f,%8.3f) MeV\n",j,p->GetPDGCode(),p->GetEnergy(),p->GetMomentum().X(),p->GetMomentum().Y(),p->GetMomentum().Z());
	  }
	  for(Int_t j=0;j<vtx->GetNParticleOut();j++) {
	    TMCParticle* p = vtx->ParticleOut(j);
	    //printf("         Particle Out %2d PDGCode %4d Energy %8.3f MeV Momentum (%8.3f,%8.3f,%8.3f) MeV\n",j,p->GetPDGCode(),p->GetEnergy(),p->GetMomentum().X(),p->GetMomentum().Y(),p->GetMomentum().Z());
	  }
	}
      }


    }
    if (fVerbose>3) {
      if (fTargetRecoEvent)  fTargetRecoEvent->Print();
      if (fECalRecoEvent)    fECalRecoEvent->Print();
      if (fPVetoRecoEvent)   fPVetoRecoEvent->Print();
      if (fEVetoRecoEvent)   fEVetoRecoEvent->Print();
      if (fETagRecoEvent)    fETagRecoEvent->Print(); //MR 06/2022
      if (fHEPVetoRecoEvent) fHEPVetoRecoEvent->Print();
      if (fSACRecoEvent)     fSACRecoEvent->Print();
      if (fLeadGlassRecoEvent) fLeadGlassRecoEvent->Print();
    }
    
    if (doNtuple) stdNtuple->Fill(event);
    UserAn->Process();    
  }
  
  if (fVerbose) printf("---> Finalizing user analysis\n");
  UserAn->Finalize();
  if (fVerbose) printf("---> Finalizing histogram service\n");
  hSvc->Finalize();
  
  delete UserAn;
  delete hSvc;
  
  return 0;   
}
