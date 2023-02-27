#ifndef ECalRootRecoIO_h
#define ECalRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TECalRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"

class TECalRecoEvent;
class RecoVRootIO;
class RecoRootIOManager;

class ECalRecoRootIO : public RecoVRootIO
{
public:

  ECalRecoRootIO(RecoRootIOManager*);
  virtual ~ECalRecoRootIO();

private:

};
#endif // ECalRecoRootIO_h
