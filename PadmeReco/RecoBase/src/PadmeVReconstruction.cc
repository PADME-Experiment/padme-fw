#include "PadmeVReconstruction.hh"
#include "PadmeReconstruction.hh"
#include "PadmeVClusterization.hh"

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
  fClusterization = 0;
  fTriggerProcessor = 0;
  HistoFile->mkdir(Name.Data());
  fConfigFileName = ConfigFileName;
  fConfigParser = new utl::ConfigParser(ConfigFileName.Data());
  fClusterization = new PadmeVClusterization();
    
  fConfig = new PadmeVRecoConfig(fConfigParser,Name);
  fWriteHits = true;
  fWriteClusters = true;
  fWriteHits     = (Bool_t)fConfig->GetParOrDefault("Output", "Hits"          , 1 );
  fWriteClusters = (Bool_t)fConfig->GetParOrDefault("Output", "Clusters"      , 1 );
  
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
  //if(fClusterization) {delete fClusterization; fClusterization = 0;};
  if(fChannelCalibration) {delete fChannelCalibration; fChannelCalibration = 0;};
  if(fTriggerProcessor) {delete fTriggerProcessor; fTriggerProcessor = 0;};
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
  InitFlags(); //Initialize running modes M. Raggi   
  if(fChannelReco) {

    fChannelReco->Init(GetConfig());
 
    std::cout<<"digitizer inititialized OK"<<std::endl;
    fClusterization->Init(GetConfig());
    std::cout<<"Clusterization inititialized OK"<<std::endl;
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

void PadmeVReconstruction::ProcessEvent(TRecoVObject* tEvent,TRecoEvent* tRecoEvent)
{
  //std::cout<<this->GetName()<<"::ProcessEvent(TRecoVObject*) ... nhits read on input branch "<<tEvent->GetNHits()<<std::endl;  
  ReadHits(tEvent, tRecoEvent);
  //std::cout<<this->GetName()<<"::ProcessEvent(TRecoVObject*) ...   now  "<<fHits.size()<<" copied to internal vector "<<std::endl;  

  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetRecoHits());
  
  // Clustering  
  ClearClusters();
  BuildClusters();

}

void PadmeVReconstruction::ReadHits(TRecoVObject* tEvent,TRecoEvent* tRecoEvent)
{

  //ClearHits();
  fHits.clear(); // here we need to clear the content of the vector ... OBJECTS pointed by the components of the vector must not be deleted since the TClonesArray has already be cleared by ROOT 
  for (Int_t ih=0; ih<tEvent->GetNHits(); ++ih)
    {
      fHits.push_back( tEvent->Hit(ih) );
    }
  //std::cout<<this->GetName()<<"::ReadHits(TRecoVObject*) ... nhits read from input tree  "<<tEvent->GetNHits()<<" in local vector "<<fHits.size()<<std::endl;

}


void PadmeVReconstruction::EndProcessing(){ 
  // to be called from the derived classes
  cout << "Entering EndProcessing for " << this->GetName() << endl;

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
void PadmeVReconstruction::InitFlags(){;}

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

void PadmeVReconstruction::BuildTriggerInfo(TRawEvent* rawEv){
  fTriggerProcessor->Clear();
  UChar_t nBoards = rawEv->GetNADCBoards();
  
  TADCBoard* ADC;
  // Added trigMask M. Raggi 21/01/2019
  UInt_t TrigMask = rawEv->GetEventTrigMask();
  //  std::cout<<" trig Mask "<<TrigMask<<std::endl;
  fTriggerProcessor->SetTrigMask(TrigMask);
  //  std::cout<<" trig Mask recupera "<<fTriggerProcessor->GetTrigMask()<<std::endl;

  for(Int_t iBoard = 0; iBoard < nBoards; iBoard++) {
    ADC = rawEv->ADCBoard(iBoard);
    if(GetConfig()->BoardIsMine( ADC->GetBoardId())) {
      //Loop over the trigger channels and perform reco
      UChar_t nTriggers  = ADC->GetNADCTriggers();
      for(Int_t iTr = 0; iTr<nTriggers;iTr++) {
	TADCTrigger* trigger = ADC->ADCTrigger(iTr);
	fTriggerProcessor->ProcessTrigger(ADC->GetBoardId(),trigger);
      }
    }    
  }
}

void PadmeVReconstruction::ProcessEvent(TRawEvent* rawEv){

  // From waveforms to Hits
  
  if(fTriggerProcessor) {
    BuildTriggerInfo(rawEv);
  }

  BuildHits(rawEv);
   
  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetRecoHits());
  
  // from Hits to Clusters
  ClearClusters();
  BuildClusters();

  //Processing is over, let's analyze what's here, if foreseen
  AnalyzeEvent(rawEv);
  
}

void PadmeVReconstruction::ClearHits(){

  //Delete Hits pointed by the vector components; set to zero all components and resize to 0 the vector; 
  //NOTE: this must be done only if the hits are owned (have been created with new) by this ! 

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  if (Hits.size()==0) return;
  //std::cout<<" hits to be cleared ... now "<<Hits.size()<<std::endl;  
  for(unsigned int iHit = 0; iHit < Hits.size(); ++iHit){
    //std::cout<<" ClearHit "<<iHit<<std::endl;
    if (Hits[iHit]) delete Hits[iHit];
  }
  Hits.clear();
  
}
void PadmeVReconstruction::ClearClusters(){
  //Perform some cleaning before:
  vector<TRecoVCluster *> &Clusters = GetClusters();
  if (Clusters.size()==0) return;
  for(unsigned int i = 0; i < Clusters.size(); ++i){
    if (Clusters[i]) delete Clusters[i];
  }
  Clusters.clear();
}

void PadmeVReconstruction::BuildHits(TRawEvent* rawEv){


  ClearHits();
  vector<TRecoVHit *> &Hits  = GetRecoHits();

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
	  Hits[iHit]->setBDCHid( ADC->GetBoardId(), chn->GetChannelNumber() );
	  if(fTriggerProcessor)
	    Hits[iHit]->SetTime(
				Hits[iHit]->GetTime() - 
				fTriggerProcessor->GetChannelTriggerTime( ADC->GetBoardId(), chn->GetChannelNumber() )
				);
	}
      }
    } else {
    }
  }  
}


void PadmeVReconstruction::BuildClusters()
{

  vector<TRecoVCluster *> &myClusters  = GetClusters();
  for(unsigned int iCl = 0;iCl < myClusters.size();iCl++){
    delete myClusters[iCl];
  }
  myClusters.clear();

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  if(Hits.size()==0){
    return;
  }

  fClusterization->Reconstruct(Hits, myClusters);

}

void PadmeVReconstruction::AnalyzeEvent(TRawEvent *rawEv){;}


