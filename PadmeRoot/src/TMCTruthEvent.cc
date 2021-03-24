// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------

#include "TMCTruthEvent.hh"

#include "TMCVertex.hh"

ClassImp(TMCTruthEvent)

TMCTruthEvent::TMCTruthEvent()
{
  fRunNumber   = -1;
  fEventNumber = -1;

  fNVertex = 0;
  fVertex  = new TClonesArray(TMCVertex::Class(),1000);
}

TMCTruthEvent::~TMCTruthEvent()
{
  if (fVertex) delete fVertex;
}

void TMCTruthEvent::Clear(Option_t * option)
{
  fNVertex = 0;
  if(fVertex) fVertex->Clear(option);
}

TMCVertex* TMCTruthEvent::AddVertex()
{
  return (TMCVertex*)(fVertex->New(fNVertex++));
}

TMCVertex* TMCTruthEvent::Vertex(Int_t iVtx)
{
  return (TMCVertex*)fVertex->At(iVtx);
}
