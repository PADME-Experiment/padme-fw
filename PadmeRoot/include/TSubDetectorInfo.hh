// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-16
//
// --------------------------------------------------------------
#ifndef TSubDetectorInfo_h
#define TSubDetectorInfo_h 1

#include "TString.h"
#include "TObjArray.h"

class TSubDetectorInfo {

public:

  TSubDetectorInfo() {};
  TSubDetectorInfo(TString name) { fName = name; }
  virtual ~TSubDetectorInfo() {}
  void Clear(Option_t* option="");
  void Print(Option_t* option="") const;

public:

  TString GetName()              { return fName;  }
  void    SetName(TString value) { fName = value; }

  std::vector<TString> GetGeometryParameters() { return fGeometryParameters; }
  void SetGeometryParameters(std::vector<TString> value) { fGeometryParameters = value; }

private:

  TString fName;

  std::vector<TString> fGeometryParameters;

  ClassDef(TSubDetectorInfo,1)
};
#endif
