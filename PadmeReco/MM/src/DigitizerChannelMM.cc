#include "DigitizerChannelMM.hh"
#include "TObject.h"
#include "TMath.h"
#include "TTree.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "Math/ProbFunc.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>



DigitizerChannelMM::~DigitizerChannelMM(){
  delete fSignalShape;
}

void DigitizerChannelMM::Init(GlobalRecoConfigOptions *gMode, PadmeVRecoConfig *cfg){
  std::cout <<"Init method of DigitizerChannelMM " << std::endl;
  fCounters = 0;
  fGlobalMode = gMode;
  
  fADCUnitToCharge = cfg->GetParOrDefault("RECO","ADCUnitToCharge",300.); // electrons / adccount
  fAPVTimeBin      = cfg->GetParOrDefault("RECO","APVTimeBin",25.);       // ns
  fThresholdTruncatedMean = cfg->GetParOrDefault("RECO","ThresholdTruncatedMean",0.2);       // ns
  fTimeTau = cfg->GetParOrDefault("RECO","TimeTau",50.);       // ns
  fTimeWindowSamplesLow = cfg->GetParOrDefault("RECO","TimeWindowSamplesLow",7);  // number of samples
  fTimeWindowSamplesHigh = cfg->GetParOrDefault("RECO","TimeWindowSamplesHigh",9);  // number of samples
  fHitChargeThreshold = cfg->GetParOrDefault("RECO","HitChargeThreshold",100);  // ADC counts
  fHitChargeSmoothnessThreshold = cfg->GetParOrDefault("RECO","HitChargeSmoothnessThreshold",200);  // ADC counts
  fHitTimeLow  = cfg->GetParOrDefault("RECO","HitTimeLow",-250); // ns
  fHitTimeHigh = cfg->GetParOrDefault("RECO","HitTimeHigh",800); // ns
  fHitTimeMinimum = cfg->GetParOrDefault("RECO","HitTimeMinimum",20); // ns
  fHitChargeMaxForPChi2Cut = cfg->GetParOrDefault("Reco","HitChargeMaxForPChi2Cut",600); // ADC counts
  fHitPChi2Cut = cfg->GetParOrDefault("Reco","HitPChi2Cut",0.05); // prob
  
  //  fClusterDeltaCellMax = cfg->GetParOrDefault("RECOCLUSTER","ClusterDelta1CellMax",2); // 
  

  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();

  fSignalShape = new TF1("SignalShape",DigitizerChannelMM::fitSignalShape,-200,800,4);
  fSignalShape->SetParName(0,"MMSignalBaseLine") ; 
  //  fSignalShape->FixParameter(0,0.);
  fSignalShape->SetParName(1,"MMSignalAmplitude"); 
  fSignalShape->SetParName(2,"MMSignalTimeMax")  ; 
  fSignalShape->SetParName(3,"MMSignalTimeWidth");
  fSignalShape->FixParameter(3,fTimeTau);

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
  Short_t sampleMaxId = 0;
  for (Short_t i=0; i<fNSamples; i++){
    charge += fSamples[i];
    if (fSamples[i] > chargeMax){
      sampleMaxId = i;
      chargeMax = fSamples[i];
    }
  }

//  if (sampleMaxId == fNSamples-1) return; //  do not produce reco hits when the max is at the edge
//  if (sampleMaxId == 0          ) return; //  do not produce reco hits when the max is at the edge

  if (chargeMax < fHitChargeThreshold) return;

  
  if (sampleMaxId == -1) {
    std::cout << "DigitizerChannelMM >> wrong sampleMaxId " << std::endl;
  }

  // quality cut for signal smoothness around the maximum

  double deltaq=0;
  if(sampleMaxId == 0)                deltaq = chargeMax - fSamples[sampleMaxId+1];
  else if(sampleMaxId == fNSamples-1) deltaq = chargeMax - fSamples[sampleMaxId-1];
  else deltaq = chargeMax - TMath::Min(fSamples[sampleMaxId-1],fSamples[sampleMaxId+1]);
  if(deltaq > fHitChargeSmoothnessThreshold) return;

  // fit with signal shape

  double sigAmplitude = 0;
  double sigTimePeak = 0;
  bool isFitOK = evaluateSig(sampleMaxId,fSamples[sampleMaxId],&sigAmplitude, &sigTimePeak);
  if (!isFitOK) return;
  
  // truncated
  
  double tmean = 0;
  double chargeTrunk = 0;
  for (Short_t i=0; i<fNSamples; i++){
    if (fSamples[i] < charge*fThresholdTruncatedMean/fNSamples) continue;
    chargeTrunk += fSamples[i];
    tmean += fSamples[i]*i;
  }
  if (chargeTrunk > 0) tmean /= chargeTrunk;

//  std::cout << "bdid " << bdid << " chid " << chid << " chargemax = " << chargeMax << " samplemax = " << sampleMaxId << " trunk " << tmean << " twindow " << twindow << std::endl;
//  std::cout << "q=";
//  for (UShort_t i=0; i<fNSamples; i++){std::cout << " " << fSamples[i];}
//  std::cout << std::endl;

  
  
  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetChannelId(channelid);      // will be used to determine the geometrical position by the MMGeometry method ComputePositions using GlobalPosition(ich)
  Hit->SetTime(sigTimePeak); // ns
  Hit->SetEnergy(sigAmplitude); // electrons
  //  Hit->SetTime(tmean*fAPVTimeBin);   // ns
  //  Hit->SetTime(sampleMaxId*fAPVTimeBin);   // ns
  //  Hit->SetTime(twindow*fAPVTimeBin);   // ns
  //  Hit->SetEnergy(charge*fADCUnitToCharge); // in electrons
  Hit->setStatus(ch->IsChannelFailed());
  hitArray.push_back(Hit);

}

void DigitizerChannelMM::PrintConfig(){
  ;
}

bool DigitizerChannelMM::evaluateSig(Short_t sampleMaxId, Short_t maxQ, double* sigAmplitude, double* sigTimePeak){

//  std::cout << "original points " << std::endl;
//  for (int i=0; i<fNSamples ; i++){
//    std::cout << "point " << i << " x = " << i << " y = " << fSamples[i] << std::endl;
//  }
  
  TGraphErrors* grafoSignal = new TGraphErrors();
  grafoSignal->SetName("graphChargeVsTime");

  
  Double_t chargewindow = 0; // total charge inside the window considered
  Double_t twindow = 0; // first order estimate of the max  
  for (Short_t i=TMath::Max(0,sampleMaxId-fTimeWindowSamplesLow); i<TMath::Min((Short_t)(sampleMaxId+fTimeWindowSamplesHigh),fNSamples); i++){
    int grafopts = grafoSignal->GetN();
    grafoSignal->SetPoint(grafopts,i*fAPVTimeBin,fSamples[i]);
    grafoSignal->SetPointError(grafopts,0,40+fSamples[i]*0.05); // euristic error. The charge is 11 bits -> 0.5 per mil.
    chargewindow += fSamples[i];
    twindow += fSamples[i]*i;
  }
  twindow /= chargewindow;
  
//  fSignalShape->SetParameter(1,maxQ*fADCUnitToCharge); // electrons
//  fSignalShape->SetParameter(2,twindow*fAPVTimeBin - fTimeTau); // ns

  fSignalShape->SetParameter(0,0.); // automatic pedestal suppression [electrons]
  fSignalShape->SetParameter(1,maxQ/4);  // [/4 because it would have been /(27*Exp(-2)) ~ 3.65]
  //fSignalShape->SetParameter(2,twindow*clock - fTimeTau); // ns
  fSignalShape->SetParameter(2,sampleMaxId*fAPVTimeBin-3*fTimeTau); // ns [was 170 WHY?! ] 
  fSignalShape->SetParameter(3,fTimeTau); // ns
  fSignalShape->SetParLimits(2,fHitTimeLow,fHitTimeHigh); //ns
  fSignalShape->FixParameter(0,0.);
  fSignalShape->FixParameter(3,fTimeTau); // ns
  
  TFitResultPtr fitres = grafoSignal->Fit(fSignalShape,"SQ");
  
//  if (fCounters < 100) {
//    fitres->Print();
//    std::cout << "fitres " << fitres << " prob = " << fitres->Prob() << std::endl;
//    for (uint i=0; i<4; i++){
//      std::cout << "====== Parameter " << i << " = " << fSignalShape->GetParameter(i) << " +- " << fSignalShape->GetParError(i) << std::endl;
//    }
//  }
  //  if (fCounters < 20) grafoSignal->Write(Form("%s_%d.C",grafoSignal->GetName(),fCounters));
  
  *sigAmplitude = fSignalShape->GetParameter(1)*27*TMath::Exp(-2);
  *sigTimePeak = fSignalShape->GetParameter(2) + 3*fTimeTau; // time of the max of the function -> we might use fSignalShape->GetMaximumX() to be independent of the functional form
  double chi2 = fSignalShape->GetChisquare();
  double ndf = fSignalShape->GetNDF();
  double p_chi2 = 1.0 - ROOT::Math::chisquared_cdf(chi2, ndf, 0);
  //  q_int  = fSignalShape->Integral(-200,800);
 
  if (*sigTimePeak < fHitTimeLow+fHitTimeMinimum) return kFALSE;
  if (maxQ < fHitChargeMaxForPChi2Cut && p_chi2 < fHitPChi2Cut) return kFALSE;
  
  delete grafoSignal;  
  fCounters++;
  return kTRUE;
  //  return fitres;
}

Double_t DigitizerChannelMM::fitSignalShape(Double_t *x, Double_t *par) {    
  double xprime = (x[0]-par[2])/par[3];
  double funval = TMath::E()*par[1]*xprime*xprime*xprime*TMath::Exp(-xprime);
  // dfunval/dxprime = 3x^2 -x^3 = 0 when x = 3 -> t_max = par2 + 50ns x 3, t_flex = par2 + 50ns x 2
  // max value = par1 x 27 x Exp(-2)
  if (funval < 0) funval = 0;
  return par[0] + funval;
}
