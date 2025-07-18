#include "MMStudy.hh"
#include "TGraph.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"

struct tracklet{
  double slope; // dv/dz
  double inter; // v at mesh plane
  double chi2;
  double pars[4];// x0,y0,x1,y1
  TVector3 lambda; // cosines of track directions
  int nstrips;
};

MMStudy* MMStudy::fInstance = 0;

MMStudy* MMStudy::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new MMStudy(); }
  return fInstance;
}

MMStudy::~MMStudy(){
  delete fCfgParser;
  for (int i=0; i<3; i++) delete fTracker[i];
}

Bool_t MMStudy::Init(PadmeAnalysisEvent* event,  Bool_t fHistoModeVal, TString InputHistofileVal){

  if (fVerbose) printf("---> Initializing MMStudy\n");
  TString cfgFile = "config/UserAnalysis.conf";
  fHS = HistoSvc::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();
  fNPoTAnalysis = NPoTAnalysis::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
//  fApplyQualityCheck = true;x
//  if(fCfgParser->HasConfig("GENERAL", "DataQualityLevel")){
//     fDataQualityLevel =TString(fCfgParser->GetSingleArg("GENERAL", "DataQualityLevel")).Atoi();
//  } //handling del DataQualityLevel non implementato

  // deve poter leggere il config e sapere se e' in read mode o flag mode, se e' in flag mode deve leggere il file di testo coi periodi con problemi
  // e determina la flag per quell'evento in base al tempo  
  fEvent = event;
  fNRun = fGeneralInfo->GetRunNumberFromDB(); //30000 vale solo per il 2022
  fHistoMode = fHistoModeVal;
  InputHistofile = InputHistofileVal;
  fEventCounter = 0;
  InitHistos(fNRun);

  for (int i = 0; i<3; i++) { // y view, x view, 3d
    fTracker[i] = new MMTracker(i);
    fTracker[i]->InitFitter();
  }


  return true;
}

Bool_t MMStudy::InitHistos(Int_t nRun){
  // MMStudy directory will contain all histograms related to this analysis

  fHS->CreateList("MMStudy");
  cout<<" Creating MMStudy Hystograms for Run "<<nRun<<" "<<endl;  
  fHS->BookHisto2List("MMStudy","FitChi2",100,0,100.,500,0,500); 
  for (int i=0; i<100; i++){
    for (int j=0; j<16; j++){
      fHS->BookHisto2List("MMStudy",Form("zvsv_board%d_ev%d",j,i),1200,-600.,600.,100,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+50);
      fHS->BookHisto2List("MMStudy",Form("zvsv_board%d_ev%d_fit",j,i),1200,-600.,600.,100,fGeneralInfo->GetMMPosPlaneZ(0)-50,fGeneralInfo->GetMMPosPlaneZ(1)+50);
    }
  }

  
  fHS->BookHisto2List("MMStudy","MM_Nclus_vs_NHits",100,0,3000.,500,0,500); 
  fHS->BookHistoList("MMStudy","NHitsPerCluster",100,0,100.);
  fHS->BookHistoList("MMStudy","NJumpsPerCluster",100,0,100.);
  fHS->BookHisto2List("MMStudy","NClustersPerBoard",16,-0.5,15.5,100,0,100.);
  fHS->BookHistoList("MMStudy","HitCharge",500,-10e3,800e3);
  fHS->BookHistoList("MMStudy","ClusterCharge",500,-10e3,10e6);
  for (int view = 0; view < 2; view++) {
    fHS->BookHistoList("MMStudy",Form("PositionAtECal_view%d",view)  ,500,-500,500.);
    fHS->BookHistoList("MMStudy",Form("PositionAtTarget_view%d",view),500,-500,500.);
    fHS->BookHisto2List("MMStudy",Form("PositionAtECalVsAtTarget_view%d",view),500,-500,500.,500,-500,500.);
  }

  for (int i = 0; i< 4; i++) { // strip number bins: 1-2, 3, 4, 5+
    fHS->BookHisto2List("MMStudy",Form("MatchingPlotSlope_stripBin%d",i),1000,-1.,1.,1000,-1.,1.);
  }

  
  for (int side = 0; side < 2; side++) {
    for (int view = 0; view < 2; view++) {
      for (int othview = 0; othview < 2; othview++) {
	fHS->BookHisto2List("MMStudy",Form("MatchingPlotNStrips_side%d_view%d_otherview%d",side,view,othview),30,0.,30.,30,0,30);
	for (int i = 0; i< 4; i++) { // strip number bins: 1-2, 3, 4, 5+
	  fHS->BookHisto2List("MMStudy",Form("MatchingPlotSlope_side%d_view%d_otherview%d_stripBin%d",side,view,othview,i),1000,-1.,1.,1000,-1.,1.);
	  fHS->BookHisto2List("MMStudy",Form("MatchingPlot_side%d_view%d_otherview%d_stripBin%d",side,view,othview,i),5000,-500.,500.,1000,-5000,5000);
	  fHS->BookHisto2List("MMStudy",Form("MatchingPlotDZ_side%d_view%d_otherview%d_stripBin%d",side,view,othview,i),5000,-500.,500.,1000,-5000,5000);
	}
      }
    }
  }
  fHS->BookHisto2List("MMStudy",Form("Chi2OverNdfVsNdf"),30,0.,30.,100,0,100);
  fHS->BookHisto2List("MMStudy",Form("ResVsNdf"),30,0.,30.,100,-100,100);

  return true;
}

Bool_t MMStudy::Process(){
//
//
//
//
//
//
// Geometrical afferent positions are defined by the same values of side, view, otherview but different planes
// 
  
// boardSN plane Layer side  view otherview hole offset stripid_orig
// 0       0     0     0     0    0         6    0      1-256
// 1       0     0     1     0    0         6    256    257-512
// 2       0     1     0     0    1         6    0      0-255
// 3       0     1     1     0    1         6    256    256-511
// 4       0     2     0     1    0         1    0      0-255
// 5       0     2     1     1    0         1    256    256-511
// 6       0     3     0     1    1         1    0      0-255
// 7       0     3     1     1    1         1    256    256-511
// 8       1     4     0     0    0         1    0      0-255
// 9       1     4     1     0    0         1    256    256-511
// 10      1     5     0     0    1         1    0      0-255
// 11      1     5     1     0    1         1    256    256-511
// 12      1     6     0     1    0         6    0      0-255
// 13      1     6     1     1    0         6    256    256-511
// 14      1     7     0     1    1         6    0      0-255
// 15      1     7     1     1    1         6    256    256-511

// corresponding boards
// BoardSN: 0 <--> 8
//          1 <--> 9
//          2 <--> 10
//....


  
// layer 0,1 -> y layer/2 = 0 
// layer 2,3 -> x layer/2 = 1
// layer 4,5 -> y layer/2 = 2
// layer 6,7 -> x layer/2 = 3
// layer 0,1 large hole
// layer 2,3 little hole
// layer 4,5 little hole
// layer 6,7 large hole

  vector<tracklet> tracklets[2][8]; // plane wise, side/view/otherview packed
  for (int i=0; i<2; i++){
    for (int j=0; j<8; j++){
      tracklets[i][j].clear();
    }
  }
  
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  //long long int fTimeStamp =(long long int) fEvent->RecoEvent->GetEventTime().GetSec();
  if(trigMask & (1 << 0)) {
  }


  int nclus = fEvent->MMRecoCl->GetNElements();
  int nhits = fEvent->MMRecoEvent->GetNHits();
  fHS->FillHisto2List("MMStudy","MM_Nclus_vs_NHits",nhits,nclus,1.);

  
  //  std::cout << "********      Ev = " << fEvent->RecoEvent->GetEventNumber() << " Nclus " << nclus << "  ********** NHits " << nhits << std::endl;
  fEventCounter++;

  
  for (int i=0; i<nclus; i++){
    TRecoVCluster* clus = fEvent->MMRecoCl->Element(i);
    int nhitsPerClus = clus->GetNHitsInClus();
    
    //    if (nhitsPerClus < 5) continue;

    MMchInfo mmi;
    for (int j=0; j<nhitsPerClus; j++){
      TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
      mmi = fGeneralInfo->DecodeMMChannel(hit->GetChannelId());

      if (j==0) fTracker[mmi.view]->Clear();
      
      double z = mmi.verse*hit->GetTime()*0.105 + hit->GetPosition().Z() ;
      double v = (mmi.view == 0? hit->GetPosition().Y() : hit->GetPosition().X());
      TVector3 newpos;
      newpos.SetXYZ(hit->GetPosition().X(),hit->GetPosition().Y(),z);
      hit->SetPosition(newpos);
      fTracker[mmi.view]->AddHit(hit);
      if (fEventCounter  < 100) fHS->FillHisto2List("MMStudy",Form("zvsv_board%d_ev%d",mmi.bdid,fEventCounter-1),v,hit->GetPosition().Z(),hit->GetEnergy());
    }
    fTracker[mmi.view]->InitFit(); 
    bool goodfit = fTracker[mmi.view]->MakeFit();

    if (goodfit && TMath::Abs(fTracker[mmi.view]->GetFitResults(1-mmi.view)) < 350. && TMath::Abs(fTracker[mmi.view]->GetFitResults(3-mmi.view)) < 350.) { 
      tracklet traco;
      traco.chi2 = fTracker[mmi.view]->GetFitChi2();
      for (int q = 0; q<4; q++) traco.pars[q] = fTracker[mmi.view]->GetFitResults(q);

      double slopevsz = (fTracker[mmi.view]->GetFitResults(3-mmi.view) - fTracker[mmi.view]->GetFitResults(1-mmi.view))/(fGeneralInfo->GetMMPosPlaneZ(1)-fGeneralInfo->GetMMPosPlaneZ(0));// dv/dz      
      double interatzmid = fTracker[mmi.view]->GetFitResults(1-mmi.view) + slopevsz*0.5*(fGeneralInfo->GetMMPosPlaneZ(1)-fGeneralInfo->GetMMPosPlaneZ(0)); // v0 + dv/dz*(z1-z0)/2
      traco.slope = slopevsz;
      traco.inter = interatzmid;
      traco.nstrips = nhitsPerClus;
      tracklets[mmi.plane][mmi.packed].push_back(traco);
    
      fHS->FillHisto2List("MMStudy","FitChi2",nhitsPerClus,fTracker[mmi.view]->GetFitChi2(),1.);

      TVector3 fitpoints[2];
      for (uint i=0; i<2; i++) fitpoints[i].SetXYZ(traco.pars[2*i],traco.pars[1+2*i],fGeneralInfo->GetMMPosPlaneZ(i)); // points at Z0, Z1
	
      TVector3 lambda(fitpoints[1].X()-fitpoints[0].X(),fitpoints[1].Y()-fitpoints[0].Y(),fitpoints[1].Z()-fitpoints[0].Z());
      double dist = lambda.Mag();
      lambda *= (1./dist);      
      traco.lambda = lambda; // (P1-P0)/|P1-P0|
      
      if (fEventCounter  < 100) {
	// r0 + lambda t, r0 = par0,1,2; lambda(theta,phi)=par3,4
	// z = z0 + dz/dy (y-y0)

	for (int tt = 0; tt < 100; tt++){
	  TVector3 errenow(fitpoints[0].X(),fitpoints[0].Y(),fitpoints[0].Z());
	  TVector3 delta;
	  delta.SetXYZ(lambda.X(),lambda.Y(),lambda.Z());
	  double t = tt*dist/100.;	  
	  delta *= t;
	  errenow += delta;
	  fHS->FillHisto2List("MMStudy",Form("zvsv_board%d_ev%d_fit",mmi.bdid,fEventCounter-1),errenow[1-mmi.view],errenow.Z(),1.);
	}
      }
    }    
  }

  // tracklet pairs whatsoever, between planes
  for (int i=0; i<8; i++){
    int view = (i&0x2)>>1;
    for (uint i0 = 0; i0<tracklets[0][i].size(); i0++){
      tracklet traco0 = tracklets[0][i].at(i0);
      int istripBin0 = 0;
      if (traco0.nstrips < 3) istripBin0 = 0; // 1,2
      else if (traco0.nstrips == 3) istripBin0 = 1; // 3
      else if (traco0.nstrips == 4) istripBin0 = 2; // 4
      else                          istripBin0 = 3; // 5+

      for (int j=0; j<8; j++){
	for (uint i1 = 0; i1<tracklets[1][j].size(); i1++){
	  tracklet traco1 = tracklets[1][j].at(i1);
	  
	  int istripBin1 = 0;
	  if (traco1.nstrips < 3) istripBin1 = 0; // 1,2
	  else if (traco1.nstrips == 3) istripBin1 = 1; // 3
	  else if (traco1.nstrips == 4) istripBin1 = 2; // 4
	  else                          istripBin1 = 3; // 5+

	  int istripBin = TMath::Min(istripBin0,istripBin1);

	  fHS->FillHisto2List("MMStudy",Form("MatchingPlotSlope_stripBin%d",istripBin),traco0.slope,traco1.slope,1.);
	  
	}
      }
    }
  }


  
  for (int i=0; i<8; i++){
    int view = (i&0x2)>>1;
    for (uint i0 = 0; i0<tracklets[0][i].size(); i0++){
      tracklet traco0 = tracklets[0][i].at(i0);
      int istripBin0 = 0;
      if (traco0.nstrips < 3) istripBin0 = 0; // 1,2
      else if (traco0.nstrips == 3) istripBin0 = 1; // 3
      else if (traco0.nstrips == 4) istripBin0 = 2; // 4
      else                          istripBin0 = 3; // 5+

      
      for (uint i1 = 0; i1<tracklets[1][i].size(); i1++){
	tracklet traco1 = tracklets[1][i].at(i1);

	int istripBin1 = 0;
	if (traco1.nstrips < 3) istripBin1 = 0; // 1,2
	else if (traco1.nstrips == 3) istripBin1 = 1; // 3
	else if (traco1.nstrips == 4) istripBin1 = 2; // 4
	else                          istripBin1 = 3; // 5+

	int istripBin = TMath::Min(istripBin0,istripBin1);
	
	fHS->FillHisto2List("MMStudy",Form("MatchingPlotNStrips_side%d_view%d_otherview%d",((i&0x4)>>2),((i&0x2)>>1),(i&0x1)),traco0.nstrips,traco1.nstrips,1.);

	double vmid = 0.5*(traco0.inter + traco1.inter); // half position of the two intercepts at the zmid plane (mesh plane)
	// z = z0 + dz/dv x Dv

	double zp0 = fGeneralInfo->GetMMPosPlaneZ(0) + (vmid-traco0.pars[1-view])/traco0.slope;
	double zp1 = fGeneralInfo->GetMMPosPlaneZ(0) + (vmid-traco1.pars[1-view])/traco1.slope;
	
	fHS->FillHisto2List("MMStudy",Form("MatchingPlotSlope_side%d_view%d_otherview%d_stripBin%d",((i&0x4)>>2),((i&0x2)>>1),(i&0x1),istripBin),traco0.slope,traco1.slope,1.);

	fHS->FillHisto2List("MMStudy",Form("MatchingPlot_side%d_view%d_otherview%d_stripBin%d",((i&0x4)>>2),((i&0x2)>>1),(i&0x1),istripBin),traco0.slope-traco1.slope,traco0.inter-traco1.inter,1.);
	fHS->FillHisto2List("MMStudy",Form("MatchingPlotDZ_side%d_view%d_otherview%d_stripBin%d",((i&0x4)>>2),((i&0x2)>>1),(i&0x1),istripBin),traco0.slope-traco1.slope,zp0-zp1,1.);
      }
    }
  }

  /*  
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
    int otherview = layer%2; // 0 means the half-strip left (bottom) depending on the view

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

    // v = a z + b, where v = x or y depending on the view
    // slope (<vz> - <v><z>)/(<z^2> - <z>^2)
    // intercept = - <z> x slope + <v>
    if (nhitsPerClus > 2) {

      double z2av = 0;
      double vzav = 0;
      double vav = 0;
      double zav = 0;
      double clucharge = 0;

      for (int j=0; j<nhitsPerClus; j++){
	TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
	double v = (view == 0)? hit->GetPosition().Y() : hit->GetPosition().X();
	int verse = (plane == 0)? 1 : -1;
	double z = verse*hit->GetTime()*0.105 + hit->GetPosition().Z() ;

	fHS->FillHistoList("MMStudy",Form("HitCharge"),hit->GetEnergy(),1.);
	clucharge += hit->GetEnergy();
	//	std::cout << "Clus " << i << " hit " << j << " / " << nhitsPerClus << " v,z = " << v << " , " << z << " time = " << hit->GetTime() << " bd = " << bdid << " lay = " << layer << " side = " << side << " view = " << view << " otherview = " << otherview << " strip = " << strip << std::endl;

	z2av += z*z; // now without error weights
	vzav += z*v;
	vav  += v;
	zav  += z;
      }    
      fHS->FillHistoList("MMStudy",Form("ClusterCharge"),clucharge,1.);

      z2av /= nhitsPerClus;
      vzav /= nhitsPerClus;
      vav /= nhitsPerClus;
      zav /= nhitsPerClus;
      double slope = (vzav - vav*zav)/(z2av - zav*zav);
      double inter = -slope*zav + vav;

      double chi2 = 0;
      for (int j=0; j<nhitsPerClus; j++) {
	TRecoVHit* hit = fEvent->MMRecoEvent->Hit(clus->GetHitVecInClus().at(j));
	double v = (view == 0)? hit->GetPosition().Y() : hit->GetPosition().X();
	int verse = (plane == 0)? 1 : -1;
	double z = verse*hit->GetTime()*0.105 + hit->GetPosition().Z() ;
	double res = v - (slope*z + inter);
	fHS->FillHisto2List("MMStudy",Form("ResVsNdf"),nhitsPerClus-2,res,1.);
	chi2 += res*res;
      }

      fHS->FillHisto2List("MMStudy",Form("Chi2OverNdfVsNdf"),nhitsPerClus-2,chi2/(nhitsPerClus-2),1.);


      int packed = (side << 2);
      packed |= (view<<1);
      packed |= (otherview);

      tracklet traco;
      traco.slope = slope;
      traco.inter = inter;
      tracklets[plane][packed].push_back(traco);
      
      fHS->FillHistoList("MMStudy",Form("PositionAtECal_view%d",view),(slope*fGeneralInfo->GetCOG().Z() + inter),1.);
      fHS->FillHistoList("MMStudy",Form("PositionAtTarget_view%d",view),(slope*fGeneralInfo->GetTargetPos().Z() + inter),1.);
      fHS->FillHisto2List("MMStudy",Form("PositionAtECalVsAtTarget_view%d",view),(slope*fGeneralInfo->GetTargetPos().Z() + inter),(slope*fGeneralInfo->GetCOG().Z() + inter),1.);

    }

  }

  for (int i=0; i<8; i++){
    for (uint i0 = 0; i0<tracklets[0][i].size(); i0++){
      tracklet traco0 = tracklets[0][i].at(i0);
      for (uint i1 = 0; i1<tracklets[1][i].size(); i1++){
	tracklet traco1 = tracklets[1][i].at(i1);

	fHS->FillHisto2List("MMStudy",Form("MatchingPlot"),traco0.slope-traco1.slope,traco0.inter-traco1.inter,1.);
	
      }
    }
  }
      

  
  // tracklets per plane
  // correlate slopes for the same view in the different planes
  // find cluster with slopes similar for the two planes
  // determine the time offset between the planes: DeltaZ/v_drift, where DeltaZ = Zplane1-Zplane0 at a specific v
  // correlate with calorimeter clusters: TimeECal-(TimeOffsetPlanes) vs posECal - posTrack

  // plot of nclus per board
  for (int bdid = 0; bdid<16; bdid++) fHS->FillHisto2List("MMStudy","NClustersPerBoard",bdid,nclusPerBoard[bdid],1.);
  */
  
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



