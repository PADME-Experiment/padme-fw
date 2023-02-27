#ifndef PVetoRootRecoIO_h
#define PVetoRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TPVetoRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"

class TPVetoRecoEvent;
class RecoVRootIO;
class RecoRootIOManager;

class PVetoRecoRootIO : public RecoVRootIO
{
public:

  PVetoRecoRootIO(RecoRootIOManager*);
  virtual ~PVetoRecoRootIO();

private:

};
#endif // PVetoRecoRootIO_h
