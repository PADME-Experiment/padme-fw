#ifndef TargetRootRecoIO_h
#define TargetRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TTargetRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TTargetRecoEvent;
class RecoVRootIO;

class TargetRecoRootIO : public RecoVRootIO
{
public:

  TargetRecoRootIO();
  virtual ~TargetRecoRootIO();

private:

  

};
#endif // TargetRecoRootIO_h
