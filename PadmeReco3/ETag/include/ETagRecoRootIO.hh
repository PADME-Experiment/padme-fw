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

  Bool_t GetEnabled() { return fEnabled; }
  void SetEnabled(Bool_t value) { fEnabled = value; }

  void SetETagReconstruction(ETagReconstruction* etReco) { fETagReconstruction = etReco; }

private:

  Bool_t ExportHit(ETagHit*,TRecoVHit*);
  Bool_t ExportCluster(ETagCluster*,TRecoVCluster*);

  Bool_t fEnabled;

  TTree* fEventTree;

  TBranch* fBranchHitsColl;
  TBranch* fBranchClusColl;

  TETagRecoEvent*      fHitsCollection;
  TETagClusCollection* fClusCollection;

  ETagReconstruction* fETagReconstruction;

};
#endif // ETagRecoRootIO_h
