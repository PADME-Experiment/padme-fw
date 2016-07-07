#include "PadmeReconstruction.hh"

#include "TPadmeRun.hh"
#include "TMCEvent.hh"
#include "TTargetMCEvent.hh"
#include "TEVetoMCEvent.hh"
#include "TPVetoMCEvent.hh"
#include "THEPVetoMCEvent.hh"
#include "TECalMCEvent.hh"
#include "TSACMCEvent.hh"

#include "TargetReconstruction.hh"
#include "EVetoReconstruction.hh"
#include "PVetoReconstruction.hh"
#include "HEPVetoReconstruction.hh"
#include "ECalReconstruction.hh"
#include "SACReconstruction.hh"

#include "TEntryList.h"

PadmeReconstruction::PadmeReconstruction(TObjArray* InputFileNameList, TString ConfFileName, TFile* OutputFile, Int_t NEvt, UInt_t Seed) :
  PadmeVReconstruction(OutputFile,"Padme",ConfFileName),fInputFileNameList(InputFileNameList)
{
  Init(NEvt,Seed);
  InitLibraries();
}

PadmeReconstruction::~PadmeReconstruction()
{;}

void PadmeReconstruction::InitLibraries()
{
  TString dummyConfFile = "pippo.conf";
  fRecoLibrary.push_back(new TargetReconstruction(fHistoFile,dummyConfFile));
  fRecoLibrary.push_back(new EVetoReconstruction(fHistoFile,dummyConfFile));
  fRecoLibrary.push_back(new PVetoReconstruction(fHistoFile,dummyConfFile));
  fRecoLibrary.push_back(new HEPVetoReconstruction(fHistoFile,dummyConfFile));
  fRecoLibrary.push_back(new ECalReconstruction(fHistoFile,dummyConfFile));
  fRecoLibrary.push_back(new SACReconstruction(fHistoFile,dummyConfFile));
}

void PadmeReconstruction::InitDetectorsInfo()
{
  fMainReco = this; //init PadmeReconstruction main reco as itself
  if (FindReco("Target")) ((TargetReconstruction*) FindReco("Target"))->Init(this);
  if (FindReco("EVeto")) ((EVetoReconstruction*) FindReco("EVeto"))->Init(this);
  if (FindReco("PVeto")) ((PVetoReconstruction*) FindReco("PVeto"))->Init(this);
  if (FindReco("HEPVeto")) ((HEPVetoReconstruction*) FindReco("HEPVeto"))->Init(this);
  if (FindReco("ECal")) ((ECalReconstruction*) FindReco("ECal"))->Init(this);
  if (FindReco("SAC")) ((SACReconstruction*)FindReco("SAC"))->Init(this);
}

void PadmeReconstruction::Init(Int_t NEvt, UInt_t Seed)
{

  //gRandom->SetSeed(Seed);
  //fNEvt = NEvt;

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
	std::cout << "=== MC Run information - End ===" << std::endl << std::endl;
      }
    }
  }

  Int_t nEntries = 0;
  TString treeName;
  treeName = "RawEvents";
  fRawChain = BuildChain(treeName);
  std::cout<<fRawChain->GetEntries()<<std::endl;
  //TEntryList* entryList=fRawChain->GetEntryList();
  //for(int i=0;entryList->GetEntries();++i){
  //  entryList->At(i)->Print();
  //}



  if(fRawChain) {
    TObjArray* branches = fRawChain->GetListOfBranches();
    std::cout << "Found Tree '" << treeName << "' with " << branches->GetEntries() << " branches and " << nEntries << " entries" << std::endl;
    TBranch* bRawEv = fRawChain->GetBranch("RawEvent");
      std::cout<<fRawChain->GetEntries()<<std::endl;

    //if(bRawEv!=NULL){
    //  std::cout<<"FOUND"<<std::endl;
    //}else{
    //  std::cout<<"NOT FOUND"<<std::endl;
    //}
    //for(Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++){
    //  TString branchName = ((TBranch*)(*branches)[iBranch])->GetName();
    //  TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*branches)[iBranch])->GetClassName());
    //  std::cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << std::endl;
    //}

  }




  treeName = "MC";
  fMCChain = BuildChain(treeName);
  if(fMCChain) {
    nEntries = fMCChain->GetEntries();
    TObjArray* branches = fMCChain->GetListOfBranches();
    std::cout << "Found Tree '" << treeName << "' with " << branches->GetEntries() << " branches and " << nEntries << " entries" << std::endl;
    fMCEvent = new TMCEvent();
    fSACMCEvent = new TSACMCEvent();
    for(Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++){
      TString branchName = ((TBranch*)(*branches)[iBranch])->GetName();
      TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*branches)[iBranch])->GetClassName());
      std::cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << std::endl;
      if (branchName=="Event") {
	fMCChain->SetBranchAddress(branchName.Data(),&fMCEvent);
      } else if (branchName=="Target") {
	fMCChain->SetBranchAddress(branchName.Data(),&fTargetMCEvent);
      } else if (branchName=="EVeto") {
	fMCChain->SetBranchAddress(branchName.Data(),&fEVetoMCEvent);
      } else if (branchName=="PVeto") {
	fMCChain->SetBranchAddress(branchName.Data(),&fPVetoMCEvent);
      } else if (branchName=="HEPVeto") {
	fMCChain->SetBranchAddress(branchName.Data(),&fHEPVetoMCEvent);
      } else if (branchName=="ECal") {
	fMCChain->SetBranchAddress(branchName.Data(),&fECalMCEvent);
      } else if (branchName=="SAC") {
	fMCChain->SetBranchAddress(branchName.Data(),&fSACMCEvent);
      }
    }
  }

  fNProcessedEventsInTotal = 0;

  // Initialize reconstruction for each subdetector
  InitDetectorsInfo();

}

Bool_t PadmeReconstruction::NextEvent()
{
  if ( fMCChain && fMCChain->GetEntry(fNProcessedEventsInTotal) ) {
    std::cout << "=== Read event in position " << fNProcessedEventsInTotal << " ===" << std::endl;
    std::cout << "PadmeReconstruction: run/event/time " << fMCEvent->GetRunNumber()
	 << " " << fMCEvent->GetEventNumber() << " " << fMCEvent->GetTime() << std::endl;
    for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
      if (fRecoLibrary[iLib]->GetName() == "Target") {
	fRecoLibrary[iLib]->ProcessEvent(fTargetMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "EVeto") {
	fRecoLibrary[iLib]->ProcessEvent(fEVetoMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "PVeto") {
	fRecoLibrary[iLib]->ProcessEvent(fPVetoMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "HEPVeto") {
	fRecoLibrary[iLib]->ProcessEvent(fHEPVetoMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "ECal") {
	fRecoLibrary[iLib]->ProcessEvent(fECalMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "SAC") {
	fRecoLibrary[iLib]->ProcessEvent(fSACMCEvent,fMCEvent);
      }

    }
    fNProcessedEventsInTotal++;
    return true;
  }
  return false;
}

void PadmeReconstruction::EndProcessing(){

  //fHistoFile->cd("/");
  //for(Int_t iReco = 0; iReco < fNReconstructions; iReco++){
  //  fReconstructions[iReco]->EndProcessing();
  //  fHistoFile->cd("/");
  //}
  //for(Int_t iReco = 0; iReco < fNReconstructions; iReco++){
  //  fReconstructions[iReco]->EvaluateROSettings();
  //}
  //for(UInt_t iTree=0;iTree<fOutputTrees.size();iTree++){
  //  fOutputTrees[iTree]->Write();
  //}
  //if(fMstd::COutputTree) fMstd::COutputTree->Write();

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
