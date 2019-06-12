#ifndef TTargetFitEvent_H
#define TTargetFitEvent_H

#include "TClass.h"
#include "TObject.h"
#include "TTargetSignalFitParams.hh"
#include "TClonesArray.h"


class TTargetFitEvent : public TObject
{

public:

  TTargetFitEvent();
  TTargetFitEvent(TClass* hCls);
  virtual ~TTargetFitEvent(); 

  Int_t GetNHits() { return fNSetPar; };
  
public:

  TTargetSignalFitParams* AddFitParSet();                       // Append a new (empty) SetPar to hits array
  TTargetSignalFitParams* AddFitParSet(TTargetSignalFitParams*); // Append given SetPar to hits array
  TTargetSignalFitParams* GetFitParSet(Int_t);                     // Return pointer to SetPar at given index in hits array
  TTargetSignalFitParams* GetLastFitParSet();                      // Return pointer to last SetPar in hits array
  void RemoveFitParSet(Int_t);                   // Remove SetPar at given index and compress hits array
  
  void Clear(Option_t* = "");                 // Clear SetPar arrays (all SetPar are removed)


protected:
  Int_t fNSetPar;
  TClonesArray* fFitParams; 

  ClassDef(TTargetFitEvent,1);
};
#endif
