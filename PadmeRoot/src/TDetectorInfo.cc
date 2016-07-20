// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-16
//
// --------------------------------------------------------------
#include "TDetectorInfo.hh"

ClassImp(TDetectorInfo)

TDetectorInfo::TDetectorInfo()
{;}

void TDetectorInfo::Clear(Option_t* opt)
{;}

void TDetectorInfo::Print(Option_t * /*option*/) const
{
  Int_t nDetectors = fSubDetectorsInfo.size();
  for(Int_t iDet = 0; iDet < nDetectors; iDet++)
    fSubDetectorsInfo[iDet]->Print();
}

TSubDetectorInfo* TDetectorInfo::FindSubDetectorInfo(TString name)
{
  Int_t nDetectors = fSubDetectorsInfo.size();
  for(Int_t iDet = 0; iDet < nDetectors; iDet++)
    if(fSubDetectorsInfo[iDet]->GetName() == name)
      return fSubDetectorsInfo[iDet];
  return 0;
}

TSubDetectorInfo* TDetectorInfo::AddSubDetectorInfo(TString name)
{
  TSubDetectorInfo* subDet = FindSubDetectorInfo(name);
  if(!subDet){
    subDet = new TSubDetectorInfo(name);
    fSubDetectorsInfo.push_back(subDet);
  }
  return subDet;
}
