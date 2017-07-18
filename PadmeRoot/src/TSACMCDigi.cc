// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------

#include "TSACMCDigi.hh"

#include "Riostream.h"

ClassImp(TSACMCDigi)

TSACMCDigi::TSACMCDigi() : TMCVDigi()
{
  ResetTHisto();
}

TSACMCDigi::~TSACMCDigi()
{;}

void TSACMCDigi::ResetTHisto()
{
  for (Int_t i=0; i<TSACMCDIGI_N_BINS; i++) fTHisto[i] = 0.;
}

void TSACMCDigi::Print(Option_t *) const {

  std::cout << "SAC Digi - ChannelId = " << fChannelId
	    << " Energy = " << fEnergy
	    << " Time = " << fTime
	    << std::endl;

  Int_t was0 = 0;
  for (Int_t i=0; i<TSACMCDIGI_N_BINS; i++) {
    Double_t t = fTHistoStart+fTHistoStep*i;
    if (fTHisto[i]==0.) {
      if (!was0) std::cout << t;
      was0 = 1;
    } else {
      if (was0) std::cout << "\t" << t << "\t-" << std::endl;
      std::cout << t << "\t" << t+fTHistoStep << "\t" << fTHisto[i] << std::endl;
      was0 = 0;
    }
  }
  if (was0) std::cout << "\t" << fTHistoStart+fTHistoStep*TSACMCDIGI_N_BINS << "\t-" << std::endl;

}
