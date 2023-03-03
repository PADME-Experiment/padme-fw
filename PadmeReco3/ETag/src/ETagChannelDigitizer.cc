#include "Riostream.h"

#include "TRawEvent.hh"

#include "TMath.h"
#include "TH1D.h"

#include "PadmeVRecoConfig.hh"
#include "TrigTimeSvc.hh"
#include "HistoSvc.hh"

#include "ETagHit.hh"

#include "ETagChannelDigitizer.hh"

ETagChannelDigitizer::ETagChannelDigitizer(PadmeVRecoConfig* cfg)
  : fETagConfig(cfg)
{

  // Define verbose level
  fVerbose = fETagConfig->GetParOrDefault("Output","Verbose",0);

  // Attach to Trigger Time service
  fTrigTimeSvc = TrigTimeSvc::GetInstance();

  // Attach to Histogrammin service
  fHistoSvc = HistoSvc::GetInstance();

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

  // Subtract pedestal and convert samples to positive signal in mV
  Double_t ybinD[1024];
  Double_t maxValue = 0.;
  //UInt_t   maxSample = 0;
  for(Int_t i=0; i<1024; i++) {
    ybinD[i] = -(samplesD[i]-pedestal)/4096.*1000.;
    if (i<fGoodSamples) {
      if (ybinD[i]>maxValue) {
	maxValue = ybinD[i];
	//maxSample = i;
      }
    }
  }
  
  // Smooth the waveform by averaging over N consecutive points
  Double_t yavgD[1024];
  Int_t navg = 3; // Number of samples before and after each bin to use in average
  for(Int_t i=0;i<1024;i++) {
    yavgD[i] = 0.;
    Int_t iavg = 0;
    for(Int_t j=std::max(0,i-navg); j<std::min(1024,i+navg+1); j++) {
      yavgD[i] += ybinD[j];
      iavg++;
    }
    yavgD[i] = yavgD[i]/(Double_t)iavg;
  }

  /*
  // Save inital (w) and smoothed (s) waveforms to histogram for debug
  char buffer[100];
  std::string hname,htitle;
  sprintf(buffer,"ETag_%d_%d_%d_w",fRunNumber,fEventNumber,channel);
  hname = buffer;
  sprintf(buffer,"ETag waveform R%d E%d C%d",fRunNumber,fEventNumber,channel);
  htitle = buffer;
  TH1D* hw = fHistoSvc->BookHisto("ETag",hname,htitle,fGoodSamples,0.,(Double_t)fGoodSamples);
  sprintf(buffer,"ETag_%d_%d_%d_s",fRunNumber,fEventNumber,channel);
  hname = buffer;
  sprintf(buffer,"ETag spline R%d E%d C%d",fRunNumber,fEventNumber,channel);
  htitle = buffer;
  TH1D* hs = fHistoSvc->BookHisto("ETag",hname,htitle,fGoodSamples,0.,(Double_t)fGoodSamples);
  for(Int_t i=0; i<fGoodSamples; i++) {
    hw->SetBinContent(i,ybinD[i]);
    hs->SetBinContent(i,yavgD[i]);
  }
  */

  // Scan the smoothed waveform to find where hit threshold is crossed
  Double_t oldVal = 0.;
  for(Int_t i=0.; i<fGoodSamples; i++) {
    Double_t val = yavgD[i];
    if (oldVal<fHitThreshold && val>=fHitThreshold) {

      // Linear interpolation to find precise crossing position
      Double_t hitPos = (Double_t)i-(val-fHitThreshold)/(val-oldVal);
      // Convert sample position to trigger-corrected time
      Double_t hitTime = fTrigTimeSvc->GetTimeDifference(adcBoard,adcChannel/8,hitPos);

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
