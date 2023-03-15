#include "Riostream.h"

#include "TRawEvent.hh"

#include "TMath.h"
#include "TH1D.h"

#include "PadmeVRecoConfig.hh"
#include "TrigTimeSvc.hh"
#include "HistoSvc.hh"

#include "ECalHit.hh"

#include "ECalChannelDigitizer.hh"

ECalChannelDigitizer::ECalChannelDigitizer(PadmeVRecoConfig* cfg)
  : fECalConfig(cfg)
{

  // Define verbose level
  fVerbose = fECalConfig->GetParOrDefault("Output","Verbose",0);

  // Attach to Trigger Time service
  fTrigTimeSvc = TrigTimeSvc::GetInstance();

  // Attach to Histogrammin service
  fHistoSvc = HistoSvc::GetInstance();

  // Define parameters to use for hit building process
  fGoodSamples = fECalConfig->GetParOrDefault("RECO","GoodSamples",1000);
  fPedestalSamples = fECalConfig->GetParOrDefault("RECO","PedestalSamples",100);
  fZeroSupRMSThreshold = fECalConfig->GetParOrDefault("RECO","ZeroSupRMSThreshold",10.);

  if (fVerbose>1) {
    printf("ECalChannelDigitizer::ECalChannelDigitizer - digitization parameters set to:\n");
    printf("\tGoodSamples\t\t%d\n",fGoodSamples);
    printf("\tPedestalSamples\t\t%d\n",fPedestalSamples);
    printf("\tZeroSupRMSThreshold\t%.1f\n",fZeroSupRMSThreshold);
  }

  if (fVerbose) printf("ECalChannelDigitizer::ECalChannelDigitizer - ECal channel digitization system created\n");
}

ECalChannelDigitizer::~ECalChannelDigitizer()
{
  if (fVerbose) printf("ECalChannelDigitizer::~ECalChannelDigitizer - Deleting ECal channel digitization system\n");
}

void ECalChannelDigitizer::Init()
{
  if (fVerbose) printf("ECalChannelDigitizer::Init - Initializing ECal channel digitizer\n");
}

Bool_t ECalChannelDigitizer::DigitizeChannel(UChar_t adcBoard,UChar_t adcChannel,Int_t channel,Short_t* samples,vector<ECalHit*>& hits)
{

  if (fVerbose>3) printf("ECalChannelDigitizer::DigitizeChannel - Digitizing channel %d\n",channel);
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

  return true;

}
