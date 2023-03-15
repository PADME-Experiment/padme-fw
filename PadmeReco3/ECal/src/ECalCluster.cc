#include "Riostream.h"

#include "ECalCluster.hh"

ECalCluster::ECalCluster()
{;}

ECalCluster::~ECalCluster()
{;}

void ECalCluster::Print()
{
  printf("ECal cluster St 0x%llx E %.1fMeV T%.3fs Pos (%.2f,%.2f,%.2f)cm Seed %d Hits",
	 fStatus,fEnergy,fTime,fPosition.X(),fPosition.Y(),fPosition.Z(),fSeed);
  for(UInt_t h=0; h<fNHits; h++) printf(" %d",fHits[h]);
  printf("\n");
}
