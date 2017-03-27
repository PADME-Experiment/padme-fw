// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#include "TSACMCEvent.hh"

#include "TSACMCHit.hh"
#include "TSACMCDigi.hh"

ClassImp(TSACMCEvent)

TSACMCEvent::TSACMCEvent() : TMCVEvent(TSACMCHit::Class(),TSACMCDigi::Class())
{;}

TSACMCEvent::~TSACMCEvent()
{;}

TSACMCHit* TSACMCEvent::GetOrCreateHit(Int_t chid)
{

  // Return pointer to hit for given channel id
  for (Int_t iHit = 0; iHit<fNHits; iHit++)
    if ( ((TSACMCHit*)fHits->At(iHit))->GetChannelId() == chid )
      return (TSACMCHit*)fHits->At(iHit);

  // Channel id was not found: create new hit
  TSACMCHit* newHit = (TSACMCHit*)AddHit();
  newHit->SetChannelId(chid);
  return newHit;

}
