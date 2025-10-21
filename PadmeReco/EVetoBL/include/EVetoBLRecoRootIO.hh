#ifndef EVetoBLRootRecoIO_h
#define EVetoBLRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TEVetoBLRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TEVetoBLRecoEvent;
class RecoVRootIO;

class EVetoBLRecoRootIO : public RecoVRootIO
{
public:

  EVetoBLRecoRootIO();
  virtual ~EVetoBLRecoRootIO();

private:

  

};
#endif // EVetoBLRecoRootIO_h
