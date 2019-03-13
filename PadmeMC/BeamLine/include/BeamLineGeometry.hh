// BeamLineGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by M. Raggi (mauro.raggi@roma1.infn.it) 2019-03-07
// --------------------------------------------------------------

#ifndef BeamLineGeometry_H
#define BeamLineGeometry_H 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#define CHAMBERGEOMETRY_N_PORTHOLES 8

class BeamLineGeometry
{

public:

  ~BeamLineGeometry();
  static BeamLineGeometry* GetInstance();

private:

  static BeamLineGeometry* fInstance;

protected:

  BeamLineGeometry();

public:

//  The Be window
  G4double GetBeWThick()         { return fBeWThick; }
  G4double GetBeWDiameter()      { return fBeWDiameter; }
  G4double GetBeWFrontFacePosZ() { return fBeWFrontFacePosZ; }
  
  G4double GetfBeWPosX() { return fBeWPosX; }
  G4double GetfBeWPosY() { return fBeWPosY; }

//  The Be window Flange
  G4double GetBeFlThick()    { return fBeFlThick;}
  G4double GetBeFlDiameter() { return fBeFlDiameter;}
  G4double GetBeFlInnHole()  { return fBeFlInnHole;}

//  DHSTB002 magnet
  G4double GetDHSTB002Thick()    {return fDHSTB002Thick;}
  G4double GetDHSTB002Radius()   {return fDHSTB002Radius;}
  G4double GetDHSTB002InnHole()  {return fDHSTB002InnHole;}

  //Lower radius Gap
  G4double GetDHSTB002Gap1Thick()    {return fDHSTB002Gap1Thick;}
  G4double GetDHSTB002Gap1Radius()   {return fDHSTB002Gap1Radius;}
  G4double GetDHSTB002Gap1InnHole()  {return fDHSTB002Gap1InnHole;}

  //Higer radius Gap
  G4double GetDHSTB002Gap2Thick()    {return fDHSTB002Gap2Thick;}
  G4double GetDHSTB002Gap2Radius()   {return fDHSTB002Gap2Radius;}
  G4double GetDHSTB002Gap2InnHole()  {return fDHSTB002Gap2InnHole;}

  G4double GetDHSTB002TGapThick()    {return fDHSTB002TGapThick;}
  G4double GetDHSTB002TGapRadius()   {return fDHSTB002TGapRadius;}
  G4double GetDHSTB002TGapInnHole()  {return fDHSTB002TGapInnHole;}

  G4double GetDHSTB002PosX() {return fDHSTB002PosX;}
  G4double GetDHSTB002PosY() {return fDHSTB002PosY;}
  G4double GetDHSTB002PosZ() {return fDHSTB002PosZ;}

//  Berillium thin window and stainless still flange
  G4double fBeWFrontFacePosZ;
  G4double fBeWThick;
  G4double fBeWDiameter;
  G4double fBeWPosX;
  G4double fBeWPosY;
  
  G4double fBeFlDiameter;
  G4double fBeFlThick;
  G4double fBeFlInnHole;

//  DHSTB002 magnet
  G4double fDHSTB002Thick;   
  G4double fDHSTB002Radius;
  G4double fDHSTB002InnHole; 

  G4double fDHSTB002PosX ;
  G4double fDHSTB002PosY ;
  G4double fDHSTB002PosZ ;

  //First gap in the H structure
  G4double fDHSTB002Gap1Thick;   
  G4double fDHSTB002Gap1Radius;
  G4double fDHSTB002Gap1InnHole; 

  G4double fDHSTB002Gap1PosX ;
  G4double fDHSTB002Gap1PosY ;
  G4double fDHSTB002Gap1PosZ ;

  //Second gap in the H structure
  G4double fDHSTB002Gap2Thick;   
  G4double fDHSTB002Gap2Radius;
  G4double fDHSTB002Gap2InnHole; 

  G4double fDHSTB002Gap2PosX ;
  G4double fDHSTB002Gap2PosY ;
  G4double fDHSTB002Gap2PosZ ;

  //Thin gap in the H structure
  G4double fDHSTB002TGapThick;   
  G4double fDHSTB002TGapRadius;
  G4double fDHSTB002TGapInnHole; 

  G4double fDHSTB00TGap2PosX ;
  G4double fDHSTB00TGap2PosY ;
  G4double fDHSTB00TGap2PosZ ;



};

#endif
