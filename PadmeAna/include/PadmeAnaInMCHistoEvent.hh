#ifndef PadmeAnaInMCHistoEvent_H
#define PadmeAnaInMCHistoEvent_H

#include "PadmeVAnaInEvent.hh"

#include "TTree.h"

class PadmeAnaInMCHistoEvent:public PadmeVAnaInEvent{
  
public:
  PadmeAnaInMCHistoEvent(){  evType = MCHisto; fEntry = 0; fMCHistoTree=NULL;};
  PadmeAnaInMCHistoEvent( TTree * tree);
  int NextEvent();
  ~PadmeAnaInMCHistoEvent();

  
  TTree * fMCHistoTree;
  int fEntry;
  // Declaration of leaf types
  Int_t           Nevent;
   Double_t        ETot;
   Double_t        IDProc;
   Double_t        PBeam;
   Double_t        PPrim;
   Double_t        XBeam;
   Double_t        YBeam;
   Int_t           NClusters;
   Int_t           NTracks;
   Int_t           NHEPVetoTracks;
   Int_t           NPVetoTracks;
   Int_t           NEVetoTracks;
   Int_t           NSAC;
   Int_t           NCal;
   Int_t           NLAV;
   Int_t           NTarget;
   Double_t        ESAC[100];
   Double_t        TSAC[100];
   Double_t        PTypeSAC[100];
   Double_t        XSAC[100];
   Double_t        YSAC[100];
   Int_t           SACCh[100];
   Double_t        EPartCal[20];
   Double_t        TPartCal[20];
   Int_t           PTypePartCal[20];
   Double_t        XPartCal[20];
   Double_t        YPartCal[20];
   Double_t        ECluster[20];
   Double_t        QCluster[20];
   Double_t        XCluster[20];
   Double_t        YCluster[20];
   Double_t        ThCluster[20];
   Double_t        M2Cluster[20];
   Double_t        TCluster[20];
   Double_t        NClusCells[20];
   Double_t        ETarget;
   Double_t        TTarget;
   Double_t        XTarget;
   Double_t        YTarget;
   Double_t        HEPVetoTrEne[100];
   Int_t           HEPVetoNFing[100];
   Double_t        HEPVetoTrTime[100];
   Double_t        HEPVetoFingE[100];
   Double_t        HEPVetoX[100];
   Double_t        HEPVetoY[100];
   Double_t        PVetoTrEne[100];
   Int_t           PVetoNFing[100];
   Double_t        PVetoTrTime[100];
   Double_t        PVetoFingE[100];
   Double_t        PVetoX[100];
   Double_t        PVetoY[100];
   Double_t        EVetoTrEne[100];
   Int_t           EVetoNFing[100];
   Double_t        EVetoTrTime[100];
   Double_t        EVetoFingE[100];
   Double_t        EVetoX[100];
   Double_t        EVetoY[100];

     // List of branches
   TBranch        *b_Nevent;   //!
   TBranch        *b_ETot;   //!
   TBranch        *b_IDProc;   //!
   TBranch        *b_PBeam;   //!
   TBranch        *b_PPrim;   //!
   TBranch        *b_XBeam;   //!
   TBranch        *b_YBeam;   //!
   TBranch        *b_NClusters;   //!
   TBranch        *b_NTracks;   //!
   TBranch        *b_NHEPVetoTracks;   //!
   TBranch        *b_NPVetoTracks;   //!
   TBranch        *b_NEVetoTracks;   //!
   TBranch        *b_NSAC;   //!
   TBranch        *b_NCal;   //!
   TBranch        *b_NLAV;   //!
   TBranch        *b_NTarget;   //!
   TBranch        *b_ESAC;   //!
   TBranch        *b_TSAC;   //!
   TBranch        *b_PTypeSAC;   //!
   TBranch        *b_XSAC;   //!
   TBranch        *b_YSAC;   //!
   TBranch        *b_SACCh;   //!
   TBranch        *b_CalE;   //!
   TBranch        *b_CalT;   //!
   TBranch        *b_CalPType;   //!
   TBranch        *b_CalX;   //!
   TBranch        *b_CalY;   //!
   TBranch        *b_ECluster;   //!
   TBranch        *b_QCluster;   //!
   TBranch        *b_XCluster;   //!
   TBranch        *b_YCluster;   //!
   TBranch        *b_ThCluster;   //!
   TBranch        *b_M2Cluster;   //!
   TBranch        *b_TCluster;   //!
   TBranch        *b_NClusCells;   //!
   TBranch        *b_ETarget;   //!
   TBranch        *b_TTarget;   //!
   TBranch        *b_XTarget;   //!
   TBranch        *b_YTarget;   //!
   TBranch        *b_NTHEPVetoTrkEne;   //!
   TBranch        *b_NTHEPVetoTrkFinger;   //!
   TBranch        *b_NTHEPVetoTrkTime;   //!
   TBranch        *b_NTHEPVetoFingE;   //!
   TBranch        *b_NTHEPVetoX;   //!
   TBranch        *b_NTHEPVetoY;   //!
   TBranch        *b_NTPVetoTrkEne;   //!
   TBranch        *b_NTPVetoTrkFinger;   //!
   TBranch        *b_NTPVetoTrkTime;   //!
   TBranch        *b_NTPVetoFingE;   //!
   TBranch        *b_NTPVetoX;   //!
   TBranch        *b_NTPVetoY;   //!
   TBranch        *b_NTEVetoTrkEne;   //!
   TBranch        *b_NTEVetoTrkFinger;   //!
   TBranch        *b_NTEVetoTrkTime;   //!
   TBranch        *b_NTEVetoFingE;   //!
   TBranch        *b_NTEVetoX;   //!
   TBranch        *b_NTEVetoY;   //!


};

#endif
