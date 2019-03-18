// BeamLineGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#include "BeamLineGeometry.hh"

using namespace CLHEP;

BeamLineGeometry* BeamLineGeometry::fInstance = 0;

BeamLineGeometry* BeamLineGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new BeamLineGeometry(); }
  return fInstance;
}

BeamLineGeometry::BeamLineGeometry()
{

  // Some basic chamber quantities from Cesidio's CAD drawings
  fBeWThick=250*um;  //250 microns 
  //  fBeWThick=250*mm;  //250 microns 
  fBeWDiameter=61*mm;

  //  fBeWPosZ=-3374.2 -1000.*cos(45*deg);  //1 meter from DHSTB002
  //  fBeWPosX= 659.5  +1000.*sin(45*deg);  //1 meter from DHSTB002

  // Be window support flange.
  fBeFlThick=18*mm;      //250 microns 
  fBeFlDiameter=114*mm;   //From DHSTB002 inner chamber design
  fBeFlInnHole =28*mm;   //From DHSTB002 inner chamber design

  fDHSTB002Thick         =  670*mm;  // from drawings
  fDHSTB002Radius        = 2057*mm;  //radius
  fDHSTB002InnHole       = 1389*mm; //radius
  fDHSTB002CenterRadius  = 1723.*mm; //radius

  //From DHSTB002 inner chamber design
  fDHSTB002PipeThick    = 40*mm;   
  fDHSTB002PipeRadius   = ( fDHSTB002CenterRadius+59/2)*mm;  //radius
  fDHSTB002PipeInnHole  = ( fDHSTB002CenterRadius-59/2)*mm;  //radius

  //From DHSTB002 inner chamber design
  fDHSTB002PipeGapThick    = 32*mm;   
  fDHSTB002PipeGapRadius   = ( fDHSTB002CenterRadius+53/2)*mm;  //radius
  fDHSTB002PipeGapInnHole  = ( fDHSTB002CenterRadius-53/2)*mm;  //radius

  fDHSTB002PosY = 0*mm;
  fDHSTB002PosZ = -2001*mm; 
  
  double TGapXSize =170*mm;
  //First gap in the H structure
  fDHSTB002Gap1Thick   = 128.*mm ; //From Drawings   
  fDHSTB002Gap1Radius  = (fDHSTB002CenterRadius-TGapXSize/2)*mm ;
  fDHSTB002Gap1InnHole = (fDHSTB002InnHole +127.)*mm ; //Inner wall thickness 

  fDHSTB002Gap1PosX = 0.*mm ;
  fDHSTB002Gap1PosY = 0.*mm ;
  fDHSTB002Gap1PosZ = 0.*mm ;

  //Second gap in the H structure
  fDHSTB002Gap2Thick   = 128.*mm ; //From Drawings      
  fDHSTB002Gap2Radius  = (fDHSTB002Radius-127.)*mm ;
  fDHSTB002Gap2InnHole = (fDHSTB002CenterRadius+TGapXSize/2)*mm; 

  fDHSTB002Gap2PosX = 0.*mm;
  fDHSTB002Gap2PosY = 0.*mm; 
  fDHSTB002Gap2PosZ = 0.*mm;

  //Thin gap in the H structure check drawings
  fDHSTB002TGapThick   = 42.*mm; //From Drawings          
  fDHSTB002TGapRadius  = (fDHSTB002CenterRadius+86.)*mm;
  fDHSTB002TGapInnHole = (fDHSTB002CenterRadius-86.)*mm; 

  fDHSTB00TGap2PosX = 0.*mm;
  fDHSTB00TGap2PosY = 0.*mm;
  fDHSTB00TGap2PosZ = 0.*mm;


}

BeamLineGeometry::~BeamLineGeometry()
{}
