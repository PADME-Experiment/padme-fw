// Written by M. Raggi 20/12/2018
// M. Raggi Methods to switch on/off different reco in RootIo 
#ifndef ReadConf_h
#define ReadConf_h

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"

class ReadConf{

public:
  ~ReadConf();
  static ReadConf* GetInstance();
  void ReadConfig();

  //M. Raggi Methods to switch on/off different reco in RootIo 
  Bool_t IsTargetON(){return fTargetisON;};
  Bool_t IsEVetoON(){return fEVetoisON;};
  Bool_t IsPVetoON(){return fPVetoisON;};
  Bool_t IsHEPVetoON(){return fHEPVetoisON;};
  Bool_t IsECalON(){return fECalisON;};
  Bool_t IsSACON(){return fSACisON;};
  Bool_t IsTPixON(){return fTPixisON;};
  
  Bool_t IsMonitorMode(){return fUseMonitorMode;};
  Bool_t IsFullMode(){return fUseFullMode;};
  

private:
  static ReadConf* fInstance;

  Bool_t fTargetisON;
  Bool_t fEVetoisON;  
  Bool_t fPVetoisON;
  Bool_t fHEPVetoisON;
  Bool_t fECalisON;
  Bool_t fSACisON;
  Bool_t fTPixisON;
  
  Bool_t fUseMonitorMode;
  Bool_t fUseFullMode;
 
protected:
  ReadConf();

};
#endif
