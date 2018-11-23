#ifndef TargetRootRecoIO_h
#define TargetRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "RecoRootIOManager.hh"
//#include "TTargetRecoEvent.hh"
//#include "TTargetRecoBeam.hh"
#include "TTree.h"
#include "TBranch.h"



class RecoVRootIO;
class TTargetRecoBeam;
class TTargetFitEvent;

class TargetRecoRootIO : public RecoVRootIO
{
public:

  TargetRecoRootIO();
  virtual ~TargetRecoRootIO();
  virtual void NewRun(Int_t nRun, TFile* hfile);
  virtual void SaveEvent();
  void SaveTargetFitEvent();
  
protected:
  TBranch* fBranchTargetRecoBeam;
  TTargetRecoBeam* fBeam;
  TBranch* fBranchTargetFitEvent;
  TTargetFitEvent* fFitEvent;

};
#endif // TargetRecoRootIO_h
