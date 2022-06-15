// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------
#ifndef TMCParticle_H
#define TMCParticle_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"

class TMCParticle : public TObject
{

public:

  TMCParticle();
  ~TMCParticle();

  Int_t GetPDGCode() const { return fPDGCode; };
  void SetPDGCode(Int_t v) { fPDGCode = v; };

  Double_t GetEnergy() const { return fE; };
  void  SetEnergy(Double_t v) { fE = v; };

  TVector3 GetMomentum() const { return TVector3(fPx,fPy,fPz); };
  void  SetMomentum(TVector3 v) { fPx = v.X(); fPy = v.Y(); fPz = v.Z(); };

  Double_t GetMomentumX() const { return fPx; };
  void  SetMomentumX(Double_t v) { fPx = v; };

  Double_t GetMomentumY() const { return fPy; };
  void  SetMomentumY(Double_t v) { fPy = v; };

  Double_t GetMomentumZ() const { return fPz; };
  void  SetMomentumZ(Double_t v) { fPz = v; };

  void Print(Option_t* option="") const; // Print particle to output

  void Clear(Option_t* = ""); // Clear particle

private:

  Int_t fPDGCode; // PDG code of the particle

  Double_t fPx; // x component of momentum
  Double_t fPy; // y component of momentum
  Double_t fPz; // z component of momentum
  Double_t fE;  // Energy

  ClassDef(TMCParticle,1);
};
#endif
