#ifndef ECalMLRootRecoIO_h
#define ECalMLRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TECalMLRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TECalMLRecoEvent;
class RecoVRootIO;

class ECalMLRecoRootIO : public RecoVRootIO
{
public:

  ECalMLRecoRootIO();
  virtual ~ECalMLRecoRootIO();

private:

  

};
#endif // ECalMLRecoRootIO_h
