#include "PadmeReconstruction.hh"

//#include "SACReconstruction.hh"

PadmeReconstruction::PadmeReconstruction(TObjArray* InputFileNameList, TString ConfFileName, TFile* OutputFile, Int_t NEvt, UInt_t Seed) :
  PadmeVReconstruction(OutputFile,"Padme",ConfFileName),fInputFileNameList(InputFileNameList)
{
  Init(NEvt,Seed);
}

PadmeReconstruction::~PadmeReconstruction()
{;}

void PadmeReconstruction::InitDetectorsInfo() {
  fMainReco = this; //init PadmeReconstruction main reco as itself
  //if(FindReco("SAC"))((SACReconstruction*) FindReco("SAC"))->Init(this);
}

void PadmeReconstruction::Init(Int_t NEvt, UInt_t Seed)
{

  //gRandom->SetSeed(Seed);
  //fNEvt = NEvt;

  TTree::SetMaxTreeSize(190000000000);

  Int_t nEntries = 0;
  TString treeName = "Generated";
  fMCChain = BuildChain(treeName);
  if(fMCChain) {
    fMCTruthEvent = new TPadmeEvent();
    TObjArray* branches = fMCChain->GetListOfBranches();
    nEntries = fMCChain->GetEntries();
    cout << "Found Tree '" << treeName << "' with " << branches->GetEntries() << " branches and " << nEntries << " entries" << endl;
    for(Int_t iBranch = 0; iBranch < branches->GetEntries(); iBranch++){
      TString branchName = ((TBranch*)(*branches)[iBranch])->GetName();
      TClass* branchObjectClass = TClass::GetClass(((TBranch*)(*branches)[iBranch])->GetClassName());
      cout << "Found Branch " << branchName.Data() << " containing " << branchObjectClass->GetName() << endl;
      if(branchName=="Event") {
	fMCChain->SetBranchAddress(branchName.Data(),&fMCTruthEvent);
      }
    }
  }

  fNProcessedEventsInTotal = 0;

}

Bool_t PadmeReconstruction::NextEvent()
{
  if (fMCChain) fMCChain->GetEntry(fNProcessedEventsInTotal);
  ProcessEvent();
  fNProcessedEventsInTotal++;

  return true;
}

void PadmeReconstruction::EndProcessing(){

  fHistoFile->cd("/");
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

  fHistoFile->Purge();
  fHistoFile->Close();
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

