// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-16
//
// --------------------------------------------------------------
#ifndef TDetectorInfo_h
#define TDetectorInfo_h 1

#include "TSubDetectorInfo.hh"
#include <vector>

class TDetectorInfo {

public:

  TDetectorInfo();
  virtual ~TDetectorInfo() { }
  void Clear(Option_t* option ="");
  void Print(Option_t* option="") const;

  TSubDetectorInfo* AddSubDetectorInfo(TString name);
  TSubDetectorInfo* FindSubDetectorInfo(TString name);

private:

  std::vector<TSubDetectorInfo*> fSubDetectorsInfo;

  ClassDef(TDetectorInfo,1)
};

#endif
