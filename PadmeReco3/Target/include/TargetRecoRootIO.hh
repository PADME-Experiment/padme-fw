#ifndef TargetRecoRootIO_h
#define TargetRecoRootIO_h 1

#include "TTargetRecoEvent.hh"
//#include "TTargetClusCollection.hh"

class TTree;
class TBranch;
class TargetReconstruction;
class TargetHit;
//class TargetCluster;

class TargetRecoRootIO
{
public:

  TargetRecoRootIO();
  virtual ~TargetRecoRootIO();

  void NewRun();
  void EndRun();
  void SaveEvent();

  void  SetVerbose(Int_t v) { fVerbose = v;    }
  Int_t GetVerbose()        { return fVerbose; }

  void SetEventTree(TTree* t) { fEventTree = t; }
  void SetTargetReconstruction(TargetReconstruction* reco) { fTargetReconstruction = reco; }

private:

  Bool_t ExportHit(TargetHit*,TRecoVHit*);
  //Bool_t ExportCluster(TargetCluster*,TRecoVCluster*);

  TargetReconstruction* fTargetReconstruction;

  TTree* fEventTree;

  TBranch* fBranchHitsColl;
  //TBranch* fBranchClusColl;

  TTargetRecoEvent*      fHitsCollection;
  //TTargetClusCollection* fClusCollection;

  Int_t fVerbose;

};
#endif // TargetRecoRootIO_h
