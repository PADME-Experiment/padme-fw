#ifndef ECalParameters_h
#define ECalParameters_h

#include <iostream>
#include "cstdlib"

#include "Rtypes.h"

class TSubDetectorInfo;

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

  Double_t GetECalFrontFacePosZ() { return fECalFrontFacePosZ; }

  Int_t GetECalNRows() { return fECalNRows; }
  Int_t GetECalNCols() { return fECalNCols; }

  Double_t GetCrystalSizeX() { return fCrystalSizeX; }
  Double_t GetCrystalSizeY() { return fCrystalSizeY; }
  Double_t GetCrystalSizeZ() { return fCrystalSizeZ; }

  Double_t GetCrystalGap() { return fCrystalGap; }

  void SetMCDetInfo(TSubDetectorInfo*);

private :

  Int_t NCryX;
  Int_t NCryY;
  Double_t a, b, c; 
  Double_t TargetZ,ECALZ,SACZ;
  Double_t ECalToTarg;

  Int_t fECalNRows;
  Int_t fECalNCols;

  Double_t fECalFrontFacePosZ;
  Double_t fCrystalSizeX;
  Double_t fCrystalSizeY;
  Double_t fCrystalSizeZ;
  Double_t fCrystalGap; // Warning: this gap includes the paint coating

 public:

  static ECalParameters* GetInstance();

 private:

  static ECalParameters* fInstance;

};
#endif
