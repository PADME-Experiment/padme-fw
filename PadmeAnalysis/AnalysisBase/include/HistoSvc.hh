#ifndef HistoSvc_h
#define HistoSvc_h 1

#include <map>
//#include "globals.hh"
       	       	       
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  #define NPVetoBars   100
  #define NEVetoBars   100
  #define NHEPVetoBars 100
  #define NPVetoMaxNCl 10
  #define NEVetoMaxNCl 10
  #define NHEPVetoMaxNCl 10

  #define NPVetoMaxHits   1000
  #define NEVetoMaxHits   1000
  #define NHEPVetoMaxHits 1000
  #define NSACMaxHits     1000
  #define NECalMaxHits    1000
  #define NTargetMaxHits  1000

  #define NPVetoMaxClusters     1000
  #define NEVetoMaxClusters     1000
  #define NHEPVetoMaxClusters   1000 
  #define NSACMaxClusters       1000
  #define NECalMaxClusters      1000


  struct NTEvent{
  int NTNevent;
  double NTEtot;
  double NTPBeam;
  double NTPriBeam;
  int NTNtotCell;
  double NTXBeam;
  double NTYBeam;
  double PMomX;
  double PMomY;
  double PMomZ;

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

  double NTTargetBeamX           ;
  double NTTargetBeamXErr        ; 
  double NTTargetBeamXW          ;
  double NTTargetBeamXWErr       ;
  double NTTargetBeamXfit        ;
  double NTTargetBeamXfitErr     ;
  double NTTargetBeamXWfit       ;
  double NTTargetBeamXWfitErr    ;
  double NTTargetBeamXChi2       ;
  double NTTargetBeamXNdof       ;
  double NTTargetBeamXCharge     ;
  double NTTargetBeamXChargeErr  ;
  double NTTargetBeamXTime       ;
  double NTTargetBeamXTimeErr    ;
  	                         
  double NTTargetBeamY           ;
  double NTTargetBeamYErr	 ;
  double NTTargetBeamYW		 ;
  double NTTargetBeamYWErr	 ;
  double NTTargetBeamYfit	 ;
  double NTTargetBeamYfitErr	 ;
  double NTTargetBeamYWfit	 ;
  double NTTargetBeamYWfitErr	 ;
  double NTTargetBeamYChi2	 ;
  double NTTargetBeamYNdof	 ;
  double NTTargetBeamYCharge	 ;
  double NTTargetBeamYChargeErr	 ;
  double NTTargetBeamYTime	 ;
  double NTTargetBeamYTimeErr	 ;
  double NTTargetBeamNPOT	 ;
  double NTTargetBeamNPOTErr	 ;


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

   //for flat Ntuple

  int NTNPVeto_Hits;
  int NTNPVeto_Clusters;
  int NTNEVeto_Hits;
  int NTNEVeto_Clusters;
  int NTNHEPVeto_Hits;
  int NTNHEPVeto_Clusters;
  int NTNECal_Hits;
  int NTNECal_Clusters;
  int NTNSAC_Hits;
  int NTNSAC_Clusters;
  int NTNTarget_Hits;
  
  
  double NTPVeto_Hits_Energy[NPVetoMaxHits];
  double NTPVeto_Hits_Time[NPVetoMaxHits];
  double NTPVeto_Hits_ChannelId[NPVetoMaxHits];
  double NTPVeto_Hits_Xpos[NPVetoMaxHits];
  double NTPVeto_Hits_Ypos[NPVetoMaxHits];
  double NTPVeto_Hits_Zpos[NPVetoMaxHits];

  double NTPVeto_Clusters_Energy[NPVetoMaxClusters];
  double NTPVeto_Clusters_Time[NPVetoMaxClusters];
  double NTPVeto_Clusters_ChannelId[NPVetoMaxClusters];
  double NTPVeto_Clusters_Xpos[NPVetoMaxClusters];
  double NTPVeto_Clusters_Ypos[NPVetoMaxClusters];
  double NTPVeto_Clusters_Zpos[NPVetoMaxClusters];

  double NTEVeto_Hits_Energy[NEVetoMaxHits];
  double NTEVeto_Hits_Time[NEVetoMaxHits];
  double NTEVeto_Hits_ChannelId[NEVetoMaxHits];
  double NTEVeto_Hits_Xpos[NEVetoMaxHits];
  double NTEVeto_Hits_Ypos[NEVetoMaxHits];
  double NTEVeto_Hits_Zpos[NEVetoMaxHits];

  double NTEVeto_Clusters_Energy[NEVetoMaxClusters];
  double NTEVeto_Clusters_Time[NEVetoMaxClusters];
  double NTEVeto_Clusters_ChannelId[NEVetoMaxClusters];
  double NTEVeto_Clusters_Xpos[NEVetoMaxClusters];
  double NTEVeto_Clusters_Ypos[NEVetoMaxClusters];
  double NTEVeto_Clusters_Zpos[NEVetoMaxClusters];

  double NTHEPVeto_Hits_Energy[NHEPVetoMaxHits];
  double NTHEPVeto_Hits_Time[NHEPVetoMaxHits];
  double NTHEPVeto_Hits_ChannelId[NHEPVetoMaxHits];
  double NTHEPVeto_Hits_Xpos[NHEPVetoMaxHits];
  double NTHEPVeto_Hits_Ypos[NHEPVetoMaxHits];
  double NTHEPVeto_Hits_Zpos[NHEPVetoMaxHits];

  double NTHEPVeto_Clusters_Energy[NHEPVetoMaxClusters];
  double NTHEPVeto_Clusters_Time[NHEPVetoMaxClusters];
  double NTHEPVeto_Clusters_ChannelId[NHEPVetoMaxClusters];
  double NTHEPVeto_Clusters_Xpos[NHEPVetoMaxClusters];
  double NTHEPVeto_Clusters_Ypos[NHEPVetoMaxClusters];
  double NTHEPVeto_Clusters_Zpos[NHEPVetoMaxClusters];

  double NTECal_Hits_Energy[NECalMaxHits];
  double NTECal_Hits_Time[NECalMaxHits];
  double NTECal_Hits_ChannelId[NECalMaxHits];
  double NTECal_Hits_Xpos[NECalMaxHits];
  double NTECal_Hits_Ypos[NECalMaxHits];
  double NTECal_Hits_Zpos[NECalMaxHits];

  double NTECal_Clusters_Energy[NECalMaxClusters];
  double NTECal_Clusters_Time[NECalMaxClusters];
  double NTECal_Clusters_ChannelId[NECalMaxClusters];
  double NTECal_Clusters_Xpos[NECalMaxClusters];
  double NTECal_Clusters_Ypos[NECalMaxClusters];
  double NTECal_Clusters_Zpos[NECalMaxClusters];

  double NTSAC_Hits_Energy[NSACMaxHits];
  double NTSAC_Hits_Time[NSACMaxHits];
  double NTSAC_Hits_ChannelId[NSACMaxHits];
  double NTSAC_Hits_Xpos[NSACMaxHits];
  double NTSAC_Hits_Ypos[NSACMaxHits];
  double NTSAC_Hits_Zpos[NSACMaxHits];

  double NTSAC_Clusters_Energy[NSACMaxClusters];
  double NTSAC_Clusters_Time[NSACMaxClusters];
  double NTSAC_Clusters_ChannelId[NSACMaxClusters];
  double NTSAC_Clusters_Xpos[NSACMaxClusters];
  double NTSAC_Clusters_Ypos[NSACMaxClusters];
  double NTSAC_Clusters_Zpos[NSACMaxClusters];

  double NTTarget_Hits_Energy[NTargetMaxHits];
  double NTTarget_Hits_Time[NTargetMaxHits];
  double NTTarget_Hits_ChannelId[NTargetMaxHits];
  double NTTarget_Hits_Xpos[NTargetMaxHits];
  double NTTarget_Hits_Ypos[NTargetMaxHits];
  double NTTarget_Hits_Zpos[NTargetMaxHits];

  double NTTargetBeam_X           ;
  double NTTargetBeam_XErr        ; 
  double NTTargetBeam_XW          ;
  double NTTargetBeam_XWErr       ;
  double NTTargetBeam_Xfit        ;
  double NTTargetBeam_XfitErr     ;
  double NTTargetBeam_XWfit       ;
  double NTTargetBeam_XWfitErr    ;
  double NTTargetBeam_XChi2       ;
  double NTTargetBeam_XNdof       ;
  double NTTargetBeam_XCharge     ;
  double NTTargetBeam_XChargeErr  ;
  double NTTargetBeam_XTime       ;
  double NTTargetBeam_XTimeErr    ;
  	                         
  double NTTargetBeam_Y           ;
  double NTTargetBeam_YErr	 ;
  double NTTargetBeam_YW		 ;
  double NTTargetBeam_YWErr	 ;
  double NTTargetBeam_Yfit	 ;
  double NTTargetBeam_YfitErr	 ;
  double NTTargetBeam_YWfit	 ;
  double NTTargetBeam_YWfitErr	 ;
  double NTTargetBeam_YChi2	 ;
  double NTTargetBeam_YNdof	 ;
  double NTTargetBeam_YCharge	 ;
  double NTTargetBeam_YChargeErr	 ;
  double NTTargetBeam_YTime	 ;
  double NTTargetBeam_YTimeErr	 ;
  double NTTargetBeam_NPOT	 ;
  double NTTargetBeam_NPOTErr	 ;


  };


class TFile;
class TTree;
class TH1D;
class TH2D;

const Int_t MaxHisto = 40;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoSvc
{

protected:
  HistoSvc();
public:
  ~HistoSvc();
  static HistoSvc* GetInstance();
private:
  static HistoSvc* fInstance;
public:
  //void book();
  //void book(Int_t validation);
  void book(Int_t validation, Int_t ntuple); //for FlatNTP
  void save();

  void BookHisto (std::string name, Int_t nx, Double_t xlow, Double_t xup);
  void BookHisto2(std::string name, Int_t nx, Double_t xlow, Double_t xup, Int_t ny, Double_t ylow, Double_t yup);
  void FillHisto (std::string name, Double_t bin, Double_t weight = 1.0);
  void FillHisto2(std::string name, Double_t xbin, Double_t ybin, Double_t weight = 1.0);
  void Normalize (std::string name, Double_t fac);    

  //  void FillHisto (Int_t id, Double_t bin, Double_t weight = 1.0);
  //  void FillHisto2(Int_t id, Double_t bin, Double_t ybin, Double_t weight = 1.0);
  //  void Normalize (Int_t id, Double_t fac);    

  //void FillNtuple(NTEvent* Evt);
  void FillNtuple();

  void    setOutputFileName(TString s){ fOutputFileName=s; }
  TString getOutputFileName(){ return fOutputFileName; }

  //  void PrintStatistic();


public:
  NTEvent  myEvt;    
private:
    
  TString fOutputFileName;
  TFile*   fRootOutputFile;
  std::map<std::string, TH1D*>    fHisto1DMap;            
  std::map<std::string, TH2D*>    fHisto2DMap;            
  TTree*   ntupl;    

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

