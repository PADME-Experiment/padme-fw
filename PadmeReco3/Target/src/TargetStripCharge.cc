#include "Riostream.h"

#include "TargetStripCharge.hh"

TargetStripCharge::TargetStripCharge()
{;}

TargetStripCharge::~TargetStripCharge()
{;}

void TargetStripCharge::Print()
{
  printf("Target hit B/C %2d/%2d Ch %4.4d St 0x%4.4llx E %5.1fC T %.3fns Pos (%.2f,%.2f,%.2f)cm\n",
	 fADCBoard,fADCChannel,fChannelId,fStatus,fCharge,fTime,fPosition.X(),fPosition.Y(),fPosition.Z());
}
