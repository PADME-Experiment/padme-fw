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

  void EnableBeWindow() { fBeWindowEnable = true; }
  void DisableBeWindow() { fBeWindowEnable = false; }
  G4bool BeWindowIsEnabled() { return fBeWindowEnable; }

//  The Be window
  G4double GetBeWThick()         { return fBeWThick; }
  G4double GetBeWDiameter()      { return fBeWDiameter; }
  G4double GetBeWFrontFacePosZ() { return fBeWFrontFacePosZ; }
  
  G4double GetBeWPosX() { return fBeWPosX; }
  G4double GetBeWPosY() { return fBeWPosY; }
  G4double GetBeWPosZ() { return fBeWPosZ; }

  //  The Be window Flange
  G4double GetBeFlThick()    { return fBeFlThick;}
  G4double GetBeFlDiameter() { return fBeFlDiameter;}
  G4double GetBeFlInnHole()  { return fBeFlInnHole;}

  //  DHSTB002 magnet
  G4double GetDHSTB002Thick()    {return fDHSTB002Thick;}
  G4double GetDHSTB002Radius()   {return fDHSTB002Radius;}
  G4double GetDHSTB002InnHole()  {return fDHSTB002InnHole;}
  G4double GetDHSTB002CenterRadius() {return fDHSTB002CenterRadius;}
  G4double GetDHSTB002PosX()         {return fDHSTB002PosX;}
  G4double GetDHSTB002PosY()         {return fDHSTB002PosY;}
  G4double GetDHSTB002PosZ()         {return fDHSTB002PosZ;}

  //  DHSTB002 magnet inner vacuum pipe iron
  G4double GetDHSTB002PipeThick()    {return fDHSTB002PipeThick;}
  G4double GetDHSTB002PipeRadius()   {return fDHSTB002PipeRadius;}
  G4double GetDHSTB002PipeInnHole()  {return fDHSTB002PipeInnHole;}

  //  DHSTB002 magnet inner vacuum pipe vacuum gap
  G4double GetDHSTB002PipeGapThick()    {return fDHSTB002PipeGapThick;}
  G4double GetDHSTB002PipeGapRadius()   {return fDHSTB002PipeGapRadius;}
  G4double GetDHSTB002PipeGapInnHole()  {return fDHSTB002PipeGapInnHole;}

  //Lower radius Gap
  G4double GetDHSTB002Gap1Thick()    {return fDHSTB002Gap1Thick;}
  G4double GetDHSTB002Gap1Radius()   {return fDHSTB002Gap1Radius;}
  G4double GetDHSTB002Gap1InnHole()  {return fDHSTB002Gap1InnHole;}

  //Higer radius Gap
  G4double GetDHSTB002Gap2Thick()    {return fDHSTB002Gap2Thick;}
  G4double GetDHSTB002Gap2Radius()   {return fDHSTB002Gap2Radius;}
  G4double GetDHSTB002Gap2InnHole()  {return fDHSTB002Gap2InnHole;}

  //Central thin gap radius 
  G4double GetDHSTB002TGapThick()    {return fDHSTB002TGapThick;}
  G4double GetDHSTB002TGapRadius()   {return fDHSTB002TGapRadius;}
  G4double GetDHSTB002TGapInnHole()  {return fDHSTB002TGapInnHole;}

 //Gamma line structure	      
                                     
  G4double GetDHSTB002GLineGapPosX(){return fDHSTB002GLineGapPosX;}    
  G4double GetDHSTB002GLineGapPosY(){return fDHSTB002GLineGapPosY;}    
  G4double GetDHSTB002GLineGapPosZ(){return fDHSTB002GLineGapPosZ;}    
  				      
  G4double GetDHSTB002GLineGapSizeX(){return fDHSTB002GLineGapSizeX;}   
  G4double GetDHSTB002GLineGapSizeY(){return fDHSTB002GLineGapSizeY;}   
  G4double GetDHSTB002GLineGapSizeZ(){return fDHSTB002GLineGapSizeZ;}   
  				     
  G4double GetDHSTB002GLinePipePosX(){return fDHSTB002GLinePipePosX;}   
  G4double GetDHSTB002GLinePipePosY(){return fDHSTB002GLinePipePosY;}   
  G4double GetDHSTB002GLinePipePosZ(){return fDHSTB002GLinePipePosZ;}   
  				      
  G4double GetDHSTB002GLinePipeSizeX(){return fDHSTB002GLinePipeSizeX;}   
  G4double GetDHSTB002GLinePipeSizeY(){return fDHSTB002GLinePipeSizeY;}   
  G4double GetDHSTB002GLinePipeSizeZ(){return fDHSTB002GLinePipeSizeZ;}   
  				      
  G4double GetDHSTB002GLinePipeGapSizeX(){return fDHSTB002GLinePipeGapSizeX;}
  G4double GetDHSTB002GLinePipeGapSizeY(){return fDHSTB002GLinePipeGapSizeY;}
  G4double GetDHSTB002GLinePipeGapSizeZ(){return fDHSTB002GLinePipeGapSizeZ;}

  // Magnetic field inside DHSTB002

  void SetDHSTB002MagneticFieldY(G4double f) { fDHSTB002MagneticFieldY = f; }
  G4double GetDHSTB002MagneticFieldY() { return fDHSTB002MagneticFieldY; }

private:

  //  Berillium thin window and stainless still flange

  G4bool fBeWindowEnable;

  G4double fBeWFrontFacePosZ;
  G4double fBeWThick;
  G4double fBeWDiameter;
  G4double fBeWPosX;
  G4double fBeWPosY;
  G4double fBeWPosZ;
  
  G4double fBeFlDiameter;
  G4double fBeFlThick;
  G4double fBeFlInnHole;

//  DHSTB002 magnet
  G4double fDHSTB002Thick;   
  G4double fDHSTB002Radius;
  G4double fDHSTB002InnHole; 
  G4double fDHSTB002CenterRadius; 

//  DHSTB002 magnet inner vacuum pipe
  G4double fDHSTB002PipeThick;   
  G4double fDHSTB002PipeRadius;
  G4double fDHSTB002PipeInnHole; 

  G4double fDHSTB002PipeGapThick;   
  G4double fDHSTB002PipeGapRadius;
  G4double fDHSTB002PipeGapInnHole; 

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

  G4double fDHSTB002TGap2PosX ;
  G4double fDHSTB002TGap2PosY ;
  G4double fDHSTB002TGap2PosZ ;

 
 //Gamma line structure	      
                                      
 G4double fDHSTB002GLineGapPosX ;     
 G4double fDHSTB002GLineGapPosY ;     
 G4double fDHSTB002GLineGapPosZ ;     
				       
 G4double fDHSTB002GLineGapSizeX ;    
 G4double fDHSTB002GLineGapSizeY ;    
 G4double fDHSTB002GLineGapSizeZ ;    
				       
 G4double fDHSTB002GLinePipePosX ;    
 G4double fDHSTB002GLinePipePosY ;    
 G4double fDHSTB002GLinePipePosZ ;    
				       
 G4double fDHSTB002GLinePipeSizeX;    
 G4double fDHSTB002GLinePipeSizeY;    
 G4double fDHSTB002GLinePipeSizeZ;    
				       
 G4double fDHSTB002GLinePipeGapSizeX; 
 G4double fDHSTB002GLinePipeGapSizeY; 
 G4double fDHSTB002GLinePipeGapSizeZ;

  // Magnetic field
  G4double fDHSTB002MagneticFieldY;

};

#endif
