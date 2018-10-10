#ifndef SACRootRecoIO_h
#define SACRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TSACRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TSACRecoEvent;
class RecoVRootIO;

class SACRecoRootIO : public RecoVRootIO
{
public:

  SACRecoRootIO();
  virtual ~SACRecoRootIO();

private:

  

};
#endif // SACRecoRootIO_h
