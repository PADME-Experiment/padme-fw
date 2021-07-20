// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------

#include "TMCParticle.hh"

ClassImp(TMCParticle)

TMCParticle::TMCParticle()
{
  fPDGCode = 0;
  fE = 0.;
  fPx = 0.;
  fPy = 0.;
  fPz = 0.;
}

TMCParticle::~TMCParticle()
{;}

void TMCParticle::Clear(Option_t* option)
{
  fPDGCode = 0;
  fE = 0.;
  fPx = 0.;
  fPy = 0.;
  fPz = 0.;
}

void TMCParticle::Print(Option_t* option) const
{
  printf("PDGCode %5d Energy %8.3f MeV Momentum (%8.3f,%8.3f,%8.3f) MeV\n",fPDGCode,fE,fPx,fPy,fPz);
}
