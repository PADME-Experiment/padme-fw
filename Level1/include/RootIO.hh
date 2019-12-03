#ifndef RootIO_H
#define RootIO_H

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "ADCBoard.hh"

#include "TRawEvent.hh"

#define ROOTIO_OK 0
#define ROOTIO_ERROR -1

class Configuration;
class DBService;

class RootIO
{

public:

  RootIO();
  ~RootIO();

  //int Init(std::string,int);
  int Init();
  int Exit();

  // FillRawEvent gets all information from a single merged event,
  // converts it to a TRawEvent, and sends it to the output RawData file.
  int FillRawEvent(
		   int,               // run number
		   unsigned int,      // event number
		   struct timespec,   // time when event was merged
		   unsigned long int, // event clock time within run
		   unsigned int,      // event trigger mask
		   unsigned int,      // event status
		   unsigned int,      // missing adc boards bit mask
		   unsigned int,      // trigger mask
		   unsigned int,      // trigger counter
		   unsigned long int, // trigger clock
		   unsigned char,     // trigger fifo
		   unsigned char,     // trigger autopass
		   unsigned int,      // number of ADC boards
		   ADCBoard**         // array of pointers to ADCBoard
		   );
  TRawEvent* GetRawEvent() { return fTRawEvent; }

  unsigned int GetTotalFiles() { return (unsigned int)fOutFileIndex+1; }
  unsigned int GetTotalEvents() { return (unsigned int)fOutEventsTotal; }
  unsigned long long int GetTotalSize() { return (unsigned long long int)fOutSizeTotal; }

private:

  Int_t OpenOutFile();
  Int_t ChangeOutFile();
  Int_t CloseOutFile();
  Int_t SetOutFile();
  unsigned long int GetAdler32(TString);

  Configuration* fConfig;
  //DBService* fDB;

  UInt_t  fOutEventsTotal;
  ULong_t fOutSizeTotal;

  TString fOutFile;
  Int_t   fOutFileDBId;
  TString fOutFileTemplate;
  UInt_t  fOutFileIndex;
  UInt_t  fOutFileEvents;
  ULong_t fOutFileSize;
  UInt_t  fNMaxEvtsPerOutFile;

  std::vector<TString> fOutFileList;

  TFile* fTFileHandle;
  TTree* fTTreeMain;
  TRawEvent* fTRawEvent;

  inline bool file_exists (const std::string& file) {
    struct stat buffer;   
    return (stat(file.c_str(), &buffer) == 0); 
  }

};
#endif
