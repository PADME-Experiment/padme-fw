// ChamberGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#include "ChamberGeometry.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"

ChamberGeometry* ChamberGeometry::fInstance = 0;

ChamberGeometry* ChamberGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ChamberGeometry(); }
  return fInstance;
}

ChamberGeometry::ChamberGeometry()
{
}

ChamberGeometry::~ChamberGeometry()
{}
