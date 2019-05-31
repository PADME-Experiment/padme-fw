#ifndef QuadSetup_H
#define QuadSetup_H

#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

class G4FieldManager;
class G4MagneticField;
class G4ChordFinder;
class G4Mag_UsualEqRhs;
class G4MagIntegratorStepper;

class QuadrupoleMagField;
//#include "QuadrupoleMagField.hh"
//class MagneticFieldMessenger;

///  A class for setting up the Magnetic Field

class QuadSetup
{

public:

  QuadSetup(G4double          pGradient, 
	    G4ThreeVector     pOrigin, 
	    G4RotationMatrix* pMatrix);
  ~QuadSetup();
      
  void SetStepperType(G4int i) { fStepperType = i; }

  void SetStepper();

  void SetMinStep(G4double s) { fMinStep = s; }

  void UpdateField();

  G4FieldManager*   GetLocalFieldManager()  { return fLocalFieldManager; }
  QuadrupoleMagField* GetMagneticField() { return fMagneticField; }

  void SetMagneticFieldValue(G4double);

protected:

  G4FieldManager* GetGlobalFieldManager();

private:

  G4FieldManager* fFieldManager;
  G4FieldManager* fLocalFieldManager;

  QuadrupoleMagField* fMagneticField; 

  G4ChordFinder* fChordFinder;
  G4ChordFinder* fLocalChordFinder;

  G4Mag_UsualEqRhs* fEquation;
  G4Mag_UsualEqRhs* fLocalEquation; 

  G4MagIntegratorStepper* fStepper;
  G4MagIntegratorStepper* fLocalStepper;
  G4int fStepperType;
  G4double fMinStep;
 
  //MagneticFieldMessenger* fFieldMessenger;

};

#endif
