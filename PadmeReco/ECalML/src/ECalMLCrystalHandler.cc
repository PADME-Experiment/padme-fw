#include "ECalMLCrystalHandler.hh"

#include "ECalMLParameters.hh"

#include <algorithm>
#include <iostream>
#include "cstdlib"
#include "math.h"

ECalMLCrystalHandler::ECalMLCrystalHandler()
{

  fNCrystals=0;

  // Get current number of crystals being used
  ECalMLParameters* para = ECalMLParameters::GetInstance();
  fNCryX = para->GetECalMLNCols();
  if (fNCryX>CRYSTALHANDLER_N_MAX_CRYSTALS_X) {
    std::cout << "ERROR - CrystalHandler::CrystalHandler - Too many crystals along X: redefine CRYSTALHANDLER_N_MAX_CRYSTALS_X" << std::endl;
    exit(1);
  }
  fNCryY = para->GetECalMLNRows();
  if (fNCryY>CRYSTALHANDLER_N_MAX_CRYSTALS_Y) {
    std::cout << "ERROR - CrystalHandler::CrystalHandler - Too many crystals along Y: redefine CRYSTALHANDLER_N_MAX_CRYSTALS_Y" << std::endl;
    exit(1);
  }

  //Clean Crystal map
  for (Int_t i=0;i<fNCryX;i++) {
    for (Int_t j=0;j<fNCryY;j++) {
      fCrystalMap[i][j]=0;
    }
  }

}

ECalMLCrystalHandler::~ECalMLCrystalHandler()
{
  // Kill all pointers to the crystals of the event
  for(Int_t kk=0;kk<fNCrystals;kk++) delete fCrystalList[kk];
}

ECalMLCrystal* ECalMLCrystalHandler::GetSeed(double Ethr)
{
  for(Int_t kk=0;kk<fNCrystals;kk++) {
    if ( !(fCrystalList[kk]->IsUsed()) && (fCrystalList[kk]->GetEnergy() > Ethr) ) return fCrystalList[kk];
  }
  return 0;
}

ECalMLCrystal* ECalMLCrystalHandler::CreateCrystal(Int_t ix,Int_t iy)
{
  if (ix<0 || ix>=fNCryX || iy<0 || iy>=fNCryY) {
    std::cout << "WARNING - CrystalHandler::CreateCrystal - Attept to create crystal outside map: "
	      << ix << " " << iy << std::endl;
    return 0;
  }
  if(fCrystalMap[ix][iy]!=0) {
    std::cout << "WARNING - CrystalHandler::CreateCrystal - Attept to create already existing crystal: "
	      << ix << " " << iy << std::endl;
    return fCrystalMap[ix][iy];
  }
  ECalMLCrystal* cry = new ECalMLCrystal(ix,iy);
  fCrystalList[fNCrystals++]=cry;
  fCrystalMap[ix][iy]=cry;
  return cry;
}

bool SortByEnergy(ECalMLCrystal*a ,ECalMLCrystal*b)
{
  return (a->GetEnergy() > b->GetEnergy());
}

void ECalMLCrystalHandler::SortEnergy()
{
  if (fNCrystals) std::sort(fCrystalList,fCrystalList+fNCrystals,SortByEnergy);
}

void ECalMLCrystalHandler::Print()
{
  std::cout << "I know " << fNCrystals << " Crystals " <<std::endl;
  for (Int_t kk=0;kk<fNCrystals;kk++) fCrystalList[kk]->Print();
}

ECalMLCrystal* ECalMLCrystalHandler::GetCrystal(Int_t ix,Int_t iy)
{
  if (ix<0 || ix>=fNCryX || iy<0 || iy>=fNCryY) return 0;
  return fCrystalMap[ix][iy];
}

ECalMLCrystal* ECalMLCrystalHandler::GetCrystal(Int_t i)
{
  if (i<0 || i>=fNCrystals) {
    std::cout << "WARNING - CrystalHandler::GetCrystal(list) - Attempt to access crystal outside list: "
	      << i << std::endl;
    return 0;
  }
  return fCrystalList[i];
}

ECalMLCrystal* ECalMLCrystalHandler::GetCrystal(double x, double y)
{
  // Will be implemented when parameters are available
  return 0;
}

void ECalMLCrystalHandler::ResetCrystals()
{
  for (Int_t i=0; i<fNCrystals; i++) {
    fCrystalList[i]->ResetUsed();
    fCrystalList[i]->ResetSeed();
  }
}
