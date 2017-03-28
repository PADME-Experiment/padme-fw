#ifndef ECalParameters_h
#define ECalParameters_h

#include <iostream>
#include "cstdlib"

#include "Rtypes.h"

class ECalParameters {

public :

  ECalParameters();
  ~ECalParameters();
  
  void InitGeomParam();
  void WriteGeomParam();

  Int_t GetNCryX() { return NCryX; }
  Int_t GetNCryY() { return NCryY; }

  Double_t GetCryXSize() { return a; }
  Double_t GetCryYSize() { return b; }
  Double_t GetCryZSize() { return c; }

  Double_t GetECalToTarg() { return ECalToTarg; }
 
  Double_t GetTargetZ() { return TargetZ; }
  Double_t GetECalZ()   { return ECALZ; }
  Double_t GetSACZ()    { return SACZ; }

private :

  Int_t NCryX;
  Int_t NCryY;
  Double_t a, b, c; 
  Double_t TargetZ,ECALZ,SACZ;
  Double_t ECalToTarg;

 public:

  static ECalParameters* GetInstance();

 private:

  static ECalParameters* fInstance;

};
#endif
