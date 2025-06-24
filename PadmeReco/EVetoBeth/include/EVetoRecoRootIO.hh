#ifndef EVetoRootRecoIO_h
#define EVetoRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TEVetoRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TEVetoRecoEvent;
class RecoVRootIO;

class EVetoRecoRootIO : public RecoVRootIO
{
public:

  EVetoRecoRootIO();
  virtual ~EVetoRecoRootIO();

private:

  

};
#endif // EVetoRecoRootIO_h
