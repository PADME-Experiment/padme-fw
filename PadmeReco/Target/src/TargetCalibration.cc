// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "TargetCalibration.hh"
#include "RecoVChannelID.hh"
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

void TargetCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){

  PadmeVCalibration::Init(cfg, chIdMgr );

  fHVsign            = (double)cfg->GetParOrDefault("EnergyCalibration",    "HVsign"                       ,   -1.);
  fUseIndChannelGain =    (int)cfg->GetParOrDefault("EnergyCalibration",    "UseIndividualChannelGain"     ,    1 );
  fCCD               = (double)cfg->GetParOrDefault("EnergyCalibration",    "CCD"                          ,   12.);
  fCommonModeNoiseSubtr = (int)cfg->GetParOrDefault("EnergyCalibration",    "CommonModeNoise_Subtraction"  ,    1 );
  fCMNS_offBeamIdForX   = (int)cfg->GetParOrDefault("EnergyCalibration",    "CMNS_offBeamIdForX"             ,    1 );
  fCMNS_offBeamIdForY   = (int)cfg->GetParOrDefault("EnergyCalibration",    "CMNS_offBeamIdForY"             ,    1 );

  if (fUseIndChannelGain) 
    {
      fChannelGainMap   = new std::map<int, double>;
      // calibration maps 
      std::vector<int> chIds = chIdMgr->GetChannelIDVector();
      for (unsigned int j=0; j<chIds.size();++j)
	{
	  //std::cout<<" init calib consts .... j "<<j<<" ID="<<chIds[j]<<" .... "<<std::endl;
	  double channelGain = (double)cfg->GetParOrDefault("EnergyCalibration",    "Gain_"+std::to_string(chIds[j]),fAverageGain);
	  if (fChannelGainMap->find(chIds[j])==fChannelGainMap->end()) fChannelGainMap->insert ( std::pair<int,double>(chIds[j], channelGain) ); 
	  if (fEnergyCalibMap->find(chIds[j])!=fEnergyCalibMap->end()) 
	    {
	      fEnergyCalibMap->find(chIds[j])->second = (fEnergyCalibMap->find(chIds[j])->second)*fAverageGain/channelGain;
	    }
	  else std::cout<<"TargetCalibration::Init - WARNING - no entry found in energy calib map for id="<<chIds[j]<<std::endl;  
	}
    }
}

void TargetCalibration::InitCommonEnergyScale(PadmeVRecoConfig *cfg)
{
  fCommonEnergyScale = (double)cfg->GetParOrDefault("EnergyCalibration",    "Common_EnergyScale", 1.);
  fAverageGain       = (double)cfg->GetParOrDefault("EnergyCalibration",    "AverageGain", 1300.);
  fCommonEnergyScale = fCommonEnergyScale/fAverageGain;
}
void TargetCalibration::PerformBeamCalibration(TTargetRecoBeam* b)
{
  double ccdCorr = 12./fCCD;
  double npot    =b->getnPOT();
  double npoterr =b->getnPOTError();
  //std::cout<<" POT before calib "<<npot; 
  b->setPOT( npot*ccdCorr, npoterr*sqrt(ccdCorr) );
  //std::cout<<" and after calib "<<npot*ccdCorr<<std::endl; 
}

void TargetCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits)
{
  // apply common charge scale factor (pC) [1/commonGain] and channel to channel equalization
  PadmeVCalibration::PerformCalibration(Hits);

  double chargeCommonModeNoiseX = 0.;
  double chargeCommonModeNoiseY = 0.;
  // if requested 
  //    subtract common mode noise assuming the charge of the strip with chID=fCMNS_offBeamIdForX is 0  
  //    subtract common mode noise assuming the charge of the strip with chID=fCMNS_offBeamIdForY is 0  
  // else subtract 0. 
  if (fCommonModeNoiseSubtr == 1) 
    {
      for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
	int ich = Hits[iHit]->GetChannelId();
	if (ich == fCMNS_offBeamIdForX) {//looking for charge on strip with chID = fCMNS_offBeamForX
	  chargeCommonModeNoiseX = Hits[iHit]->GetEnergy();
	  if (fCMNS_offBeamIdForY == fCMNS_offBeamIdForX) 
	    {
	      chargeCommonModeNoiseY = chargeCommonModeNoiseX;
	      if (ich<17) chargeCommonModeNoiseY = -chargeCommonModeNoiseY;  //new
	      else        chargeCommonModeNoiseX = -chargeCommonModeNoiseX;  //new
	      //chargeCommonModeNoiseY = chargeCommonModeNoiseX;  //new1
	      break;
	    }
	}
	else if (ich == fCMNS_offBeamIdForY) {//looking for charge on strip with chID = fCMNS_offBeamForY
	  chargeCommonModeNoiseY = Hits[iHit]->GetEnergy();
	  if (fCMNS_offBeamIdForY == fCMNS_offBeamIdForX) 
	    {
	      chargeCommonModeNoiseX = chargeCommonModeNoiseY;  //new
	      if (ich<17) chargeCommonModeNoiseY = -chargeCommonModeNoiseY;  //new
	      else        chargeCommonModeNoiseX = -chargeCommonModeNoiseX;  //new
	      //chargeCommonModeNoiseX = chargeCommonModeNoiseY;  //new1
	      break;
	    }
	}
      }
    if( fHVsign<0 && fCMNS_offBeamIdForX>16 ) chargeCommonModeNoiseX = - chargeCommonModeNoiseX;
    if( fHVsign>0 && fCMNS_offBeamIdForX<17 ) chargeCommonModeNoiseX = - chargeCommonModeNoiseX;
    if( fHVsign<0 && fCMNS_offBeamIdForY>16 ) chargeCommonModeNoiseY = - chargeCommonModeNoiseY;
    if( fHVsign>0 && fCMNS_offBeamIdForY<17 ) chargeCommonModeNoiseY = - chargeCommonModeNoiseY;
    }
  
  // apply sign correction for X or Y view depending on the bias-HV sign and subtruct CMN if  
  double hCharge = 0.;
  int ich = 0;
  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    ich     = Hits[iHit]->GetChannelId();
    hCharge = Hits[iHit]->GetEnergy();
    if( fHVsign<0 && ich>16 ) hCharge = - hCharge;
    if( fHVsign>0 && ich<17 ) hCharge = - hCharge;
    //if (ich==8)  std::cout<<"X strip 8*** charge before corr. "<<hCharge<<" (CMN = "<<chargeCommonModeNoiseX<<") ";
    //if (ich==24) std::cout<<"Y strip 8*** charge before corr. "<<hCharge<<" (CMN = "<<chargeCommonModeNoiseY<<") ";
    //subtract common mode noise{
    if ( ich<17 ) hCharge = hCharge-chargeCommonModeNoiseX;
    else          hCharge = hCharge-chargeCommonModeNoiseY;
    //if (ich==8 || ich==24) std::cout<<" after corr. "<<hCharge<<std::endl;
    //}
    Hits[iHit]->SetEnergy(hCharge);
  }    
}

