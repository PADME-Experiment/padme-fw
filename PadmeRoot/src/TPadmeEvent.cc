// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (Emanuele Leonardi@cern.ch) 2016-03-15
//
// --------------------------------------------------------------
#include <stdlib.h>
#include "Riostream.h"
#include "TPadmeEvent.hh"

ClassImp(TPadmeEvent)

//TRandom3* TPadmeEvent::fgRandomDecayState = 0;

TPadmeEvent::TPadmeEvent()
{
  fRunNumber = 0;
  fEventNumber = 0;
  fTime = 0.;

  //fGeneParts = new TClonesArray("GenePart", 100);
  //fNGeneParts = 0;

  //fKineParts = new TClonesArray("KinePart", 1000);
  //fNKineParts = 0;

  //fNPurgedKineParts = 0;

  //fRandomDecayState = new TRandom3();
}

void TPadmeEvent::Clear(Option_t * /*option*/)
{
  fRunNumber = 0;
  fEventNumber = 0;
  fTime = 0.;

  //fNGeneParts = 0;
  //fNKineParts = 0;
  //if(fGeneParts) fGeneParts->Clear("C");
  //if(fKineParts) fKineParts->Clear("C");
  //if(fRandomDecayState) fRandomDecayState->Clear("C");
}

void TPadmeEvent::Reset(Option_t * /*option*/)
{
  //if(fGeneParts){
  //    delete fGeneParts; 
  //    fGeneParts = 0;
  //}
  //if(fKineParts){
  //    delete fKineParts; 
  //    fKineParts = 0;
  //}
}

void TPadmeEvent::Print(Option_t * /*option*/) const
{
  cout << "Run "   << fRunNumber
       << " Event " << fEventNumber
       << " Time "  << fTime << endl;
  //cout << "   NGeneParticles " << fNGeneParts << endl;
  //cout << "   NKineParticles " << fNKineParts << endl;
  //PrintAll();
}

//void TPadmeEvent::PrintAll() const 
//{
//if (fNGeneParts>0) {
//    cout << "--- Gene Particles ---" << endl;
//    fGeneParts->Print();
//}
//if (fNKineParts>0) {
//    cout << "--- Kine Particles ---" << endl;
//    fKineParts->Print();
//}
//}

//GenePart* TPadmeEvent::AddGenePart() {
//  TClonesArray &parts = *fGeneParts;
//  GenePart* part = new(parts[fNGeneParts++]) GenePart();
//  return part;
//}

//KinePart* TPadmeEvent::AddKinePart() {
//  TClonesArray &parts = *fKineParts;
//  KinePart* part = new(parts[fNKineParts++]) KinePart();
//  return part;
//}

//void TPadmeEvent::EraseGeneParts() {
//    // Erase old collection of hits and replace it with a new (empty) one
//    delete fGeneParts;
//    fGeneParts = new TClonesArray("GenePart", 100);
//    fNGeneParts = 0;
//}

//void TPadmeEvent::EraseKineParts() {
//    // Erase old collection of hits and replace it with a new (empty) one
//    delete fKineParts;
//    fKineParts = new TClonesArray("KinePart", 1000);
//    fNKineParts = 0;
//}

//void TPadmeEvent::StoreRandomState(TRandom3* RandomDecayState, long *RanecuState)
//{
//
//    *fRandomDecayState = *RandomDecayState;
//    fRanecuState[0] = RanecuState[0];
//    fRanecuState[1] = RanecuState[1];
//}
