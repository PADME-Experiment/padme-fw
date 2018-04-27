#include "PadmeVReconstruction.hh"
#include "PadmeReconstruction.hh"

#include "Riostream.h"

#include "TRegexp.h"
#include "TF1.h"

//#include "Debug.h" 

//PadmeVReconstruction::PadmeVReconstruction(TFile* HistoFile, TString Name, TString ConfigFileName) : PadmeVNamedModule(Name), fHistoFile(HistoFile), fRecoEvent(0)
PadmeVReconstruction::PadmeVReconstruction(TFile* HistoFile, TString Name, TString ConfigFileName) : PadmeVNamedModule(Name), fHistoFile(HistoFile)
{

  fMainReco = 0;

  fConfigFileName = ConfigFileName;
  HistoFile->mkdir(Name.Data());

  //----------- Parse config file for common parameters ----------//

  //ParseConfFile(ConfigFileName);

}

PadmeVReconstruction::~PadmeVReconstruction(){
  std::cout << "Deleting " << fName << " Reconstruction.." << std::endl;
  //if(fRecoEvent){
  //  delete fRecoEvent;
  //  fRecoEvent=0;
  //}
}

void PadmeVReconstruction::Exception(TString Message){
  std::cerr << std::endl << "********************************************************************************" << std::endl;
  std::cerr << "Exception:" << std::endl << "        " << Message.Data() << std::endl;
  std::cerr << std::endl << "********************************************************************************" << std::endl;
  exit(1);
}

void PadmeVReconstruction::Init(PadmeVReconstruction* MainReco) {

  fMainReco = MainReco;

  //GetOrMakeDir(fHistoFile,fName+"Monitor")->cd(); //go to the SubDet directory

}

/*
TRecoVEvent* PadmeVReconstruction::ProcessEvent (TDetectorVEvent* tEvent, Event* tGenEvent) {
  // to be called from the derived classes

  (*(TVEvent*)fRecoEvent) = (*(TVEvent*)tEvent);
  fRecoEvent->Clear("C");
  fRecoEvent->SetDigiEvent((TDigiVEvent*)tEvent);
  fRecoEvent->SetErrorMask(fRecoEvent->GetDigiEvent()->GetErrorMask());

  return fRecoEvent;
}
*/
void PadmeVReconstruction::ProcessEvent(TMCVEvent* tEvent,TMCEvent* tMCEvent) {;}

void PadmeVReconstruction::ProcessEvent(TRawEvent* tRawEvent) {;}

void PadmeVReconstruction::EndProcessing(){ 
  // to be called from the derived classes

  if(!fHistoFile) return;
  HistoExit();
  /*
  if(!fHistoFile->cd(fName+"Monitor")){
    std::cerr << "["+fName+"Reconstruction] WARNING: Failed to find directory '" << fName+"Monitor' in the output file, this should not happen" << std::endl;
    return;
  }
  */
}

/*
TDirectory * PadmeVReconstruction::GetOrMakeDir(TDirectory *inDir,TString dirName){
  if( !inDir ){
    std::cerr << "*** can not make a directory in a non-existent file ***" << std::endl;
    return 0;
  }
  TDirectory * newDir = inDir->GetDirectory(dirName);
  if(newDir) return newDir;
  return inDir->mkdir(dirName);
}
*/

void PadmeVReconstruction::ParseConfFile(TString ConfFileName) {

  std::ifstream confFile(ConfFileName.Data());
  if (!confFile.is_open()) {
    perror(ConfFileName);
    exit(1);
  }

  TString Line;
  while (Line.ReadLine(confFile)) {
    if (Line.BeginsWith("#")) continue;
  }

  confFile.close();

}


void PadmeVReconstruction::HistoInit(){;}

void PadmeVReconstruction::AddHisto(string name, TH1 *histo){
  fHistoMap[name] = histo;
}

TH1*  PadmeVReconstruction::GetHisto(string name){
  return fHistoMap[name];
}




void PadmeVReconstruction::HistoExit(){
 
  cout << "Entering HistoExit for " << this->GetName() << endl;
  
  if(fHistoFile == 0) 
    return;

  fHistoFile->cd();
  TDirectory *dir =  (TDirectory *)fHistoFile->Get(this->GetName().Data());
  dir->cd();
  
  map<string, TH1*>::iterator itr;
  for(itr = fHistoMap.begin(); itr != fHistoMap.end(); itr++)
    {
      TH1* histo = itr->second;
      histo->Write();
      delete histo;
    }
}
