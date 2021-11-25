#ifndef StdNtuple_h
#define StdNtuple_h 1

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


// Standard Ntuple structure
struct StdNtupleEvent{

  Int_t     NTEvent_Run_Number;
  Int_t     NTEvent_Event_Number;
  Double_t  NTEvent_Reco_Time;
  Double_t  NTEvent_Event_Time;
  ULong64_t NTEvent_Run_Clock;
  UInt_t    NTEvent_Event_Status;
  UInt_t    NTEvent_Trigger_Mask;

  Int_t    NTNPVeto_Hits;
  Int_t    NTPVeto_Hits_ChannelId[NPVetoMaxHits];
  Double_t NTPVeto_Hits_Energy[NPVetoMaxHits];
  Double_t NTPVeto_Hits_Time[NPVetoMaxHits];
  Double_t NTPVeto_Hits_Xpos[NPVetoMaxHits];
  Double_t NTPVeto_Hits_Ypos[NPVetoMaxHits];
  Double_t NTPVeto_Hits_Zpos[NPVetoMaxHits];

  Int_t    NTNPVeto_Clusters;
  Int_t    NTPVeto_Clusters_ChannelId[NPVetoMaxClusters];
  Double_t NTPVeto_Clusters_Energy[NPVetoMaxClusters];
  Double_t NTPVeto_Clusters_Time[NPVetoMaxClusters];
  Double_t NTPVeto_Clusters_Xpos[NPVetoMaxClusters];
  Double_t NTPVeto_Clusters_Ypos[NPVetoMaxClusters];
  Double_t NTPVeto_Clusters_Zpos[NPVetoMaxClusters];

  Int_t    NTNEVeto_Hits;
  Int_t    NTEVeto_Hits_ChannelId[NEVetoMaxHits];
  Double_t NTEVeto_Hits_Energy[NEVetoMaxHits];
  Double_t NTEVeto_Hits_Time[NEVetoMaxHits];
  Double_t NTEVeto_Hits_Xpos[NEVetoMaxHits];
  Double_t NTEVeto_Hits_Ypos[NEVetoMaxHits];
  Double_t NTEVeto_Hits_Zpos[NEVetoMaxHits];

  Int_t    NTNEVeto_Clusters;
  Int_t    NTEVeto_Clusters_ChannelId[NEVetoMaxClusters];
  Double_t NTEVeto_Clusters_Energy[NEVetoMaxClusters];
  Double_t NTEVeto_Clusters_Time[NEVetoMaxClusters];
  Double_t NTEVeto_Clusters_Xpos[NEVetoMaxClusters];
  Double_t NTEVeto_Clusters_Ypos[NEVetoMaxClusters];
  Double_t NTEVeto_Clusters_Zpos[NEVetoMaxClusters];

  Int_t    NTNHEPVeto_Hits;
  Int_t    NTHEPVeto_Hits_ChannelId[NHEPVetoMaxHits];
  Double_t NTHEPVeto_Hits_Energy[NHEPVetoMaxHits];
  Double_t NTHEPVeto_Hits_Time[NHEPVetoMaxHits];
  Double_t NTHEPVeto_Hits_Xpos[NHEPVetoMaxHits];
  Double_t NTHEPVeto_Hits_Ypos[NHEPVetoMaxHits];
  Double_t NTHEPVeto_Hits_Zpos[NHEPVetoMaxHits];

  Int_t    NTNHEPVeto_Clusters;
  Int_t    NTHEPVeto_Clusters_ChannelId[NHEPVetoMaxClusters];
  Double_t NTHEPVeto_Clusters_Energy[NHEPVetoMaxClusters];
  Double_t NTHEPVeto_Clusters_Time[NHEPVetoMaxClusters];
  Double_t NTHEPVeto_Clusters_Xpos[NHEPVetoMaxClusters];
  Double_t NTHEPVeto_Clusters_Ypos[NHEPVetoMaxClusters];
  Double_t NTHEPVeto_Clusters_Zpos[NHEPVetoMaxClusters];

  Int_t    NTNECal_Hits;
  Int_t    NTECal_Hits_ChannelId[NECalMaxHits];
  Double_t NTECal_Hits_Energy[NECalMaxHits];
  Double_t NTECal_Hits_Time[NECalMaxHits];
  Double_t NTECal_Hits_Xpos[NECalMaxHits];
  Double_t NTECal_Hits_Ypos[NECalMaxHits];
  Double_t NTECal_Hits_Zpos[NECalMaxHits];

  Int_t    NTNECal_Clusters;
  Int_t    NTECal_Clusters_ChannelId[NECalMaxClusters];
  Double_t NTECal_Clusters_Energy[NECalMaxClusters];
  Double_t NTECal_Clusters_Time[NECalMaxClusters];
  Double_t NTECal_Clusters_Xpos[NECalMaxClusters];
  Double_t NTECal_Clusters_Ypos[NECalMaxClusters];
  Double_t NTECal_Clusters_Zpos[NECalMaxClusters];

  Int_t    NTNSAC_Hits;
  Int_t    NTSAC_Hits_ChannelId[NSACMaxHits];
  Double_t NTSAC_Hits_Energy[NSACMaxHits];
  Double_t NTSAC_Hits_Time[NSACMaxHits];
  Double_t NTSAC_Hits_Xpos[NSACMaxHits];
  Double_t NTSAC_Hits_Ypos[NSACMaxHits];
  Double_t NTSAC_Hits_Zpos[NSACMaxHits];

  Int_t    NTNSAC_Clusters;
  Int_t    NTSAC_Clusters_ChannelId[NSACMaxClusters];
  Double_t NTSAC_Clusters_Energy[NSACMaxClusters];
  Double_t NTSAC_Clusters_Time[NSACMaxClusters];
  Double_t NTSAC_Clusters_Xpos[NSACMaxClusters];
  Double_t NTSAC_Clusters_Ypos[NSACMaxClusters];
  Double_t NTSAC_Clusters_Zpos[NSACMaxClusters];

  Int_t    NTNTarget_Hits;  
  Int_t    NTTarget_Hits_ChannelId[NTargetMaxHits];
  Double_t NTTarget_Hits_Energy[NTargetMaxHits];
  Double_t NTTarget_Hits_Time[NTargetMaxHits];
  Double_t NTTarget_Hits_Xpos[NTargetMaxHits];
  Double_t NTTarget_Hits_Ypos[NTargetMaxHits];
  Double_t NTTarget_Hits_Zpos[NTargetMaxHits];

  Double_t NTTargetBeam_X;
  Double_t NTTargetBeam_XErr; 
  Double_t NTTargetBeam_XW;
  Double_t NTTargetBeam_XWErr;
  Double_t NTTargetBeam_Xfit;
  Double_t NTTargetBeam_XfitErr;
  Double_t NTTargetBeam_XWfit;
  Double_t NTTargetBeam_XWfitErr;
  Double_t NTTargetBeam_XChi2;
  Double_t NTTargetBeam_XNdof;
  Double_t NTTargetBeam_XCharge;
  Double_t NTTargetBeam_XChargeErr;
  Double_t NTTargetBeam_XTime;
  Double_t NTTargetBeam_XTimeErr;
  	                         
  Double_t NTTargetBeam_Y;
  Double_t NTTargetBeam_YErr;
  Double_t NTTargetBeam_YW;
  Double_t NTTargetBeam_YWErr;
  Double_t NTTargetBeam_Yfit;
  Double_t NTTargetBeam_YfitErr;
  Double_t NTTargetBeam_YWfit;
  Double_t NTTargetBeam_YWfitErr;
  Double_t NTTargetBeam_YChi2;
  Double_t NTTargetBeam_YNdof;
  Double_t NTTargetBeam_YCharge;
  Double_t NTTargetBeam_YChargeErr;
  Double_t NTTargetBeam_YTime;
  Double_t NTTargetBeam_YTimeErr;
  Double_t NTTargetBeam_NPOT;
  Double_t NTTargetBeam_NPOTErr;

  };

class TTree;
class PadmeAnalysisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class StdNtuple
{

public:

  StdNtuple(){ Book(); }
  ~StdNtuple(){;}

  void Fill(PadmeAnalysisEvent*);

private:

  void Book();

  StdNtupleEvent fMyEvt;
  TTree* fNtuple;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
