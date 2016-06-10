// ChamberGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#ifndef ChamberGeometry_H
#define ChamberGeometry_H 1

#include "G4ThreeVector.hh"

#define HALLGEOMETRY_NUMBER_OF_BLOCKS 16

class ChamberGeometry
{

public:

  ~ChamberGeometry();
  static ChamberGeometry* GetInstance();

private:

  static ChamberGeometry* fInstance;

protected:

  ChamberGeometry();

public:

private:

};

#endif
