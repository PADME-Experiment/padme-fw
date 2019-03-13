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
  fBeWDiameter=61*mm;
//  fBeWSizeX=55*mm;   //From DHSTB002 inner chamber design
//  fBeWSizeY=35*mm;   //From DHSTB002 inner chamber design

  // Be window support flange.
  fBeFlThick=18*mm;      //250 microns 
  fBeFlDiameter=114*mm;   //From DHSTB002 inner chamber design
  fBeFlInnHole =28*mm;   //From DHSTB002 inner chamber design

  fDHSTB002Thick    =  670*mm;   
  fDHSTB002Radius   = 2057*mm;  //radius
  fDHSTB002InnHole  = 1389*mm; //radius

  //  fDHSTB002PosX = (fDHSTB002Radius-fDHSTB002InnHole)/2.+fDHSTB002InnHole;   
  fDHSTB002PosY = 0*mm;
  fDHSTB002PosZ = -3000*mm; 

  //First gap in the H structure
  fDHSTB002Gap1Thick   = 300.*mm ; //Need Drawings   
  fDHSTB002Gap1Radius  = (1723.-85)*mm ;
  fDHSTB002Gap1InnHole =  (fDHSTB002InnHole +127.)*mm ; //Inner wall thickness 

  fDHSTB002Gap1PosX = 0.*mm ;
  fDHSTB002Gap1PosY = 0.*mm ;
  fDHSTB002Gap1PosZ = 0.*mm ;

  //Second gap in the H structure
  fDHSTB002Gap2Thick   = 300.*mm ; //Need Drawings      
  fDHSTB002Gap2Radius  = (fDHSTB002Radius-127.)*mm ;
  fDHSTB002Gap2InnHole = (1723.+85.)*mm; 

  fDHSTB002Gap2PosX = 0.*mm;
  fDHSTB002Gap2PosY = 0.*mm; 
  fDHSTB002Gap2PosZ = 0.*mm;

  //Thin gap in the H structure check drawings
  fDHSTB002TGapThick   = 43.*mm;    
  fDHSTB002TGapRadius  = (1723.+85.)*mm;
  fDHSTB002TGapInnHole = (1723.-85.)*mm; 

  fDHSTB00TGap2PosX = 0.*mm;
  fDHSTB00TGap2PosY = 0.*mm;
  fDHSTB00TGap2PosZ = 0.*mm;


}

BeamLineGeometry::~BeamLineGeometry()
{}
