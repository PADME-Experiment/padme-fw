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
#include "SACGeometry.hh"
#include "TargetGeometry.hh"
#include "BeamParameters.hh"
#include <vector>

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

  // M. Raggi 23/06/2018 datacard commands
  void EnableSaveEcal()  { fEnableSaveEcal = 1; }
  void DisableSaveEcal() { fEnableSaveEcal = 0; }

  void EnableSaveSAC()  { fEnableSaveSAC = 1; }
  void DisableSaveSAC() { fEnableSaveSAC = 0; }

  void EnableSaveVeto() { fEnableSaveVeto = 1; }
  void DisableSaveVeto(){ fEnableSaveVeto = 0; }

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
  void  AddLAVHits(LAVHitsCollection*);

  G4double GetCharge(G4double Energia);
  G4double GetSACCharge(G4double Energia);
  G4double GGMass();

  private:
  RunAction*    fRunAct;
  HistoManager* fHistoManager;
  ECalGeometry   * Egeom; 
  TargetGeometry * Tgeom;
  SACGeometry * Sgeom;
  BeamParameters * Bpar;

  //che devo fare ce debbo mettere il detector?
  G4double ETotCal;
  G4double EtotEVeto;
  G4double ProcID;
  G4double ECalHitT,CalEvtT,EtotFiltEvt; 
  G4double ClPosX,ClPosY;
  //  G4double ClTime;
  G4double EClus,QClus,Theta,ClRadius,Mmiss2,ETotTra;
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


   //introduced vectors to sobstitue arrays M. Raggi 28/06/2018
   std::vector<G4int>     HEPVetoTrackCh;
   std::vector<G4double>  HEPVetoEtrack ;
   std::vector<G4double>  HEPVetoTrackTime;
   std::vector<G4double>  HEPVetoX;
   std::vector<G4double>  HEPVetoY;

   G4double ETotHEPVeto[100];
   G4double HEPVetoTimeCl[100][10 ];
   G4double HEPVetoECl[100][10];
   G4int    HEPVetoClIndex[100];

  //introduced vectors to sobstitue arrays M. Raggi 28/06/2018
  std::vector<G4int>     PVetoTrackCh;  //int
  std::vector<G4double>  PVetoEtrack;
  std::vector<G4double>  PVetoTrackTime;
  std::vector<G4double>  PVetoX;
  std::vector<G4double>  PVetoY;
  //std::vector<double> ETotPVeto(100);
  // This part I did'nt touched 
  G4double  ETotPVeto[100];
  G4double  TimePVeto[100];
  G4double  PVetoTimeCl[100][10];
  G4double  PVetoECl[100][10];
  G4int     PVetoClIndex[100];  //int


  std::vector<G4int>     EVetoTrackCh;
  std::vector<G4double>  EVetoEtrack ;
  std::vector<G4double>  EVetoTrackTime;
  std::vector<G4double>  EVetoX;
  std::vector<G4double>  EVetoY;

   G4double ETotEVeto[100];
   G4double EVetoTimeCl[100][10];
   G4double EVetoECl[100][10];
   G4int    EVetoClIndex[100];

   G4double  ETotSACCh[25][100];
   std::vector<G4double>  SACEtrack;
   std::vector<G4double>  SACTrackTime;
   std::vector<int>       SACPType;
   std::vector<G4double>  SACX;   
   std::vector<G4double>  SACY;
   std::vector<int>       SACCh;
   std::vector<G4double>  SACQ;
  
  //Particles properties entering ECAL form stepping action to be checked
  std::vector<G4double> CalE;
  std::vector<G4double> CalTime;
  std::vector<G4int>    CalPType;
  std::vector<G4double> CalX;   
  std::vector<G4double> CalY;
  G4int    NCalPart;

  // not used 
  std::vector<G4double> ETotLAV;
  std::vector<G4double> LAVTrackCh;
  std::vector<G4double> LAVPType;
  std::vector<G4double> LAVEtrack ;
  std::vector<G4double> LAVTrackTime;
  std::vector<G4double> LAVX;   
  std::vector<G4double> LAVY;
  
   std::vector<G4double> EneCl;     
   std::vector<G4double> QCl;	   
   std::vector<G4double> TimeCl;  
   std::vector<G4double> XCl;     
   std::vector<G4double> YCl;     
   std::vector<G4double> ThCl;    
   std::vector<G4double> MM2;     
   std::vector<G4int> NCellsCl;
  
   std::vector<G4double> ETotCry;
   std::vector<G4double> QTotCry;
   std::vector<G4double> TimeCry;
  //std::vector<G4double> ETotRing;

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

    
