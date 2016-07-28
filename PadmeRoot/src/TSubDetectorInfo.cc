// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-16
//
// --------------------------------------------------------------
#include "TSubDetectorInfo.hh"

#include <iostream>

ClassImp(TSubDetectorInfo)

void TSubDetectorInfo::Clear(Option_t*)
{
  fGeometryParameters.clear();
}

void TSubDetectorInfo::Print(Option_t*) const
{
  for(UInt_t iPar = 0; iPar < fGeometryParameters.size(); iPar++)
    std::cout << fGeometryParameters[iPar].Data() << std::endl;
}
