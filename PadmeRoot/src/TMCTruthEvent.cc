// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------

#include "TMCTruthEvent.hh"

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

void TMCTruthEvent::Print(Option_t* option) const
{
  printf("MCTruthEvent - Run %d Event %d Weight %8.3f Vertices %d\n",fRunNumber,fEventNumber,fEventWeight,fNVertex);
  if (fVertex)
    for(Int_t i=0; i<fNVertex; i++) {
      printf("\tVertex %2d ",i);
      ((TMCVertex*)fVertex->At(i))->Print(option);
    }
}
