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
  double NTECluster[20];
  double NTQCluster[20];
  double NTXCluster[20];
  double NTYCluster[20];
  double NTThCluster[20];
  double NTM2Cluster[20];
  double NTTCluster[20];
  double NTNClusCells[20];

  double NTECell[1000];
  double NTQCell[1000];
  double NTTCell[1000];

  //NTLAV variables
  double  NTCalPartE[20];
  double  NTCalPartT[20];
  int     NTCalPartPType[20];
  double  NTCalPartX[20];
  double  NTCalPartY[20];
  int     NTNCal;

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

  //HEPVeto variables
  double  NTHEPVetoTrkEne[100];
  int  NTHEPVetoTrkFinger[100];
  double NTHEPVetoTrkTime[100];
  double NTHEPVetoFingerE[100];
  double NTHEPVetoX[100];
  double NTHEPVetoY[100];
  double NTHEPVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTHEPVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTHEPVetoClIndex[NPVetoBars];

  //PVeto variables
  double NTPVetoTrkEne[100];
  int    NTPVetoTrkFinger[100];
  double NTPVetoTrkTime[100];
  double NTPVetoFingerE[100];
  double NTPVetoX[100];
  double NTPVetoY[100];
  double NTPVetoBarEnergy[NPVetoBars];
  double NTPVetoBarTime[NPVetoBars];
  double NTPVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTPVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTPVetoClIndex[NPVetoBars];


  //EVeto variables
  double NTEVetoTrkEne[100];
  int    NTEVetoTrkFinger[100];
  double NTEVetoTrkTime[100];
  double NTEVetoFingerE[100];
  double NTEVetoX[100];
  double NTEVetoY[100];
  double NTEVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTEVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTEVetoClIndex[NPVetoBars];

  //NTSAC variables
  double  NTSACE[100];
  double  NTSACT[100];
  double  NTSACPType[100];
  double  NTSACX[100];
  double  NTSACY[100];
  int     NTSACCh[100];
  int     NTSACNHit;

  //NTLAV variables
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

