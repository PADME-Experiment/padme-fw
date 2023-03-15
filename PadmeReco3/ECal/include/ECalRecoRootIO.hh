#ifndef ECalRecoRootIO_h
#define ECalRecoRootIO_h 1

#include "TECalRecoEvent.hh"
#include "TECalClusCollection.hh"

class TTree;
class TBranch;
class ECalReconstruction;
class ECalHit;
class ECalCluster;

class ECalRecoRootIO
{
public:

  ECalRecoRootIO();
  virtual ~ECalRecoRootIO();

  void NewRun();
  void EndRun();
  void SaveEvent();

  void  SetVerbose(Int_t v) { fVerbose = v;    }
  Int_t GetVerbose()        { return fVerbose; }

  void SetEventTree(TTree* t) { fEventTree = t; }
  void SetECalReconstruction(ECalReconstruction* etReco) { fECalReconstruction = etReco; }

private:

  Bool_t ExportHit(ECalHit*,TRecoVHit*);
  Bool_t ExportCluster(ECalCluster*,TRecoVCluster*);

  ECalReconstruction* fECalReconstruction;

  TTree* fEventTree;

  TBranch* fBranchHitsColl;
  TBranch* fBranchClusColl;

  TECalRecoEvent*      fHitsCollection;
  TECalClusCollection* fClusCollection;

  Int_t fVerbose;

};
#endif // ECalRecoRootIO_h
