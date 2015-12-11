//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: EventAction.hh,v 1.4 2014/06/23 13:44:14 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "ECalHit.hh"
#include "MRodHit.hh"
#include "TrackerHit.hh"
#include "TRodHit.hh"
#include "EVetoHit.hh"
#include "PosVetoHit.hh"
#include "EleVetoHit.hh"
#include "SACHit.hh"
#include "LAVHit.hh"
#include "GFiltHit.hh"
#include "DetectorConstruction.hh"

class G4Event;
class RunAction;
class HistoManager;
class SteppingAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
  public:
  EventAction(RunAction*, HistoManager*);
   ~EventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
  SteppingAction * myStepping;
  private:
  void  AddECryHits(ECalHitsCollection*);
  void  AddMRodHits(MRodHitsCollection*);
  void  AddTRodHits(TRodHitsCollection*);
  void  AddEVetoHits(EVetoHitsCollection*); 
  void  AddPosVetoHits(PosVetoHitsCollection*);
  void  AddEleVetoHits(EleVetoHitsCollection*);
  void  AddSACHits(SACHitsCollection*);
  void  AddLAVHits(LAVHitsCollection*);
  void  AddTrackerHits(TrackerHitsCollection*);

  G4double GetCryPosX(G4int CryNb);
  G4double GetCryPosY(G4int CryNb);
  G4int    GetSeedCell();
  G4int    GetSeedRing();
  G4double GetEClus(G4int SeedCell);
  G4double GetETrack(){return 0.;};
  G4double GetETrack(G4int SeedTrack);
  G4double GetCharge(G4double Energia);
  G4int GetNeig(G4int SeedCell);
  G4int GetNeigPos(G4int SeedCell);

  //  G4double Dot(G4double* v1,G4double* v1,G4int N)
  private:
   RunAction*    fRunAct;
   HistoManager* fHistoManager;
  //che devo fare ce debbo mettere il detector?
   G4double ETotCal;
   G4double EtotTRod; 
   G4double EtotMRod;
   G4double EtotEVeto;
   G4double ProcID;
   G4double ECalHitT,CalEvtT,EtotFiltEvt; 
   G4double ClPosX,ClPosY;
   G4double ClTime,EClus,QClus,Theta,ClRadius,Mmiss2,ETotTra;
  G4int NcellsCl,NClusters,NTracks,NVetoTracks,NPosVetoTracks,NEleVetoTracks,SACTracks,LAVTracks,NTarget;

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

   G4double ETotVeto[100];
   G4int    VetoTrackCh[100];
   G4double VetoEtrack [100];
   G4double VetoTrackTime[100];
   G4double VetoX[100];
   G4double VetoY[100];

   G4double PosETotVeto[100];
   G4int    PosVetoTrackCh[100];
   G4double PosVetoEtrack [100];
   G4double PosVetoTrackTime[100];
   G4double PosVetoX[100];
   G4double PosVetoY[100];

   G4double EleETotVeto[100];
   G4int    EleVetoTrackCh[100];
   G4double EleVetoEtrack [100];
   G4double EleVetoTrackTime[100];
   G4double EleVetoX[100];
   G4double EleVetoY[100];

   G4double ETotSAC[100];
   G4double SACTrackCh[100];
   G4double SACPType[100];
   G4double SACEtrack [100];
   G4double SACTrackTime[100];
   G4double SACX[100];   
   G4double SACY[100];

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
   G4double ETotCry[1000];
   G4double QTotCry[1000];
   G4double TimeCry[1000];
   G4double ETotRing[1000];
   G4int Used[1000];
   G4int UsedRing[1000];
   G4int Empty[1000];
   G4int Neig[1000];
   G4double CalEvtPosX,CalEvtPosY;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
