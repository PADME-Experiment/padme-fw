#include "MagneticFieldSetup.hh"

#include "G4UniformMagField.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"

#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"
#include "G4SystemOfUnits.hh"

#include "MagneticFieldMap.hh"

//////////////////////////////////////////////////////////////////////////
//
//  Constructors:

MagneticFieldSetup::MagneticFieldSetup()
  : fChordFinder(0), fStepper(0)
{

  fMagneticField = new MagneticFieldMap();

  //fFieldMessenger = new MagneticFieldMessenger(this) ;  

  fEquation = new G4Mag_UsualEqRhs(fMagneticField); 
 
  fMinStep     = 0.25*mm; // minimal step of 1 mm is default
  fStepperType = 4;       // ClassicalRK4 is default stepper

  fFieldManager = GetGlobalFieldManager();

  UpdateField();

}

////////////////////////////////////////////////////////////////////////////////

MagneticFieldSetup::~MagneticFieldSetup()
{
  if (fMagneticField) delete fMagneticField;
  if (fChordFinder)   delete fChordFinder;
  if (fStepper)       delete fStepper;
}

/////////////////////////////////////////////////////////////////////////////
//
// Update field
//

void MagneticFieldSetup::UpdateField()
{

  SetStepper();
  G4cout << "The minimal step is equal to " << fMinStep/mm << " mm" <<G4endl ;

  fFieldManager->SetDetectorField(fMagneticField);

  if (fChordFinder) delete fChordFinder;
  fChordFinder = new G4ChordFinder(fMagneticField,fMinStep,fStepper);
  fFieldManager->SetChordFinder(fChordFinder);

  G4cout << "Delta chord is " << fChordFinder->GetDeltaChord()/cm << " cm" << G4endl;

}

/////////////////////////////////////////////////////////////////////////////
//
// Set stepper according to the stepper type
//

void MagneticFieldSetup::SetStepper()
{

  if(fStepper) delete fStepper;
  switch ( fStepperType ) 
  {
    case 0:  
      fStepper = new G4ExplicitEuler( fEquation ); 
      G4cout << "G4ExplicitEuler is called" << G4endl;     
      break;
    case 1:  
      fStepper = new G4ImplicitEuler( fEquation );      
      G4cout << "G4ImplicitEuler is called" << G4endl;     
      break;
    case 2:  
      fStepper = new G4SimpleRunge( fEquation );        
      G4cout << "G4SimpleRunge is called" << G4endl;     
      break;
    case 3:  
      fStepper = new G4SimpleHeum( fEquation );         
      G4cout << "G4SimpleHeum is called" << G4endl;     
      break;
    case 4:  
      fStepper = new G4ClassicalRK4( fEquation );       
      G4cout << "G4ClassicalRK4 (default) is called" << G4endl;     
      break;
    case 5:  
      fStepper = new G4HelixExplicitEuler( fEquation ); 
      G4cout << "G4HelixExplicitEuler is called" << G4endl;     
      break;
    case 6:  
      fStepper = new G4HelixImplicitEuler( fEquation ); 
      G4cout << "G4HelixImplicitEuler is called" << G4endl;     
      break;
    case 7:  
      fStepper = new G4HelixSimpleRunge( fEquation );   
      G4cout << "G4HelixSimpleRunge is called" << G4endl;     
      break;
    case 8:  
      fStepper = new G4CashKarpRKF45( fEquation );      
      G4cout << "G4CashKarpRKF45 is called" << G4endl;     
      break;
    case 9:  
      fStepper = new G4RKG3_Stepper( fEquation );       
      G4cout << "G4RKG3_Stepper is called" << G4endl;     
      break;
    default: fStepper = 0;
  }

}

////////////////////////////////////////////////////////////////////////////////
//
//  Utility methods

G4FieldManager*  MagneticFieldSetup::GetGlobalFieldManager()
{
  return G4TransportationManager::GetTransportationManager()->GetFieldManager();
}
