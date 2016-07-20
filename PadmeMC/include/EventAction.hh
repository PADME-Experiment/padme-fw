#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "ECalHit.hh"
#include "TargetHit.hh"
#include "TrackerHit.hh"
#include "HEPVetoHit.hh"
#include "PVetoHit.hh"
#include "EVetoHit.hh"
#include "SACHit.hh"
#include "LAVHit.hh"
#include "GFiltHit.hh"
#include "DetectorConstruction.hh"
#include "ECalGeometry.hh"
#include "TargetGeometry.hh"
#include "BeamParameters.hh"

class G4Event;
class RunAction;
class HistoManager;
class SteppingAction;

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

  private:
  void  AddECryHits(ECalHitsCollection*);
  void  FindClusters();

  void  AddTargetHits(TargetHitsCollection*);
  void  AddHEPVetoHits(HEPVetoHitsCollection*); 
  void  AddPVetoHits(PVetoHitsCollection*);
  void  AddEVetoHits(EVetoHitsCollection*);

  void  AddSACHits(SACHitsCollection*);
  void  AddLAVHits(LAVHitsCollection*);

  G4double GetCharge(G4double Energia);
  G4double GGMass();

  private:
  RunAction*    fRunAct;
  HistoManager* fHistoManager;
  ECalGeometry   * Egeom; 
  TargetGeometry * Tgeom;
  BeamParameters * Bpar;

  //che devo fare ce debbo mettere il detector?
  G4double ETotCal;
  G4double EtotEVeto;
  G4double ProcID;
  G4double ECalHitT,CalEvtT,EtotFiltEvt; 
  G4double ClPosX,ClPosY;
  G4double ClTime,EClus,QClus,Theta,ClRadius,Mmiss2,ETotTra;
  G4int NcellsCl,NClusters,NTracks,NHEPVetoTracks,NPVetoTracks,NEVetoTracks,SACTracks,LAVTracks,NTarget;
  G4int CalNPart;

   G4double Etrack[100];    //For spectrometer reco
   G4int    TrackCh[100];      //For spectrometer reco
   G4double TrackerLay[100];     //For spectrometer reco
   G4double TrackTime[100];
   G4double TrackZ[100];
   G4double TrackX[100];
   G4double TrackY[100];

   G4double ETarget;
   G4double TTarget;
   G4double YTarget;
   G4double XTarget;

   G4double ETotHEPVeto[100];
   G4int    HEPVetoTrackCh[100];
   G4double HEPVetoEtrack [100];
   G4double HEPVetoTrackTime[100];
   G4double HEPVetoX[100];
   G4double HEPVetoY[100];
   G4double HEPVetoTimeCl[100][10 ];
   G4double HEPVetoECl[100][10];
   G4int    HEPVetoClIndex[100];

   G4double ETotPVeto[100];
   G4double TimePVeto[100];
   G4int    PVetoTrackCh[100];
   G4double PVetoEtrack [100];
   G4double PVetoTrackTime[100];
   G4double PVetoX[100];
   G4double PVetoY[100];
   G4double PVetoTimeCl[100][10 ];
   G4double PVetoECl[100][10];
   G4int    PVetoClIndex[100];

   G4double ETotEVeto[100];
   G4int    EVetoTrackCh[100];
   G4double EVetoEtrack [100];
   G4double EVetoTrackTime[100];
   G4double EVetoX[100];
   G4double EVetoY[100];
   G4double EVetoTimeCl[100][10];
   G4double EVetoECl[100][10];
   G4int    EVetoClIndex[100];

   G4double ETotSAC[100];

   G4double SACEtrack [100];
   G4double SACTrackTime[100];
   G4double SACPType[100];
   G4double SACX[100];   
   G4double SACY[100];
   G4double SACCh[100];

  //Particles properties entering ECAL form stepping action
   G4double CalE[20];
   G4double CalTime[20];
   G4int    CalPType[20];
   G4double CalX[20];   
   G4double CalY[20];
   G4double NCalPart;

   G4double ETotLAV[100];
   G4double LAVTrackCh[100];
   G4double LAVPType[100];
   G4double LAVEtrack [100];
   G4double LAVTrackTime[100];
   G4double LAVX[100];   
   G4double LAVY[100];

   G4double EneCl[20];
   G4double QCl[20];
   G4double TimeCl[20];
   G4double XCl[20]; //For the Ntuple
   G4double YCl[20]; //For the Ntuple
   G4double ThCl[20];//For the Ntuple
   G4double MM2[20]; //For the Ntuple
   G4double NCellsCl[20]; //For the Ntuple

   G4double ETotCry[1000];
   G4double QTotCry[1000];
   G4double TimeCry[1000];
   G4double ETotRing[1000];

   G4double MatEtot[30][30];   
   G4double MatQtot[30][30];   
   G4double MatTstart[30][30]; 
   G4int    MatUsed[30][30];   
  

   G4int Used[1000];
   G4int UsedRing[1000];
   G4int Empty[1000];
   G4int Neig[1000];
   G4double CalEvtPosX,CalEvtPosY;
};

#endif

    
