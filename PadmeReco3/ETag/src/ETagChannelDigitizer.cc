#include "Riostream.h"

#include "TRawEvent.hh"

#include "TMath.h"
#include "TSpline.h"

#include "PadmeVRecoConfig.hh"
#include "TrigTimeSvc.hh"

#include "ETagHit.hh"

#include "ETagChannelDigitizer.hh"

ETagChannelDigitizer::ETagChannelDigitizer(PadmeVRecoConfig* cfg)
  : fETagConfig(cfg)
{

  // Define verbose level
  fVerbose = fETagConfig->GetParOrDefault("Output","Verbose",0);

  // Attach to Trigger Time service
  fTrigTimeSvc = TrigTimeSvc::GetInstance();

  // Define parameters to use for hit building process
  fGoodSamples = fETagConfig->GetParOrDefault("RECO","GoodSamples",1000);
  fPedestalSamples = fETagConfig->GetParOrDefault("RECO","PedestalSamples",100);
  fZeroSupRMSThreshold = fETagConfig->GetParOrDefault("RECO","ZeroSupRMSThreshold",10.);
  fHitThreshold = fETagConfig->GetParOrDefault("RECO","HitThreshold",10.);

  if (fVerbose>1) {
    printf("ETagChannelDigitizer::ETagChannelDigitizer - digitization parameters set to:\n");
    printf("\tGoodSamples\t\t%d\n",fGoodSamples);
    printf("\tPedestalSamples\t\t%d\n",fPedestalSamples);
    printf("\tZeroSupRMSThreshold\t%.1f\n",fZeroSupRMSThreshold);
  }

  if (fVerbose) printf("ETagChannelDigitizer::ETagChannelDigitizer - ETag channel digitization system created\n");
}

ETagChannelDigitizer::~ETagChannelDigitizer()
{
  if (fVerbose) printf("ETagChannelDigitizer::~ETagChannelDigitizer - Deleting ETag channel digitization system\n");
}

void ETagChannelDigitizer::Init()
{
  if (fVerbose) printf("ETagChannelDigitizer::Init - Initializing ETag channel digitizer\n");
}

Bool_t ETagChannelDigitizer::DigitizeChannel(UChar_t adcBoard,UChar_t adcChannel,Int_t channel,Short_t* samples,vector<ETagHit*>& hits)
{

  if (fVerbose>3) printf("ETagChannelDigitizer::DigitizeChannel - Digitizing channel %d\n",channel);
  if (fVerbose>4) {
    printf("Samples");
    for(UInt_t i=0; i<1024; i++) printf(" %5d",samples[i]);
    printf("\n");
  }

  // Convert samples to double
  Double_t samplesD[1024];
  for(UInt_t i=0; i<1024; i++) samplesD[i] = (Double_t)samples[i];

  // Get RMS of "good" samples and use it to reject empty waveforms (poor man's zero suppression)
  Double_t rms = TMath::RMS(fGoodSamples,samplesD);
  //printf("RMS %f\n",rms);
  if (rms<fZeroSupRMSThreshold) return true;

  // Compute pedestal
  Double_t pedestal = TMath::Mean(fPedestalSamples,samplesD);
  //printf("Pedestal %f\n",pedestal);

  // Prepare vectors for spline interpolation
  Double_t xbinD[1024];
  Double_t ybinD[1024];
  Double_t maxValue = 0.;
  //UInt_t   maxSample = 0;
  for(Int_t i=0; i<1024; i++) {
    xbinD[i] = (Double_t)i;
    ybinD[i] = -(samplesD[i]-pedestal)/4096.*1000.; // Subtract pedestal and convert to positive signal in mV
    if (i<fGoodSamples) {
      if (ybinD[i]>maxValue) {
	maxValue = ybinD[i];
	//maxSample = i;
      }
    }
  }
  
  // Use quintic natural spline for interpolation of "good" samples (avoid problematic samples at the end)
  TSpline5 spline = TSpline5("wsp",xbinD,ybinD,fGoodSamples);

  // Scan the spline to find where hit threshold is crossed
  Double_t oldVal = 0.;
  for(Double_t i=0.; i<(Double_t)fGoodSamples; i++) {
    Double_t val = spline.Eval(i);
    //printf("%f %f\n",i,val);
    if (oldVal<fHitThreshold && val>=fHitThreshold) {

      // Linear interpolation to find precise crossing position
      Double_t hitPos = i-(val-fHitThreshold)/(val-oldVal);
      // Convert sample position to trigger-corrected time
      Double_t hitTime = fTrigTimeSvc->GetTimeDifference(adcBoard,adcChannel/8,hitPos);
      //printf("Hit pos %f time %f\n",hitPos,hitTime);

      // Save hit (still need to compute energy, position and status)
      ETagHit* hit = new ETagHit();
      hit->SetADCBoard(adcBoard);
      hit->SetADCChannel(adcChannel);
      hit->SetChannelId(channel);
      hit->SetStatus(0x0000000000000000);
      hit->SetPosition(TVector3(0.,0.,0.));
      hit->SetEnergy(0.);
      hit->SetTime(hitTime);
      hits.push_back(hit);
      if (fVerbose>3) hit->Print();

    }
    oldVal = val;
  }

  return true;

}

Bool_t ETagChannelDigitizer::DigitizeCard(Int_t card,Short_t* samples,vector<ETagHit*>& hits)
{
  if (fVerbose>3) printf("ETagChannelDigitizer::DigitzeCard - Digitizing card %d\n",card);
  if (fVerbose>4) {
    printf("Samples");
    for(UInt_t i=0; i<1024; i++) printf(" %5d",samples[i]);
    printf("\n");
  }
  return true;
}
