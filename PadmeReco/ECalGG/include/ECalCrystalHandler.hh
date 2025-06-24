#ifndef ECalCrystalHandler_h
#define ECalCrystalHandler_h

#include "ECalCrystal.hh"

#define CRYSTALHANDLER_N_MAX_CRYSTALS_X 30
#define CRYSTALHANDLER_N_MAX_CRYSTALS_Y 30

class ECalCrystalHandler {

 public :
  ECalCrystalHandler();
  ~ECalCrystalHandler();
  
 public :

  Int_t GetNCrystals(){return fNCrystals;};
  void SortEnergy();
  void Print();

  ECalCrystal* GetSeed(Double_t Ethr);

  ECalCrystal* CreateCrystal(Int_t ix,Int_t iy);

  ECalCrystal* GetCrystal(Int_t x,Int_t y);       //return pointer from crystal map
  ECalCrystal* GetCrystal(Int_t i);               //return pointer from sorted crystal array
  ECalCrystal* GetCrystal(Double_t x,Double_t y); //return pointer from geometrical position of point

  // The following method is needed for multiple cluster finding on same event
  void ResetCrystals(); // Set status of all crystals to "not used"/"not seed"

 private:

  Int_t fNCrystals; 

  ECalCrystal* fCrystalMap[CRYSTALHANDLER_N_MAX_CRYSTALS_X][CRYSTALHANDLER_N_MAX_CRYSTALS_Y];
  ECalCrystal* fCrystalList[CRYSTALHANDLER_N_MAX_CRYSTALS_X*CRYSTALHANDLER_N_MAX_CRYSTALS_Y];

  Int_t fNCryX;
  Int_t fNCryY;

};
#endif
