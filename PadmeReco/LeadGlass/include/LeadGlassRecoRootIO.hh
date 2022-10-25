#ifndef LeadGlassRootRecoIO_h
#define LeadGlassRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TLeadGlassRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"

class TLeadGlassRecoEvent;
class RecoVRootIO;

class LeadGlassRecoRootIO : public RecoVRootIO
{
public:

  LeadGlassRecoRootIO();
  virtual ~LeadGlassRecoRootIO();

private:

};
#endif // LeadGlassRecoRootIO_h
