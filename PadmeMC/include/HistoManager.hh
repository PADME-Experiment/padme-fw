#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
       	       	       
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#define NPVetoBars   100
#define NEVetoBars   100
#define NHEPVetoBars 100
#define NPVetoMaxNCl 10
#define NEVetoMaxNCl 10
#define NHEPVetoMaxNCl 10


struct NTEvent{
  int NTNevent;
  double NTEtot;
  double NTPBeam;
  double NTPriBeam;
  int NTNtotCell;
  double NTXBeam;
  double NTYBeam;
  G4double PMomX;
  G4double PMomY;
  G4double PMomZ;

  double NTIDProc;
  int NTNCluster;
  int NTNTracks;	
  int NTNHEPVetoTracks;
  int NTNPVetoTracks;
  int NTNEVetoTracks;

  //The generated event variables:
  double  PrimE;
  //Simulated event variables

  //Calorimeter variables:
  double NTECluster[300];
  double NTQCluster[300];
  double NTXCluster[300];
  double NTYCluster[300];
  double NTThCluster[300];
  double NTM2Cluster[300];
  double NTTCluster[300];
  double NTNClusCells[300];

  double NTECell[1000];
  double NTQCell[1000];
  double NTTCell[1000];

  //NTLAV variables
  double  NTCalPartE[300];
  double  NTCalPartT[300];
  int     NTCalPartPType[300];
  double  NTCalPartX[300];
  double  NTCalPartY[300];
  int     NTNCal;

  //Tracker variables
  double NTETracker[300];
  double NTTrackerLay[300];
  double NTTrackerTime[300];
  double NTTrackerZ[300];
  double NTTrackerX[300];
  double NTTrackerY[300];

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

  //HEPVeto variables
  double  NTHEPVetoTrkEne[300];
  int  NTHEPVetoTrkFinger[300];
  double NTHEPVetoTrkTime[300];
  double NTHEPVetoFingerE[300];
  double NTHEPVetoX[300];
  double NTHEPVetoY[300];
  double NTHEPVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTHEPVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTHEPVetoClIndex[NPVetoBars];

  //PVeto variables
  double NTPVetoTrkEne[300];
  int    NTPVetoTrkFinger[300];
  double NTPVetoTrkTime[300];
  double NTPVetoFingerE[300];
  double NTPVetoX[300];
  double NTPVetoY[300];
  double NTPVetoBarEnergy[NPVetoBars];
  double NTPVetoBarTime[NPVetoBars];
  double NTPVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTPVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTPVetoClIndex[NPVetoBars];


  //EVeto variables
  double NTEVetoTrkEne[300];
  int    NTEVetoTrkFinger[300];
  double NTEVetoTrkTime[300];
  double NTEVetoFingerE[300];
  double NTEVetoX[300];
  double NTEVetoY[300];
  double NTEVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTEVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTEVetoClIndex[NPVetoBars];

  //NTSAC variables
  double  NTSACE[300];
  double  NTSACT[300];
  double  NTSACPType[300];
  double  NTSACX[300];
  double  NTSACY[300];
  int     NTSACCh[300];
  int     NTSACNHit;

  //NTLAV variables
  double  NTLAVE[300];
  double  NTLAVT[300];
  double  NTLAVPType[300];
  double  NTLAVX[300];
  double  NTLAVY[300];
  int     NTLAVNHit;




}; 

 class TFile;
 class TTree;
 class TH1D;
 class TH2D;

 const G4int MaxHisto = 200;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager
{

protected:
  HistoManager();
public:
  ~HistoManager();
  static HistoManager* GetInstance();
private:
  static HistoManager* fInstance;
public:
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

