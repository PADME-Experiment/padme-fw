// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "TargetCalibration.hh"
#include "TTargetRecoBeam.hh"
#include "TRecoVHit.hh"
 
TargetCalibration::TargetCalibration()
  : PadmeVCalibration()
{
  std::cout<<"TargetCalibration being created ............"<<std::endl ;
  fAverageGain = 0.;
  fHVsign =      0.;
}

TargetCalibration::~TargetCalibration()
{;}


void TargetCalibration::InitCommonEnergyScale(PadmeVRecoConfig *cfg)
{
  fCommonEnergyScale = (double)cfg->GetParOrDefault("EnergyCalibration",    "Common_EnergyScale", 1.);
  fAverageGain       = (double)cfg->GetParOrDefault("EnergyCalibration",    "AverageGain", 1300.);
  fHVsign            = (double)cfg->GetParOrDefault("EnergyCalibration",    "HVsign"     ,   -1.);
  fCCD               = (double)cfg->GetParOrDefault("EnergyCalibration",    "CCD"        ,   10.);
  fCommonEnergyScale = fCommonEnergyScale/fAverageGain;
}
void TargetCalibration::PerformBeamCalibration(TTargetRecoBeam* b)
{
  double ccdCorr = 10./fCCD;
  double npot    =b->getnPOT();
  double npoterr =b->getnPOTError();
  b->setPOT( npot*ccdCorr, npoterr*sqrt(ccdCorr) );
}

void TargetCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits)
{
  // apply common charge scale factor (pC) [1/commonGain] and channel to channel equalization
  PadmeVCalibration::PerformCalibration(Hits);

  // subtract common mode noise assuming the charge of the first X strip is 0 => biasing xharge on X1 
  double chargeCommonModeNoise = 0.;
  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    int ich = Hits[iHit]->GetChannelId();
    if (ich==1) {//looking for charge on X1 
      chargeCommonModeNoise = Hits[iHit]->GetEnergy();
      break;
    }
  }
  double hCharge = 0.;
  int ich = 0;
  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    hCharge = Hits[iHit]->GetEnergy()-chargeCommonModeNoise;
    ich     = Hits[iHit]->GetChannelId();
    if( fHVsign<0 && ich>16 ) hCharge = - hCharge;
    if( fHVsign>0 && ich<17 ) hCharge = - hCharge;
    Hits[iHit]->SetEnergy(hCharge);
  }
    
}

