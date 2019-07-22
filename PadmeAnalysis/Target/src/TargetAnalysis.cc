#include "TargetAnalysis.hh"
#include "TRecoEvent.hh"
#include "TTargetRecoEvent.hh"
#include "TRecoVHit.hh"
#include "HistoSvc.hh"
#include <iostream>

TargetAnalysis::TargetAnalysis()
{
  fRecoEvent = NULL; 
  fhitEvent  = NULL;
  fRecoBeam  = NULL;
  fProcessingMode = 0;
  fVerbose   = 0;
  fAlgoName  = "Target"; 


  nEvsTarget         = 0;
  nEvsGoodX          = 0;
  nEvsGoodY          = 0;
  nEvsGoodXY         = 0;
  nPOTgoodXY_job     = 0.; 
  nPOTErrgoodXY_job  = 0.;
  nPOTbadXorY_job    = 0.;
  InitHistos();

}

TargetAnalysis::TargetAnalysis(Int_t Validation, Int_t verb)
{
  fRecoEvent = NULL; 
  fhitEvent  = NULL;
  fRecoBeam  = NULL;
  fProcessingMode = Validation;
  fVerbose    = verb;
  fAlgoName   = "Target"; 

  nEvsTarget         = 0;
  nEvsGoodX          = 0;
  nEvsGoodY          = 0;
  nEvsGoodXY         = 0;
  nPOTgoodXY_job     = 0.; 
  nPOTErrgoodXY_job  = 0.;
  nPOTbadXorY_job    = 0.;
  InitHistos();

}

TargetAnalysis::~TargetAnalysis()
{
  fhitEvent=NULL;
}
Bool_t TargetAnalysis::Init(TRecoEvent* EventHeader, TTargetRecoEvent* ev, TTargetRecoBeam* b)
{
  Bool_t retCode = 0;
  fRecoEvent = EventHeader;
  fhitEvent = ev;
  fRecoBeam = b;
  return retCode;
}

Bool_t TargetAnalysis::InitHistosAnalysis()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;

   hname = "Target_Yprofile";
   hSvc->BookHisto(hname, 19, 0.5, 19.5);
   hname = "Target_Xprofile";
   hSvc->BookHisto(hname, 19, 0.5, 19.5);
   hname = "TargetBeam_nPOT";
   hSvc->BookHisto(hname, 100, 0., 30000.);
   return true;
}
Bool_t TargetAnalysis::InitHistosValidation()
{
    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;

    int Bin=  18;
    int minX= -0.5;
    int maxX= 17.5;
    int minY= -0.5;
    int maxY= 17.5;
    hname = "Target_HitXprofile";
    hSvc->BookHisto(hname, Bin, minX, maxX);
    hname = "Target_HitYprofile";
    hSvc->BookHisto(hname, Bin, minY, maxY);
    hname="Target_HitChannelId";
    hSvc->BookHisto(hname, Bin, minX, maxY);
    int nBin=36;
    int min=-0.5;
    int max=35.5;
    hname="Target_NHits";
    hSvc->BookHisto(hname, nBin, min, max);
    nBin= 500;
    int tmin=0;
    int tmax=500;
    hname ="Target_XstripTime";
    hSvc->BookHisto(hname,nBin ,tmin,tmax );
    hname ="Target_YstripTime";
    hSvc->BookHisto(hname,nBin ,tmin,tmax);
    hname="Target_XstripEne";
    hSvc->BookHisto(hname,nBin, -0.5,0.5);
    hname="Target_YstripEne";
    hSvc->BookHisto(hname,nBin, -0.5,0.5);
    
    nBin= 800;
    min =-8.5;
    max = 7.5;
    hname ="TargetBeam_XCentroid";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCentroid";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_XCfit";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCfit";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin= 500;
    min=-1;
    max=2.5;
    hname ="TargetBeam_XCentroidError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCentroidError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_XCfitError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCfitError";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin= 500;
    min=-4; //pC
    max=4;
    hname ="TargetBeam_XCharge";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCharge";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin= 110;
    min=-1;
    max=10;
    hname ="TargetBeam_XChargeError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YChargeError";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=400;
    min=0;
    max=400;
    hname ="TargetBeam_XTime";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YTime";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=20;
    min=-10;
    max=10;
    hname ="TargetBeam_XTimeError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YTimeError";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=80;
    min=0;
    max=15;
    hname ="TargetBeam_XWidth";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YWidth";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_XWfit";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YWfit";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=100;
    min=-1;
    max=1;
    hname ="TargetBeam_XWidthError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YWidthError";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=150;
    min=-3;
    max=100;
    hname ="TargetBeam_XWfitError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YWfitError";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=500;
    min=0;
    max=50;
    hname ="TargetBeam_XChi2";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YChi2";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=100;
    min=0;
    max=10;
    hname ="TargetBeam_XNdof";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YNdof";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=33000;
    min=0;
    max=33000;
    hname ="TargetBeam_nPOT";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=400;
    min=0;
    max=200;
    hname ="TargetBeam_nPOTError";
    hSvc->BookHisto(hname,nBin ,min,max);

    
    

    return true;
}

Bool_t TargetAnalysis::InitHistosDataQuality()
{
    HistoSvc* hSvc =  HistoSvc::GetInstance();
    std::string hname;

    int Bin=  19;
    int minX= -0.5;
    int maxX= 18.5;
    int minY= -0.5;
    int maxY= 18.5;
    /*int minX= -8.5;
    int maxX=  7.5;
    int minY= -8.5;
    int maxY=  7.5;*/
    hname = "Target_HitXprofile";
    hSvc->BookHisto(hname, Bin, minX, maxX);
    hname = "Target_HitYprofile";
    hSvc->BookHisto(hname, Bin, minY, maxY);
    hname="Target_HitChannelId";
    hSvc->BookHisto(hname, Bin, minX, maxY);
    int nBin=36;
    int min=-0.5;
    int max=35.5;
    hname="Target_NHits";
    hSvc->BookHisto(hname, nBin, min, max);
    nBin= 200;
    min=-1; //pC
    max=3;
    hname ="TargetBeam_XCharge";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCharge";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_XCharge_CRtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCharge_CRtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_XCharge_BTFtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCharge_BTFtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_XCharge_AUTOtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCharge_AUTOtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_MeanXYCharge";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_MeanXYCharge_CRtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_MeanXYCharge_BTFtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_MeanXYCharge_AUTOtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);


    nBin= 110;
    min=-1;
    max=10;
    hname ="TargetBeam_XChargeError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YChargeError";
    hSvc->BookHisto(hname,nBin ,min,max);

    nBin= 800;
    min =-8.5;
    max = 7.5;
    hname ="TargetBeam_XCentroid";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCentroid";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_XCfit";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCfit";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin= 500;
    min=-1;
    max=5;
    hname ="TargetBeam_XCentroidError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCentroidError";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin= 500;
    min=-1;
    max=30;
    hname ="TargetBeam_XCfitError";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_YCfitError";
    hSvc->BookHisto(hname,nBin ,min,max);
 
    min=0;
    max=33000;
    hname ="TargetBeam_nPOT";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_nPOT_CRtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_nPOT_BTFtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    hname ="TargetBeam_nPOT_AUTOtrigger";
    hSvc->BookHisto(hname,nBin ,min,max);
    nBin=400;
    min=0;
    max=200;
    hname ="TargetBeam_nPOTError";
    hSvc->BookHisto(hname,nBin ,min,max);

    nBin=36;
    int binX=15000;
    int binY=20;
    minX=0;
    maxX=15000;
    minY=-1;
    maxY=3;
    hname="Trend_ChargeXvsEventNumber";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="Trend_ChargeYvsEventNumber";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="Trend_CentroidXvsEventNumber";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="Trend_CentroidYvsEventNumber";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="Trend_XCfitvsEventNumber";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
    hname="Trend_YCfitvsEventNumber";
    hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);


   

    return true;
}

Bool_t TargetAnalysis::ProcessAnalysis()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit* hit=NULL;
  std::string hname;
  Int_t fNhits = fhitEvent->GetNHits();
  (hSvc->myEvt).NTNTarget_Hits=fhitEvent->GetNHits();
  if ( fNhits < 0 ) return retCode;
 
  //fill Hits flat ntuple

  
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    (hSvc->myEvt).NTTarget_Hits_ChannelId[i]=(Double_t)hit->GetChannelId();
    (hSvc->myEvt).NTTarget_Hits_Energy[i]=hit->GetEnergy();
    (hSvc->myEvt).NTTarget_Hits_Time[i]=hit->GetTime();
    (hSvc->myEvt).NTTarget_Hits_Xpos[i]=hit->GetPosition().X();
    (hSvc->myEvt).NTTarget_Hits_Ypos[i]=hit->GetPosition().Y();
    (hSvc->myEvt).NTTarget_Hits_Zpos[i]=hit->GetPosition().Z();
  }


   //fill ntuple with target recobeam

   (hSvc->myEvt).NTTargetBeam_X=fRecoBeam->getX();
   (hSvc->myEvt).NTTargetBeam_Y=fRecoBeam->getY();
   (hSvc->myEvt).NTTargetBeam_XErr=fRecoBeam->getXError();
   (hSvc->myEvt).NTTargetBeam_YErr=fRecoBeam->getYError();
   (hSvc->myEvt).NTTargetBeam_XW=fRecoBeam->getXWidth();
   (hSvc->myEvt).NTTargetBeam_YW=fRecoBeam->getYWidth();
   (hSvc->myEvt).NTTargetBeam_XWErr=fRecoBeam->getXWidthError();
   (hSvc->myEvt).NTTargetBeam_YWErr=fRecoBeam->getYWidthError();
   (hSvc->myEvt).NTTargetBeam_XChi2=fRecoBeam->getXChi2();
   (hSvc->myEvt).NTTargetBeam_YChi2=fRecoBeam->getYChi2();
   (hSvc->myEvt).NTTargetBeam_XNdof=fRecoBeam->getXNdof();
   (hSvc->myEvt).NTTargetBeam_YNdof=fRecoBeam->getYNdof();
   (hSvc->myEvt).NTTargetBeam_XCharge=fRecoBeam->getXCharge();
   (hSvc->myEvt).NTTargetBeam_XChargeErr=fRecoBeam->getXChargeErr();
   (hSvc->myEvt).NTTargetBeam_YCharge=fRecoBeam->getYCharge();
   (hSvc->myEvt).NTTargetBeam_YChargeErr=fRecoBeam->getYChargeErr();
   (hSvc->myEvt).NTTargetBeam_XTime=fRecoBeam->getXTime();
   (hSvc->myEvt).NTTargetBeam_XTimeErr=fRecoBeam->getXTimeErr();
   (hSvc->myEvt).NTTargetBeam_YTime=fRecoBeam->getYTime();
   (hSvc->myEvt).NTTargetBeam_YTimeErr=fRecoBeam->getYTimeErr();
   (hSvc->myEvt).NTTargetBeam_NPOT=fRecoBeam->getnPOT();
   (hSvc->myEvt).NTTargetBeam_NPOTErr=fRecoBeam->getnPOTError();

   
   
  
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    //Bool_t isX=true;
    Int_t    istrip    = hit->GetChannelId();
    Double_t energy = hit->GetEnergy();
    
    if (istrip > 16) {
      istrip = istrip-16;
      hname = "Target_Yprofile";
      hSvc->FillHisto(hname,(Double_t)istrip,energy);
    }
    else {
      hname = "Target_Xprofile";
      hSvc->FillHisto(hname,(Double_t)istrip,energy);
    }
  }

    
    
   //TTargetRecoBeam* pRecoBeam = fhitEvent->getTargetRecoBeam();
   if (fVerbose>1) std::cout<<" from the TargetRecoEvent Beam X, Y "<<fRecoBeam->getX()<<" "<<fRecoBeam->getY()<<std::endl;


   Bool_t goodX = false;
   Bool_t goodY = false;
   ++nEvsTarget;
   if (true/*fRecoBeam->getXChi2()/fRecoBeam->getXNdof() < 200000.*/) 
     {
       goodX=true;
       ++nEvsGoodX;
     }
 else 
      {
	if (fVerbose>0) std::cout<<"TargetAnalysis::Process .... " <<nEvsTarget<<" Events processed so far; This bunch has bad X profile;  "<<nEvsTarget-nEvsGoodX<<" bad X profile so far"<<fRecoBeam->getXChi2()<<" "<<fRecoBeam->getXNdof()<<std::endl;
      }
   if (true/*fRecoBeam->getYChi2()/fRecoBeam->getYNdof() < 200. */) 
     {
       goodY=true;
       ++nEvsGoodY;
     }
   else 
     {
       if (fVerbose>0) std::cout<<"TargetAnalysis::Process .... " <<nEvsTarget<<" Events processed so far; This bunch has bad Y profile;  "<<nEvsTarget-nEvsGoodY<<" bad Y profile so far: chi2, ndof = "<<fRecoBeam->getYChi2()<<" "<<fRecoBeam->getYNdof()<<std::endl;
     }
   if (goodX && goodY) 
     {
       ++nEvsGoodXY;
       
       hname = "TargetBeam_nPOT";
       hSvc->FillHisto(hname, fRecoBeam->getnPOT(), 1.);
       
       nPOTgoodXY_job = nPOTgoodXY_job+fRecoBeam->getnPOT();
       nPOTErrgoodXY_job = nPOTErrgoodXY_job + fRecoBeam->getnPOTError()*fRecoBeam->getnPOTError();

       if (fVerbose>0) {
	 std::cout<<"TargetAnalysis::Process .... " <<nEvsTarget<<" Events processed so far; This bunch has good X,Y profiles  "<<std::endl;
	 std::cout<<"TargetAnalysis::Process .... " <<fRecoBeam->getnPOT()<<" POT in this event; "<<nPOTgoodXY_job<<" POT in good events so far"<<std::endl;
	 std::cout<<"TargetAnalysis::Process .... " <<fRecoBeam->getnPOTError()<<" POTerr in this event; "<<nPOTErrgoodXY_job<<" POTerr in good events so far"<<std::endl;
       }

     }
   else
     {
       nPOTbadXorY_job = nPOTbadXorY_job+fRecoBeam->getnPOT();
       nPOTErrbadXorY_job = nPOTErrbadXorY_job + fRecoBeam->getnPOTError()*fRecoBeam->getnPOTError();
       if (fVerbose>0) {
	 std::cout<<"TargetAnalysis::Process .... " <<nEvsTarget<<" Events processed so far; This bunch has BAD X or Y profile  "<<std::endl;
	 std::cout<<"TargetAnalysis::Process .... " <<fRecoBeam->getnPOT()<<" POT in this event; "<<nPOTbadXorY_job<<" POT in BAD events so far"<<std::endl;
	 std::cout<<"TargetAnalysis::Process .... " <<fRecoBeam->getnPOTError()<<" POTerr in this event; "<<nPOTErrbadXorY_job<<" POTerr in BAD events so far"<<std::endl;
       }
     }

   return retCode;
}


Bool_t TargetAnalysis::ProcessDataQuality()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::cout<<"In TargetAnalysis::ProcessDataQuality"<<std::endl;


  TRecoVHit* hit=NULL;
  std::string hname;
  Int_t fNhits = fhitEvent->GetNHits();
  hname="Target_NHits";
  hSvc->FillHisto(hname,fNhits,1.);
  if ( fNhits < 0 ) return retCode;
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Bool_t isX=true;
    Int_t istrip    = hit->GetChannelId();
    hname="Target_HitChannelId";
    hSvc->FillHisto(hname,istrip,1.); 
    Double_t energy = hit->GetEnergy();
    Double_t faenergy = fabs(energy);
    Double_t time   = hit->GetTime();
    
 if(fRecoEvent->GetTriggerMask()==1) 

   {

    if (istrip > 16) {
      istrip = istrip-16;
      hname = "Target_HitYprofile";
      hSvc->FillHisto(hname,(Double_t)istrip,energy);
    }
    else {
      hname = "Target_HitXprofile";
      hSvc->FillHisto(hname,(Double_t)istrip,energy);
         }
   }
   
   }
   
   //recoBeam sector
    hname ="TargetBeam_XCharge";
    hSvc->FillHisto(hname,fRecoBeam->getXCharge(),1.);
    hname ="TargetBeam_YCharge";
    hSvc->FillHisto(hname,fRecoBeam->getYCharge(),1.);
    hname ="TargetBeam_XChargeError";
    hSvc->FillHisto(hname,fRecoBeam->getXChargeErr(),1.);
    hname ="TargetBeam_YChargeError";
    hSvc->FillHisto(hname,fRecoBeam->getYChargeErr(),1.);

    hname ="TargetBeam_XCentroid";
    hSvc->FillHisto(hname,fRecoBeam->getX(),1.);
    hname ="TargetBeam_YCentroid";
    hSvc->FillHisto(hname,fRecoBeam->getY(),1.);
    hname ="TargetBeam_XCentroidError";
    hSvc->FillHisto(hname,fRecoBeam->getXError(),1.);
    hname ="TargetBeam_YCentroidError";
    hSvc->FillHisto(hname,fRecoBeam->getYError(),1.);
 
    hname ="TargetBeam_XCfit";
    hSvc->FillHisto(hname,fRecoBeam->getXCfit(),1.);
    hname ="TargetBeam_XCfitError";
    hSvc->FillHisto(hname,fRecoBeam->getXCfitError(),1.);
    hname ="TargetBeam_YCfit";
    hSvc->FillHisto(hname,fRecoBeam->getYCfit(),1.);
    hname ="TargetBeam_YCfitError";
    hSvc->FillHisto(hname,fRecoBeam->getYCfitError(),1.);
    hname ="TargetBeam_nPOT";
    hSvc->FillHisto(hname,fRecoBeam->getnPOT(),1.);
    hname ="TargetBeam_nPOTError";
    hSvc->FillHisto(hname,fRecoBeam->getnPOTError(),1.);

    
    hname ="TargetBeam_MeanXYCharge";
    hSvc->FillHisto(hname,(fRecoBeam->getXCharge()+fRecoBeam->getYCharge())/2,1.);

   if(fRecoEvent->GetTriggerMask()==1) {
    hname ="TargetBeam_XCharge_BTFtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getXCharge(),1.);
    hname ="TargetBeam_YCharge_BTFtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getYCharge(),1.);
    hname ="TargetBeam_MeanXYCharge_BTFtrigger";
    hSvc->FillHisto(hname,(fRecoBeam->getXCharge()+fRecoBeam->getYCharge())/2,1.);
    hname ="TargetBeam_nPOT_BTFtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getnPOT(),1.);
   }
   
   if(fRecoEvent->GetTriggerMask()==2) {
    hname ="TargetBeam_XCharge_CRtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getXCharge(),1.);
    hname ="TargetBeam_YCharge_CRtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getYCharge(),1.);
    hname ="TargetBeam_MeanXYCharge_CRtrigger";
    hSvc->FillHisto(hname,(fRecoBeam->getXCharge()+fRecoBeam->getYCharge())/2,1.);
    hname ="TargetBeam_nPOT_CRtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getnPOT(),1.);
   }

  if(fRecoEvent->GetTriggerMask()==128) {
    hname ="TargetBeam_XCharge_AUTOtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getXCharge(),1.);
    hname ="TargetBeam_YCharge_AUTOtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getYCharge(),1.);
    hname ="TargetBeam_MeanXYCharge_AUTOtrigger";
    hSvc->FillHisto(hname,(fRecoBeam->getXCharge()+fRecoBeam->getYCharge())/2,1.);
    hname ="TargetBeam_nPOT_AUTOtrigger";
    hSvc->FillHisto(hname,fRecoBeam->getnPOT(),1.);
   }


 /*   hname ="TargetBeam_XCharge_CRtrigger";
    if(fRecoEvent->GetTriggerMask()==2) hSvc->FillHisto(hname,fRecoBeam->getXCharge(),1.);   //only cosmic rays
    hname ="TargetBeam_YCharge_CRtrigger";
    if(fRecoEvent->GetTriggerMask()==2) hSvc->FillHisto(hname,fRecoBeam->getYCharge(),1.);
    hname ="TargetBeam_XCharge_BTFtrigger";
    if(fRecoEvent->GetTriggerMask()==1) hSvc->FillHisto(hname,fRecoBeam->getXCharge(),1.);   
    hname ="TargetBeam_YCharge_BTFtrigger";
    if(fRecoEvent->GetTriggerMask()==1) hSvc->FillHisto(hname,fRecoBeam->getYCharge(),1.); 
    hname ="TargetBeam_XCharge_AUTOtrigger";
    if(fRecoEvent->GetTriggerMask()==128) hSvc->FillHisto(hname,fRecoBeam->getXCharge(),1.);   
    hname ="TargetBeam_YCharge_AUTOtrigger";
    if(fRecoEvent->GetTriggerMask()==128) hSvc->FillHisto(hname,fRecoBeam->getYCharge(),1.); */
  
    hname ="Trend_ChargeXvsEventNumber";
    hSvc->FillHisto2(hname,fRecoEvent->GetEventNumber(),fRecoBeam->getXCharge(), 1.);
    hname ="Trend_ChargeYvsEventNumber";
    hSvc->FillHisto2(hname,fRecoEvent->GetEventNumber(),fRecoBeam->getYCharge(), 1.);
    hname ="Trend_CentroidXvsEventNumber";
    hSvc->FillHisto2(hname,fRecoEvent->GetEventNumber(),fRecoBeam->getX(), 1.);
     hname ="Trend_CentroidYvsEventNumber";
    hSvc->FillHisto2(hname,fRecoEvent->GetEventNumber(),fRecoBeam->getY(), 1.);
    hname ="Trend_XCfitvsEventNumber";
    hSvc->FillHisto2(hname,fRecoEvent->GetEventNumber(),fRecoBeam->getXCfit(), 1.);
    hname ="Trend_YCfitvsEventNumber";
    hSvc->FillHisto2(hname,fRecoEvent->GetEventNumber(),fRecoBeam->getYCfit(), 1.);
  

   return retCode;
}
Bool_t TargetAnalysis::ProcessValidation()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit* hit=NULL;
  std::string hname;
  Int_t fNhits = fhitEvent->GetNHits();
  hname="Target_NHits";
  hSvc->FillHisto(hname,fNhits,1.);
  if ( fNhits < 0 ) return retCode;
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    //Bool_t isX=true;
    Int_t istrip    = hit->GetChannelId();
    hname="Target_HitChannelId";
    hSvc->FillHisto(hname,istrip,1.); 
    Double_t energy = hit->GetEnergy();
    Double_t faenergy = fabs(energy);
    Double_t time   = hit->GetTime();
    if (istrip > 16) {
      istrip = istrip-16;
      hname = "Target_HitYprofile";
      hSvc->FillHisto(hname,(Double_t)istrip,energy);
      hname = "Target_YstripTime";
      hSvc->FillHisto(hname,time,  1.);
      hname = "Target_YstripEne";
      hSvc->FillHisto(hname,energy,1.);
    }
    else {
      hname = "Target_HitXprofile";
      hSvc->FillHisto(hname,(Double_t)istrip,energy);
      hname = "Target_XstripTime";
      hSvc->FillHisto(hname,time,  1.);
      hname = "Target_XstripEne";
      hSvc->FillHisto(hname,energy,1.);
    }
   }
   
   //recoBeam sector

    hname ="TargetBeam_XCentroid";
    hSvc->FillHisto(hname,fRecoBeam->getX(),1.);
    hname ="TargetBeam_YCentroid";
    hSvc->FillHisto(hname,fRecoBeam->getY(),1.);
    hname ="TargetBeam_XCentroidError";
    hSvc->FillHisto(hname,fRecoBeam->getXError(),1.);
    hname ="TargetBeam_YCentroidError";
    hSvc->FillHisto(hname,fRecoBeam->getYError(),1.);
    hname ="TargetBeam_XCharge";
    hSvc->FillHisto(hname,fRecoBeam->getXCharge(),1.);
    hname ="TargetBeam_YCharge";
    hSvc->FillHisto(hname,fRecoBeam->getYCharge(),1.);
    hname ="TargetBeam_XChargeError";
    hSvc->FillHisto(hname,fRecoBeam->getXChargeErr(),1.);
    hname ="TargetBeam_YChargeError";
    hSvc->FillHisto(hname,fRecoBeam->getYChargeErr(),1.);
    hname ="TargetBeam_XTime";
    hSvc->FillHisto(hname,fRecoBeam->getXTime(),1.);
    hname ="TargetBeam_XTimeError";
    hSvc->FillHisto(hname,fRecoBeam->getXTimeErr(),1.);
    hname ="TargetBeam_YTime";
    hSvc->FillHisto(hname,fRecoBeam->getYTime(),1.);
    hname ="TargetBeam_YTimeError";
    hSvc->FillHisto(hname,fRecoBeam->getYTimeErr(),1.);
    hname ="TargetBeam_XWidth";
    hSvc->FillHisto(hname,fRecoBeam->getXWidth(),1.);
    hname ="TargetBeam_XWidthError";
    hSvc->FillHisto(hname,fRecoBeam->getXWidthError(),1.);
    hname ="TargetBeam_YWidth";
    hSvc->FillHisto(hname,fRecoBeam->getYWidth(),1.);
    hname ="TargetBeam_YWidthError";
    hSvc->FillHisto(hname,fRecoBeam->getYWidthError(),1.);
    hname ="TargetBeam_XCfit";
    hSvc->FillHisto(hname,fRecoBeam->getXCfit(),1.);
    hname ="TargetBeam_XCfitError";
    hSvc->FillHisto(hname,fRecoBeam->getXCfitError(),1.);
    hname ="TargetBeam_YCfit";
    hSvc->FillHisto(hname,fRecoBeam->getYCfit(),1.);
    hname ="TargetBeam_YCfitError";
    hSvc->FillHisto(hname,fRecoBeam->getYCfitError(),1.);
    hname ="TargetBeam_XWfit";
    hSvc->FillHisto(hname,fRecoBeam->getXWfit(),1.);
    hname ="TargetBeam_XWfitError";
    hSvc->FillHisto(hname,fRecoBeam->getXWfitError(),1.);
    hname ="TargetBeam_YWfit";
    hSvc->FillHisto(hname,fRecoBeam->getYWfit(),1.);
    hname ="TargetBeam_YWfitError";
    hSvc->FillHisto(hname,fRecoBeam->getYWfitError(),1.);
    hname ="TargetBeam_XChi2";
    hSvc->FillHisto(hname,fRecoBeam->getXChi2(),1.);
    hname ="TargetBeam_XNdof";
    hSvc->FillHisto(hname,fRecoBeam->getXNdof(),1.);
    hname ="TargetBeam_YChi2";
    hSvc->FillHisto(hname,fRecoBeam->getYChi2(),1.);
    hname ="TargetBeam_YNdof";
    hSvc->FillHisto(hname,fRecoBeam->getYNdof(),1.);
    hname ="TargetBeam_nPOT";
    hSvc->FillHisto(hname,fRecoBeam->getnPOT(),1.);
    hname ="TargetBeam_nPOTError";
    hSvc->FillHisto(hname,fRecoBeam->getnPOTError(),1.);

   return retCode;
}

Bool_t TargetAnalysis::Finalize()
{
  Bool_t retCode = 0;

  if (fProcessingMode!=0) return retCode;
  
  nPOTErrgoodXY_job = sqrt(nPOTErrgoodXY_job);
  nPOTErrbadXorY_job = sqrt(nPOTErrbadXorY_job);
  Double_t relErr_nPOTgood  = nPOTErrgoodXY_job/nPOTgoodXY_job;  

  Double_t aver_nPOTgood = nPOTgoodXY_job/Double_t(nEvsGoodXY);
  Double_t aver_nPOTbad  = 0.; 
  Double_t relErr_nPOTbad  = 0.; 
  if (nEvsTarget>nEvsGoodXY) {
    aver_nPOTbad = nPOTbadXorY_job/Double_t(nEvsTarget-nEvsGoodXY);
    relErr_nPOTbad = nPOTErrbadXorY_job/nPOTbadXorY_job;
  }
  std::cout<<"*************************TargetAnalysis::Finalize*****************************"<<std::endl; 
  std::cout<<" total nPOT (all events)= "<<nPOTgoodXY_job+nPOTbadXorY_job <<" nEvts = "<<nEvsTarget<<std::endl;
  std::cout<<" total nPOT (events good x and y profiles)= "<<nPOTgoodXY_job<<" +/- "<<nPOTErrgoodXY_job <<" relErr = "<<relErr_nPOTgood<<std::endl;
  std::cout<<" total nPOT (events bad  x or  y profiles)= "<<nPOTbadXorY_job<<" +/- "<<nPOTErrbadXorY_job <<" relErr = "<<relErr_nPOTbad<<std::endl;
  std::cout<<" average nPOT/bunch good events / bad events = "<<aver_nPOTgood <<" / "<<aver_nPOTbad  <<std::endl;

  return retCode;
}
  
