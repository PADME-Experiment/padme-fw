#ifndef ETagAn_h
#define ETagAn_h 1

#include "TObject.h"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "ECalSel.hh"

using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;

#define maxETagAssPerSide 10

struct ETagECalAss{
  int indexECal; // indices of the ecal cluster associated
  int nETagBar; // index of ETagBar identified from the ECal cluster
  int nAss[2]; // number of Left,Right SiPM associations
  int iAss[2][maxETagAssPerSide]; //indices of the Left,Right SiPM hits associated 
  double avgTime[2];  
  double rmsTime[2];  
};


class ETagAn : public TObject
{

protected:
  ETagAn();
public:
  ~ETagAn();
  static ETagAn* GetInstance();
private:
  static ETagAn* fInstance;
public:
  virtual Bool_t Init(PadmeAnalysisEvent* event);
  Bool_t Finalize();
  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  ETagECalAss getETagECalAss(int i){return fETagECalAss.at(i);} //to be protected

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVClusCollection* fECal_clEvent    ;
  TRecoVObject*         fETag_hitEvent   ;
  TRecoVClusCollection* fETag_clEvent    ;

  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  Int_t    fVerbose;

private:
  Double_t fdistanceTarget;
  Int_t ETagMatch();
  Int_t ETagMatchSave();
  ETagECalAss AssociateECalCluster(int indexCl, int toff);

  std::vector<ETagECalAss> fETagECalAss; // associations of ECal clusters

  int HitToClusterMapLeft[1500];
  int HitToClusterMapRight[1500];

  HistoSvc* fhSvcVal;
  ECalSel* fECalSel;
  TString fLabel[2]; // Left, Right
  TString fOffLabel;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
