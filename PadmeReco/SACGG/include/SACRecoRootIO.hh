#ifndef SACRootRecoIO_h
#define SACRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class SACRecoRootIO : public RecoVRootIO
{
public:

  SACRecoRootIO();
  virtual ~SACRecoRootIO();
  
private:

  Int_t iev;
  

};
#endif // SACRecoRootIO_h
