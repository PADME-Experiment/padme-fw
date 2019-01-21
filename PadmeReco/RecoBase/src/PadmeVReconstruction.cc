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
  fChannelReco = 0;
  fChannelCalibration = 0;
  fConfigFileName = ConfigFileName;
  HistoFile->mkdir(Name.Data());
  fConfigParser = new utl::ConfigParser(ConfigFileName.Data());
  
  fConfig = new PadmeVRecoConfig(fConfigParser,Name);
  //  InitChannelID(fConfig);

  //----------- Parse config file for common parameters ----------//

  //ParseConfFile(ConfigFileName);

}

PadmeVReconstruction::~PadmeVReconstruction(){
  std::cout << "Deleting " << fName << " Reconstruction.." << std::endl;
  //if(fRecoEvent){
  //  delete fRecoEvent;
  //  fRecoEvent=0;
  //}
  if(fConfigParser) {delete fConfigParser; fConfigParser=0;};
  if(fConfig) {delete fConfig; fConfig=0;}; 
  if(fChannelReco) {delete fChannelReco; fChannelReco = 0;};
  if(fChannelCalibration) {delete fChannelCalibration; fChannelCalibration = 0;};
}

void PadmeVReconstruction::Exception(TString Message){
  std::cerr << std::endl << "********************************************************************************" << std::endl;
  std::cerr << "Exception:" << std::endl << "        " << Message.Data() << std::endl;
  std::cerr << std::endl << "********************************************************************************" << std::endl;
  exit(1);
}

void PadmeVReconstruction::Init(PadmeVReconstruction* MainReco) {
  std::cout << this->GetName() <<": Initializing" << std::endl;

  fMainReco = MainReco;
  if(fChannelReco) {
    fChannelReco->Init(GetConfig());
    InitChannelID(GetConfig());
    if(fChannelCalibration) fChannelCalibration->Init(GetConfig());
    std::cout <<"Number of ADCs for detector: " << this->GetName() << ": " << GetConfig()-> GetNBoards() << std::endl;
  }
  
  HistoInit();
  

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

//void PadmeVReconstruction::ProcessEvent(TRawEvent* tRawEvent) {;}


void PadmeVReconstruction::EndProcessing(){ 
  // to be called from the derived classes
  cout << "Entering End processing for " << this->GetName() << endl;

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


void PadmeVReconstruction::ProcessEvent(TRawEvent* rawEv){

  // If this is not a channel reconstruction, better exit
  // No idea what to do with it...
  if (!fChannelReco) return;

  //Perform some cleaning before:
  vector<TRecoVHit *> &Hits  = GetRecoHits();
  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    delete Hits[iHit];
  }

  Hits.clear();

  UChar_t nBoards = rawEv->GetNADCBoards();

  TADCBoard* ADC;

  for(Int_t iBoard = 0; iBoard < nBoards; iBoard++) {
    ADC = rawEv->ADCBoard(iBoard);
    if(GetConfig()->BoardIsMine( ADC->GetBoardId())) {
      //Loop over the channels and perform reco
      for(unsigned ich = 0; ich < ADC->GetNADCChannels();ich++) {
	TADCChannel* chn = ADC->ADCChannel(ich);
	fChannelReco->SetDigis(chn->GetNSamples(),chn->GetSamplesArray());
	unsigned int nHitsBefore = Hits.size();
	fChannelReco->Reconstruct(Hits);
	unsigned int nHitsAfter = Hits.size();
	for(unsigned int iHit = nHitsBefore; iHit < nHitsAfter;++iHit) {
	  Hits[iHit]->SetChannelId(GetChannelID(ADC->GetBoardId(),chn->GetChannelNumber()));
	}
      }
    } else {
    }
  }
   
  if(fChannelCalibration) fChannelCalibration->PerformCalibration(Hits);
  //Processing is over, let's analyze what's here, if foreseen
  AnalyzeEvent(rawEv);
  
}


void PadmeVReconstruction::AnalyzeEvent(TRawEvent *rawEv){
  ;
}


