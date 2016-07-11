// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (Emanuele Leonardi@cern.ch) 2016-03-15
//
// --------------------------------------------------------------
#include <stdlib.h>
#include "Riostream.h"
#include "TMCEvent.hh"

ClassImp(TMCEvent)

//TRandom3* TMCEvent::fgRandomDecayState = 0;

TMCEvent::TMCEvent()
{
  fRunNumber = 0;
  fEventNumber = 0;
  fTime = 0.;

  //fRandomDecayState = new TRandom3();
}

void TMCEvent::Clear(Option_t * /*option*/)
{
  fRunNumber = 0;
  fEventNumber = 0;
  fTime = 0.;
}

void TMCEvent::Reset(Option_t * /*option*/)
{;}

void TMCEvent::Print(Option_t * /*option*/) const
{
  std::cout << "Run "   << fRunNumber
       << " Event " << fEventNumber
       << " Time "  << fTime <<std:: endl;
}

//void TMCEvent::StoreRandomState(TRandom3* RandomDecayState, long *RanecuState)
//{
//
//    *fRandomDecayState = *RandomDecayState;
//    fRanecuState[0] = RanecuState[0];
//    fRanecuState[1] = RanecuState[1];
//}
