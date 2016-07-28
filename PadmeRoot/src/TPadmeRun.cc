// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-15
//
// --------------------------------------------------------------
#include <stdlib.h>
#include "Riostream.h"

#include "TPadmeRun.hh"

ClassImp(TPadmeRun)

TPadmeRun::TPadmeRun()
{
  fRunNumber = -1;
  fRunType   = -1;
  fTimeStart =  0;
  fTimeStop  =  0;
  fNEvents   =  0;
}

void TPadmeRun::Clear(Option_t * /*option*/)
{
  fRunNumber = -1;
  fRunType   = -1;
  fTimeStart =  0;
  fTimeStop  =  0;
  fNEvents   =  0;
  fDetInfo.Clear("C");
}

void TPadmeRun::Print(Option_t * /*option*/) const
{
  std::cout << "Run nr "   << fRunNumber
       << " type "    << fRunType
       << " Tstart "  << fTimeStart
       << " Tstop "   << fTimeStop
       <<  " #events " << fNEvents << std::endl;
  fDetInfo.Print();
}
