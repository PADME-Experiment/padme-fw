#ifndef LeadGlassRecoRootIO_h
#define LeadGlassRecoRootIO_h 1

class TTree;
class TBranch;
class TLeadGlassRecoEvent;
class LeadGlassReconstruction;

class LeadGlassRecoRootIO
{
public:

  LeadGlassRecoRootIO();
  ~LeadGlassRecoRootIO();

  void NewRun();
  void EndRun();
  void SaveEvent();

  void  SetVerbose(Int_t v) { fVerbose = v;    }
  Int_t GetVerbose()        { return fVerbose; }

  void SetEventTree(TTree* t) { fEventTree = t; }
  void SetLeadGlassReconstruction(LeadGlassReconstruction* lgReco) { fLeadGlassReconstruction = lgReco; }

private:

  LeadGlassReconstruction* fLeadGlassReconstruction;

  TTree* fEventTree;
  TBranch* fBranchHitsColl;

  TLeadGlassRecoEvent* fLeadGlassEvent;

  Int_t fVerbose;

};
#endif // LeadGlassRecoRootIO_h
