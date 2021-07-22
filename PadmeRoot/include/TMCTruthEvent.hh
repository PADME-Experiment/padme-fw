// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------
#ifndef TMCTruthEvent_H
#define TMCTruthEvent_H

#include "TObject.h"
#include "TClonesArray.h"

#include "TMCVertex.hh"

class TMCTruthEvent : public TObject
{

public:

  TMCTruthEvent();
  ~TMCTruthEvent();

  Int_t GetRunNumber() { return fRunNumber;  };
  void SetRunNumber(Int_t value) { fRunNumber = value; };

  Int_t GetEventNumber() { return fEventNumber;  };
  void SetEventNumber(Int_t value) { fEventNumber = value; };

  Double_t GetEventWeight() { return fEventWeight;  };
  void SetEventWeight(Double_t value) { fEventWeight = value; };

  Int_t GetNVertices() { return fNVertex; }; // Return number of vertexes in array
  TMCVertex* AddVertex();                    // Append a new (empty) vertex to array
  TMCVertex* Vertex(Int_t);                  // Return pointer to vertex at given index in array

  void Print(Option_t* = "") const; // Print MCTruth info to stdout

  void Clear(Option_t* = ""); // Clear vertices array

private:

  Int_t fRunNumber;
  Int_t fEventNumber;
 
  Double_t fEventWeight;

  Int_t fNVertex;
  TClonesArray* fVertex;

  ClassDef(TMCTruthEvent,1);
};
#endif
