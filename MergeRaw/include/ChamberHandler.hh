#ifndef ChamberHandler_H
#define ChamberHandler_H

class Configuration;

//class TFile;
class TBranch;
class TChain;
class TTree;

class ChamberHandler
{

public:

  ChamberHandler();
  ~ChamberHandler();

public:

  Int_t Initialize();
  Int_t Finalize();

  Int_t NextEvent();

  UInt_t TotalFiles() { return fTotalFiles; }
  Long64_t TotalEvents() { return fTotalEvents; }
  
  Long64_t EventsRead() { return fCurrentEvent+1; } // Event number starts at 0
  Long64_t EventNumber() { return fCurrentEvent; }

  // Declare public variables used to access chamber events
  ULong64_t       evt;
  UInt_t          error;
  Int_t           daqTimeSec;
  Int_t           daqTimeMicroSec;
  Int_t           srsTimeStamp;
  UInt_t          srsTrigger;
  std::vector<unsigned int> *srsFec;
  std::vector<unsigned int> *srsChip;
  std::vector<unsigned int> *srsChan;
  std::vector<std::string>  *mmChamber;
  std::vector<int>     *mmLayer; // strips fired
  std::vector<char>    *mmReadout; 
  std::vector<int>     *mmStrip; // strips fired
  std::vector<std::vector<short>> *raw_q; //external vector: strips fired, 27(number of samplings)
  std::vector<short>   *max_q;
  std::vector<int>     *t_max_q;

private:

  // Connection to configuration class
  Configuration* fConfig;

  TString FormatFilename(UInt_t);
  Bool_t FileExists(TString);

  // Chain of chamber files for this run
  TChain* fTChain;
  //TTree* fTChain;
  UInt_t fTotalFiles;
  Long64_t fTotalEvents;
  Long64_t fCurrentEvent;
  
  // Declare private branches used to access chamber events
  TBranch* b_evt;
  TBranch* b_error;
  TBranch* b_daqTimeSec;
  TBranch* b_daqTimeMicroSec;
  TBranch* b_srsTimeStamp;
  TBranch* b_srsTrigger;
  TBranch* b_srsFec;
  TBranch* b_srsChip;
  TBranch* b_srsChan;
  TBranch* b_mmChamber;
  TBranch* b_mmLayer;
  TBranch* b_mmReadout;
  TBranch* b_mmStrip;
  TBranch* b_raw_q;
  TBranch* b_max_q;
  TBranch* b_t_max_q;

};
#endif
