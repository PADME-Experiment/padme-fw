#ifndef HistoSvc_h
#define HistoSvc_h 1

#include <map>
//#include "globals.hh"
       	       	       
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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


   //for flat Ntuple
  struct NTEvent{
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

  int NTNEvent;//CT
    
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

