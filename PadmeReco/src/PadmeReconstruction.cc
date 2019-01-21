#include "PadmeReconstruction.hh"

#include "TPadmeRun.hh"
#include "TRawEvent.hh"
#include "TMCEvent.hh"

#include "TTargetMCEvent.hh"
#include "TEVetoMCEvent.hh"
#include "TPVetoMCEvent.hh"
#include "THEPVetoMCEvent.hh"
#include "TECalMCEvent.hh"
#include "TSACMCEvent.hh"
#include "TTPixMCEvent.hh"

#include "TargetReconstruction.hh"
#include "EVetoReconstruction.hh"
#include "PVetoReconstruction.hh"
#include "HEPVetoReconstruction.hh"
#include "ECalReconstruction.hh"
#include "SACReconstruction.hh"
#include "TPixReconstruction.hh"

#include "ECalParameters.hh"

#include "RecoRootIOManager.hh"
#include "ReadConf.hh"

PadmeReconstruction::PadmeReconstruction(TObjArray* InputFileNameList, TString ConfFileName, TFile* OutputFile, Int_t NEvt, UInt_t Seed) :
  PadmeVReconstruction(OutputFile,"Padme",ConfFileName),fInputFileNameList(InputFileNameList), fHistoFile(OutputFile)
{

  // Input event structures will be allocated if corresponding branch exists
  fMCEvent        = 0;
  fTargetMCEvent  = 0;
  fEVetoMCEvent   = 0;
  fPVetoMCEvent   = 0;
  fHEPVetoMCEvent = 0;
  fECalMCEvent    = 0;
  fSACMCEvent     = 0;
  fTPixMCEvent    = 0;
  fRawEvent       = 0;

  InitLibraries();
  Init(NEvt,Seed);
}

PadmeReconstruction::~PadmeReconstruction()
{
  for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
    delete fRecoLibrary[iLib];
  }
}

void PadmeReconstruction::InitLibraries()
{
// need to add the possibility of switching off different detectors reco M. Raggi 29/11/2018
  ReadConf *readconf = ReadConf::GetInstance();
  std::cout<<"Read conf"<<readconf<<std::endl;
  //Added flags to exclude different reco analysis M. Raggi
  if(readconf->IsTargetON())     fRecoLibrary.push_back(new TargetReconstruction (fHistoFile,"config/Target.cfg"));
  if(readconf->IsEVetoON())   fRecoLibrary.push_back(new EVetoReconstruction  (fHistoFile,"config/EVeto.cfg"));
  if(readconf->IsPVetoON())   fRecoLibrary.push_back(new PVetoReconstruction  (fHistoFile,"config/PVeto.cfg"));
  if(readconf->IsHEPVetoON()) fRecoLibrary.push_back(new HEPVetoReconstruction(fHistoFile,"config/HEPVeto.cfg"));
  if(readconf->IsECalON())    fRecoLibrary.push_back(new ECalReconstruction   (fHistoFile,"config/ECal.cfg"));
  if(readconf->IsSACON())     fRecoLibrary.push_back(new SACReconstruction    (fHistoFile,"config/SAC.cfg"));
  if(readconf->IsTPixON())    fRecoLibrary.push_back(new TPixReconstruction   (fHistoFile,"config/TPix.cfg"));
  		
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
  if (FindReco("TPix"))    ((TPixReconstruction*)    FindReco("TPix"))   ->Init(this);
}

void PadmeReconstruction::Init(Int_t NEvt, UInt_t Seed)
{

  //gRandom->SetSeed(Seed);
  fNEvt = NEvt;

  TTree::SetMaxTreeSize(190000000000);

  // Show run information
  TString runTree = "Runs";
  TChain* runChain = BuildChain(runTree);
  if (runChain) {
    Int_t runNEntries = runChain->GetEntries();
    TObjArray* runBranches = runChain->GetListOfBranches();
    std::cout << "Found Tree '" << runTree << "' with " << runBranches->GetEntries() << " branches and " << runNEntries << " entries" << std::endl;
    TPadmeRun* run = new TPadmeRun();
    for(Int_t iBranch = 0; iBranch < runBranches->GetEntries(); iBranch++){
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
	ShowSubDetectorInfo(detInfo,"TPix");
	std::cout << "=== MC Run information - End ===" << std::endl << std::endl;

	// Pass detector info to corresponding Parameters class for decoding
	TSubDetectorInfo* subDetInfo = detInfo->FindSubDetectorInfo("ECal");
	if (subDetInfo) ECalParameters::GetInstance()->SetMCDetInfo(subDetInfo);
    
      }
    }
  }

  Int_t nEntries;

  nEntries = 0;
  TString mcTreeName = "MC";
  fMCChain = BuildChain(mcTreeName);
  if(fMCChain) {

    nEntries = fMCChain->GetEntries();
    TObjArray* branches = fMCChain->GetListOfBranches();
    std::cout << "Found Tree '" << mcTreeName << "' with " << branches->GetEntries() << " branches and " << nEntries << " entries" << std::endl;

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
      } else if (branchName=="TPix") {
	fTPixMCEvent = new TTPixMCEvent();
	fMCChain->SetBranchAddress(branchName.Data(),&fTPixMCEvent);
      }

    }

  }

  nEntries = 0;
  TString rawTreeName = "RawEvents";
  fRawChain = BuildChain(rawTreeName);
  if(fRawChain) {
    fRawEvent = new TRawEvent();
    nEntries = fRawChain->GetEntries();
    TObjArray* branches = fRawChain->GetListOfBranches();
    std::cout << "Found Tree '" << rawTreeName << "' with " << branches->GetEntries() << " branches and " << nEntries << " entries" << std::endl;
    fRawChain->SetBranchAddress("RawEvent",&fRawEvent);
  }

  fNProcessedEventsInTotal = 0;

  // Initialize reconstruction for each subdetector
  InitDetectorsInfo();

}

Bool_t PadmeReconstruction::NextEvent()
{

  // Check if there is a new event to process
  if ( fMCChain && fMCChain->GetEntry(fNProcessedEventsInTotal) &&  (fNEvt == 0 || fNProcessedEventsInTotal < fNEvt) ) {

    std::cout << "=== Read event in position " << fNProcessedEventsInTotal << " ===" << std::endl;
    std::cout << "--- PadmeReconstruction --- run/event/time " << fMCEvent->GetRunNumber()
	 << " " << fMCEvent->GetEventNumber() << " " << fMCEvent->GetTime() << std::endl;

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
      } else if (fRecoLibrary[iLib]->GetName() == "TPix" && fTPixMCEvent) {
	fRecoLibrary[iLib]->ProcessEvent(fTPixMCEvent,fMCEvent);
      }
    }

    fNProcessedEventsInTotal++;
    return true;

  }

  if ( fRawChain && fRawChain->GetEntry(fNProcessedEventsInTotal) && (fNEvt == 0 || fNProcessedEventsInTotal < fNEvt) ) {

    // std::cout << "=== Read raw event in position " << fNProcessedEventsInTotal << " ===" << std::endl;
    // std::cout << "--- PadmeReconstruction --- run/event/time " << fRawEvent->GetRunNumber()
    // 	 << " " << fRawEvent->GetEventNumber() << " " << fRawEvent->GetEventAbsTime() << std::endl;

    // Reconstruct individual detectors (but check if they exist, first!)
    for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
      fRecoLibrary[iLib]->ProcessEvent(fRawEvent);
    }

    fNProcessedEventsInTotal++;
    return true;

  }

  return false;

}

void PadmeReconstruction::EndProcessing(){

  // Reconstruct individual detectors (but check if they exist, first!)
  for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
    fHistoFile->cd("/");
    fRecoLibrary[iLib]->EndProcessing();
  }
  fHistoFile->cd("/");


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
  TString NewStr=OldStr;
  if(NewStr.EndsWith("\r")) NewStr.Remove(NewStr.Last('\r'));
  //if(NewStr.Contains("/eos/") && !NewStr.Contains("root://eosna62")){
  //  NewStr = "root://eosna62.cern.ch/"+NewStr;
  //}
  //else if(NewStr.Contains("/castor/")) {
  //  if(!NewStr.Contains("root://")){
  //    NewStr = "root://castorpublic.cern.ch/"+NewStr;
  //  }
  //  if(!NewStr.Contains("svcClass")){
  //    NewStr = NewStr+"?svcClass="+fSvcClass;
  //  }
  //}
  return NewStr;
}

PadmeVReconstruction * PadmeReconstruction::FindReco(TString Name){
  for(UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++)
    if(fRecoLibrary[iLib]->GetName().CompareTo(Name) == 0)
      return fRecoLibrary[iLib];
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


//void PadmeReconstruction::ReadConfig(){
////  TString ConfFileName = "config/PadmeReconstruction.conf";
////  
////  std::ifstream confFile(ConfFileName.Data());
////  if (!confFile.is_open()) {
////    perror(ConfFileName);
////    std::cout<<"cannot open config file : "<<ConfFileName.Data()<<std::endl;
////    exit(1);
////  }
////  //  
////  TString Line;
////  while (Line.ReadLine(confFile)) {
////    if(Line.BeginsWith("#")) continue;
////    if(Line.Contains("ECALReco")    && Line.Contains("true"))    fECalisON=true;
////    if(Line.Contains("TargetReco")  && Line.Contains("true"))  fTargetisON=true;
////    if(Line.Contains("SACReco")     && Line.Contains("true"))     fSACisON=true;
////    if(Line.Contains("PVetoReco")   && Line.Contains("true"))   fPVetoisON=true;
////    if(Line.Contains("EVetoReco")   && Line.Contains("true"))   fEVetoisON=true;
////    if(Line.Contains("HEPVetoReco") && Line.Contains("true")) fHEPVetoisON=true;    
////    if(Line.Contains("TPixReco")    && Line.Contains("true"))    fTPixisON=true;
////    
////    if(Line.Contains("UseMonitorMode") && Line.Contains("true"))  fUseMonitorMode=true;
////    if(Line.Contains("UseFullMode")    && Line.Contains("true"))     fUseFullMode=true;
////    //    std::cout<<Line.Data()<<std::endl;
////  }
////  confFile.close();  
//}
