#include "Riostream.h"

#include "ETagHit.hh"

ETagHit::ETagHit()
{;}

ETagHit::~ETagHit()
{;}

void ETagHit::Print()
{
  printf("ETag hit B/C %d/%d Ch %d St 0x%llx E %.1fMeV T%.3fs Pos (%.2f,%.2f,%.2f)cm\n",
	 fADCBoard,fADCChannel,fChannelId,fStatus,fEnergy,fTime,fPosition.X(),fPosition.Y(),fPosition.Z());
}
