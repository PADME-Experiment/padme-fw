#ifndef PVetoRootRecoIO_h
#define PVetoRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TPVetoRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TPVetoRecoEvent;-
class RecoVRootIO;

class PVetoRecoRootIO : public RecoVRootIO
{
public:

  PVetoRecoRootIO();
  virtual ~PVetoRecoRootIO();

private:

  

};
#endif // PVetoRecoRootIO_h
