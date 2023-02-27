#ifndef SACRootRecoIO_h
#define SACRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"

class RecoRootIOManager;

class SACRecoRootIO : public RecoVRootIO
{
public:

  SACRecoRootIO(RecoRootIOManager*);
  virtual ~SACRecoRootIO();
  
private:

};
#endif // SACRecoRootIO_h
