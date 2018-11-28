#include <stdlib.h>
#include "Riostream.h"
#include "TRecoEvent.hh"

ClassImp(TRecoEvent)

//TRandom3* TRecoEvent::fgRandomDecayState = 0;

TRecoEvent::TRecoEvent()
{
  fRunNumber = 0;
  fEventNumber = 0;
  fTime = 0.;

  //fRandomDecayState = new TRandom3();
}

void TRecoEvent::Clear(Option_t * /*option*/)
{
  fRunNumber = 0;
  fEventNumber = 0;
  fTime = 0.;
}
void TRecoEvent::Reset(Option_t * /*option*/)
{;}

void TRecoEvent::Print(Option_t * /*option*/) const
{
  std::cout << "Run "   << fRunNumber
       << " Event " << fEventNumber
       << " Time "  << fTime <<std:: endl;
}

