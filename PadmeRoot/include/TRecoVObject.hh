#ifndef TRecoVObject_H
#define TRecoVObject_H

#include "TClass.h"
#include "TObject.h"
#include "TRecoVHit.hh"
#include "TClonesArray.h"

class TRecoVObject : public TObject
{

public:

  TRecoVObject();
  TRecoVObject(TClass* hCls);
  virtual ~TRecoVObject(); 

  void Print(Option_t* option="") const;

  TRecoVHit* AddHit();         // Append a new (empty) hit to hits array
  TRecoVHit* AddHit(TRecoVHit*); // Append given hit to hits array
  TRecoVHit* Hit(Int_t);       // Return pointer to hit at given index in hits array
  TRecoVHit* LastHit();        // Return pointer to last hit in hits array
  void RemoveHit(Int_t);     // Remove hit at given index and compress hits array
  void Clear(Option_t* = ""); // Clear hits/digi arrays (all hits/digi are removed)

  Int_t GetNHits() { return fNHits; };

public:
  Int_t fNHits;
  TClonesArray* fHits;



protected:



  ClassDef(TRecoVObject,1);
};
#endif
