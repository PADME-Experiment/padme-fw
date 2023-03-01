#include "Riostream.h"

#include "TRawEvent.hh"

#include "PadmeVRecoConfig.hh"

#include "ETagHit.hh"

#include "ETagChannelDigitizer.hh"

ETagChannelDigitizer::ETagChannelDigitizer(PadmeVRecoConfig* cfg)
  : fETagConfig(cfg)
{

  // Define verbose level
  fVerbose = fETagConfig->GetParOrDefault("Output","Verbose",0);

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

Bool_t ETagChannelDigitizer::DigitizeChannel(UChar_t adcBoard, UChar_t adcChannel, Int_t channel,Short_t* samples,vector<ETagHit*>& hits)
{

  if (fVerbose>3) printf("ETagChannelDigitizer::DigitizeChannel - Digitizing channel %d\n",channel);
  if (fVerbose>4) {
    printf("Samples");
    for(UInt_t i=0; i<1024; i++) printf(" %5d",samples[i]);
    printf("\n");
  }

  ETagHit* hit = new ETagHit();
  hit->SetADCBoard(adcBoard);
  hit->SetADCChannel(adcChannel);
  hit->SetChannelId(channel);
  hit->SetStatus(0x0000000000000003);
  hit->SetPosition(TVector3(1.,1.,2.));
  hit->SetEnergy(324.3);
  hit->SetTime(23456.456);
  hits.push_back(hit);
  if (fVerbose>3) hit->Print();

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
