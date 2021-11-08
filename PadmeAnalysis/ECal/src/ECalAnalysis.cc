#include "ECalAnalysis.hh"
#include "TECalRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include <iostream>

ECalAnalysis::ECalAnalysis(): ValidationBase()
{
  fAlgoName = "ECal";
  InitHistos();
  return;
}
ECalAnalysis::ECalAnalysis(Int_t processingMode, Int_t verbosityFlag): ValidationBase(processingMode, verbosityFlag)
{
  fAlgoName = "ECal";
  InitHistos();
  UploadClShift();
  fxECalSurvey = 3.13;
  fyECalSurvey = 3.86;
  return;
}
ECalAnalysis::~ECalAnalysis()
{
}

Bool_t ECalAnalysis::InitHistosAnalysis()
{
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  std::string hname;
  int nBin, min, max;
  nBin=250;
  min=0.;
  max=500;
  hname="ECal_NHits";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=500;
  min=0.;
  max=1000;
  hname = "ECal_HitEnergy";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=5000;
  min=0.;
  max=10000;
  hname="ECal_EHitTot";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=250;
  min=0.;
  max=250;
  hname = "ECal_NCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);

  nBin=51;
  min=-0.5;
  max=50.5;
  hname = "ECal_NHitsInClus";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=500;
  min=0.;
  max=1000;
  hname = "ECal_ClusterEnergy";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=5000;
  min=0.;
  max=20000;
  hname="ECal_EClTot";
  hSvcVal->BookHisto(hname, nBin, min, max);


  nBin=1000;
  min=-200;
  max=200;
  hname="ECal_HitInClusterXw";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_HitInClusterYw";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_HitInClusterXvar";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_HitInClusterYvar";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_HitInClusterXcurt";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_HitInClusterYcurt";
  hSvcVal->BookHisto(hname, nBin, min, max);

  return true;
}

Bool_t ECalAnalysis::InitHistosValidation()
{
  //Validation histos
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  std::string hname;
  int nBin, min, max;
  nBin=100;
  min=0;
  max=200;
  hname="ECal_NHits";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=50;
  min=0;
  max=50;
  hname="ECal_NCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=300;
  min=0;
  max=600;
  hname = "ECal_HitEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = "ECal_ClusterEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=350;
  min=0;
  max=700;
  hname="ECal_TwoPhotonEnergy_TimeCoincidenceRequest5ns";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=1000;
  min=0;
  max=15000;
  hname = "ECal_HitTotEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = "ECal_ClusterTotEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=800;
  min=-400;
  max=400;
  hname = "ECal_HitTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  hname = "ECal_ClusterTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  nBin=200;
  min=-300;
  max=300;
  hname = "ECal_HitMap";
  hSvcVal->BookHisto2(hname, nBin, min, max, nBin, min, max);
  hname = "ECal_ClusterMap";
  hSvcVal->BookHisto2(hname, nBin, min, max, nBin, min, max);
  nBin=500;
  min=-300;
  max=300;
  hname="ECal_HitXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_HitYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=3;
  min=2545;
  max=2565;
  hname="ECal_HitZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=3000;
  min=0;
  max=3000;
  hname="ECal_HitChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterSeedChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=16;
  min=0;
  max=15;
  hname="ECal_NHitInCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  return true;
}


Bool_t ECalAnalysis::ProcessAnalysis()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit* hit=NULL;
  TRecoVCluster* clu=NULL;
  std::string hname;
  Int_t      chId;
  Double_t energy;
  Double_t   time;
    
  Int_t fNhits = fhitEvent->GetNHits();
  Int_t fNclus = fClColl->GetNElements();
  Int_t seedId;
  Int_t clSize;

  std::vector<Int_t> hitVInCl;

     //fillHitsFlatNTP

  (hSvc->myEvt).NTNECal_Hits=fhitEvent->GetNHits();
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTECal_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTECal_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTECal_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTECal_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTECal_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTECal_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP 

  (hSvc->myEvt).NTNECal_Clusters= fClColl->GetNElements(); 

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTECal_Clusters_ChannelId[j]=Double_t(clu->GetChannelId());
   (hSvc->myEvt).NTECal_Clusters_Energy[j]=clu->GetEnergy();
   (hSvc->myEvt).NTECal_Clusters_Time[j]=clu->GetTime();
   (hSvc->myEvt).NTECal_Clusters_Xpos[j]=clu->GetPosition().X();
   (hSvc->myEvt).NTECal_Clusters_Ypos[j]=clu->GetPosition().Y();
   (hSvc->myEvt).NTECal_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

   hname = "ECal_NHits";
   hSvc->FillHisto(hname, fNhits,1.);
   Double_t eTot  =  0.;
   for (Int_t i=0; i<fNhits; ++i){
     hit = fhitEvent->Hit(i);
     Double_t energy = hit->GetEnergy();
     hname = "ECal_HitEnergy";
     hSvc->FillHisto(hname, energy, 1.);
     eTot += energy;
   }
   hname="ECal_EHitTot";
   hSvc->FillHisto(hname, eTot, 1.);

   
   Double_t eTotCl;
   hname = "ECal_NCluster";
   hSvc->FillHisto(hname, fNclus,1.);

   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     eTotCl += clu->GetEnergy();
     hname = "ECal_ClusterEnergy";
     hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     clSize = clu->GetNHitsInClus();
     hname = "ECal_NHitsInClus";
     hSvc->FillHisto(hname, clSize,1.);

    TRecoVHit* hitSeed;
    hitSeed =  fhitEvent->Hit(clu->GetSeed());
    Double_t sumEincl=0;
    Double_t xW=0;
    Double_t yW=0;
    Double_t x2W=0;
    Double_t y2W=0;
    Double_t x3W=0;
    Double_t y3W=0;
    hitVInCl = (clu->GetHitVecInClus());
    for (int ih=0; ih< (clu->GetNHitsInClus()); ++ih)
      {
	if(hitVInCl[ih]==clu->GetSeed())continue;
	Double_t hitEn=fhitEvent->Hit(hitVInCl[ih])->GetEnergy();
	Double_t hitX=(fhitEvent->Hit(hitVInCl[ih]))->GetPosition().X();
	Double_t hitY=(fhitEvent->Hit(hitVInCl[ih]))->GetPosition().Y();
	Double_t hitXseed=(fhitEvent->Hit(clu->GetSeed()))->GetPosition().X();
	Double_t hitYseed=(fhitEvent->Hit(clu->GetSeed()))->GetPosition().Y();
	sumEincl+=hitEn;
	xW+=(hitX-hitXseed)*hitEn;
	yW+=(hitX-hitXseed)*hitEn;
	x2W+=(hitX-hitXseed)*(hitX-hitXseed)*hitEn;
	y2W+=(hitX-hitXseed)*(hitX-hitXseed)*hitEn;
	x3W+=(hitX-hitXseed)*(hitX-hitXseed)*(hitX-hitXseed)*hitEn;
	y3W+=(hitX-hitXseed)*(hitX-hitXseed)*(hitX-hitXseed)*hitEn;
      }
    Double_t xWeig=xW/sumEincl;
    Double_t yWeig=yW/sumEincl;
    Double_t xvar=sqrt(x2W/sumEincl);
    Double_t yvar=sqrt(y2W/sumEincl);
    Double_t xCurt=pow(x3W/sumEincl, 0.33333333);
    Double_t yCurt=pow(y3W/sumEincl, 0.33333333);
    hname="ECal_HitInClusterXw";
    hSvc->FillHisto(hname, xWeig, 1.);
    hname="ECal_HitInClusterYw";
    hSvc->FillHisto(hname, yWeig, 1.);
    hname="ECal_HitInClusterXvar";
    hSvc->FillHisto(hname, xvar, 1.);
    hname="ECal_HitInClusterYvar";
    hSvc->FillHisto(hname, yvar, 1.);
    hname="ECal_HitInClusterXcurt";
    hSvc->FillHisto(hname, xCurt, 1.);
    hname="ECal_HitInClusterYcurt";
    hSvc->FillHisto(hname, yCurt, 1.);
   }
   
   hname="ECal_EClTot";
   hSvc->FillHisto(hname, eTotCl, 1.);


  // HistoSvc* hSvc =  HistoSvc::GetInstance();

  // Double_t eMax  =  0.;
  // Int_t    iLead = -1;
  // Double_t eTot  =  0.;
  // TRecoVHit* hit=NULL;
  // TRecoVHit* hitn=NULL;
  // std::string hname;
  // Int_t fNhits = fhitEvent->GetNHits();
  // std::cout<<"+++++++++++++++++++++++++++++++++++++++++ ECalAnalysis ... "<<fNhits<<std::endl;
  // for (Int_t i=0; i<fNhits; ++i){
  //   hit = fhitEvent->Hit(i);
  //   Int_t ix = hit->GetChannelId()/100;
  //   Int_t iy = hit->GetChannelId()%100;
  //   Double_t energy = hit->GetEnergy();
  //   eTot += energy;
  //   if (energy > eMax) {iLead = i; eMax=energy;}
  //   Double_t time = hit->GetTime();

  //   // ntuple variables
  //   hSvc->myEvt.NTECluster[i]=hit->GetEnergy();
  //   hSvc->myEvt.NTTCluster[i]=hit->GetTime();
  //   hSvc->myEvt.NTXCluster[i]=hit->GetChannelId()/100;
  //   hSvc->myEvt.NTYCluster[i]=hit->GetChannelId()%100;

  //  }

  //  if (fNhits<1) return true;
  //  TRecoVHit* lHit = fhitEvent->Hit(iLead);
  //  Int_t lix = lHit->GetChannelId()/100;
  //  Int_t liy = lHit->GetChannelId()%100;
  //  Double_t lenergy = lHit->GetEnergy();

  //  hname = "ECal_LeadingHitTime";
  //  hSvc->FillHisto(hname,lHit->GetTime(),1.);
  //  hname = "ECal_LeadingHitEnergy";
  //  hSvc->FillHisto(hname,lenergy,1.);
  //  hname = "ECal_LeadingHitMap";
  //  hSvc->FillHisto2(hname, lix, liy, 1.);
  //  hname = "ECal_LeadingEneMap";
  //  hSvc->FillHisto2(hname, lix, liy, lenergy);


  //  TRecoVCluster* clu=NULL;
  //  TRecoVCluster* clun=NULL;
  //  //std::cout<<"ECal sanity checks:"
  //  Int_t seed;
  //  Int_t seedId;
  //  Double_t seedE;
  //  Double_t seedT;
  //  Double_t eTotCl;
  //  Int_t clSize;
  //  Int_t fNclus = fClColl->GetNElements();
  //  for (Int_t i=0; i<fNclus; ++i){
  //    clu    = fClColl->Element(i);
  //    seed   = clu->GetSeed();
  //    seedId = clu->GetChannelId();
  //    Int_t checkseedId =  fhitEvent->Hit(seed)->GetChannelId();
  //    if ( checkseedId != seedId ) {
  //      std::cout<<"ERROR on ECal indices --- seed: clustID, hit(seed).id = "<<seed<<" "<<seedId<<" "<<checkseedId<<std::endl;
  //    }
  //    //     else
  //    //       {
  //    //	 std::cout<<"++++++++++++++++++++     Sanity check OK ... "<<std::endl;
  //    //       }
  //    seedE  = clu->GetEnergy();
  //    seedT  = clu->GetTime();
  //    clSize = clu->GetNHitsInClus();
  //    //std::cout<<"ECal:: cluster n. "<<i<<" size, seed  "<<clSize<<" "<<seed<<"  chId, E, T "<<seedId<<" "<<seedE<<" "<<seedT<<std::endl;
  //    std::vector<Int_t> vhinclu = clu->GetHitVecInClus();
  //    for (Int_t ih=0; ih<clSize; ++ih){
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<std::endl;
  //      Int_t hitInd    = vhinclu[ih];
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" hitIndex = "<<hitInd<<std::endl;
  //      if (hitInd == seed) {std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<"- skipping seed"<<std::endl;continue;}
  //      hit = fhitEvent->Hit(hitInd);
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" hitIndex = "<<hitInd<<" chid, E, T "
  //      //	<<hit->GetChannelId()<<" "<<hit->GetEnergy()<<" "<<hit->GetTime()<<std::endl;
  //      hname = "ECal_inClus_DCHXseed";
  //      hSvc->FillHisto(hname,hit->GetChannelId()/100-seedId/100,1.);
  //      hname = "ECal_inClus_DCHYseed";
  //      hSvc->FillHisto(hname,hit->GetChannelId()%100-seedId%100,1.);
  //      hname = "ECal_inClus_DTseed";
  //      hSvc->FillHisto(hname,hit->GetTime()-seedT,1.);
  //      for (Int_t ihn=ih+1; ihn<clSize; ++ihn){
  // 	 Int_t hitIndn    = vhinclu[ihn];
  // 	 //if (hitIndn == seed) continue;
  // 	 hitn = fhitEvent->Hit(hitIndn);
  // 	 //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" vs comp in="<<ihn<<" chId, T "<<hitn->GetChannelId()<<" "<<hitn->GetTime()<<std::endl;
  // 	 hname = "ECal_inClus_DCHXhits";
  // 	 hSvc->FillHisto(hname,hit->GetChannelId()/100-hitn->GetChannelId()/100,1.);
  // 	 hname = "ECal_inClus_DCHYhits";
  // 	 hSvc->FillHisto(hname,hit->GetChannelId()%100-hitn->GetChannelId()%100,1.);
  // 	 hname = "ECal_inClus_DThits";
  // 	 hSvc->FillHisto(hname,hit->GetTime()-hitn->GetTime(),1.);
  //      }
  //    }
  //    for (Int_t in=0; in<fNclus; ++in){
  //      clun    = fClColl->Element(in);
  //      //
  //      hname = "ECal_Clus2Clus_seedDCHX";
  //      hSvc->FillHisto(hname,clun->GetChannelId()/100-clu->GetChannelId()/100,1.);
  //      hname = "ECal_Clus2Clus_seedDCHY";
  //      hSvc->FillHisto(hname,clun->GetChannelId()/100-clu->GetChannelId()%100,1.);
  //      hname = "ECal_Clus2Clus_seedDT";
  //      hSvc->FillHisto(hname,clun->GetTime()-clu->GetTime(),1.);
  //    }
  //  }


   return retCode;
}


Bool_t ECalAnalysis::ProcessValidation()
{  
  ValidationBase::ProcessValidation("ECal");
  
  
  Bool_t retCode = 0;

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  TRecoVHit* hit=NULL;
  std::string hname;
  Double_t ETotHit=0.;
  Int_t fNhits = fhitEvent->GetNHits();
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Int_t ix = hit->GetPosition().X();
    Int_t iy = hit->GetPosition().Y();
    ETotHit += hit->GetEnergy();
    //Int_t ix=position.X();
    //Int_t iy=position.Y();
  
    hname = "ECal_HitMap";
    hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);

    
   }
   hname="ECal_HitTotEnergy";
   hSvcVal->FillHisto(hname, ETotHit, 1.);

   TRecoVCluster* clu=NULL;
   TRecoVCluster* clun=NULL;
   Int_t fNclus = fClColl->GetNElements();
   Double_t ETotCl=0.;
   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     Int_t ix = clu->GetPosition().X();
     Int_t iy = clu->GetPosition().Y();
     ETotCl+=clu->GetEnergy();
     //Int_t ix=position.X();
     //Int_t iy=position.Y();
     hname = "ECal_ClusterMap";
     hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
     for(int j=i+1; j< fNclus; j++){
       clun   = fClColl->Element(j);
       if(fabs(clu->GetTime() - clun->GetTime())<5.)
       {
          hname="ECal_TwoPhotonEnergy_TimeCoincidenceRequest5ns";
          hSvcVal->FillHisto(hname, clu->GetEnergy()+clun->GetEnergy());
       }
     }
   }
   hname="ECal_ClusterTotEnergy";
   hSvcVal->FillHisto(hname, ETotCl, 1.);


   return retCode;
}




void ECalAnalysis::EnergyCalibration(Bool_t isMC , Bool_t SPAnalysis)
{
  TRecoVHit* hit;
  Int_t fNhits =fhitEvent->GetNHits();
  Double_t eTot  =  0.;
  Double_t constantToAddForDataSeptember20=1.02242195116151890e+00;
  Double_t constantForDataJuly19=1.084; 
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    if(hit){
      if (!isMC)
	{
	  if (SPAnalysis) hit  ->SetEnergy(constantForDataJuly19*(hit->GetEnergy()));
	  else hit  ->SetEnergy(constantForDataJuly19*constantToAddForDataSeptember20*(hit->GetEnergy())); 
	}
    }
  }
  
  TRecoVCluster* xClu;
  for (int h=0; h<fClColl->GetNElements(); ++h)
    {
      xClu = fClColl->Element((int)h);
      if (xClu) 
	{
	  if (!isMC){
	    if (SPAnalysis) xClu  ->SetEnergy(constantForDataJuly19*(xClu->GetEnergy()));
	    else xClu  ->SetEnergy(constantForDataJuly19*constantToAddForDataSeptember20*(xClu->GetEnergy()));
	  }
	}
    }
  return;

}




void ECalAnalysis::UploadClShift(){
  //from Selector
  /*
  fXBeamOnECalPlanePerRun.insert({30369,1.861}); // my COGx after ECal survey position 
  fYBeamOnECalPlanePerRun.insert({30369,0.655});  // my COGy after ECal survey position 
  
  fXBeamOnECalPlanePerRun.insert({30386,3.641}); // my COGx after ECal survey position 
  fYBeamOnECalPlanePerRun.insert({30386,0.28});  // my COGy after ECal survey position   

  fXBeamOnECalPlanePerRun.insert({30547,1.2124}); // my COGx after ECal survey position 
  fYBeamOnECalPlanePerRun.insert({30547,-0.46});  // my COGy after ECal survey position 

  fXBeamOnECalPlanePerRun.insert({30553,1.0357}); // my COGx after ECal survey position 
  fYBeamOnECalPlanePerRun.insert({30553,0.99});  // my COGy after ECal survey position   

  fXBeamOnECalPlanePerRun.insert({30563,0.69}); // my COGx after ECal survey position 
  fYBeamOnECalPlanePerRun.insert({30563,0.0});  // my COGy after ECal survey position
   
  fXBeamOnECalPlanePerRun.insert({30617,4.075}); // my COGx after ECal survey position 
  fYBeamOnECalPlanePerRun.insert({30617,0.1157});  // my COGy after ECal survey position 

  fXBeamOnECalPlanePerRun.insert({30624,5.007}); // my COGx after ECal survey position 
  fYBeamOnECalPlanePerRun.insert({30624,0.0});  // my COGy after ECal survey position 
  */

  //from parameters.dat
  
  fXBeamOnECalPlanePerRun.insert({30369,1.94603}); 
  fYBeamOnECalPlanePerRun.insert({30369,1.13799}); 
  
  fXBeamOnECalPlanePerRun.insert({30386,3.59759}); 
  fYBeamOnECalPlanePerRun.insert({30386,1.36553});  

  fXBeamOnECalPlanePerRun.insert({30547, 1.06341});
  fYBeamOnECalPlanePerRun.insert({30547,0.152757}); 

  fXBeamOnECalPlanePerRun.insert({30553, -0.110995});
  fYBeamOnECalPlanePerRun.insert({30553,1.72303});  

  fXBeamOnECalPlanePerRun.insert({30563,0.856649}); 
  fYBeamOnECalPlanePerRun.insert({30563, 0.667697});  
   
  fXBeamOnECalPlanePerRun.insert({30617, 4.075});
  fYBeamOnECalPlanePerRun.insert({30617, 0.1157}); 

  fXBeamOnECalPlanePerRun.insert({30624,5.61558}); 
  fYBeamOnECalPlanePerRun.insert({30624,0.784524});  
  

}




void ECalAnalysis::ShiftPositions(int RunN, Bool_t isMC){

  TRecoVHit* hit;
  for (Int_t i=0; i<fhitEvent->GetNHits(); ++i){
    hit = fhitEvent->Hit(i);
    if(hit){
      if (!isMC)
	{
	  Double_t x =fxECalSurvey + hit->GetPosition().X();
	  Double_t y =fyECalSurvey + hit->GetPosition().Y();
	  Double_t z =               hit->GetPosition().Z();
	  if (fXBeamOnECalPlanePerRun.find(RunN)==fXBeamOnECalPlanePerRun.end()){
	    Double_t xoff = 0.;
	    Double_t yoff = 0.;
	    Double_t xL = x - xoff;
	    Double_t yL = y - yoff;
	    TVector3 position(xL, yL, z);
	    hit->SetPosition(position);
	  }
	  else{
	      Double_t xoff = fXBeamOnECalPlanePerRun[RunN];
	      Double_t yoff = fYBeamOnECalPlanePerRun[RunN];
	      Double_t xL = x - xoff;
	      Double_t yL = y - yoff;
	      TVector3 position(xL, yL, z);
	      hit->SetPosition(position);
	  }
	}
    }
  }
  
  TRecoVCluster* xClu;
  for (int h=0; h<fClColl->GetNElements(); ++h)
    {
      xClu = fClColl->Element((int)h);
      if (xClu) 
	{
	  if (!isMC){
	    Double_t x =fxECalSurvey + xClu->GetPosition().X();
	    Double_t y =fyECalSurvey + xClu->GetPosition().Y();
	    Double_t z =               xClu->GetPosition().Z();
	    //std::cout<<"cluster "<< h <<" before shifting x " << xClu->GetPosition().X() << " ecal shift " << fxECalSurvey << " cog " << fXBeamOnECalPlanePerRun[RunN] << " -> " <<fxECalSurvey+xClu->GetPosition().X()-fXBeamOnECalPlanePerRun[RunN] << std::endl; 
	    if (fXBeamOnECalPlanePerRun.find(RunN)==fXBeamOnECalPlanePerRun.end()){
	      Double_t xoff = 0.;
	      Double_t yoff = 0.;
	      Double_t xL = x - xoff;
	      Double_t yL = y - yoff;
	      TVector3 position(xL, yL, z);
	      xClu->SetPosition(position);
	    }
	    else{
	      Double_t xoff = fXBeamOnECalPlanePerRun[RunN];
	      Double_t yoff = fYBeamOnECalPlanePerRun[RunN];
	      Double_t xL = x - xoff;
	      Double_t yL = y - yoff;
	      TVector3 position(xL, yL, z);
	      xClu->SetPosition(position);
	      // std::cout<<"after shifting x " << xClu->GetPosition().X()  << " y " << xClu->GetPosition().Y() << std::endl; 
	    }
	   
	  }
	}
    }
  return;


}



std::vector<int> ECalAnalysis::passClCut(){

  std::vector<int> ClPassedCut;

  Double_t  distClosestClInT, distClosestClOutT, ELCl, EsLClInT, EsLClOutT, dist, xp, yp;
  Int_t iCloseInT, iCloseOutT;
  Double_t distFromSeed;
  TRecoVHit* ahit;
  std::vector<Int_t> hitV;

  Int_t passedCut = 0;
  TRecoVCluster* xClu;
  TRecoVCluster* xClu2;
  for (int h=0; h<fClColl->GetNElements(); ++h){
      xClu = fClColl->Element((int)h);
      hitV = (xClu->GetHitVecInClus());
      ClPassedCut.push_back(passedCut);
      if (xClu->GetTime()<-110.) continue; 
       ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 1

      Double_t  ClusterXDistFromSeed=xClu->GetPosition().X() - (fhitEvent->Hit(xClu->GetSeed()))->GetPosition().X();
      Double_t  ClusterYDistFromSeed=xClu->GetPosition().Y() - (fhitEvent->Hit(xClu->GetSeed()))->GetPosition().Y();
      distFromSeed = sqrt(ClusterXDistFromSeed*ClusterXDistFromSeed   +  ClusterYDistFromSeed*ClusterYDistFromSeed);
      if(distFromSeed > 20.) continue; 
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 2


      //if (ClusterE->at(ic)<90.) continue;
      //fClPassedCut.at(ic) = fClPassedCut.at(ic) + 1; 

      //if (ClusterE->at(ic)>500.) continue;
      //fClPassedCut.at(ic) = fClPassedCut.at(ic) + 1; 

      Double_t hitE=0.;
      Double_t hitX=0.;
      Double_t hitY=0.;
      Double_t hitT=0.;

      Double_t xC=0.;
      Double_t yC=0.;
      Double_t tC=0.;
      
      Double_t sumE =0.;
      Double_t xSres=0.;
      Double_t ySres=0.;
      Double_t xSres2=0.;
      Double_t ySres2=0.;
      Double_t xSres3=0.;
      Double_t ySres3=0.;
      Double_t tSres  = 0.;
      Double_t tSres2 = 0.;
      Double_t covS = 0.;
      Double_t clFarX = -100.;
      Double_t clFarY = -100.;
      Double_t distX = 0.;
      Double_t distY = 0.;
      Double_t xres; 
      Double_t yres; 
      Double_t tres; 
      for (int i=0; i< xClu->GetNHitsInClus(); ++i){
	ahit =  fhitEvent->Hit(hitV[i]);;
	hitE = 	ahit->GetEnergy();
	hitT = 	ahit->GetTime();
	hitX = ahit->GetPosition().X();
	hitY = ahit->GetPosition().Y();
	sumE = sumE + hitE;
	// x,y,t centroid
	xC = xC + hitX*hitE; 
	yC = yC + hitY*hitE; 
	tC = tC + hitT*hitE;

	xres = hitX-xClu->GetPosition().X();
	yres = hitY-xClu->GetPosition().Y();
	tres = hitT-xClu->GetTime();
	
	//residuals x,y,t
	xSres += (xres*hitE);
	ySres += (yres*hitE);
	tSres += (tres*hitE);
	//residuals^2 x,y,t
	xSres2 = xSres2 + xres * xres * hitE;
	ySres2 = ySres2 + yres * yres * hitE;
	tSres2 = tSres2 + tres * tres * hitE;
	covS   = covS   + xres * yres * hitE;
	
	//residuals^3 x,y,t
	xSres3 = xSres3 + xres * xres * xres * hitE;
	ySres3 = ySres3 + yres * yres * yres * hitE;
	//if (clE>90.) std::cout<<" xres, xres * xres * xres * hitE, hitE, xSres3 "<<xres<<" "<<xres * xres * xres * hitE<<" "<<hitE<<" "<<xSres3<<std::endl;
	
	//dist from seed
	distX = fabs(hitX -  (fhitEvent->Hit(xClu->GetSeed()))->GetPosition().X());
	if (distX>clFarX) clFarX = distX;
	distY = fabs(hitY -  (fhitEvent->Hit(xClu->GetSeed()))->GetPosition().Y());
	if (distY>clFarY) clFarY = distY;	
      }
  
      xC = xC / sumE;
      yC = yC / sumE;
      tC = tC / sumE;
      xSres = xSres/ sumE;
      ySres = ySres/ sumE;
      tSres = tSres/ sumE;
      xSres2 = xSres2/ sumE;
      ySres2 = ySres2/ sumE;
      tSres2 = tSres2/ sumE;

      Double_t xRMS = sqrt(xSres2);
      Double_t yRMS = sqrt(ySres2);
      Double_t tRMS = sqrt(tSres2);
      Double_t lcc = covS/sumE;
      lcc = lcc/xRMS/yRMS;


      if (xClu->GetNHitsInClus()>2 && xRMS<1.) continue; 
      if (xRMS<1.) continue; 
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 3

      if (xClu->GetNHitsInClus()>2 && yRMS<1.) continue; 
      if (yRMS<1.) continue; 
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 4

      if (xClu->GetNHitsInClus()==1) continue;
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 5

      if (xClu->GetNHitsInClus()>2 && clFarX<1.) continue; 
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 6

      if (xClu->GetNHitsInClus()>2 && clFarY<1.) continue; 
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 7

      if (fabs(lcc)>0.99) continue; 
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 8
      
      if (tRMS>3.) continue;
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 9

      
      distClosestClInT=999999.;
      ELCl     = 999999.;
      EsLClInT = 999999.;
      distClosestClOutT=999999.;
      EsLClOutT = 999999.;
      iCloseInT = -1;
      iCloseOutT = -1;
      
      for(Int_t i=1; i<fClColl->GetNElements(); ++i)
	{
	  if (i==h) continue;
	  xClu2 = fClColl->Element((int)i);
	  dist = sqrt( (xClu2->GetPosition().X() - xClu->GetPosition().X())*(xClu2->GetPosition().X() - xClu->GetPosition().X())+(xClu2->GetPosition().Y() - xClu->GetPosition().Y())*(xClu2->GetPosition().Y() - xClu->GetPosition().Y()) ); 

	  if (fabs(xClu->GetTime()-xClu2->GetTime())<10.)
	    {
	      if ( dist < distClosestClInT )
		{
		  distClosestClInT = dist;
		  iCloseInT = i;
		}
	    }
	  else
	    {
	      if ( dist < distClosestClOutT )
		{
		  distClosestClOutT = dist;
		  iCloseOutT = i;
		}
	    }
	}/*
      ELCl      = xClu->GetEnergy();
      if (iCloseInT > ic) {
	if ( iCloseInT>-1) 
	  {
	    EsLClInT = ClusterE->at(iCloseInT);
	    if (ClusterE->at(ic) <  ClusterE->at(iCloseInT))  {EsLClInT  = ClusterE->at(ic);ELCl     = ClusterE->at(iCloseInT);}
	    iFill ("hClQ_distClosestClInT", distClosestClInT);
	    iFill2d("hClQ_distClosestClInTVsEsL", EsLClInT, distClosestClInT);
	    iFill2d("hClQ_distClosestClInTVsSumE", EsLClInT+ELCl, distClosestClInT);
	  }
      }
      if (iCloseOutT > ic) {
	if ( iCloseOutT>-1) 
	  {      
	    EsLClOutT = ClusterE->at(iCloseOutT);
	    if (ClusterE->at(ic) <  ClusterE->at(iCloseOutT)) {EsLClOutT = ClusterE->at(ic);ELCl     = ClusterE->at(iCloseOutT);}
	    iFill ("hClQ_distClosestClOutT", distClosestClOutT);
	    iFill2d("hClQ_distClosestClOutTVsEsL", EsLClOutT, distClosestClOutT);
	    iFill2d("hClQ_distClosestClOutTVsSumE", EsLClOutT+ELCl, distClosestClOutT);
	  }
      }
	 */
      if (distClosestClInT < 200.) continue;  
      ClPassedCut.at(h) = ClPassedCut.at(h) + 1; // 10
            
    }


  //std::cout<<"Ncluster " << (int)(fClColl->GetNElements()) << " vector size " << ClPassedCut.size() << std::endl;

    return ClPassedCut;
}
