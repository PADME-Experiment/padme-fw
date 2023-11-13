#ifndef ECalParameters_h
#define ECalParameters_h

#include <iostream>
#include "cstdlib"

#include "Rtypes.h"

class TSubDetectorInfo;

class ECalParameters {

public :

  static ECalParameters* GetInstance();
  ~ECalParameters();

private:

  static ECalParameters* fInstance;

protected:

  ECalParameters();

public:

  void SetMCDetInfo(TSubDetectorInfo*);

  Double_t GetECalFrontFacePosZ() { return fECalFrontFacePosZ; }

  Int_t GetECalNRows() { return fECalNRows; }
  Int_t GetECalNCols() { return fECalNCols; }

  Double_t GetCrystalSizeX() { return fCrystalSizeX; }
  Double_t GetCrystalSizeY() { return fCrystalSizeY; }
  Double_t GetCrystalSizeZ() { return fCrystalSizeZ; }

  Double_t GetCrystalGap() { return fCrystalGap; }

  Double_t GetSignalToEnergy(Int_t c) { return fSignalToEnergy; } // No channel dependency yet

  Double_t GetIslandEThreshold() { return fIslandEThreshold; }
  Double_t GetIslandEThresholdSeed() { return fIslandEThresholdSeed; }

  Double_t GetRadiusEThreshold() { return fRadiusEThreshold; }
  Double_t GetRadiusEThresholdSeed() { return fRadiusEThresholdSeed; }
  Double_t GetRadiusRadius() { return fRadiusRadius; }

private :

  // Geometry parameters

  Int_t fECalNRows;
  Int_t fECalNCols;

  Double_t fCrystalSizeX;
  Double_t fCrystalSizeY;
  Double_t fCrystalSizeZ;
  Double_t fCrystalGap; // Warning: this gap includes the paint coating

  Double_t fECalFrontFacePosZ;

  // Calibration parameters

  Double_t fSignalToEnergy; // Convert digi signal value to energy in MeV

  // Cluster finding parameters

  Double_t fIslandEThreshold;
  Double_t fIslandEThresholdSeed;

  Double_t fRadiusEThreshold;
  Double_t fRadiusEThresholdSeed;
  Double_t fRadiusRadius;

};
#endif
