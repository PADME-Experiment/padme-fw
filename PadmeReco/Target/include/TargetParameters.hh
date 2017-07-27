#ifndef TargetParameters_h
#define TargetParameters_h

#include <iostream>
#include "cstdlib"

#include "Rtypes.h"

class TSubDetectorInfo;

class TargetParameters {

public :

  static TargetParameters* GetInstance();
  ~TargetParameters();

private:

  static TargetParameters* fInstance;

protected:

  TargetParameters();

public:

  void SetMCDetInfo(TSubDetectorInfo*);

  Double_t GetTargetSizeX() { return fTargetSizeX; }
  Double_t GetTargetSizeY() { return fTargetSizeY; }
  Double_t GetTargetSizeZ() { return fTargetSizeZ; }
  Double_t GetTargetFrontFacePosZ() { return fTargetFrontFacePosZ; }

  Bool_t GetTargetFastDigitization() { return fTargetFastDigitization; }
  Bool_t GetTargetSaveWaveformToDigi() { return fTargetSaveWaveformToDigi; }

  Bool_t GetTargetEfficiencyEnabled() { return fTargetEfficiency; }

  Double_t GetPitch() { return fPitch; }
  Double_t GetSigmaQi() { return fSigmaQi; }
  Double_t GetTimeError() { return fTimeErr; }

private :


  // MC Geometry-Run parameters
  Double_t fTargetSizeX;
  Double_t fTargetSizeY;
  Double_t fTargetSizeZ;
  Double_t fTargetFrontFacePosZ; // Position along Z axis of Target front face

  bool fTargetFastDigitization; // Enable/disable use of fast digitization
  bool fTargetSaveWaveformToDigi; // Enable/disable saving of digitized waveforms to persistent digis



  // Beam Center finding parameters
  Double_t fPitch;

  // Digi-Reco parameters

  bool fTargetEfficiency; // Enable/disable hit based target efficiency calculation
  Double_t fSigmaQi; 
  Double_t fTimeErr; 

};
#endif
