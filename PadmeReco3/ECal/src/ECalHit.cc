#include "Riostream.h"

#include "ECalHit.hh"

ECalHit::ECalHit()
{;}

ECalHit::~ECalHit()
{;}

void ECalHit::Print()
{
  printf("ECal hit B/C %2d/%2d Ch %4.4d St 0x%4.4llx E %5.1fMeV T %.3fns Pos (%.2f,%.2f,%.2f)cm\n",
	 fADCBoard,fADCChannel,fChannelId,fStatus,fEnergy,fTime,fPosition.X(),fPosition.Y(),fPosition.Z());
}