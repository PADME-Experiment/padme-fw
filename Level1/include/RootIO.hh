#ifndef RootIO_H
#define RootIO_H

#include <sys/stat.h>
#include <unistd.h>
#include <string>

#include "TFile.h"
#include "TTree.h"

#include "ADCBoard.hh"

#include "TRawEvent.hh"

#define ROOTIO_OK 0
#define ROOTIO_ERROR -1

class RootIO
{

public:

  RootIO();
  ~RootIO();

  int Init(std::string,int);
  int Exit();
  int FillRawEvent(int,int,std::vector<ADCBoard*>&);
  TRawEvent* GetRawEvent() { return fTRawEvent; }
  void SetVerbose(Int_t v) { fVerbose = v; }

private:

  Int_t OpenOutFile();
  Int_t ChangeOutFile();
  Int_t CloseOutFile();
  Int_t SetOutFile();

  Int_t   fVerbose;

  ULong_t fOutEventsTotal;
  ULong_t fOutEventsCounter;

  TString fOutFile;
  TString fOutFileTemplate;
  UInt_t  fOutFileIndex;
  UInt_t  fNMaxEvtsPerOutFile;

  TFile* fTFileHandle;
  TTree* fTTreeMain;
  TRawEvent* fTRawEvent;

  inline bool file_exists (const std::string& file) {
    struct stat buffer;   
    return (stat(file.c_str(), &buffer) == 0); 
  }

};
#endif
