// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------

#include "TSACMCDigi.hh"

ClassImp(TSACMCDigi)

TSACMCDigi::TSACMCDigi() : TMCVDigi()
{;}

TSACMCDigi::~TSACMCDigi()
{;}

void TSACMCDigi::ResetTHisto()
{
  for(Int_t i=0; i<TSACMCDIGI_N_BINS; i++) fTHisto[i] = 0.;
}
