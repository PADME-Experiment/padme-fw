#include "MultiHit.hh"

#include <vector>
#include <string>

MultiHit::MultiHit(TString cfg)
{

  fConfigParser = new utl::ConfigParser((const std::string)cfg);

  fSmoothLen = 20;
  fDerivativeLen = 10;
  fDerivativeThreshold = .01; // Just a guess
  fMaxNHits = MULTIHIT_MAXHITS;

  fNHits = 0;

  // Initialize template to null values
  fTempIMax = 0;
  fTempMaxValue = 0.;
  fTempIntegral = 0.;
  for (Int_t i=0; i<MULTIHIT_NSAMPLES; i++) fTempWF[i] = 0.;

  ReadTemplate();

}

MultiHit::~MultiHit()
{
  if (fConfigParser) delete fConfigParser;
}

Int_t MultiHit::ReadTemplate()
{
  utl::ConfigParser *cP = fConfigParser;
  if (cP->HasConfig("TEMPLATE","Integral")) fTempIntegral = std::stof(cP->GetSingleArg("TEMPLATE","Integral"));
  if (cP->HasConfig("TEMPLATE","MaxValue")) fTempMaxValue = std::stof(cP->GetSingleArg("TEMPLATE","MaxValue"));
  if (cP->HasConfig("TEMPLATE","BinMax"))   fTempIMax = std::stoi(cP->GetSingleArg("TEMPLATE","BinMax"));
  if (cP->HasConfig("TEMPLATE","Waveform")) {
    std::vector<std::string> wf = cP->GetConfig("TEMPLATE","Waveform");
    Int_t i = 0;
    for(auto it = wf.begin(); it != wf.end(); ++it) {
      fTempWF[i] = std::stof(*it);
      i++;
    }
    if (i!=MULTIHIT_NSAMPLES) {
      printf("MultiHit::ReadTemplate ERROR - Template waveform has %d samples (%d expected)\n",i,MULTIHIT_NSAMPLES);
      return -1;
    }
  }
  return 0;
}

Int_t MultiHit::SetWaveform(UShort_t nSamples, Double_t* samples)
{

  if (nSamples != MULTIHIT_NSAMPLES) {
    printf("MultiHit::SetWaveform ERROR - Waveform has %d samples (%d expected)\n",nSamples,MULTIHIT_NSAMPLES);
    return -1;
  }

  for (Int_t i=0; i<nSamples; i++) {
    fWorkWF[i] = samples[i];
  }

  // By default waveform is NOT saturated
  // User must set it with WaveformIsSaturated() before calling FindHits()
  fWorkIsSaturated = false;

  // Smooth waveform
  SmoothWF();

  return 0;

}

void MultiHit::SmoothWF() {

  Double_t tmpWF[MULTIHIT_NSAMPLES];

  // Define smoothing interval [-iS;iE]. E.g. Even: 20 -> [-10;9] - Odd: 21 -> [-10;10]
  //Int_t iS = fSmoothLen/2;
  Int_t iE = fSmoothLen/2-1+(fSmoothLen%2);
  UInt_t nsum = 0;
  Double_t sum = 0.;
  for (Int_t i=0; i<MULTIHIT_NSAMPLES+iE; i++) {
    if (i<iE) {
      sum += fWorkWF[i];
      nsum++;
    } else if (i<=MULTIHIT_NSAMPLES) {
      sum = sum-fWorkWF[i-fSmoothLen-1]+fWorkWF[i];
    } else {
      sum -= -fWorkWF[i-fSmoothLen-1];
      nsum--;
    }
    if (i>=iE) {
      tmpWF[i-iE] = sum/(Double_t)nsum;
    }
  }
  for(Int_t i=0; i<MULTIHIT_NSAMPLES+iE; i++) fWorkWF[i] = tmpWF[i];

}

Int_t MultiHit::FindMax()
{

  // Find maximum of derivative
  fWorkIMax = 0;
  Double_t dmax = 0.;
  for (UInt_t i=fDerivativeLen;i<MULTIHIT_NSAMPLES;i++) {
    Double_t dy = fWorkWF[i]-fWorkWF[i-fDerivativeLen];
    if (dy>dmax) {
      dmax = dy;
      fWorkIMax = i;
    }
  }

  // If maximum is above threshold, we found a hit
  if (dmax > fDerivativeThreshold) {
    fWorkMaxValue = fWorkWF[fWorkIMax];
    return 1;
  }

  return 0; // No good maximum was found

}

Int_t MultiHit::SubtractTemplate()
{
  Double_t scale = fWorkMaxValue/fTempMaxValue;
  Int_t di = fTempIMax-fWorkIMax;
  for (UInt_t i=0;i<MULTIHIT_NSAMPLES;i++) {
    Int_t itemp = i+di;
    if (itemp>=0) { fWorkWF[i] -= fTempWF[itemp]*scale; }
  }
  return 0;
}

Int_t MultiHit::FindHits()
{

  fNHits = 0;
  while( (fNHits<fMaxNHits) && (FindMax()==1) ) {

    // Save new hit
    fHitBin[fNHits] = fWorkIMax;
    fHitIntegral[fNHits] = fTempIntegral/fTempMaxValue*fWorkMaxValue;
    fNHits++;

    // Subtract template
    SubtractTemplate();

  }
  return fNHits;

}

UInt_t MultiHit::GetHitBin(UInt_t nh)
{
  if (nh >= fNHits) {
    printf("MultiHit::GetHitBin ERROR - Requested hit index %d with only %d hits avaliable\n",nh,fNHits);
    return 0.;
  }
  return fHitBin[nh];
}

Double_t MultiHit::GetHitIntegral(UInt_t nh)
{
  if (nh >= fNHits) {
    printf("MultiHit::GetHitIntegral ERROR - Requested hit index %d with only %d hits avaliable\n",nh,fNHits);
    return 0.;
  }
  return fHitIntegral[nh];
}
