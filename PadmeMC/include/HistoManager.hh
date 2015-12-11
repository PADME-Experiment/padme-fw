//
// ********************************************************************
// * License and Disclaimer                                            *
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
/// \file analysis/AnaEx02/include/HistoManager.hh
/// \brief Definition of the HistoManager class
//
// $Id: HistoManager.hh,v 1.6 2014/06/23 13:44:14 veni Exp $
// GEANT4 tag $Name:  $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
       	       	       
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

struct NTEvent{
  int NTNevent;
  double NTEtot;
  double NTPBeam;
  double NTPriBeam;
  int NTNtotCell;
  double NTXBeam;
  double NTYBeam;
  
  double NTIDProc;
  int NTNCluster;
  int NTNTracks;	
  int NTNVetoTracks;

  int NTNPosVetoTracks;
  int NTNEleVetoTracks;

  //The generated event variables:
  double  PrimE;
  //Simulated event variables

  //Calorimeter variables:
  double NTECluster[20];
  double NTQCluster[20];
  double NTXCluster[20];
  double NTYCluster[20];
  double NTThCluster[20];
  double NTM2Cluster[20];
  double NTTCluster[20];

  double NTECell[1000];
  double NTQCell[1000];
  double NTTCell[1000];

  //Tracker variables
  double NTETracker[100];
  double NTTrackerLay[100];
  double NTTrackerTime[100];
  double NTTrackerZ[100];
  double NTTrackerX[100];
  double NTTrackerY[100];

  double NTETarget;
  double NTTTarget;
  double NTXTarget;
  double NTYTarget;
  int NTNTarget;

  //Tracker variables
  int     NTNTrClus;
  double  NTTrClusX[1000];
  double  NTTrClusY[1000];
  double  NTTrClusZ[1000];
  int     NTTrClusLayer[1000];

  //EVeto variables
  double  NTVetoTrkEne[100];
  int  NTVetoTrkFinger[100];
  double NTVetoTrkTime[100];
  double NTVetoFingerE[100];
  double NTVetoX[100];
  double NTVetoY[100];

  //Pos Veto variables
  double NTPosVetoTrkEne[100];
  int    NTPosVetoTrkFinger[100];
  double NTPosVetoTrkTime[100];
  double NTPosVetoFingerE[100];
  double NTPosVetoX[100];
  double NTPosVetoY[100];

  //Ele Veto variables
  double NTEleVetoTrkEne[100];
  int    NTEleVetoTrkFinger[100];
  double NTEleVetoTrkTime[100];
  double NTEleVetoFingerE[100];
  double NTEleVetoX[100];
  double NTEleVetoY[100];

  //NTSAC variables
  double  NTSACE[100];
  double  NTSACT[100];
  double  NTSACPType[100];
  double  NTSACX[100];
  double  NTSACY[100];
  int     NTSACNHit;

  //NTSAC variables
  double  NTLAVE[100];
  double  NTLAVT[100];
  double  NTLAVPType[100];
  double  NTLAVX[100];
  double  NTLAVY[100];
  int     NTLAVNHit;
}; 

 class TFile;
 class TTree;
 class TH1D;
 class TH2D;

 const G4int MaxHisto = 40;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager
{
  public:
  
    HistoManager();
   ~HistoManager();
   
    void book();
    void save();

    void FillHisto(G4int id, G4double bin, G4double weight = 1.0);
    void FillHisto2(G4int id, G4double bin, G4double ybin, G4double weight = 1.0);
    void Normalize(G4int id, G4double fac);    

    void FillNtuple(NTEvent* Evt);
    
    void PrintStatistic();

  void FillGenEvent(){};
  void FillSimEvent(){};



  public:
    NTEvent  myEvt;    
  private:
    
    TFile*   rootFile;
    TH1D*    histo[MaxHisto];            
    TH2D*    histo2[MaxHisto];            
    TTree*   ntupl;    

  TTree* ntSim;
  TTree* ntGen;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

