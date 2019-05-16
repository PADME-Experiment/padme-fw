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
  fRecoTime = TTimeStamp(0,0);
  fEventTime = TTimeStamp(0,0);
  fRunClock = 0;
  fEventStatus = 0;
  fTriggerMask = 0;

  //fRandomDecayState = new TRandom3();
}

void TRecoEvent::Clear(Option_t * /*option*/)
{
  fRunNumber = 0;
  fEventNumber = 0;
  fTime = 0.;
  fRecoTime = TTimeStamp(0,0);
  fEventTime = TTimeStamp(0,0);
  fRunClock = 0;
  fEventStatus = 0;
  fTriggerMask = 0;
}
void TRecoEvent::Reset(Option_t * /*option*/)
{;}

void TRecoEvent::Print(Option_t * /*option*/) const
{
  printf("Run %7d Event %7d Time %8u-%06u.%9d Clock %12llu Status 0x%08x Trigger 0x%08x RecoTime %8u-%06u.%9d\n",
	 fRunNumber,fEventNumber,fEventTime.GetDate(),fEventTime.GetTime(),fEventTime.GetNanoSec(),
	 fRunClock,fEventStatus,fTriggerMask,fRecoTime.GetDate(),fRecoTime.GetTime(),fRecoTime.GetNanoSec());
  //std::cout << "Run "   << fRunNumber
  //     << " Event " << fEventNumber
  //     << " Time "  << fTime <<std:: endl;
}
