// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------

#include "TMCVertex.hh"

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
  printf("Process %s Time %8.3f ns Position (%8.3f,%8.3f,%8.3f) mm Particles in %d out %d\n",fProcess.Data(),fTime,fPosition.X(),fPosition.Y(),fPosition.Z(),fNPartIn,fNPartOut);
  if (fPartIn)
    for (Int_t i=0; i<fNPartIn; i++) {
      printf("\tParticle IN  %2d ",i);
      ((TMCParticle*)fPartIn->At(i))->Print(option);
    }
  if (fPartOut)
    for (Int_t i=0; i<fNPartOut; i++) {
      printf("\tParticle OUT %2d ",i);
      ((TMCParticle*)fPartOut->At(i))->Print(option);
    }
}
