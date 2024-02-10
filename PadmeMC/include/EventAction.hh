#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "ECalHit.hh"
#include "TargetHit.hh"
#include "HEPVetoHit.hh"
#include "PVetoHit.hh"
#include "EVetoHit.hh"
#include "SACHit.hh"
#include "ETagHit.hh"
#include "TPixHit.hh"  //M. Raggi 26/03/2019
#include "BeWHit.hh"  //M. Raggi 29/04/2019
#include "MylarWHit.hh"  //M. Raggi 15/03/2021
#include "BeamFlagHit.hh"  //M. Raggi 30/08/2019

#include "DetectorConstruction.hh"
#include "ECalGeometry.hh"
#include "ETagGeometry.hh"
#include "TargetGeometry.hh"
#include "BeamParameters.hh"

class G4Event;
class RunAction;
class HistoManager;
class SteppingAction;

class TargetDigitizer;
class PVetoDigitizer;
class EVetoDigitizer;
class HEPVetoDigitizer;
class ECalDigitizer;
class SACDigitizer;
class TPixDigitizer;
class ETagDigitizer;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
  public:
  //  EventAction(RunAction*, HistoManager*);
  EventAction(RunAction*);
   ~EventAction();

  public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  
  void  AddSACHitsStep(G4double ,G4double , G4int , G4double , G4double, G4int);
  void  AddCalHitsStep(G4double ,G4double , G4int , G4double , G4double);

  SteppingAction * myStepping;

  G4int GetPrintoutfrequency() { return fPrintoutFrequency; }
  void SetPrintoutfrequency(G4int f) { fPrintoutFrequency = f; }

  // M. Raggi 23/06/2018 datacard commands
  void EnableSaveEcal()  { fEnableSaveEcal = 1; }
  void DisableSaveEcal() { fEnableSaveEcal = 0; }

  void EnableSaveSAC()  { fEnableSaveSAC = 1; }
  void DisableSaveSAC() { fEnableSaveSAC = 0; }

  void EnableSaveVeto()  { 
    fEnableSaveVeto = 1; 
    G4cout<< "saving Veto"<<G4endl;
  }
  void DisableSaveVeto() { fEnableSaveVeto = 0; }

  private:

  //M. Raggi 
  G4int fEnableSaveEcal;
  G4int fEnableSaveSAC;
  G4int fEnableSaveVeto;

  void  AddECryHits(ECalHitsCollection*);
  void  FindClusters();

  void  AddTargetHits(TargetHitsCollection*);
  void  AddHEPVetoHits(HEPVetoHitsCollection*); 
  void  AddPVetoHits(PVetoHitsCollection*);
  void  AddEVetoHits(EVetoHitsCollection*);

  void  AddSACHits(SACHitsCollection*);
  void  AddETagHits(ETagHitsCollection*);
  void  AddTPixHits(TPixHitsCollection*);         //M. Raggi 26/03/2019
  void  AddBeWHits(BeWHitsCollection*);           //M. Raggi 26/03/2019
  void  AddMylarWHits(MylarWHitsCollection*);        //M. Raggi 15/03/2021
  void  AddBeamFlagHits(BeamFlagHitsCollection*); //M. Raggi 30/08/2019

  G4double GetCharge(G4double Energia);
  //G4double GGMass();

  private:

  RunAction*    fRunAct;
  HistoManager* fHistoManager;

  G4int fPrintoutFrequency;

  ECalGeometry   * Egeom; 
  TargetGeometry * Tgeom;
  BeamParameters * Bpar;

  G4bool fFirstEvent;

  // Pointers to digitizers
  TargetDigitizer*  fTargetDigitizer;
  PVetoDigitizer*   fPVetoDigitizer;
  EVetoDigitizer*   fEvetoDigitizer;
  ETagDigitizer*    fETagDigitizer;
  HEPVetoDigitizer* fHEPVetoDigitizer;
  ECalDigitizer*    fECalDigitizer;
  SACDigitizer*     fSACDigitizer;
  TPixDigitizer*    fTPixDigitizer;

  //che devo fare ce debbo mettere il detector?
  G4double ETotCal;
  G4double EtotEVeto;
  G4double ProcID;
  G4double ECalHitT,CalEvtT,EtotFiltEvt; 
  G4double ClPosX,ClPosY;
  G4double ClTime,EClus,QClus,Theta,ClRadius,Mmiss2,ETotTra;
  G4int NcellsCl,NClusters,NTracks,NHEPVetoTracks,NPVetoTracks,NEVetoTracks,SACTracks,ETagTracks,NTarget,NBeW,NBeamFlag,NMylarW;
  G4int CalNPart;

   G4double Etrack[300];    //For spectrometer reco
   G4int    TrackCh[300];      //For spectrometer reco
   G4double TrackerLay[300];     //For spectrometer reco
   G4double TrackTime[300];
   G4double TrackZ[300];
   G4double TrackX[300];
   G4double TrackY[300];

   G4double ETarget;
   G4double TTarget;
   G4double YTarget;
   G4double XTarget;

   G4double EBeW;
   G4double TBeW;
   G4double YBeW;
   G4double XBeW;

   G4double EMylarW;
   G4double TMylarW;
   G4double YMylarW;
   G4double XMylarW;

   G4double EBeamFlag;
   G4double TBeamFlag;
   G4double YBeamFlag;
   G4double XBeamFlag;

   G4double ETotHEPVeto[300];
   G4int    HEPVetoTrackCh[300];
   G4double HEPVetoEtrack [300];
   G4double HEPVetoTrackTime[300];
   G4double HEPVetoX[300];
   G4double HEPVetoY[300];
   G4double HEPVetoTimeCl[100][10 ];
   G4double HEPVetoECl[100][10];
   G4int    HEPVetoClIndex[300];

   G4double ETotPVeto[300];
   G4double TimePVeto[300];
   G4int    PVetoTrackCh[300];
   G4double PVetoEtrack [300];
   G4double PVetoTrackTime[300];
   G4double PVetoX[300];
   G4double PVetoY[300];
   G4double PVetoTimeCl[100][10 ];
   G4double PVetoECl[100][10];
   G4int    PVetoClIndex[300];

   G4double ETotEVeto[300];
   G4int    EVetoTrackCh[300];
   G4double EVetoEtrack [300];
   G4double EVetoTrackTime[300];
   G4double EVetoX[300];
   G4double EVetoY[300];
   G4double EVetoTimeCl[100][10];
   G4double EVetoECl[100][10];
   G4int    EVetoClIndex[300];


   G4double ETotSAC[300];
   G4double SACEtrack [300];
   G4double SACTrackTime[300];
   G4double SACPType[300];
   G4double SACX[300];   
   G4double SACY[300];
   G4double SACCh[300];

  //Particles properties entering ECAL form stepping action
   G4double CalE[300];
   G4double CalTime[300];
   G4int    CalPType[300];
   G4double CalX[300];   
   G4double CalY[300];
   G4double NCalPart;

   G4double ETotETag[300];
   G4int ETagNBar[300];
   G4double ETagPType[300];
   G4double ETagEtrack [300];
   G4double ETagTrackTime[300];
   G4double ETagX[300];   
   G4double ETagY[300];

   G4double EneCl[300];
   G4double QCl[300];
   G4double TimeCl[300];
   G4double XCl[300];      //For the Ntuple
   G4double YCl[300];      //For the Ntuple
   G4double ThCl[300];     //For the Ntuple
   G4double MM2[300];      //For the Ntuple
   G4double NCellsCl[300]; //For the Ntuple

   G4double ETotCry[1000];
   G4double QTotCry[1000];
   G4double TimeCry[1000];
   G4double ETotRing[1000];

   G4double MatEtot[30][30];   
   G4double MatQtot[30][30];   
   G4double MatTstart[30][30]; 
   G4int    MatUsed[30][30];   
  G4float fECalWaveForm[30][30][1024];
  

   G4int Used[1000];
   G4int UsedRing[1000];
   G4int Empty[1000];
   G4int Neig[1000];
   G4double CalEvtPosX,CalEvtPosY;
};

#endif

    
