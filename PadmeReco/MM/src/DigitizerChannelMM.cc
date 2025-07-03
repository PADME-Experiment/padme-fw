#include "DigitizerChannelMM.hh"
#include "TObject.h"
#include "TMath.h"
#include "TTree.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>



DigitizerChannelMM::~DigitizerChannelMM(){;}

void DigitizerChannelMM::Init(GlobalRecoConfigOptions *gMode, PadmeVRecoConfig *cfg){

  fGlobalMode = gMode;
  
  fADCUnitToCharge = cfg->GetParOrDefault("RECO","ADCUnitToCharge",300.); // electrons / adccount
  fADCTimeBin      = cfg->GetParOrDefault("RECO","ADCTimeBin",25.);       // ns
  //  fClusterDeltaCellMax = cfg->GetParOrDefault("RECOCLUSTER","ClusterDelta1CellMax",2); // 
  

  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();
}

void DigitizerChannelMM::Reconstruct(std::vector<TRecoVHit *> &hitArray, TMMBoard* bd, TMMChannel* ch){
  int bdid = bd->GetBoardSN();
  int chid = ch->GetChannelNumber();
  // bdid 0--15
  // chid 0--255
  int channelid = chid;     // channelid occupy 8 least significant bits of channelid
  channelid |= (bdid << 8); // pack into channelid the bdid

  // evaluate total charge and peak time

  Double_t charge = 0;
  Double_t chargeMax = -99999;
  Int_t sampleMaxId = -1;
  for (UShort_t i=0; i<fNSamples; i++){
    charge += fSamples[i];
    if (fSamples[i] > chargeMax){
      sampleMaxId = i;
      chargeMax = fSamples[i];
    }
  }

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetChannelId(channelid);            // will be used to determine the geometrical position by the MMGeometry method ComputePositions using GlobalPosition(ich)
  Hit->SetTime(sampleMaxId*fADCTimeBin);   // ns
  Hit->SetEnergy(charge*fADCUnitToCharge); // in electrons
  Hit->setStatus(ch->IsChannelFailed());
  hitArray.push_back(Hit);

}

void DigitizerChannelMM::PrintConfig(){
  ;
}
