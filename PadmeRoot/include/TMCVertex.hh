// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------
#ifndef TMCVertex_H
#define TMCVertex_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"

#include "TMCParticle.hh"

class TMCVertex : public TObject
{

public:

  TMCVertex();
  ~TMCVertex();

  TString  GetProcess()  const { return fProcess;  };
  TVector3 GetPosition() const { return fPosition; };
  Double_t GetTime()     const { return fTime;     };

  void  SetProcess (TString value)  { fProcess = value;  };
  void  SetPosition(TVector3 value) { fPosition = value; };
  void  SetTime    (Double_t value) { fTime = value;     };

  Int_t GetNParticleIn() { return fNPartIn; }; // Return number of incoming particles
  TMCParticle* AddParticleIn();                // Append a new (empty) incoming particle
  TMCParticle* ParticleIn(Int_t);              // Return pointer to incoming particle at given index in array

  Int_t GetNParticleOut() { return fNPartOut; }; // Return number of outgoing particles
  TMCParticle* AddParticleOut();                 // Append a new (empty) outgoing particle
  TMCParticle* ParticleOut(Int_t);               // Return pointer to outgoing particle at given index in array

  void Print(Option_t* = "") const; // Print vertex to output

  void Clear(Option_t* = ""); // Clear particle arrays

private:

  TString  fProcess;
  TVector3 fPosition;
  Double_t fTime;

  Int_t fNPartIn;
  TClonesArray* fPartIn;

  Int_t fNPartOut;
  TClonesArray* fPartOut;

  ClassDef(TMCVertex,1);
};
#endif
