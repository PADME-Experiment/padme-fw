#include "MMStudy.hh"
#include "TGraph.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"


MMStudy* MMStudy::fInstance = 0;

MMStudy* MMStudy::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new MMStudy(); }
  return fInstance;
}

MMStudy::~MMStudy(){
  delete fCfgParser;
}

Bool_t MMStudy::Init(PadmeAnalysisEvent* event,  Bool_t fHistoModeVal, TString InputHistofileVal){

  if (fVerbose) printf("---> Initializing MMStudy\n");
  TString cfgFile = "config/UserAnalysis.conf";
  fHS = HistoSvc::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();
  fNPoTAnalysis = NPoTAnalysis::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
//  fApplyQualityCheck = true;
//  if(fCfgParser->HasConfig("GENERAL", "DataQualityLevel")){
//     fDataQualityLevel =TString(fCfgParser->GetSingleArg("GENERAL", "DataQualityLevel")).Atoi();
//  } //handling del DataQualityLevel non implementato

  // deve poter leggere il config e sapere se e' in read mode o flag mode, se e' in flag mode deve leggere il file di testo coi periodi con problemi
  // e determina la flag per quell'evento in base al tempo  
  fEvent = event;
  fNRun = fGeneralInfo->GetRunNumberFromDB(); //30000 vale solo per il 2022
  fHistoMode = fHistoModeVal;
  InputHistofile = InputHistofileVal;
  
  InitHistos(fNRun);
  return true;
}

Bool_t MMStudy::InitHistos(Int_t nRun){
  // MMStudy directory will contain all histograms related to this analysis

  fHS->CreateList("MMStudy");
  cout<<" Creating MMStudy Hystograms for Run "<<nRun<<" "<<endl;  
  fHS->BookHisto2List("MMStudy","MM_Nclus_vs_NHits",100,0,3000.,500,0,500); 
  fHS->BookHistoList("MMStudy","NHitsPerCluster",100,0,100.);
  fHS->BookHistoList("MMStudy","NJumpsPerCluster",100,0,100.);
  fHS->BookHisto2List("MMStudy","NClustersPerBoard",16,-0.5,15.5,100,0,100.);
  for (int view = 0; view < 2; view++) {
    fHS->BookHistoList("MMStudy",Form("PositionAtECal_view%d",view)  ,500,-500,500.);
    fHS->BookHistoList("MMStudy",Form("PositionAtTarget_view%d",view),500,-500,500.);
    fHS->BookHisto2List("MMStudy",Form("PositionAtECalVsAtTarget_view%d",view),500,-500,500.,500,-500,500.);
  }
  return true;
}

Bool_t MMStudy::Process(){
//
//
//
//
//
//
//
  
// boardSN Layer side plane view hole offset stripid_orig
// 0       0     0    0     0    6    0      0-255
// 1       0     1    0     0    6    256    256-511
// 2       1     0    0     0    6    0      0-255
// 3       1     1    0     0    6    256    256-511
// 4       2     0    0     1    1    0      0-255
// 5       2     1    0     1    1    256    256-511
// 6       3     0    0     1    1    0      0-255
// 7       3     1    0     1    1    256    256-511
// 8       4     0    1     0    1    0      0-255
// 9       4     1    1     0    1    256    256-511
// 10      5     0    1     0    1    0      0-255
// 11      5     1    1     0    1    256    256-511
// 12      6     0    1     1    6    0      0-255
// 13      6     1    1     1    6    256    256-511
// 14      7     0    1     1    6    0      0-255
// 15      7     1    1     1    6    256    256-511
  
// layer 0,1 -> y layer/2 = 0 
// layer 2,3 -> x layer/2 = 1
// layer 4,5 -> y layer/2 = 2
// layer 6,7 -> x layer/2 = 3
// layer 0,1 large hole
// layer 2,3 little hole
// layer 4,5 little hole
// layer 6,7 large hole

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  //long long int fTimeStamp =(long long int) fEvent->RecoEvent->GetEventTime().GetSec();
  if(trigMask & (1 << 0)) {
  }

  int nclus = fEvent->MMRecoCl->GetNElements();
  int nhits = fEvent->MMRecoEvent->GetNHits();
  fHS->FillHisto2List("MMStudy","MM_Nclus_vs_NHits",nhits,nclus,1.);

  int nclusPerBoard[16]={0};
  for (int i=0; i<nclus; i++){
    TRecoVCluster* clus = fEvent->MMRecoCl->Element(i);
    int nhitsPerClus = clus->GetNHitsInClus();
    TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(0));
    int chId = hit->GetChannelId();
    int bdid = (chId & 0xF00 ) >> 8; // board SN 0-15
    nclusPerBoard[bdid]++;

    int layer = bdid/2;  // layer 0-7
    int plane = layer/4;  // plane 0-1
    int side = bdid%2; // left/right (X view), bottom/top (Y view)
    int strip = (chId & 0x0FF); // strip 0-255
    int view = (layer/2)%2; // 0 means Y view, 1 means X view    

    // nhits per clus
    fHS->FillHistoList("MMStudy","NHitsPerCluster",nhitsPerClus,1.);
    int njumps = 0;
    for (int j=1; j<nhitsPerClus; j++){
      TRecoVHit* hitR = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
      int stripR = hitR->GetChannelId() & 0x0FF;
      TRecoVHit* hitL = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j-1));
      int stripL = hitL->GetChannelId() & 0x0FF;
      njumps += stripR-stripL-1;
    }
    fHS->FillHistoList("MMStudy","NJumpsPerCluster",njumps,1.);

    // perform evaluation of the tracklet parameters

    double z2av = 0;
    double vzav = 0;
    double vav = 0;
    double zav = 0;
    // v = a z + b, where v = x or y depending on the view
    // slope (<vz>-<v><z>)/(<z^2>-<z>^2)^1/2
    // intercept = <z> x slope - <v>
    if (nhitsPerClus > 1) {
      for (int j=0; j<nhitsPerClus; j++){
	TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
	double v = (view == 0)? hit->GetPosition().Y() : hit->GetPosition().X();
	int verse = (plane == 0)? -1 : 1;
	double z = verse*hit->GetTime()*0.105 + hit->GetPosition().Z() ;
	z2av += z*z; // now without error weights
	vzav += z*v;
	vav  += v;
	zav  += z;
      }    
      z2av /= nhitsPerClus;
      vzav /= nhitsPerClus;
      vav /= nhitsPerClus;
      zav /= nhitsPerClus;
      double slope = (vzav - vav*zav)/(z2av - zav*zav);
      double inter = slope*zav - vav;

      fHS->FillHistoList("MMStudy",Form("PositionAtECal_view%d",view),(slope*fGeneralInfo->GetCOG().Z() + inter),1.);
      fHS->FillHistoList("MMStudy",Form("PositionAtTarget_view%d",view),(slope*fGeneralInfo->GetTargetPos().Z() + inter),1.);
      fHS->FillHisto2List("MMStudy",Form("PositionAtECalVsAtTarget_view%d",view),(slope*fGeneralInfo->GetTargetPos().Z() + inter),(slope*fGeneralInfo->GetCOG().Z() + inter),1.);
    }
  }
  // nclus per board
  for (int bdid = 0; bdid<16; bdid++) fHS->FillHisto2List("MMStudy","NClustersPerBoard",bdid,nclusPerBoard[bdid],1.);
  
  return true;
}

Bool_t MMStudy::Finalize(){
  if(fGeneralInfo->isMC()){
    std::cout<<"This run is MC, MMStudy checks do not apply"<<std::endl;
    return false;
  } 

  if (fVerbose) printf("---> Finalizing MMStudy\n");
  return true;
}



