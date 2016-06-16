// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#include "MCVRootIO.hh"

MCVRootIO::MCVRootIO(G4String name)
{
  // Default output file parameters
  fBufSize = 64000; //size of output buffer
  fBranchStyle = 1; //new branch style by default

  // Default run number
  fRunNumber = 0;

  // Default verbose level
  fVerbose = 0;

  // Subdetector RootIO is disabled by default
  fName    = name;
  fEnabled = false;

}

MCVRootIO::~MCVRootIO(){}
