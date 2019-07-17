#include "EventSelection.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "TTargetRecoBeam.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include <iostream>


EventSelection::EventSelection()
{
  fRecoEvent   = NULL;

  fECal_hitEvent=0;
  fECal_ClColl=0;
  
  fPVeto_hitEvent=0;
  fPVeto_ClColl=0;
  
  fEVeto_hitEvent=0;
  fEVeto_ClColl=0;
  
  fSAC_hitEvent=0;
  fSAC_ClColl=0;
  
  fTarget_hitEvent=0;
  fTarget_RecoBeam=0;
  
  fHEPVeto_hitEvent=0;
  fHEPVeto_ClColl=0;

  fProcessingMode = 0;
  fVerbose        = 0;
}
EventSelection::EventSelection(Int_t processingMode, Int_t verbosityFlag)
{
  fRecoEvent   = NULL;
  fProcessingMode = processingMode;
  fVerbose        = verbosityFlag;
}
EventSelection::~EventSelection()
{
}

Bool_t EventSelection::Init(TRecoEvent* eventHeader, 
			    TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
			    TRecoVObject* PVetoev, TRecoVClusCollection* PVetocl,
			    TRecoVObject* EVetoev, TRecoVClusCollection* EVetocl,
			    TRecoVObject* HEPVetoev, TRecoVClusCollection* HEPVetocl,
			    TRecoVObject* SACev, TRecoVClusCollection* SACcl,
			    TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam)
{


  Bool_t retCode = 0;


  fRecoEvent = eventHeader;
  fECal_hitEvent      =ECALev   ;
  fPVeto_hitEvent     =PVetoev  ;
  fEVeto_hitEvent     =EVetoev  ;
  fSAC_hitEvent       =SACev    ;
  fTarget_hitEvent    =TARGETev ;
  fHEPVeto_hitEvent   =HEPVetoev;

  fECal_ClColl      =ECALcl   ;
  fPVeto_ClColl     =PVetocl  ;
  fEVeto_ClColl     =EVetocl  ;
  fSAC_ClColl       =SACcl    ;
  fTarget_RecoBeam  =TargetBeam ;
  fHEPVeto_ClColl   =HEPVetocl ;

  return true;
}
/*
Bool_t EventSelection::InitValidation()
{
  InitHistosValidation(fname);
}
*/

Bool_t EventSelection::InitHistos()
{
  if (fProcessingMode==1)
    {
      //return 0;//
      return InitHistosAnalysis();
    }
  else if (fProcessingMode==2)
    {
      return 0;//InitHistosDataQuality();
    }
  else if ( fProcessingMode > 2 || fProcessingMode < 0 )
    {
      std::cout<<"Error:: fProcessingModel = "<<fProcessingMode<<" out of range "<<std::endl;
      return true;
    }
  return InitHistosAnalysis();
  // TO DO move here from HistSvc ;
}
/*
Bool_t EventSelection::InitHistosValidation()
{
  return InitHistosValidation(fAlgoName);
}
*/


Bool_t EventSelection::Process()
{
  Bool_t retCode = 0;
  if (fECal_hitEvent == NULL)
    {
      std::cout<<fAlgoName<<"::Process - ERROR - no data found"<<std::endl;
      return true;
    }
  
  if (fProcessingMode==1)
    {
      //return 0;//ProcessValidation();
      return ProcessAnalysis();
    }
  else if (fProcessingMode==2)
    {
      return 0;//ProcessDataQuality();
    }
  else if ( fProcessingMode > 2 || fProcessingMode < 0 )
    {
      std::cout<<"Error:: fProcessingModel = "<<fProcessingMode<<" out of range "<<std::endl;
      return true;
    }
  return ProcessAnalysis();
}

/*
Bool_t EventSelection::ProcessValidation()
{
  return ProcessValidation(fAlgoName);
}
*/
Bool_t EventSelection::ProcessAnalysis()
{  

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit* xHit;
  TRecoVHit* yHit;
  TRecoVCluster* xClu;
  TRecoVCluster* aClu;
  TRecoVCluster* yClu;
  Double_t xTime, xTimeLinCorr, yTime, xEne, yEne, aEne, aTime;
  Int_t xChId, yChId, aChId;


  if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) std::cout<<"input data are simulatetd "<<std::endl;
  else std::cout<<"input data are NOT simulated "<<std::endl;
      
  Double_t eSumECalHits=0.;
  Double_t xEnergy=0;
  std::string hname;
  if (fPVeto_hitEvent->GetNHits() > 0){
  for (unsigned int hPVeto=0; hPVeto<fPVeto_hitEvent->GetNHits(); ++hPVeto)
    {
      xHit = fPVeto_hitEvent->Hit(hPVeto);
      xTime= xHit->GetTime();
      xChId= xHit->GetChannelId();
      xEnergy = xHit->GetEnergy();
      hname="timePVetoVsCh_Hits";
      hSvc->FillHisto2(hname, xTime, (float)xChId);
      hname="timePVetoVsCh_linearCorr_Hits";
      xTimeLinCorr = applyTimePVetoLinCorr((float)xChId, xTime);
      hSvc->FillHisto2(hname, xTimeLinCorr, (float)xChId);
      for (unsigned int hECal=0; hECal<fECal_hitEvent->GetNHits(); ++hECal)
	{
	  yHit = fECal_hitEvent->Hit(hECal);
	  yTime= yHit->GetTime();
	  yChId= yHit->GetChannelId();
	  yEne = yHit->GetEnergy();
	  /*
	  if (hPVeto==0)
	    {
	      eSumECalHits = eSumECalHits+yEne;
	      hname="ECalEnergyMap_Hits";
	      hSvc->FillHisto2(hname, float(int(yChId/100)), float(yChId%100), yEne);
	      hname="ECalMap_Hits";
	      hSvc->FillHisto2(hname, float(int(yChId/100)), float(yChId%100));
	    }
	  */
	  
	  hname="timeECalVsPVeto_Hits";
	  hSvc->FillHisto2(hname, xTime, yTime);
	}
      for (unsigned int hSAC=0; hSAC<fSAC_hitEvent->GetNHits(); ++hSAC)
	{
	  yHit = fSAC_hitEvent->Hit(hSAC);
	  if (yHit->GetChannelId()!=21) continue;

	  yTime= yHit->GetTime();
	  hname="DtimePVetoVsSAC21_Hits";
	  hSvc->FillHisto(hname, xTime-30.7-yTime);
	  hname="DtimePVetoVsSAC21LinCorr_Hits";
	  hSvc->FillHisto(hname, xTimeLinCorr-yTime);
	}
      for (unsigned int cSAC=0; cSAC<fSAC_ClColl->GetNElements(); ++cSAC)
	{
	  yClu = fSAC_ClColl->Element(cSAC);
	  
	  if (yClu->GetEnergy()<50.) continue;
	  if ( fabs(yClu->GetTime()-xTimeLinCorr) > 1. ) continue;
	  
	  hname="SACClEVsPVetoHitChId_1ns_linearCorr";
	  hSvc->FillHisto2(hname, (float)xChId, yClu->GetEnergy());
	  if (yClu->GetChannelId()!=21) continue;
	  if(xEnergy < 10.) continue;
	  if(xEnergy > 50.) continue;
	  hname="SACClE21VsPVetoHitChId_1ns_linearCorr";
	  hSvc->FillHisto2(hname, (float)xChId, yClu->GetEnergy());
	}
    }

  //Cluster Based 
  for (unsigned int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
    {
      xClu = fPVeto_ClColl->Element(hPVeto);
      xTime= xClu->GetTime();
      xChId= xClu->GetChannelId();
      for (unsigned int hECal=0; hECal<fECal_ClColl->GetNElements(); ++hECal)
	{
	  yClu = fECal_ClColl->Element(hECal);
	  yTime= yClu->GetTime();
	  yChId= yClu->GetChannelId();
	  yEne = yClu->GetEnergy();
	  
	  hname="timeECalVsPVeto_Clus";
	  hSvc->FillHisto2(hname, xTime, yTime);
	  hname="energyECalVsChIdPVeto_Clus_inTime10";
	  if (fabs(yTime-xTime)<10.) hSvc->FillHisto2(hname, float(xChId), yEne);
	  
	}
    }
  }


  //std::cout<<" Going to ECal Hits "<<std::endl;
  for (unsigned int hECal=0; hECal<fECal_hitEvent->GetNHits(); ++hECal)
    {
      yHit = fECal_hitEvent->Hit(hECal);
      yEne = yHit->GetEnergy();
      yChId= yHit->GetChannelId();
      eSumECalHits = eSumECalHits+yEne;
      hname="ECalEnergyMap_Hits";
      hSvc->FillHisto2(hname, float(int(yChId/100)), float(yChId%100), yEne);
      hname="ECalMap_Hits";
      hSvc->FillHisto2(hname, float(int(yChId/100)), float(yChId%100));
    }
  hname="energySumECalHits";
  hSvc->FillHisto(hname, eSumECalHits);

  
  int iELead = -1;
  int iEsLead = -1;
  xTime=0.;
  xEne =0.;
  xChId=-1;
  aEne =0.;
  aChId=-1;
  double eSumCl=0.;
  double dR = -999.;
  double dPhi = -999.;
  int chSubMax=-1;  int xChIdEMax=-1;
  double eMax=0.;   double eSubMax=0.; double eSubMaxIn10=0.;
  double tEMax=0.;  double tSubMax=0.;
  int igamma1 = 0; 
  int igamma2 = 0;
  int igamma1In10 = 0; 
  int igamma2In10 = 0;
  int igamma1In10PhiS = 0; 
  int igamma2In10PhiS = 0;
  double sum2GammaMax = 0.;
  double aSumClIn10PhiSMax=0.;
  double aSumClIn10Max=0.;
  double aSumClMax=0.;
  double aSumCl=0.;

  TRecoVHit* hit = 0;
  TRecoVHit* hitSeed = 0;
  std::vector<Int_t> hitVInCl;
  //std::cout<<" pointer to collection = "<<(long)fECal_ClColl<<std::endl;
  //std::cout<<" Going to ECal Clusters "<<fECal_ClColl->GetNElements()<<std::endl;
  for (unsigned int hECal=0; hECal<fECal_ClColl->GetNElements(); ++hECal)
    {
      //std::cout<<" cluster n. hECal = "<<hECal<<" pointer to collection = "<<(long)fECal_ClColl<<std::endl;
      xClu = fECal_ClColl->Element((int)hECal);
      if (xClu) 
	{
	  xTime= xClu->GetTime();
	  xChId= xClu->GetChannelId();
	  xEne = xClu->GetEnergy();
	  hname = "ECalClEnergy";
	  hSvc->FillHisto(hname, xEne);
	  

	  eSumCl = eSumCl+xEne;
	  //if (xEne<50.) continue;
	  
	  if (xEne > eMax)
	    {
	      iELead   = hECal;
	      eMax     = xEne;
	      tEMax    = xTime;
	      xChIdEMax = xChId;
	    }
	  hname = "TimeSpreadInECalClus";
	  //vector of (indices of) hits in cluster 
	  hitVInCl = xClu->GetHitVecInClus();
	  //seed index
	  hitSeed =  fECal_hitEvent->Hit(xClu->GetSeed());
	  for (unsigned int ih=0; ih<xClu->GetNHitsInClus(); ++ih)
	    {
	      if (hitVInCl[ih]==xClu->GetSeed()) continue;
	      hit = fECal_hitEvent->Hit(hitVInCl[ih]);
	      hSvc->FillHisto(hname, hit->GetTime()-hitSeed->GetTime());
	    }
	  for (unsigned int clCal=hECal+1; clCal<fECal_ClColl->GetNElements(); ++clCal)
	    {
	      //std::cout<<" cluster n. hECal = "<<hECal<<" pointer to collection = "<<(long)fECal_ClColl<<std::endl;
	      aClu = fECal_ClColl->Element((int)clCal);
	      if (aClu) 
		{
		  aEne = aClu->GetEnergy();
		  //if (aEne<50.) continue;

		  aChId  = aClu->GetChannelId();
		  aTime  = aClu->GetTime();
		  aSumCl = xEne+aEne;	
	  
		  if (aSumCl > aSumClMax)
		    {
		      igamma1 = hECal;
		      igamma2 = clCal;
		      aSumClMax = aSumCl;
		    }

		  if (fabs(aTime-xTime)<10.)
		    {
		      if (aSumCl > aSumClIn10Max)
			{
			  igamma1In10 = hECal;
			  igamma2In10 = clCal;
			  aSumClIn10Max = aSumCl;
			}
		      dR   = -999.;
		      dPhi = -999.;
		      if (phiSymmetricalInECal(xChId, aChId, dR, dPhi))
			{
			  // here fill dR 
			  hname = "DR2gammaIn10PhiSym";
			  hSvc->FillHisto(hname, dR);
			  if (aSumCl > aSumClIn10PhiSMax)
			    {
			      igamma1In10PhiS = hECal;
			      igamma2In10PhiS = clCal;
			      aSumClIn10PhiSMax = aSumCl;
			      // here fill dR for Max ESum
			      hname = "DR2gammaIn10PhiSymMaxESum";
			      hSvc->FillHisto(hname, dR);
			    }
			}
		      // here fill dPhi histo 
		      hname = "DPhi2gammaIn10";
		      hSvc->FillHisto(hname, dPhi);
		    }		  
		}
	    }
	}

    }

  for (unsigned int hECal=0; hECal<fECal_ClColl->GetNElements(); ++hECal)
    {
      //std::cout<<" cluster n. hECal = "<<hECal<<" pointer to collection = "<<(long)fECal_ClColl<<std::endl;
      xClu = fECal_ClColl->Element((int)hECal);
      if (hECal == iELead) continue;
      if (xClu) 
	{
	  xEne = xClu->GetEnergy();
	  xTime = xClu->GetTime();
	  //	  xChId = xClu->GetChannelId();
	  
	  if (xEne > eSubMax)
	    {
	      iEsLead  = iELead;
	      eSubMax  = xEne;
	      tSubMax  = xTime;
	    }
	}
    }
  //  if (iEsLead >-1) 
  //std::cout<<" Gone to ECal Clusters "<<std::endl;

  hname="energySumECalClus";
  hSvc->FillHisto(hname, eSumCl);
  //std::cout<<" Gone1 to ECal Clusters "<<std::endl;
  hname="energyLeadingECalCl";
  hSvc->FillHisto(hname, eMax);
  //std::cout<<" Gone2 to ECal Clusters "<<std::endl;
  if (eSubMax>0.01) 
    {
      hname="energySum2LeadingECalCl";
      hSvc->FillHisto(hname, eMax+eSubMax);
    }
  if (eSubMaxIn10>0.01) 
    {
      hname="energySum2LeadingECalCl_inTime10";
      if (fabs(tEMax-tSubMax)<10.) hSvc->FillHisto(hname, eMax+eSubMaxIn10);
    }
  if (aSumClMax>0.01) {
    hname="energySum2ECalCl_maxE";
    hSvc->FillHisto(hname, aSumClMax);
  }
  if (aSumClIn10Max>0.01) {
    hname="energySum2ECalCl_maxEinTime10";
    hSvc->FillHisto(hname, aSumClIn10Max);
  }
  if (aSumClIn10PhiSMax>0.01) {
    hname="energySum2ECalCl_maxEinTime10PhiS";
    hSvc->FillHisto(hname, aSumClIn10PhiSMax);
  }
  //std::cout<<" GoneDONE to ECal Clusters "<<std::endl;

  return true;
}

Bool_t EventSelection::phiSymmetricalInECal(int xChId, int aChId, double& distR, double& distPhi)
{
  Bool_t isPhiSym = false;
  distR   = -999.; 
  distPhi = -999.; 
  
  float_t x1 = (xChId/100 - 14.);
  float_t x2 = (aChId/100 - 14.);
  float_t y1 = (xChId%100 - 14.);
  float_t y2 = (aChId%100 - 14.);
  
  //  std::cout<<" 1) x,y "<<x1<<" "<<y1<<std::endl;
  //  std::cout<<" 2) x,y "<<x2<<" "<<y2<<std::endl;
  if (x1*x2 >0.1) return isPhiSym;
  if (y1*y2 >0.1) return isPhiSym;
  
  double phi1 = atan2(y1,x1);
  double phi2 = atan2(y2,x2);
  //  std::cout<<" 1) x,y "<<x1<<" "<<y1<<" phi "<<phi1<<std::endl;
  //  std::cout<<" 2) x,y "<<x2<<" "<<y2<<" phi "<<phi2<< std::endl;

  distR = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
  distPhi = phi1-phi2;
  if ( fabs(phi1-phi2) < 0.3 ) isPhiSym=true;

  return isPhiSym;
}

Bool_t EventSelection::InitHistosAnalysis()
{

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  std::string hname;
  hname="timeECalVsPVeto_Hits";
  int nBinX=  200;
  int nBinY=  200;
  int minX = -300;
  int maxX =  300;
  int minY = -300;
  int maxY =  300;
  hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
  hname="timePVetoVsCh_Hits";
  hSvc->BookHisto2(hname, nBinX, minX, maxX, 101, -0.5, 100.5);
  hname="timePVetoVsCh_linearCorr_Hits";
  hSvc->BookHisto2(hname, nBinX, minX, maxX, 101, -0.5, 100.5);
  hname="DtimePVetoVsSAC21_Hits";
  hSvc->BookHisto(hname, 200, -20., 20.);
  hname="DtimePVetoVsSAC21LinCorr_Hits";
  hSvc->BookHisto(hname, 200, -20., 20.);

  hname="SACClEVsPVetoHitChId_1ns_linearCorr";
  hSvc->BookHisto2(hname, 101, -0.5, 100.5, 250, 50., 300.);
  hname="SACClE21VsPVetoHitChId_1ns_linearCorr";
  hSvc->BookHisto2(hname, 101, -0.5, 100.5, 250, 50., 300.);

  
  hname="timeECalVsPVeto_Clus";
  nBinX=  200;
  nBinY=  200;
  minX = -300;
  maxX =  300;
  minY = -300;
  maxY =  300;
  hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);

  hname="energyECalVsChIdPVeto_Clus_inTime10";
  nBinX=  101;
  nBinY=  200;
  minX = -0.5;
  maxX =  100.5;
  minY =  0.;
  maxY =  600;
  hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);

  hname = "ECalClEnergy";
  nBinX=  750;
  minX =  0.;
  maxX =  1500.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="energySum2LeadingECalCl_inTime10_PhiSim";
  nBinX=  750;
  minX =  0.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="energySum2ECalCl_maxE";
  nBinX=  750;
  minX =  0.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="energySum2ECalCl_maxEinTime10";
  nBinX=  750;
  minX =  0.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="energySum2ECalCl_maxEinTime10PhiS";
  nBinX=  750;
  minX =  0.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);

  hname="energySum2LeadingECalCl_inTime10";
  nBinX=  750;
  minX =  0.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="energySum2LeadingECalCl";
  nBinX=  750;
  minX =  0.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="energyLeadingECalCl";
  nBinX=  750;
  minX =  0.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="energySumECalHits";
  nBinX=  400;
  minX =  0.;
  maxX =  8000.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="energySumECalClus";
  nBinX=  400;
  minX =  0.;
  maxX =  8000.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="TimeSpreadInECalClus";
  nBinX=  200;
  minX =  -30.;
  maxX =   30.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);

  hname = "DR2gammaIn10PhiSym";
  nBinX=  100;
  minX =  0.;
  maxX =  600.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname = "DR2gammaIn10PhiSymMaxESum";
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname = "DPhi2gammaIn10";
  minX =  -1.;
  maxX =   1.;
  hSvc->BookHisto(hname, nBinX, minX, maxX);


  hname="ECalEnergyMap_Hits";
  nBinX=  31;
  minX =  -0.5;
  maxX =  30.5;
  nBinY=  31;
  minY =  -0.5;
  maxY =  30.5;
  hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);

  hname="ECalMap_Hits";
  nBinX=  31;
  minX =  -0.5;
  maxX =  30.5;
  nBinY=  31;
  minY =  -0.5;
  maxY =  30.5;
  hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);

  return true;
}

Double_t EventSelection::applyTimePVetoLinCorr(Double_t  xChId, Double_t xTime)
{
  return xTime - (32.9 + 9.*(xChId - 13.)/65.);
}
