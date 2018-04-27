#include "RecoVRootIO.hh"

RecoVRootIO::RecoVRootIO(TString name)
  : PadmeVNamedModule(name)
{ 
  // Default output file parameters
  fBufSize = 64000; //size of output buffer
  fBranchStyle = 1; //new branch style by default

  // Default run number
  fRunNumber = 0;


}

RecoVRootIO::~RecoVRootIO(){;
}
