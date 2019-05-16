#ifndef TRecoVClusCollection_H
#define TRecoVClusCollection_H

#include "TClass.h"
#include "TObject.h"
#include "TRecoVCluster.hh"
#include "TClonesArray.h"

//class TRecoVCluster;

class TRecoVClusCollection : public TObject
{

public:

  TRecoVClusCollection();
  TRecoVClusCollection(TClass* hCls);
  virtual ~TRecoVClusCollection(); 

  void Print(Option_t* option="") const;

  TRecoVCluster* AddElement();               // Append a new (empty) hit to hits array
  TRecoVCluster* AddElement(TRecoVCluster*); // Append given hit to hits array
  TRecoVCluster* Element(Int_t);             // Return pointer to hit at given index in hits array
  TRecoVCluster* LastElement();              // Return pointer to last hit in hits array
  void RemoveElement(Int_t);                 // Remove hit at given index and compress hits array
  void Clear(Option_t* = "");                // Clear hits/digi arrays (all hits/digi are removed)

  Int_t GetNElements() { return fNClus; };

public:
  Int_t fNClus;
  TClonesArray* fClus;

protected:
  ClassDef(TRecoVClusCollection,1);
};
#endif
