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
  std::vector<double> NTECluster;
  std::vector<double> NTQCluster;
  std::vector<double> NTXCluster;
  std::vector<double> NTYCluster;
  std::vector<double> NTThCluster;
  std::vector<double> NTM2Cluster;
  std::vector<double> NTTCluster;
  std::vector<double> NTNClusCells;

  std::vector<double> NTECell;
  std::vector<double> NTQCell;
  std::vector<double> NTTCell;

  //True ACal values from steppin
  std::vector<double> NTCalPartE;
  std::vector<double> NTCalPartT;
  std::vector<int>    NTCalPartPType;
  std::vector<double> NTCalPartX;
  std::vector<double> NTCalPartY;
  int    NTNCal;
  
  //Tracker variables  are these variables obsolite?
//  double NTETracker[100];
//  double NTTrackerLay[100];
//  double NTTrackerTime[100];
//  double NTTrackerZ[100];
//  double NTTrackerX[100];
//  double NTTrackerY[100];

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
  std::vector<G4double>  NTHEPVetoTrkEne;
  std::vector<G4double>  NTHEPVetoTrkTime;
  std::vector<int>       NTHEPVetoTrkFinger;
  std::vector<G4double>  NTHEPVetoFingerE;
  std::vector<G4double>  NTHEPVetoX;
  std::vector<G4double>  NTHEPVetoY;

  double NTHEPVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTHEPVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTHEPVetoClIndex[NPVetoBars];

  //PVeto variables changed to vectors
  std::vector<G4double>  NTPVetoTrkEne;
  std::vector<G4double>  NTPVetoTrkTime;
  std::vector<int>       NTPVetoTrkFinger;
  std::vector<G4double>  NTPVetoFingerE;
  std::vector<G4double>  NTPVetoX;
  std::vector<G4double>  NTPVetoY;
  //  std::vector<G4double>  NTPVetoBarEnergy;
  //  std::vector<G4double>  NTPVetoBarTime;
  double NTPVetoBarEnergy[NPVetoBars];
  double NTPVetoBarTime[NPVetoBars];
  double NTPVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTPVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTPVetoClIndex[NPVetoBars];

  //EVeto variables
  std::vector<G4double> NTEVetoTrkEne;
  std::vector<G4double> NTEVetoTrkTime;
  std::vector<int>      NTEVetoTrkFinger;
  std::vector<G4double> NTEVetoFingerE;
  std::vector<G4double> NTEVetoX;
  std::vector<G4double> NTEVetoY;

  double NTEVetoTimeCl[NPVetoBars][NPVetoMaxNCl];
  double NTEVetoECl[NPVetoBars][NPVetoMaxNCl];
  int    NTEVetoClIndex[NPVetoBars];

  //NTSAC variables
  std::vector<double> NTSACE;
  std::vector<double> NTSACT;
  std::vector<double> NTSACPType;
  std::vector<double> NTSACX;
  std::vector<double> NTSACY;
  std::vector<double> NTSACQ;  //added SAC charge
  std::vector<int>    NTSACCh;
  int NTSACNHit;

  //NTLAV variables
  std::vector<double>  NTLAVE;
  std::vector<double>  NTLAVT;
  std::vector<int>     NTLAVPType;
  std::vector<double>  NTLAVX;
  std::vector<double>  NTLAVY;
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

