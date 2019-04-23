#include "GlobalRecoConfigOptions.hh"
#include <iostream>

GlobalRecoConfigOptions::GlobalRecoConfigOptions(Int_t recoM, 
						       Int_t pedM, 
						       Int_t cosmicM, 
						       Int_t monitM, 
						       Int_t gdebugM)
{
  SetRunningMode(recoM, pedM, cosmicM, monitM, gdebugM);
}

void  GlobalRecoConfigOptions::SetRunningMode(Int_t recoM, Int_t pedM, Int_t cosmicM, Int_t monitM, Int_t gdebugM)
{
  fRunningMode =            recoM;
  fRunningMode +=      10*cosmicM;
  fRunningMode +=     100*   pedM;
  fRunningMode +=    1000* monitM;
  fGlobalDebugMode =      gdebugM;
  std::cout<<"GlobalRecoConfigOptions::SetRunningMode ---- "<<fRunningMode<<"       debugMode "<<fGlobalDebugMode<<std::endl;
}

Int_t GlobalRecoConfigOptions::GetRecoBit()
{
  Int_t bit = (Int_t)(fRunningMode%10);
  return bit;
}
Int_t GlobalRecoConfigOptions::GetCosmicsBit()
{
  Int_t bit = (Int_t)(fRunningMode%100);
  bit = (Int_t)(bit/10);
  return bit;
}
Int_t GlobalRecoConfigOptions::GetPedestalBit()
{
  Int_t bit = (Int_t)(fRunningMode%1000);
  bit = (Int_t)(bit/100);
  return bit;
}
Int_t GlobalRecoConfigOptions::GetMonitorBit()
{
  Int_t bit = (Int_t)(fRunningMode%10000);
  bit = (Int_t)(bit/1000);
  return bit;
}
