#include "PadmeReconstruction.hh"
#include "GlobalRecoConfigOptions.hh"

#include "TPadmeRun.hh"
#include "TRawEvent.hh"
#include "TMCEvent.hh"
#include "TRecoEvent.hh"

#include "TMCTruthEvent.hh"
#include "TMCVertex.hh"
#include "TMCParticle.hh"

#include "TTargetMCEvent.hh"
#include "TEVetoMCEvent.hh"
#include "TPVetoMCEvent.hh"
#include "THEPVetoMCEvent.hh"
#include "TECalMCEvent.hh"
#include "TSACMCEvent.hh"
#include "TETagMCEvent.hh"
#include "TTPixMCEvent.hh"

#include "TTargetRecoEvent.hh"
#include "TEVetoRecoEvent.hh"
#include "TPVetoRecoEvent.hh"
#include "THEPVetoRecoEvent.hh"
#include "TECalRecoEvent.hh"
#include "TSACRecoEvent.hh"
#include "TETagRecoEvent.hh"
//#include "TTPixRecoEvent.hh"
#include "TLeadGlassRecoEvent.hh"

#include "TargetReconstruction.hh"
#include "EVetoReconstruction.hh"
#include "PVetoReconstruction.hh"
#include "HEPVetoReconstruction.hh"
#include "ECalReconstruction.hh"
#include "SACReconstruction.hh"
#include "ETagReconstruction.hh"
#include "TPixReconstruction.hh"
#include "LeadGlassReconstruction.hh"

#include "ECalParameters.hh"

#include "RecoRootIOManager.hh"

#include "HistoSvc.hh"
#include "TrigTimeSvc.hh"
#include "RunConditionSvc.hh"
#include "RunConfigurationSvc.hh"

#include <TObjString.h>

PadmeReconstruction::PadmeReconstruction(TObjArray* InputFileNameList, TString ConfFileName, TFile* OutputFile, Int_t NEvt, UInt_t Seed) :
  PadmeVReconstruction(OutputFile,"Padme",ConfFileName),fInputFileNameList(InputFileNameList), fHistoFile(OutputFile)
{

  // Input event structures will be allocated if corresponding branch exists
  fMCEvent        = 0;
  fMCTruthEvent   = 0;
  fTargetMCEvent  = 0;
  fEVetoMCEvent   = 0;
  fPVetoMCEvent   = 0;
  fHEPVetoMCEvent = 0;
  fECalMCEvent    = 0;
  fSACMCEvent     = 0;
  fETagMCEvent    = 0;
  fTPixMCEvent    = 0;

  fRawEvent       = 0;
  
  fRecoEvent        = 0;
  fTargetRecoEvent  = 0;
  fEVetoRecoEvent   = 0;
  fPVetoRecoEvent   = 0;
  fHEPVetoRecoEvent = 0;
  fECalRecoEvent    = 0;
  fSACRecoEvent     = 0;
  fETagRecoEvent    = 0;
  fTPixRecoEvent    = 0;
  fLeadGlassRecoEvent = 0;

  fETagReconstruction = 0;

  fGlobalRecoConfigOptions=NULL;

  //fConfigParser = new utl::ConfigParser("config/PadmeReconstruction.cfg");
  fConfigParser = new utl::ConfigParser((const std::string)ConfFileName);
  fConfig = new PadmeVRecoConfig(fConfigParser,"PadmeReconstructionConfiguration");

  // Initialize Run Configuration Service
  Int_t cfgVerbose = fConfig->GetParOrDefault("RECORunConfiguration", "Verbose", 0);
  fRunConfigurationSvc = RunConfigurationSvc::GetInstance();
  fRunConfigurationSvc->SetVerbose(cfgVerbose);
  fRunConfigurationSvc->Initialize();

  // Get general running mode information
  InitRunningModeFlags();

  // Initialize Histogramming Service
  TString hsName = fConfig->GetParOrDefault("RECOHisto", "HistoFile", "HistoFile.root");
  Int_t hsVerbose = fConfig->GetParOrDefault("RECOHisto", "Verbose", 0);
  fHistoSvc = HistoSvc::GetInstance();
  fHistoSvc->SetVerbose(hsVerbose);
  fHistoSvc->Initialize(hsName);

  // Initialize Trigger Timing Service
  Int_t ttsVerbose = fConfig->GetParOrDefault("RECOTrigTime", "Verbose", 0);
  fTrigTimeSvc = TrigTimeSvc::GetInstance();
  fTrigTimeSvc->SetVerbose(ttsVerbose);
  fTrigTimeSvc->Initialize();

  // Initialize Run Condition Service
  Int_t rcVerbose = fConfig->GetParOrDefault("RECORunCondition", "Verbose", 0);
  fRunConditionSvc = RunConditionSvc::GetInstance();
  fRunConditionSvc->SetVerbose(rcVerbose);
  fRunConditionSvc->Initialize();

  InitLibraries();
  Init(NEvt,Seed);

}
void PadmeReconstruction::InitRunningModeFlags()
{

  Int_t flagR = fConfig->GetParOrDefault("RUNNINGMODE", "Reconstruct"   ,1);
  Int_t flagC = fConfig->GetParOrDefault("RUNNINGMODE", "Cosmics"       ,0);
  Int_t flagP = fConfig->GetParOrDefault("RUNNINGMODE", "Pedestals"     ,0);
  Int_t flagM = fConfig->GetParOrDefault("RUNNINGMODE", "Monitor"       ,0);
  Int_t flagD = fConfig->GetParOrDefault("RUNNINGMODE", "Debug"         ,1);

  fGlobalRecoConfigOptions = new GlobalRecoConfigOptions();
  fGlobalRecoConfigOptions->SetRunningMode(flagR, flagP, flagC, flagM, flagD);
  std::cout<<"Global debug set to            = "<<fGlobalRecoConfigOptions->GetGlobalDebugMode()<<std::endl;
  std::cout<<"Global running mode RECO       = "<<fGlobalRecoConfigOptions->IsRecoMode()<<std::endl;
  std::cout<<"Global running mode COSMICS    = "<<fGlobalRecoConfigOptions->IsCosmicsMode()<<std::endl;
  std::cout<<"Global running mode PEDESTALS  = "<<fGlobalRecoConfigOptions->IsPedestalMode()<<std::endl;
  std::cout<<"Global running mode MONITOR    = "<<fGlobalRecoConfigOptions->IsMonitorMode()<<std::endl;

  fRunConfigurationSvc->SetRunningMode(flagR, flagP, flagC, flagM);
  fRunConfigurationSvc->SetDebugMode(flagD);
  fRunConfigurationSvc->SetRecoInput(fConfig->GetParOrDefault("RUNNINGMODE", "RecoInput", "RawData"));
  std::cout<<"Reco Input mode         = "<<fRunConfigurationSvc->GetRecoInput()<<std::endl;
  std::cout<<"Debug mode              = "<<fRunConfigurationSvc->GetDebugMode()<<std::endl;
  std::cout<<"Running mode RECO       = "<<fRunConfigurationSvc->IsRecoMode()<<std::endl;
  std::cout<<"Running mode COSMICS    = "<<fRunConfigurationSvc->IsCosmicsMode()<<std::endl;
  std::cout<<"Running mode PEDESTALS  = "<<fRunConfigurationSvc->IsPedestalMode()<<std::endl;
  std::cout<<"Running mode MONITOR    = "<<fRunConfigurationSvc->IsMonitorMode()<<std::endl;

}

PadmeReconstruction::~PadmeReconstruction()
{
  for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
    delete fRecoLibrary[iLib];
  }
  if(fGlobalRecoConfigOptions) delete fGlobalRecoConfigOptions;
}

void PadmeReconstruction::InitLibraries()
{

  // Declare detectors with required configuration file
  if (fConfig->GetParOrDefault("RECOALGORITHMS","EVeto",0)) {
    TString configEVeto = fConfig->GetParOrDefault("RECOCONFIG","EVeto","config/EVeto.cfg");
    std::cout<<"=== Enabling EVeto with configuration file "<<configEVeto<<std::endl;
    fRecoLibrary.push_back(new EVetoReconstruction(fHistoFile,configEVeto));
  }
  if (fConfig->GetParOrDefault("RECOALGORITHMS","PVeto",0)) {
    TString configPVeto = fConfig->GetParOrDefault("RECOCONFIG","PVeto","config/PVeto.cfg");
    std::cout<<"=== Enabling PVeto with configuration file "<<configPVeto<<std::endl;
    fRecoLibrary.push_back(new PVetoReconstruction(fHistoFile,configPVeto));
  }
  if (fConfig->GetParOrDefault("RECOALGORITHMS","ECal",1)) {
    TString configECal = fConfig->GetParOrDefault("RECOCONFIG","ECal","config/ECal.cfg");
    std::cout<<"=== Enabling ECal with configuration file "<<configECal<<std::endl;
    fRecoLibrary.push_back(new ECalReconstruction(fHistoFile,configECal));
  }
  if (fConfig->GetParOrDefault("RECOALGORITHMS","SAC",0)) {
    TString configSAC = fConfig->GetParOrDefault("RECOCONFIG","SAC","config/SAC.cfg");
    std::cout<<"=== Enabling SAC with configuration file "<<configSAC<<std::endl;
    fRecoLibrary.push_back(new SACReconstruction(fHistoFile,configSAC));
  }
  if (fConfig->GetParOrDefault("RECOALGORITHMS","TPix",0)) {
    TString configTPix = fConfig->GetParOrDefault("RECOCONFIG","TPix","config/TPix.cfg");
    std::cout<<"=== Enabling TPix with configuration file "<<configTPix<<std::endl;
    fRecoLibrary.push_back(new TPixReconstruction(fHistoFile,configTPix));
  }
  if (fConfig->GetParOrDefault("RECOALGORITHMS","Target",1)) {
    TString configTarget = fConfig->GetParOrDefault("RECOCONFIG","Target","config/Target.cfg");
    std::cout<<"=== Enabling Target with configuration file "<<configTarget<<std::endl;
    fRecoLibrary.push_back(new TargetReconstruction(fHistoFile,configTarget));
  }
  if (fConfig->GetParOrDefault("RECOALGORITHMS","HEPVeto",0)) {
    TString configHEPVeto = fConfig->GetParOrDefault("RECOCONFIG","HEPVeto","config/HEPVeto.cfg");
    std::cout<<"=== Enabling HEPVeto with configuration file "<<configHEPVeto<<std::endl;
    fRecoLibrary.push_back(new HEPVetoReconstruction(fHistoFile,configHEPVeto));
  }
  if (fConfig->GetParOrDefault("RECOALGORITHMS","ETag",1)) {
    TString configETag = fConfig->GetParOrDefault("RECOCONFIG","ETag","config/ETag.cfg");
    std::cout << "=== Enabling ETag with configuration file " << configETag <<std::endl;
    //fRecoLibrary.push_back(new ETagReconstruction(fHistoFile,configETag));
    fETagReconstruction = new ETagReconstruction(configETag);
  }
  if (fConfig->GetParOrDefault("RECOALGORITHMS","LeadGlass",1)) {
    TString configLeadGlass = fConfig->GetParOrDefault("RECOCONFIG","LeadGlass","config/LeadGlass.cfg");
    std::cout<<"=== Enabling LeadGlass with configuration file "<<configLeadGlass<<std::endl;
    fRecoLibrary.push_back(new LeadGlassReconstruction(fHistoFile,configLeadGlass));
  }
  std::cout<<"************************** "<<fRecoLibrary.size()<<" Reco Algorithms built"<<std::endl;
  for (unsigned int j=0; j<fRecoLibrary.size(); ++j)
    std::cout << " **** <" << fRecoLibrary[j]->GetName() << "> is in library at location " << j <<std::endl;
  std::cout<<"************************** "<<std::endl;

}

void PadmeReconstruction::InitDetectorsInfo()
{

  std::cout << "PadmeReconstruction: Initializing" << std::endl;

  fMainReco = this; //init PadmeReconstruction main reco as itself
  if (FindReco("Target"))  ((TargetReconstruction*)  FindReco("Target")) ->Init(this);
  if (FindReco("EVeto"))   ((EVetoReconstruction*)   FindReco("EVeto"))  ->Init(this);
  if (FindReco("PVeto"))   ((PVetoReconstruction*)   FindReco("PVeto"))  ->Init(this);
  if (FindReco("HEPVeto")) ((HEPVetoReconstruction*) FindReco("HEPVeto"))->Init(this);
  if (FindReco("ECal"))    ((ECalReconstruction*)    FindReco("ECal"))   ->Init(this);
  if (FindReco("SAC"))     ((SACReconstruction*)     FindReco("SAC"))    ->Init(this);
  //if (FindReco("ETag"))    ((ETagReconstruction*)    FindReco("ETag"))   ->Init(this);
  if (FindReco("TPix"))    ((TPixReconstruction*)    FindReco("TPix"))   ->Init(this);
  if (FindReco("LeadGlass")) ((LeadGlassReconstruction*) FindReco("LeadGlass"))->Init(this);
  if (fETagReconstruction) fETagReconstruction->Init();
}

void PadmeReconstruction::HistoInit()
{
  fHistoSvc->CreateDir("PadmeReco");
  fHistoSvc->BookHisto("PadmeReco","EventTrigger","Event Trigger",8,-0.5,7.5);
  fHistoSvc->BookHisto("PadmeReco","EventTriggerWord","Event Trigger word",256,-0.5,255.5);
}

void PadmeReconstruction::Init(Int_t NEvt, UInt_t Seed)
{

  //gRandom->SetSeed(Seed);
  fNEvt = NEvt;

  TTree::SetMaxTreeSize(190000000000);

  TString recoInput = fRunConfigurationSvc->GetRecoInput();

  std::cout << "PadmeReconstruction::Init - Input data format set to " << recoInput << std::endl;
  
  Int_t nEntries = 0;

  // Process MC events
  if (recoInput == "MonteCarlo") {

    // Show run information
    TString runTree = "Runs";
    TChain* runChain = BuildChain(runTree);
    if (runChain) {
      Int_t runNEntries = runChain->GetEntries();
      TObjArray* runBranches = runChain->GetListOfBranches();
      std::cout << "Found Tree '" << runTree << "' with " << runBranches->GetEntries() << " branches and " << runNEntries << " entries" << std::endl;
      TPadmeRun* run = new TPadmeRun();
      //for(Int_t iBranch = 0; iBranch < runBranches->GetEntries(); iBranch++){
      int ib = runBranches->GetEntries();
      for(Int_t iBranch = 0; iBranch < ib; iBranch++){
	TString branchName = ((TBranch*)(*runBranches)[iBranch])->GetName();
	TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*runBranches)[iBranch])->GetClassName());
	std::cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << std::endl;
	if(branchName=="Run") {
	  runChain->SetBranchAddress(branchName.Data(),&run);
	  runChain->GetEntry(0); // Currently only one run per file
	  std::cout << "=== MC Run information - Start ===" << std::endl;
	  std::cout << "Run number/type " << run->GetRunNumber() << " " << run->GetRunType() << std::endl;
	  std::cout << "Run start/stop time " << run->GetTimeStart() << " " << run->GetTimeStop() << std::endl;
	  std::cout << "Run number of events " << run->GetNEvents() << std::endl;
	  TDetectorInfo* detInfo = run->GetDetectorInfo();
	  ShowSubDetectorInfo(detInfo,"Target");
	  ShowSubDetectorInfo(detInfo,"EVeto");
	  ShowSubDetectorInfo(detInfo,"PVeto");
	  ShowSubDetectorInfo(detInfo,"HEPVeto");
	  ShowSubDetectorInfo(detInfo,"ECal");
	  ShowSubDetectorInfo(detInfo,"SAC");
	  ShowSubDetectorInfo(detInfo,"ETag");
	  ShowSubDetectorInfo(detInfo,"TPix");
	  ShowSubDetectorInfo(detInfo,"LeadGlass");
	  std::cout << "=== MC Run information - End ===" << std::endl << std::endl;
	  
	  // Pass detector info to corresponding Parameters class for decoding
	  TSubDetectorInfo* subDetInfo = detInfo->FindSubDetectorInfo("ECal");
	  if (subDetInfo) ECalParameters::GetInstance()->SetMCDetInfo(subDetInfo);
	  
	}
      }
    }
    else std::cout << " Tree " << runTree << " not found "<<std::endl;

    TString mcTreeName = "MC";
    fMCChain = NULL;
    std::cout << " Looking for tree named " << mcTreeName << std::endl;
    fMCChain = BuildChain(mcTreeName);
    if(fMCChain) {
      
      nEntries = fMCChain->GetEntries();
      std::cout<<" Tree named "<<mcTreeName<<" found with "<<nEntries<<" events"<<std::endl;
      TObjArray* branches = fMCChain->GetListOfBranches();
      for(Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++){
	
	TString branchName = ((TBranch*)(*branches)[iBranch])->GetName();
	TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*branches)[iBranch])->GetClassName());
	std::cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << std::endl;
	
	if (branchName=="Event") {
	  fMCEvent = new TMCEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fMCEvent);
	} else if (branchName=="Target") {
	  fTargetMCEvent = new TTargetMCEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fTargetMCEvent);
	} else if (branchName=="EVeto") {
	  fEVetoMCEvent = new TEVetoMCEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fEVetoMCEvent);
	} else if (branchName=="PVeto") {
	  fPVetoMCEvent = new TPVetoMCEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fPVetoMCEvent);
	} else if (branchName=="HEPVeto") {
	  fHEPVetoMCEvent = new THEPVetoMCEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fHEPVetoMCEvent);
	} else if (branchName=="ECal") {
	  fECalMCEvent = new TECalMCEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fECalMCEvent);
	} else if (branchName=="SAC") {
	  fSACMCEvent = new TSACMCEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fSACMCEvent);
	} else if (branchName=="ETag") {
	  fETagMCEvent = new TETagMCEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fETagMCEvent);
	//else if (branchName=="TPix") {
	//  fTPixMCEvent = new TTPixMCEvent();
	//  fMCChain->SetBranchAddress(branchName.Data(),&fTPixMCEvent);
	//} else if (branchName=="LeadGlass") {
	//  fLeadGlassMCEvent = new TLeadGlassMCEvent();
	//  fMCChain->SetBranchAddress(branchName.Data(),&fLeadGlassMCEvent);
	} else if (branchName=="MCTruth") {
	  printf("PadmeReconstruction - Found MCTruth branch\n");
	  fMCTruthEvent = new TMCTruthEvent();
	  fMCChain->SetBranchAddress(branchName.Data(),&fMCTruthEvent);
	}
	
      }

    }
    else std::cout << " Tree " << mcTreeName << " not found "<<std::endl;

  }        

  // Reprocess RecoHits events
  if (recoInput == "Reprocessing") {

    TString recoTreeName = "Events";
    std::cout<<" Looking for tree named "<<recoTreeName<<std::endl;
    fRecoChain = NULL;
    fRecoChain = BuildChain(recoTreeName);
    if(fRecoChain) {

      nEntries = fRecoChain->GetEntries();
      std::cout<<" Tree named "<<recoTreeName<<" found with "<<nEntries<<" events"<<std::endl;
      TObjArray* branches = fRecoChain->GetListOfBranches();
      std::cout << "Found Tree '" << recoTreeName << "' with " << branches->GetEntries() << " branches and " << nEntries << " entries" << std::endl;

      for(Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++){

	TString branchName = ((TBranch*)(*branches)[iBranch])->GetName();
	TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*branches)[iBranch])->GetClassName());
	std::cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << std::endl;

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
	} else if (branchName=="ETag_Hits") {
	  fETagRecoEvent = new TETagRecoEvent();
	  fRecoChain->SetBranchAddress(branchName.Data(),&fETagRecoEvent);
	//} else if (branchName=="LeadGlass_Hits") {
	//  fLeadGlassRecoEvent = new TLeadGlassRecoEvent();
	//  fRecoChain->SetBranchAddress(branchName.Data(),&fLeadGlassRecoEvent);
	//} else if (branchName=="TPix") {
	//  fTPixRecoEvent = new TTPixRecoEvent();
	//  fRecoChain->SetBranchAddress(branchName.Data(),&fTPixRecoEvent);
	} else if (branchName=="MCTruth") {
	  printf("PadmeReconstruction - Found MCTruth branch\n");
	  fMCTruthEvent = new TMCTruthEvent();
	  fRecoChain->SetBranchAddress(branchName.Data(),&fMCTruthEvent);
	}

      }

    }
    else std::cout << " Tree " << recoTreeName << " not found "<<std::endl;

  }

  // Process rawdata events
  if (recoInput == "RawData") {

    TString rawTreeName = "RawEvents";
    std::cout << " Looking for tree named " << rawTreeName << std::endl;
    fRawChain = BuildChain(rawTreeName);
    if (! fRawChain) {
      std::cout << " Tree " << rawTreeName << " not found "<<std::endl;
      exit(EXIT_FAILURE);
    }
    fRawEvent = new TRawEvent();
    nEntries = fRawChain->GetEntries();
    std::cout<<" Tree named "<<rawTreeName<<" found with "<<nEntries<<" events"<<std::endl;
    TObjArray* branches = fRawChain->GetListOfBranches();
    if (branches->GetEntries() != 1) {
      std::cout << "PadmeReconstruction::Init - Input Tree has %d branches while 1 was expected (wrong data format?). Aborting" << std::endl;
      if (branches->GetEntries() > 1) {
	std::cout << "Branches names:";
	for(Int_t b = 0; b < branches->GetEntries(); b++) std::cout << " " << ((TBranch*)(*branches)[b])->GetName();
	std::cout << std::endl;
      }
      exit(EXIT_FAILURE);
    }
    TString branchName = ((TBranch*)(*branches)[0])->GetName();
    TString branchClassName = ((TBranch*)(*branches)[0])->GetClassName();
    std::cout << "Found Tree '" << rawTreeName << "' with branch '" << branchName << "' of class '" << branchClassName << "' and " << nEntries << " entries" << std::endl;
    fRawChain->SetBranchAddress("RawEvent",&fRawEvent);

  }

  if (nEntries == 0) {
    std::cout << "PadmeReconstruction::Init - No entries found in input chain (wrong data format?). Aborting" << std::endl;
    exit(EXIT_FAILURE);
  }

  fNProcessedEventsInTotal = 0;
    
  // Initialize reconstruction for each subdetector
  InitDetectorsInfo();

  // Initialize global histograms
  HistoInit();

}

Bool_t PadmeReconstruction::NextEvent()
{

  // Check if there is a new MC event to process
  if ( fMCChain && fMCChain->GetEntry(fNProcessedEventsInTotal) && (fNEvt == 0 || fNProcessedEventsInTotal < fNEvt) ) {

    if (fNProcessedEventsInTotal%100==0) {
      std::cout << "=== Read event in position " << fNProcessedEventsInTotal << " ===" << std::endl;
      std::cout << "--- PadmeReconstruction --- run/event/time " << fMCEvent->GetRunNumber()
		<< " " << fMCEvent->GetEventNumber() << " " << fMCEvent->GetTime() << std::endl;
    }

    // Reconstruct individual detectors (but check if they exist, first!)
    for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
      if (fRecoLibrary[iLib]->GetName() == "Target" && fTargetMCEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fTargetMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "EVeto" && fEVetoMCEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fEVetoMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "PVeto" && fPVetoMCEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fPVetoMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "HEPVeto" && fHEPVetoMCEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fHEPVetoMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "ECal" && fECalMCEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fECalMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "SAC" && fSACMCEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fSACMCEvent,fMCEvent);
      //} else if (fRecoLibrary[iLib]->GetName() == "ETag" && fETagMCEvent) {
      //  fRecoLibrary[iLib]->ProcessEvent(fETagMCEvent,fMCEvent);
      //} else if (fRecoLibrary[iLib]->GetName() == "LeadGlass" && fLeadGlassMCEvent) {
      //  fRecoLibrary[iLib]->ProcessEvent(fLeadGlassMCEvent,fMCEvent);
      //} else if (fRecoLibrary[iLib]->GetName() == "TPix" && fTPixMCEvent) {
      //  fRecoLibrary[iLib]->ProcessEvent(fTPixMCEvent,fMCEvent);
      }
    }
    if (fETagReconstruction) fETagReconstruction->ProcessEvent(fETagMCEvent,fMCEvent);

    fNProcessedEventsInTotal++;
    return true;

  }

  // Check if there is a new reconstructed event to reprocess
  if ( fRecoChain && fRecoChain->GetEntry(fNProcessedEventsInTotal) &&  (fNEvt == 0 || fNProcessedEventsInTotal < fNEvt) ) {

    if (fNProcessedEventsInTotal%100==0) {
    std::cout << "=== Read (from Hits) event in position " << fNProcessedEventsInTotal << " ===" << std::endl;
    std::cout << "--- PadmeReconstruction --- run/event/time " << fRecoEvent->GetRunNumber()
	 << " " << fRecoEvent->GetEventNumber() << " " << fRecoEvent->GetTime() << std::endl;
    }
    // Reconstruct individual detectors (but check if they exist, first!)
    for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
      if (fRecoLibrary[iLib]->GetName() == "Target" && fTargetRecoEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fTargetRecoEvent,fRecoEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "EVeto" && fEVetoRecoEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fEVetoRecoEvent,fRecoEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "PVeto" && fPVetoRecoEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fPVetoRecoEvent,fRecoEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "HEPVeto" && fHEPVetoRecoEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fHEPVetoRecoEvent,fRecoEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "ECal" && fECalRecoEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fECalRecoEvent,fRecoEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "SAC" && fSACRecoEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fSACRecoEvent,fRecoEvent);
      //} else if (fRecoLibrary[iLib]->GetName() == "ETag" && fETagRecoEvent) {
      //  fRecoLibrary[iLib]->ProcessEvent(fETagRecoEvent,fRecoEvent);
      //} else if (fRecoLibrary[iLib]->GetName() == "LeadGlass" && fLeadGlassRecoEvent) {
      //  fRecoLibrary[iLib]->ProcessEvent(fLeadGlassRecoEvent,fRecoEvent);
      //} else if (fRecoLibrary[iLib]->GetName() == "TPix" && fTPixRecoEvent) {
      //  fRecoLibrary[iLib]->ProcessEvent(fTPixRecoEvent,fRecoEvent);
      }
    }
    if (fETagReconstruction) fETagReconstruction->ProcessEvent(fETagRecoEvent,fRecoEvent);

    ++fNProcessedEventsInTotal;
    return true;
    
  }
  
  if ( fRawChain && fRawChain->GetEntry(fNProcessedEventsInTotal) && (fNEvt == 0 || fNProcessedEventsInTotal < fNEvt) ) {

    if (fNProcessedEventsInTotal%100==0) {
      std::cout << "=== Read raw event in position " << fNProcessedEventsInTotal << " ===" << std::endl;
      std::cout << "--- PadmeReconstruction --- run/event/time " << fRawEvent->GetRunNumber()
		<< " " << fRawEvent->GetEventNumber() << " " << fRawEvent->GetEventAbsTime() << std::endl;
    }

    // Process event to extract global information
    ProcessEvent(fRawEvent);

    // Process Trigger Time information
    fTrigTimeSvc->ProcessEvent(fRawEvent);

    // Some debug tests
    //printf("Board 28 group 2 Reference %f\n",fTrigTimeSvc->GetReferencePoint(28,2));
    //printf("Board 27 group 2 Reference %f\n",fTrigTimeSvc->GetReferencePoint(27,2));
    //printf("Board 28 group 2 Frequency %d\n",fTrigTimeSvc->GetGroupFrequency(28,2));
    //printf("Board 28 group 2 point 456. time diff %f\n",fTrigTimeSvc->GetTimeDifference(28,2,456.));

    // In monitor mode just process Physics trigger
    if ( (fRunConfigurationSvc->IsMonitorMode() == 0)  || (fRawEvent->GetEventTrigMask() & 0x01) ) {
      // Reconstruct individual detectors
      for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
	fRecoLibrary[iLib]->ProcessEvent(fRawEvent);
      }
      if (fETagReconstruction) fETagReconstruction->ProcessEvent(fRawEvent);
    }

    fNProcessedEventsInTotal++;
    return true;

  }

  return false;

}

void PadmeReconstruction::ProcessEvent(TRawEvent* rawEv)
{
  UInt_t trigMask = rawEv->GetEventTrigMask();
  fHistoSvc->FillHisto("PadmeReco","EventTriggerWord",trigMask);
  for(UInt_t i=0; i<8; i++){
    if (trigMask & (1 << i)) fHistoSvc->FillHisto("PadmeReco","EventTrigger",i);
  }
}

void PadmeReconstruction::EndProcessing(){

  // Finalize individual detectors
  for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
    fHistoFile->cd("/");
    fRecoLibrary[iLib]->EndProcessing();
  }
  if (fETagReconstruction) fETagReconstruction->EndProcessing();
  fHistoFile->cd("/");
  HistoExit();

  // Finalize histogramming service
  fHistoSvc->Finalize();

  // Finalize Trigger Time Service
  fTrigTimeSvc->Finalize();

  // Finalize Run Condition Service
  fRunConditionSvc->Finalize();

  //for(Int_t iReco = 0; iReco < fNReconstructions; iReco++){
  //  fReconstructions[iReco]->EvaluateROSettings();
  //}
  //for(UInt_t iTree=0;iTree<fOutputTrees.size();iTree++){
  //  fOutputTrees[iTree]->Write();
  //}
  //if(fMCOutputTree) fMCOutputTree->Write();

  //This goes in the PadmeReco core
  //RecoRootIOManager::GetInstance()->Close();

  //fHistoFile->Purge();
  //fHistoFile->Close();
  //PrintRecoSummary(std::cout);

}

TChain* PadmeReconstruction::BuildChain(TString treeName){
  TChain* chain = new TChain(treeName.Data());
  for(Int_t iFile = 0; iFile < fInputFileNameList->GetEntries(); iFile++)
    chain->AddFile(CheckProtocols(((TObjString*)fInputFileNameList->At(iFile))->GetString()));
  if(chain->GetEntries() == 0){
    delete chain;
    chain = 0;
  }
  return chain;
}

// Verify URL for each file
TString PadmeReconstruction::CheckProtocols(TString OldStr){
  TString NewStr = OldStr;
  if(NewStr.EndsWith("\r")) NewStr.Remove(NewStr.Last('\r'));
  return NewStr;
}

PadmeVReconstruction * PadmeReconstruction::FindReco(TString Name){
  UInt_t iLib = 0;
  for(; iLib < fRecoLibrary.size(); iLib++) {
    if(fRecoLibrary[iLib]->GetName().CompareTo(Name) == 0) return fRecoLibrary[iLib];
  }
  return 0;
}

void PadmeReconstruction::ShowSubDetectorInfo(TDetectorInfo* detInfo, TString name)
{
  TSubDetectorInfo* subDetInfo = detInfo->FindSubDetectorInfo(name);
  if (subDetInfo) {
    std::cout << "--- " << name.Data() << " geometry parameters ---" << std::endl;
    std::vector<TString> subPar = subDetInfo->GetGeometryParameters();
    for(UInt_t iPar = 0; iPar < subPar.size(); iPar++) {
      std::cout << subPar[iPar].Data() << std::endl;
    }
  }
}

// Methods to return general information about current event

Int_t PadmeReconstruction::GetRunNumber()
{
  // Check if we are dealing with MC/Raw/Reco event
  if (fMCChain) {
    return fMCEvent->GetRunNumber();
  } else if (fRawChain) {
    return fRawEvent->GetRunNumber();
  } else if (fRecoChain) {
    return fRecoEvent->GetRunNumber();
  }
  printf("PadmeReconstruction::GetRunNumber() - Unknown input chain");
  return 0;
}

Int_t PadmeReconstruction::GetEventNumber()
{
  // Check if we are dealing with MC/Raw/Reco event
  if (fMCChain) {
    return fMCEvent->GetEventNumber();
  } else if (fRawChain) {
    return fRawEvent->GetEventNumber();
  } else if (fRecoChain) {
    return fRecoEvent->GetEventNumber();
  }
  printf("PadmeReconstruction::GetEventNumber() - Unknown input chain");
  return 0;
}

TTimeStamp PadmeReconstruction::GetEventTime()
{
  // Check if we are dealing with MC/Raw/Reco event
  if (fMCChain) {
    // Not defined for MC events yet
    return TTimeStamp(0,0);
  } else if (fRawChain) {
    return fRawEvent->GetEventAbsTime();
  } else if (fRecoChain) {
    return fRecoEvent->GetEventTime();
  }
  printf("PadmeReconstruction::GetEventTime() - Unknown input chain");
  return TTimeStamp(0,0);
}

ULong64_t PadmeReconstruction::GetRunClock()
{
  // Check if we are dealing with MC/Raw/Reco event
  if (fMCChain) {
    // Not defined for MC events
    return 0;
  } else if (fRawChain) {
    return fRawEvent->GetEventRunTime();
  } else if (fRecoChain) {
    return fRecoEvent->GetRunClock();
  }
  printf("PadmeReconstruction::GetRunClock() - Unknown input chain");
  return 0;
}

UInt_t PadmeReconstruction::GetEventStatus()
{
  // Check if we are dealing with MC/Raw/Reco event
  if (fMCChain) {

    // Start with a clean event status mask
    UInt_t eventstatus = 0;

    // This is a MC event: set Simulated bit
    eventstatus |= (1U << TRECOEVENT_STATUSBIT_SIMULATED);

    return eventstatus;

  } else if (fRawChain) {

    // Start with a clean event status mask
    UInt_t eventstatus = 0;

    // Get Complete bit from rawevent
    if ( fRawEvent->EventStatusGetBit(TRAWEVENT_STATUSBIT_COMPLETE) )
      eventstatus |= (1U << TRECOEVENT_STATUSBIT_COMPLETE);

    // Get autopass bit from rawevent
    if ( fRawEvent->EventStatusGetBit(TRAWEVENT_STATUSBIT_AUTOPASS) )
      eventstatus |= (1U << TRECOEVENT_STATUSBIT_AUTOPASS);

    // Simulated bit is already 0: no need for this
    //eventstatus &= ~(1U << TRECOEVENT_STATUSBIT_SIMULATED);

    // BeamOn bit is not currently used as it needs info from target: will be added asap
    //eventstatus |= (1U << TRECOEVENT_STATUSBIT_BEAMON);

    return eventstatus;

  } else if (fRecoChain) {

    // Just copy the event status mask from input event
    return fRecoEvent->GetEventStatus();

  }
  printf("PadmeReconstruction::GetEventStatus() - Unknown input chain");
  return 0;
}

UInt_t PadmeReconstruction::GetTriggerMask()
{
  // Check if we are dealing with MC/Raw/Reco event
  if (fMCChain) {
    // Not defined for MC events
    return 0;
  } else if (fRawChain) {
    return fRawEvent->GetEventTrigMask();
  } else if (fRecoChain) {
    return fRecoEvent->GetTriggerMask();
  }
  printf("PadmeReconstruction::GetTriggerMask() - Unknown input chain");
  return 0;
}

Bool_t PadmeReconstruction::IsSimulated()
{
  Bool_t isMC = false;
  if (fMCChain || ( fRecoChain && fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED) ) ) isMC=true;
  return isMC;
}
