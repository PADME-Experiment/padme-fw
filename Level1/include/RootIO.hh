#ifndef RootIO_H
#define RootIO_H

#include <string>

#include "TFile.h"
#include "TTree.h"

#include "ADCBoard.hh"

#include "TRawEvent.hh"

class RootIO
{

public:

  RootIO();
  ~RootIO();

  int Init(std::string);
  int Exit();
  int FillRawEvent(int,int,std::vector<ADCBoard*>&);
  TRawEvent* GetRawEvent() { return fTRawEvent; }

private:

  TFile* fTFileHandle;
  TTree* fTTreeMain;
  TRawEvent* fTRawEvent;

};
#endif
