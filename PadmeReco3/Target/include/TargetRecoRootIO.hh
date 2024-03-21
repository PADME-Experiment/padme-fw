#ifndef TargetRecoRootIO_h
#define TargetRecoRootIO_h 1

#include "TTargetRecoEvent.hh"
//#include "TTargetClusCollection.hh"

class TTree;
class TBranch;
class TargetReconstruction;
//class TargetHit;
//class TargetCluster;
class TargetStripCharge;

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

  //  Bool_t ExportHit(TargetHit*,TRecoVHit*);
  //Bool_t ExportCluster(TargetCluster*,TRecoVCluster*);
  Bool_t ExportHit(TargetStripCharge*,TRecoVHit*);

  TargetReconstruction* fTargetReconstruction;

  TTree* fEventTree;

  TBranch* fBranchStripChargeColl;
  //TBranch* fBranchClusColl;

  TTargetRecoEvent*      fStripChargeCollection;
  //TTargetClusCollection* fClusCollection;

  Int_t fVerbose;

};
#endif // TargetRecoRootIO_h
