#ifndef PVetoRootRecoIO_h
#define PVetoRootRecoIO_h 1

#include "RecoVRootIO.hh"
#include "TPVetoRecoEvent.hh"
#include "RecoRootIOManager.hh"

#include "TTree.h"
#include "TBranch.h"



class TPVetoRecoEvent;
class RecoVRootIO;

class PVetoRecoRootIO : public RecoVRootIO
{
public:

  PVetoRecoRootIO();
  virtual ~PVetoRecoRootIO();

  static PVetoRecoRootIO* GetInstance();


  void NewRun(Int_t, TFile*);
  void EndRun();
  void SaveEvent();
  void Close();

private:

  TTree* fEventTree;
  TBranch* fPVetoBranch;
  TPVetoRecoEvent* fEvent;

  

};
#endif // PVetoRecoRootIO_h
