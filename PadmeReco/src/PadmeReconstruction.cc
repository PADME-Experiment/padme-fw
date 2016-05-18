#include "PadmeReconstruction.hh"

#include "TPadmeRun.hh"
#include "TMCEvent.hh"
#include "TECalMCEvent.hh"
#include "TSACMCEvent.hh"

#include "ECalReconstruction.hh"
#include "SACReconstruction.hh"

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
  fRecoLibrary.push_back(new ECalReconstruction(fHistoFile,dummyConfFile));
  fRecoLibrary.push_back(new SACReconstruction(fHistoFile,dummyConfFile));
}

void PadmeReconstruction::InitDetectorsInfo()
{
  fMainReco = this; //init PadmeReconstruction main reco as itself
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
    cout << "Found Tree '" << runTree << "' with " << runBranches->GetEntries() << " branches and " << runNEntries << " entries" << endl;
    TPadmeRun* run = new TPadmeRun();
    for(Int_t iBranch = 0; iBranch < runBranches->GetEntries(); iBranch++){
      TString branchName = ((TBranch*)(*runBranches)[iBranch])->GetName();
      TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*runBranches)[iBranch])->GetClassName());
      cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << endl;
      if(branchName=="Run") {
	runChain->SetBranchAddress(branchName.Data(),&run);
	runChain->GetEntry(0); // Currently only one run per file
	cout << "=== MC Run information - Start ===" << endl;
	cout << "Run number/type " << run->GetRunNumber() << " " << run->GetRunType() << endl;
	cout << "Run start/stop time " << run->GetTimeStart() << " " << run->GetTimeStop() << endl;
	cout << "Run number of events " << run->GetNEvents() << endl;
	TDetectorInfo* detInfo = run->GetDetectorInfo();
	TSubDetectorInfo* sacDetInfo = detInfo->FindSubDetectorInfo("SAC");
	if (sacDetInfo) {
	  cout << "--- SAC geometry parameters ---" << endl;
	  std::vector<TString> sacPar = sacDetInfo->GetGeometryParameters();
	  for(UInt_t iPar = 0; iPar < sacPar.size(); iPar++) {
	    cout << sacPar[iPar].Data() << endl;
	  }
	}
	TSubDetectorInfo* ecalDetInfo = detInfo->FindSubDetectorInfo("ECal");
	if (ecalDetInfo) {
	  cout << "--- ECal geometry parameters ---" << endl;
	  std::vector<TString> ecalPar = ecalDetInfo->GetGeometryParameters();
	  for(UInt_t iPar = 0; iPar < ecalPar.size(); iPar++) {
	    cout << ecalPar[iPar].Data() << endl;
	  }
	}
	cout << "=== MC Run information - End ===" << endl << endl;
      }
    }
  }

  Int_t nEntries = 0;
  TString treeName = "MC";
  fMCChain = BuildChain(treeName);
  if(fMCChain) {
    nEntries = fMCChain->GetEntries();
    TObjArray* branches = fMCChain->GetListOfBranches();
    cout << "Found Tree '" << treeName << "' with " << branches->GetEntries() << " branches and " << nEntries << " entries" << endl;
    fMCEvent = new TMCEvent();
    fSACMCEvent = new TSACMCEvent();
    for(Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++){
      TString branchName = ((TBranch*)(*branches)[iBranch])->GetName();
      TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*branches)[iBranch])->GetClassName());
      cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << endl;
      if (branchName=="Event") {
	fMCChain->SetBranchAddress(branchName.Data(),&fMCEvent);
      } else if (branchName=="SAC") {
	fMCChain->SetBranchAddress(branchName.Data(),&fSACMCEvent);
      } else if (branchName=="ECal") {
	fMCChain->SetBranchAddress(branchName.Data(),&fECalMCEvent);
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
    cout << "=== Read event in position " << fNProcessedEventsInTotal << " ===" << endl;
    cout << "PadmeReconstruction: run/event/time " << fMCEvent->GetRunNumber()
	 << " " << fMCEvent->GetEventNumber() << " " << fMCEvent->GetTime() << endl;
    for (UInt_t iLib = 0; iLib < fRecoLibrary.size(); iLib++) {
      if (fRecoLibrary[iLib]->GetName() == "SAC") {
	fRecoLibrary[iLib]->ProcessEvent(fSACMCEvent,fMCEvent);
      } else if (fRecoLibrary[iLib]->GetName() == "ECal") {
	fRecoLibrary[iLib]->ProcessEvent(fECalMCEvent,fMCEvent);
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
  //if(fMCOutputTree) fMCOutputTree->Write();

  //fHistoFile->Purge();
  //fHistoFile->Close();
  //PrintRecoSummary(cout);
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
