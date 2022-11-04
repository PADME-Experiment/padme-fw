#ifndef ETagRootRecoIO_h
#define ETagRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TETagRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TETagRecoEvent;
class RecoVRootIO;

class ETagRecoRootIO : public RecoVRootIO
{
public:

  ETagRecoRootIO();
  virtual ~ETagRecoRootIO();

private:

};
#endif // ETagRecoRootIO_h
