#ifndef OutputHandler_H
#define OutputHandler_H

class Configuration;

class TFile;
class TTree;
class TRawEvent;

class OutputHandler
{

public:

  OutputHandler();
  ~OutputHandler();

public:

  Int_t Initialize();
  Int_t Finalize();

  //Int_t WriteEvent(TRawEvent*);
  Int_t WriteEvent();
  TRawEvent* GetRawEvent() { return fTRawEvent; }

  UInt_t GetTotalOutFiles() { return fOutFileIndex+1; }
  TString GetOutFile(UInt_t i) { return fOutFileList[i]; }
  UInt_t GetTotalEvents() { return fOutEventsTotal; }
  ULong64_t GetTotalSize() { return fOutSizeTotal; }

private:

  Int_t ChangeOutFile();
  Int_t OpenOutFile();
  Int_t CloseOutFile();
  Bool_t FileExists(TString);

  TString FormatFilename(UInt_t);
  //void CopyTRawEvent(TRawEvent*, TRawEvent*);

  // Connection to configuration class
  Configuration* fConfig;

  TFile* fTFileHandle;
  TTree* fTTreeMain;
  TRawEvent* fTRawEvent;

  std::vector<TString> fOutFileList;

  UInt_t  fOutFileIndex;
  TString fOutFileName;
  UInt_t  fOutFileEvents;
  ULong64_t fOutFileSize;

  UInt_t fOutEventsTotal;
  ULong64_t fOutSizeTotal;

};
#endif
