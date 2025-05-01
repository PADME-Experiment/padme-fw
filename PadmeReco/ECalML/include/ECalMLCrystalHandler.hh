#ifndef ECalMLCrystalHandler_h
#define ECalMLCrystalHandler_h

#include "ECalMLCrystal.hh"

#define CRYSTALHANDLER_N_MAX_CRYSTALS_X 30
#define CRYSTALHANDLER_N_MAX_CRYSTALS_Y 30

class ECalMLCrystalHandler {

 public :
  ECalMLCrystalHandler();
  ~ECalMLCrystalHandler();
  
 public :

  Int_t GetNCrystals(){return fNCrystals;};
  void SortEnergy();
  void Print();

  ECalMLCrystal* GetSeed(Double_t Ethr);

  ECalMLCrystal* CreateCrystal(Int_t ix,Int_t iy);

  ECalMLCrystal* GetCrystal(Int_t x,Int_t y);       //return pointer from crystal map
  ECalMLCrystal* GetCrystal(Int_t i);               //return pointer from sorted crystal array
  ECalMLCrystal* GetCrystal(Double_t x,Double_t y); //return pointer from geometrical position of point

  // The following method is needed for multiple cluster finding on same event
  void ResetCrystals(); // Set status of all crystals to "not used"/"not seed"

 private:

  Int_t fNCrystals; 

  ECalMLCrystal* fCrystalMap[CRYSTALHANDLER_N_MAX_CRYSTALS_X][CRYSTALHANDLER_N_MAX_CRYSTALS_Y];
  ECalMLCrystal* fCrystalList[CRYSTALHANDLER_N_MAX_CRYSTALS_X*CRYSTALHANDLER_N_MAX_CRYSTALS_Y];

  Int_t fNCryX;
  Int_t fNCryY;

};
#endif
