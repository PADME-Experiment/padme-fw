#ifndef PVetoBLRootRecoIO_h
#define PVetoBLRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TPVetoBLRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TPVetoBLRecoEvent;
class RecoVRootIO;

class PVetoBLRecoRootIO : public RecoVRootIO
{
public:

  PVetoBLRecoRootIO();
  virtual ~PVetoBLRecoRootIO();

private:

  

};
#endif // PVetoBLRecoRootIO_h
