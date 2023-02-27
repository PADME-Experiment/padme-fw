#ifndef HEPVetoRootRecoIO_h
#define HEPVetoRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "THEPVetoRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"

class THEPVetoRecoEvent;
class RecoVRootIO;
class RecoRootIOManager;

class HEPVetoRecoRootIO : public RecoVRootIO
{
public:

  HEPVetoRecoRootIO(RecoRootIOManager*);
  virtual ~HEPVetoRecoRootIO();

private:

};
#endif // HEPVetoRecoRootIO_h
