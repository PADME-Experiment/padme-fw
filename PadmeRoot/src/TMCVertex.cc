// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------

#include "TMCVertex.hh"

#include "TMCParticle.hh"

ClassImp(TMCVertex)

TMCVertex::TMCVertex()
{
  fProcess = "";
  fPosition = TVector3();
  fTime = -1.;

  fNPartIn = 0;
  fPartIn  = new TClonesArray(TMCParticle::Class(),1000);

  fNPartOut = 0;
  fPartOut  = new TClonesArray(TMCParticle::Class(),1000);
}

TMCVertex::~TMCVertex()
{
  if (fPartIn)  delete fPartIn;
  if (fPartOut) delete fPartOut;
}

void TMCVertex::Clear(Option_t * option)
{
  fProcess = "";
  fPosition = TVector3();
  fTime = -1.;

  fNPartIn = 0;
  if (fPartIn) fPartIn->Clear(option);

  fNPartOut = 0;
  if (fPartOut) fPartOut->Clear(option);
}

TMCParticle* TMCVertex::AddParticleIn()
{
  return (TMCParticle*)(fPartIn->New(fNPartIn++));
}

TMCParticle* TMCVertex::ParticleIn(Int_t iPar)
{
  return (TMCParticle*)fPartIn->At(iPar);
}

TMCParticle* TMCVertex::AddParticleOut()
{
  return (TMCParticle*)(fPartOut->New(fNPartOut++));
}

TMCParticle* TMCVertex::ParticleOut(Int_t iPar)
{
  return (TMCParticle*)fPartOut->At(iPar);
}

void TMCVertex::Print(Option_t* option) const
{
  if (fPartIn)  fPartIn->Print(option);
  if (fPartOut) fPartOut->Print(option);
}
