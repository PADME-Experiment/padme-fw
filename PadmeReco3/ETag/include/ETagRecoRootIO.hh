#ifndef ETagRecoRootIO_h
#define ETagRecoRootIO_h 1

#include "TETagRecoEvent.hh"
#include "TETagClusCollection.hh"

class TTree;
class TBranch;
class ETagReconstruction;
class ETagHit;
class ETagCluster;

class ETagRecoRootIO
{
public:

  ETagRecoRootIO();
  virtual ~ETagRecoRootIO();

  void NewRun();
  void EndRun();
  void SaveEvent();

  void  SetVerbose(Int_t v) { fVerbose = v;    }
  Int_t GetVerbose()        { return fVerbose; }

  void SetEventTree(TTree* t) { fEventTree = t; }
  void SetETagReconstruction(ETagReconstruction* etReco) { fETagReconstruction = etReco; }

private:

  Bool_t ExportHit(ETagHit*,TRecoVHit*);
  Bool_t ExportCluster(ETagCluster*,TRecoVCluster*);

  ETagReconstruction* fETagReconstruction;

  TTree* fEventTree;

  TBranch* fBranchHitsColl;
  TBranch* fBranchClusColl;

  TETagRecoEvent*      fHitsCollection;
  TETagClusCollection* fClusCollection;

  Int_t fVerbose;

};
#endif // ETagRecoRootIO_h
