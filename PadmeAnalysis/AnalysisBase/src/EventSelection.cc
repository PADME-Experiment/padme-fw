#include "EventSelection.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "TTargetRecoBeam.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <iostream>


EventSelection::EventSelection()
{

  fVersion=0;
  fInitToComplete=true;
  
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

  InitTimeCalPVeto  () ;
  InitTimeCalEVeto  () ;
  InitTimeCalHEPVeto() ;
  InitTimeCalSAC    () ;
  InitTimeCalECal   () ;

}
EventSelection::EventSelection(Int_t processingMode, Int_t verbosityFlag)
{
  fVersion=0;
  fRecoEvent   = NULL;
  fProcessingMode = processingMode;
  fVerbose        = verbosityFlag;

  InitTimeCalPVeto  () ;
  InitTimeCalEVeto  () ;
  InitTimeCalHEPVeto() ;
  InitTimeCalSAC    () ;
  InitTimeCalECal   () ;
  fInitToComplete=true;


}
EventSelection::~EventSelection()
{
}

void EventSelection::SetVersion(Int_t fv){fVersion = fv; std::cout<<"EventSelection: version set to "<<fVersion<<std::endl;}

Bool_t EventSelection::Init(TRecoEvent* eventHeader, 
			    TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
			    TRecoVObject* PVetoev, TRecoVClusCollection* PVetocl,
			    TRecoVObject* EVetoev, TRecoVClusCollection* EVetocl,
			    TRecoVObject* HEPVetoev, TRecoVClusCollection* HEPVetocl,
			    TRecoVObject* SACev, TRecoVClusCollection* SACcl,
			    TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam)
{


//  Bool_t retCode = 0;


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
      return InitHistosIO();
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
  return InitHistosIO();
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
  
//Bool_t retCode = 0;
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
void EventSelection::CalibrateTimeAndEnergy()
{

  Bool_t isMC=false;
  //std::cout<<"in ProcessAnalysis ... evsel ... "<<fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<std::endl;
  if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //std::cout<<"input data are simulatetd "<<std::endl;
  }/*
  if (fInitToComplete) {
    SetCalibTimePVeto  (isMC) ;
    SetCalibTimeEVeto  (isMC) ;
    SetCalibTimeHEPVeto(isMC) ;
    SetCalibTimeSAC    (isMC) ;
    SetCalibTimeECal   (isMC) ;
  }
  fInitToComplete=false;

  ApplyCalibTimePVeto  ();
  ApplyCalibTimeEVeto  ();
  ApplyCalibTimeHEPVeto();
  ApplyCalibTimeSAC    ();*/
  // ApplyCalibTimeEnergyECal   (isMC); done in ECALAnalysis for energy 
  return;
  
}
Bool_t EventSelection::ProcessAnalysis()
{
  CalibrateTimeAndEnergy();

  if (fVersion==1)
    {
      return ProcessAnalysisIO();
    }
 /* else if (fVersion==2)
    {
      return ProcessAnalysisGC();
    }*/
  else {std::cout<<" EventSelection: version not set ....................... choose 1 for ProcessAnalysisSS / 2 for ProcessAnalysisGC"<<std::endl;}
  return true;
}

Bool_t EventSelection::ProcessAnalysisSS()
{  

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  //std::string hprefix = "SS_";
  std::string hprefix="";


  Bool_t isMC=false;
  //std::cout<<"in ProcessAnalysis ... evsel ... "<<fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<std::endl;
  if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //std::cout<<"input data are simulatetd "<<std::endl;
  }
  // Overall bunch normalization
  hSvc->FillHisto("SelectionCutFlow",float(cut_all));
 
  // apply BTF bunch preselection 
  if (!passPreselection()) return true;
  hSvc->FillHisto(hprefix+"SelectionCutFlow",float(cut_Presel));
  // require at least one ECal cluster in the event
  if (fECal_ClColl->GetNElements()<1) return true;
  hSvc->FillHisto(hprefix+"SelectionCutFlow",float(cut_ge1ECalCl));

  TRecoVHit* xHit;
  TRecoVHit* yHit;
  TRecoVCluster* xClu;
  TRecoVCluster* aClu;
  TRecoVCluster* yClu;
  Double_t xTime, xTimeLinCorr, yTime, xEne, yEne, aEne, aTime;
  Int_t xChId, yChId;
  Int_t xChId1; 
  Int_t aChId;
  
      
  Double_t eSumECalHits=0.;
  //Double_t xEnergy=0;
  std::string hname;

  double pigreco = acos(-1.);



  int icellMaxE=0;
  double xE = 0;
  //std::cout<<" Going to ECal Hits "<<std::endl;
  for (int hECal=0; hECal<fECal_hitEvent->GetNHits(); ++hECal)
    {
      yHit = fECal_hitEvent->Hit(hECal);
      yEne = yHit->GetEnergy();
      if (yEne>xE) icellMaxE=hECal;
      yChId= yHit->GetChannelId();
      eSumECalHits = eSumECalHits+yEne;
      hname=hprefix+"ECalEnergyMap_Hits";
      if (isMC)
	{
	  hSvc->FillHisto2(hname, float(int(yChId%100)), float(yChId/100), yEne);
	  hname=hprefix+"ECalMap_Hits";
	  hSvc->FillHisto2(hname, float(int(yChId%100)), float(yChId/100));
	}
      else
	{
	  hSvc->FillHisto2(hname, float(int(yChId/100)), float(yChId%100), yEne);
	  hname=hprefix+"ECalMap_Hits";
	  hSvc->FillHisto2(hname, float(int(yChId/100)), float(yChId%100));
	}
    }
  hname=hprefix+"energySumECalHits";
  hSvc->FillHisto(hname, eSumECalHits);

  double yt;
  hname = hprefix+"TimeSpreadInECal";
  std::string hname1 = hprefix+"CellSpreadInECal";
  double ticellMaxE   = fECal_hitEvent->Hit(icellMaxE)->GetTime();
  TVector3 xicellMaxE = fECal_hitEvent->Hit(icellMaxE)->GetPosition();
  TVector3 xh;
  for (int hECal=0; hECal<fECal_hitEvent->GetNHits(); ++hECal)
    {
      yHit = fECal_hitEvent->Hit(hECal);
      if (hECal==icellMaxE) continue;
      yt = yHit->GetTime();
      xh = yHit->GetPosition()-xicellMaxE;
      hSvc->FillHisto(hname,yt-ticellMaxE);
      hSvc->FillHisto(hname1,xh.Perp());
    }

  
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
  int chSubMax=-1;  
  int xChIdEMax=-1;
  double eMax=0.;   double eSubMax=0.; double eSubMaxIn10=0.;
  double tEMax=0.;  double tSubMax=0.;
      
  /*
    int igamma1 = 0; 
    int igamma2 = 0;
    int igamma1In10 = 0; 
    int igamma2In10 = 0;
    int igamma1In10PhiS = 0; 
    int igamma2In10PhiS = 0;
    double sum2GammaMax = 0.;
  */
      
  double aSumClIn10PhiSMax=0.;
  double aSumClIn10Max=0.;
  double aSumClMax=0.;
  double aSumCl=0.;

  TRecoVHit* hit = 0;
  TRecoVHit* hitSeed = 0;
  std::vector<Int_t> hitVInCl;
  TVector3 pos1;
  TVector3 pos2;
  double impactPar=0.;


  int nhit=fECal_hitEvent->GetNHits();
  hname="SS2g_nhits";
  hSvc->FillHisto(hname,float(nhit));

  int nclus=fECal_ClColl->GetNElements();
  hname="SS2g_nclus";
  hSvc->FillHisto(hname,float(nclus));
  int nclus50=0;
  int n2gindt10=0;
  int n2gindt5=0;
  int n2gindt2_5=0;
  int n2gindt1=0;
  int n2g=0;
  int n2gDsume=0;
  int n2gFR = 0;

  int ichx=0;
  int ichy=0;
  
  //std::cout<<" pointer to collection = "<<(long)fECal_ClColl<<std::endl;
  //std::cout<<" Going to ECal Clusters "<<fECal_ClColl->GetNElements()<<std::endl;
  for (int hECal=0; hECal<fECal_ClColl->GetNElements(); ++hECal)
    {
      //std::cout<<" cluster n. hECal = "<<hECal<<" pointer to collection = "<<(long)fECal_ClColl<<std::endl;
      xClu = fECal_ClColl->Element((int)hECal);
      if (xClu) 
	{
	  xTime= xClu->GetTime();
	  xChId= xClu->GetChannelId();
	  xEne = xClu->GetEnergy();

	  hname = hprefix+"ECalClEnergy";
	  hSvc->FillHisto(hname, xEne);

	  int cls = xClu->GetNHitsInClus();
	  hname = "SS2g_clSize";
	  hSvc->FillHisto(hname, float(cls));
	  hname = "SS2g_clSizeVsE";
	  hSvc->FillHisto2(hname, xEne, float(cls));

	  pos1 = xClu->GetPosition();
	  
	  
 	  if (isMC)
	    {
	      /*
	      double tno = pos1.y();
	      pos1.SetY(pos1.x());
	      pos1.SetX(tno);
	      */
	    }
	  else
	    {
	      //std::cout<<" pos1.phi() [before]= "<<pos1.Phi()<<std::endl;
	      pos1.SetX( pos1.x() - 4.0);
	      pos1.SetY( pos1.y() + 2.5);
	      //std::cout<<" pos1.phi()  [after]= "<<pos1.Phi()<<std::endl;
	    }
	  
	  
	  // Sum of All Cluster Energy 
	  eSumCl = eSumCl+xEne;

	  if (xEne<50.) continue;
	  nclus50 +=1;
	  hname=hprefix+"GammaSeleCutFlow"; 
	  hSvc->FillHisto(hname,cut_g_all);

	  hname="SS2g_ClTime";
	  hSvc->FillHisto(hname, xTime);

	  if (xEne > eMax)
	    {
	      iELead   = hECal;
	      eMax     = xEne;
	      tEMax    = xTime;
	      xChIdEMax = xChId;
	    }
	  hname = hprefix+"TimeSpreadInECalClus";
	  //vector of (indices of) hits in cluster 
	  hitVInCl = xClu->GetHitVecInClus();
	  //seed index
	  hitSeed =  fECal_hitEvent->Hit(xClu->GetSeed());
	  for (int ih=0; ih<xClu->GetNHitsInClus(); ++ih)
	    {
	      if (hitVInCl[ih]==xClu->GetSeed()) continue;
	      hit = fECal_hitEvent->Hit(hitVInCl[ih]);
	      hSvc->FillHisto(hname, hit->GetTime()-hitSeed->GetTime());
	    }
	  for (int ih=0; ih<xClu->GetNHitsInClus(); ++ih)
	    {
	      ichx = xChId/100;
	      ichy = xChId%100;
	    }
	  
	  bool xclInSR = false;
	  bool aclInSR = false;
	  // fill Ecl vs R for clusters having the E-R correlation expected for annihilation events
	  if (xEne > (300. - 1.2*(pos1.Perp()-50.) ) && xEne > 75. ) 
	    {
	      xclInSR = true;
	      hname = hprefix+"SS2gEvsR_sClu";
	      hSvc->FillHisto2(hname,pos1.Perp(),xEne);
	      hname = "";
	      if      (fabs(     180.*pos1.Phi()/pigreco)<22.5) hname = hprefix+"SS2gEvsR_sClu_FRphi0";
	      else if (fabs(45. -180.*pos1.Phi()/pigreco)<22.5) hname = hprefix+"SS2gEvsR_sClu_FRphi45";
	      else if (fabs(90. -180.*pos1.Phi()/pigreco)<22.5) hname = hprefix+"SS2gEvsR_sClu_FRphi90";
	      else if (fabs(135.-180.*pos1.Phi()/pigreco)<22.5) hname = hprefix+"SS2gEvsR_sClu_FRphi135";
	      else if (fabs(180.-180.*fabs(pos1.Phi())/pigreco)<22.5) hname = hprefix+"SS2gEvsR_sClu_FRphi180";
	      else if (fabs(-135.-180.*pos1.Phi()/pigreco)<22.5) hname = hprefix+"SS2gEvsR_sClu_FRphi225";
	      else if (fabs(-90.-180.*pos1.Phi()/pigreco)<22.5) hname = hprefix+"SS2gEvsR_sClu_FRphi270";
	      else if (fabs(-45.-180.*pos1.Phi()/pigreco)<22.5) hname = hprefix+"SS2gEvsR_sClu_FRphi315";
	      if (hname!="") {
		hSvc->FillHisto2(hname,pos1.Perp(),xEne);
	      }
	      else {
		std::cout<<"A problem here phi = "<<180.*pos1.Phi()/pigreco<<std::endl;
	      }
	    }

	  // loop for basic 2gamma search 
	  for (int clCal=hECal+1; clCal<fECal_ClColl->GetNElements(); ++clCal)
	    {
	      //std::cout<<" cluster n. hECal = "<<hECal<<" pointer to collection = "<<(long)fECal_ClColl<<std::endl;
	      aClu = fECal_ClColl->Element((int)clCal);
	      if (aClu) 
		{
		  aEne = aClu->GetEnergy();
		  if (aEne<50.) continue;
		  
		  aChId  = aClu->GetChannelId();
		  aTime  = aClu->GetTime();
		  pos2   = aClu->GetPosition();
		  if (isMC)
		    {
		      /*
		      double tno = pos2.y();
		      pos2.SetY(pos2.x());
		      pos2.SetX(tno);
		      */
		     }
		  else
		    {		      
		      pos2.SetX( pos2.x() - 4.0);
		      pos2.SetY( pos2.y() + 2.5);
		    }

		  

		  aSumCl = xEne+aEne;	
	  
		  if (aSumCl > aSumClMax)
		    {
		      //igamma1 = hECal;
		      //igamma2 = clCal;
		      aSumClMax = aSumCl;
		    }
		  
		  aclInSR = false;
		  if (aEne > (300. - 1.2*(pos2.Perp()-50.) ) && aEne > 75. ) 
		    {
		      aclInSR = true;
		    }


		  
		  dR   = -999.;
		  dPhi = -999.;
		  //bool isPhySym = phiSymmetricalInECal(xChId, aChId, dR, dPhi);
		  bool isPhySym = phiSymmetricalInECal(pos1, pos2, dR, dPhi, impactPar);
		  double dt=aTime-xTime;
		  hname = "ECal2gsearch_dPhiVsDt";
		  hSvc->FillHisto2(hname, dt, dPhi);
		  hname = "ECal2gsearch_dRVsDt";
		  hSvc->FillHisto2(hname, dt, dR);
		  hname = "ECal2gsearch_ImpactParVsDt";
		  hSvc->FillHisto2(hname, dt, impactPar);


		  double xcog = (pos1.x()*xEne+pos2.x()*aEne)/aSumCl;
		  double ycog = (pos1.y()*xEne+pos2.y()*aEne)/aSumCl;


		  // here histograms for xCu and aClu in SR
		  if (aclInSR && xclInSR)
		    {
		      hname = "SS2gDtVsSumE_inSR";
		      hSvc->FillHisto2(hname, aSumCl, dt);
		      hname = "SS2gDphiVsSumE_inSR";
		      hSvc->FillHisto2(hname, aSumCl, 180.*dPhi/pigreco);
		      hname = "SS2gXcogVsSumE_inSR";
		      hSvc->FillHisto2(hname, aSumCl, xcog);
		      hname = "SS2gYcogVsSumE_inSR";
		      hSvc->FillHisto2(hname, aSumCl, ycog);
		      hname = "SS2gEVsE_inSR";
		      hSvc->FillHisto2(hname, xEne, aEne);
		      hname = "SS2gXY_inSR";
		      hSvc->FillHisto2(hname, pos1.X(), pos1.Y(), 0.5);
		      hSvc->FillHisto2(hname, pos2.X(), pos2.Y(), 0.5);
		      hname = "SS2gEwXY_inSR";
		      hSvc->FillHisto2(hname, pos1.X(), pos1.Y(), xEne);
		      hSvc->FillHisto2(hname, pos2.X(), pos2.Y(), aEne);

		      if (fabs(dt)<10.) {
			
			hname = "SS2gSumE_inSR_Dt10";
			hSvc->FillHisto(hname,aSumCl);
		      }
		      if (fabs(dt)<10. && cos(dPhi)<cos(160.*pigreco/180.)) {
			
			hname = "SS2gSumE_inSR_Dt10Dphi160";
			hSvc->FillHisto(hname,aSumCl);
		      }
		      if (fabs(dt)<10. && fabs(xcog)<30. && fabs(ycog)<30.) {
			
			hname = "SS2gSumE_inSR_Dt10Cog30";
			hSvc->FillHisto(hname,aSumCl);
		      }
		      if (cos(dPhi)<cos(160.*pigreco/180.) && fabs(xcog)<30. && fabs(ycog)<30.) {
			
			hname = "SS2gSumE_inSR_Dphi160Cog30";
			hSvc->FillHisto(hname,aSumCl);
		      }
		    }
		  // 

	    
		  
		  
		  if (fabs(dt)<10)
		    {
		      n2gindt10 +=1;
		      if (fabs(dt)<5)
			{
			  n2gindt5 +=1;
			  if (fabs(dt)<2.5)
			    {
			      n2gindt2_5 +=1;
			      if (fabs(dt)<1)
				{
				  n2gindt1 +=1;
				}	
			    }		  
			}
		    }


		  if (fabs(dt)<10.)
		    {
		      hname = "SS2gR_passDt";
		      hSvc->FillHisto(hname, pos1.Perp(), 0.5);
		      hSvc->FillHisto(hname, pos2.Perp(), 0.5);

		      hname = "SS2gEVsE_passDt";
		      hSvc->FillHisto2(hname, xEne, aEne);

		      hname = "SS2gPhi_passDt";
		      hSvc->FillHisto(hname, pos1.Phi(), 0.5);
		      hSvc->FillHisto(hname, pos2.Phi(), 0.5);

		      hname = "SS2gSumE_passDt";
		      hSvc->FillHisto(hname, aSumCl);

		      hname = "SS2gDphi_passDt";
		      hSvc->FillHisto(hname, dPhi*180/pigreco);
		      hname = "SS2gXcog_passDt";
		      hSvc->FillHisto(hname, xcog);
		      hname = "SS2gYcog_passDt";
		      hSvc->FillHisto(hname, ycog);


		      hname = "SS2gE_passDt";
		      hSvc->FillHisto(hname, xEne, 0.5);
		      hSvc->FillHisto(hname, aEne, 0.5);
		      hname = "SS2gX_passDt";
		      hSvc->FillHisto(hname, pos1.x(), 0.5);
		      hSvc->FillHisto(hname, pos2.x(), 0.5);
		      hname = "SS2gY_passDt";
		      hSvc->FillHisto(hname, pos1.y(), 0.5);
		      hSvc->FillHisto(hname, pos2.y(), 0.5);
		      hname = "SS2gXEw_passDt";
		      hSvc->FillHisto(hname, pos1.x(), xEne);
		      hSvc->FillHisto(hname, pos2.x(), aEne);
		      hname = "SS2gYEw_passDt";
		      hSvc->FillHisto(hname, pos1.y(), xEne);
		      hSvc->FillHisto(hname, pos2.y(), aEne);
		      hname = "SS2gXYEw_passDt";
		      hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
		      hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);
		      hname = "SS2gXY_passDt";
		      hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
		      hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);


		      hname = "SS2gEvsR_passDt";
		      hSvc->FillHisto2(hname, pos1.Perp(), xEne,0.5);
		      hSvc->FillHisto2(hname, pos2.Perp(), aEne,0.5);
		      
		      hname = "SS2gEvsPhi_passDt";
		      hSvc->FillHisto2(hname, pos1.Phi(), xEne,0.5);
		      hSvc->FillHisto2(hname, pos2.Phi(), aEne,0.5);
			      


		      if (cos(dPhi)<cos(2.3562))
			{
			  hname = "SS2gSumE_passDtDphi";
			  hSvc->FillHisto(hname, aSumCl);

			  hname = "SS2gEVsE_passDtDphi";
			  hSvc->FillHisto2(hname, xEne, aEne);

			  hname = "SS2gPhi_passDtDphi";
			  hSvc->FillHisto(hname, pos1.Phi(), 0.5);
			  hSvc->FillHisto(hname, pos2.Phi(), 0.5);
			  hname = "SS2gR_passDtDphi";
			  hSvc->FillHisto(hname, pos1.Perp(), 0.5);
			  hSvc->FillHisto(hname, pos2.Perp(), 0.5);
			  hname = "SS2gDt_passDtDphi";
			  hSvc->FillHisto(hname, dt);
			  hname = "SS2gDphi_passDtDphi";
			  hSvc->FillHisto(hname, dPhi*180/pigreco);
			  hname = "SS2gXcog_passDtDphi";
			  hSvc->FillHisto(hname, xcog);
			  hname = "SS2gYcog_passDtDphi";
			  hSvc->FillHisto(hname, ycog);


			  hname = "SS2gE_passDtDphi";
			  hSvc->FillHisto(hname, xEne, 0.5);
			  hSvc->FillHisto(hname, aEne, 0.5);
			  hname = "SS2gX_passDtDphi";
			  hSvc->FillHisto(hname, pos1.x(), 0.5);
			  hSvc->FillHisto(hname, pos2.x(), 0.5);
			  hname = "SS2gY_passDtDphi";
			  hSvc->FillHisto(hname, pos1.y(), 0.5);
			  hSvc->FillHisto(hname, pos2.y(), 0.5);
			  hname = "SS2gXEw_passDtDphi";
			  hSvc->FillHisto(hname, pos1.x(), xEne);
			  hSvc->FillHisto(hname, pos2.x(), aEne);
			  hname = "SS2gYEw_passDtDphi";
			  hSvc->FillHisto(hname, pos1.y(), xEne);
			  hSvc->FillHisto(hname, pos2.y(), aEne);
			  hname = "SS2gXYEw_passDtDphi";
			  hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
			  hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);
			  hname = "SS2gXY_passDtDphi";
			  hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
			  hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);

			  hname = "SS2gEvsR_passDtDphi";
			  hSvc->FillHisto2(hname, pos1.Perp(), xEne,0.5);
			  hSvc->FillHisto2(hname, pos2.Perp(), aEne,0.5);
			  
			  hname = "SS2gEvsPhi_passDtDphi";
			  hSvc->FillHisto2(hname, pos1.Phi(), xEne,0.5);
			  hSvc->FillHisto2(hname, pos2.Phi(), aEne,0.5);
			      
			  
			  //			  if (fabs(xcog)<20. && fabs(ycog)<20.)
			  if (fabs(xcog)<30. && fabs(ycog)<30.)
			    {
			      //hname = "SS2gSumE_passdt1dphi10cog10";
			      hname = "SS2gSumE_passDtDphiCog";
			      hSvc->FillHisto(hname, aSumCl);

			      hname = "SS2gEVsE_passDtDphiCog";
			      hSvc->FillHisto2(hname, xEne, aEne);

			      hname = "SS2gPhi_passDtDphiCog";
			      hSvc->FillHisto(hname, pos1.Phi(), 0.5);
			      hSvc->FillHisto(hname, pos2.Phi(), 0.5);
			      hname = "SS2gR_passDtDphiCog";
			      hSvc->FillHisto(hname, pos1.Perp(), 0.5);
			      hSvc->FillHisto(hname, pos2.Perp(), 0.5);
			      hname = "SS2gDt_passDtDphiCog";
			      hSvc->FillHisto(hname, dt);
			      hname = "SS2gDphi_passDtDphiCog";
			      hSvc->FillHisto(hname, dPhi*180/pigreco);
			      n2g+=1;

			      
			      hname = "SS2gE_passDtDphiCog";
			      hSvc->FillHisto(hname, xEne, 0.5);
			      hSvc->FillHisto(hname, aEne, 0.5);
			      hname = "SS2gX_passDtDphiCog";
			      hSvc->FillHisto(hname, pos1.x(), 0.5);
			      hSvc->FillHisto(hname, pos2.x(), 0.5);
			      hname = "SS2gY_passDtDphiCog";
			      hSvc->FillHisto(hname, pos1.y(), 0.5);
			      hSvc->FillHisto(hname, pos2.y(), 0.5);
			      hname = "SS2gXEw_passDtDphiCog";
			      hSvc->FillHisto(hname, pos1.x(), xEne);
			      hSvc->FillHisto(hname, pos2.x(), aEne);
			      hname = "SS2gYEw_passDtDphiCog";
			      hSvc->FillHisto(hname, pos1.y(), xEne);
			      hSvc->FillHisto(hname, pos2.y(), aEne);
			      hname = "SS2gXYEw_passDtDphiCog";
			      hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
			      hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);
			      hname = "SS2gXY_passDtDphiCog";
			      hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
			      hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);

			      hname = "SS2gEvsR_passDtDphiCog";
			      hSvc->FillHisto2(hname, pos1.Perp(), xEne,0.5);
			      hSvc->FillHisto2(hname, pos2.Perp(), aEne,0.5);
			      
			      hname = "SS2gEvsPhi_passDtDphiCog";
			      hSvc->FillHisto2(hname, pos1.Phi(), xEne,0.5);
			      hSvc->FillHisto2(hname, pos2.Phi(), aEne,0.5);
			      
			      

			      //			      if (fabs(pos1.y())>120. && fabs(pos2.y())>120.) {
			      //			      if (fabs(pos1.x())>100. && fabs(pos2.x())>100. &&  fabs(pos1.y())<200. && fabs(pos2.y())<200.) {
			      //			      if (pos1.Perp()>125. && pos2.Perp()>125. && pos1.Perp()<290. && pos2.Perp()<290.) { 
			      //			      if (pos1.Perp()<200. && pos2.Perp()<200.) {
			      double phicut=0;
			      if (xEne>aEne) phicut = pos1.Phi();
			      else phicut = pos2.Phi();
			      //if (fabs(phicut-pigreco/2.)<pigreco/8. || fabs(phicut-3.*pigreco/2.)<pigreco/8.) { 
			      //			      if (fabs(phicut-pigreco/4.)<pigreco/8. || fabs(phicut-5.*pigreco/4.)<pigreco/8.) { 

			      //			      if (fabs(phicut)<2.*pigreco/8. || fabs(phicut-pigreco)<2*pigreco/8. || fabs(-phicut-pigreco)<2*pigreco/8.) { 

			      //			      if (fabs(phicut-3.*pigreco/4.)<pigreco/8.  || fabs(phicut-7.*pigreco/4.)<pigreco/8.) { 
			      //if (fabs(pos1.y())<150. && fabs(pos2.y())<150.) {
			      bool inSpot = false;
			      //inSpot = (fabs(fabs(pos1.y())-90.)<20.)&&(fabs(pos1.x()-150.)<50.)||(fabs(fabs(pos2.y())-90.)<20.)&&(fabs(pos2.x()-150.)<50.);
			      inSpot = (fabs(pos1.x()-165.)<55.)||(fabs(pos2.x()-165.)<55.);
			      if (!inSpot){

				n2gFR+=1;
				hname = "SS2gSumE_passDtDphiCogFR";
				hSvc->FillHisto(hname, aSumCl);
				if (fabs(phicut-pigreco/2.)<pigreco/8. || fabs(phicut-3.*pigreco/2.)<pigreco/8.) 
				  {
				    hname = "SS2gSumE_passDtDphiCogFR90";
				    hSvc->FillHisto(hname, aSumCl);
				  }
				if (fabs(phicut-pigreco/4.)<pigreco/8. || fabs(phicut-5.*pigreco/4.)<pigreco/8.) 
				  {
				    hname = "SS2gSumE_passDtDphiCogFR45";
				    hSvc->FillHisto(hname, aSumCl);
				  }
				if (fabs(phicut)<2.*pigreco/8. || fabs(phicut-pigreco)<2*pigreco/8. || fabs(-phicut-pigreco)<2*pigreco/8.) 
				  {
				    hname = "SS2gSumE_passDtDphiCogFR0";
				    hSvc->FillHisto(hname, aSumCl);
				  }	
				if (fabs(phicut-3.*pigreco/4.)<pigreco/8.  || fabs(phicut-7.*pigreco/4.)<pigreco/8.) 
				  { 
				    hname = "SS2gSumE_passDtDphiCogFR135";
				    hSvc->FillHisto(hname, aSumCl);
				  }
				
				
				
				hname = "SS2gEVsE_passDtDphiCogFR";
				hSvc->FillHisto2(hname, xEne, aEne);
				
				hname = "SS2gPhi_passDtDphiCogFR";
				hSvc->FillHisto(hname, pos1.Phi(), 0.5);
				hSvc->FillHisto(hname, pos2.Phi(), 0.5);
				hname = "SS2gR_passDtDphiCogFR";
				hSvc->FillHisto(hname, pos1.Perp(), 0.5);
				hSvc->FillHisto(hname, pos2.Perp(), 0.5);
				hname = "SS2gDt_passDtDphiCogFR";
				hSvc->FillHisto(hname, dt);				  
				hname = "SS2gDphi_passDtDphiCogFR";
				hSvc->FillHisto(hname, dPhi*180/pigreco);
				hname = "SS2gXcog_passDtDphiCogFR";
				hSvc->FillHisto(hname, xcog);
				hname = "SS2gYcog_passDtDphiCogFR";
				hSvc->FillHisto(hname, ycog);
			      
			      
				hname = "SS2gE_passDtDphiCogFR";
				hSvc->FillHisto(hname, xEne, 0.5);
				hSvc->FillHisto(hname, aEne, 0.5);
				hname = "SS2gX_passDtDphiCogFR";
				hSvc->FillHisto(hname, pos1.x(), 0.5);
				hSvc->FillHisto(hname, pos2.x(), 0.5);
				hname = "SS2gY_passDtDphiCogFR";
				hSvc->FillHisto(hname, pos1.y(), 0.5);
				hSvc->FillHisto(hname, pos2.y(), 0.5);
				hname = "SS2gXEw_passDtDphiCogFR";
				hSvc->FillHisto(hname, pos1.x(), xEne);
				hSvc->FillHisto(hname, pos2.x(), aEne);
				hname = "SS2gYEw_passDtDphiCogFR";
				hSvc->FillHisto(hname, pos1.y(), xEne);
				hSvc->FillHisto(hname, pos2.y(), aEne);
				hname = "SS2gXYEw_passDtDphiCogFR";
				hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
				hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);
				hname = "SS2gXY_passDtDphiCogFR";
				hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
				hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);


				hname = "SS2gEvsR_passDtDphiCogFR";
				hSvc->FillHisto2(hname, pos1.Perp(), xEne,0.5);
				hSvc->FillHisto2(hname, pos2.Perp(), aEne,0.5);
				
				hname = "SS2gEvsPhi_passDtDphiCogFR";
				hSvc->FillHisto2(hname, pos1.Phi(), xEne,0.5);
				hSvc->FillHisto2(hname, pos2.Phi(), aEne,0.5);

				if (fabs(aSumCl-490.)<50.)
				  {
				    hname = "SS2gSumE_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, aSumCl);

				    hname = "SS2gEVsE_passDtDphiCogDsume";
				    hSvc->FillHisto2(hname, xEne, aEne);
				    
				    hname = "SS2gPhi_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, pos1.Phi(), 0.5);
				    hSvc->FillHisto(hname, pos2.Phi(), 0.5);
				    hname = "SS2gR_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, pos1.Perp(), 0.5);
				    hSvc->FillHisto(hname, pos2.Perp(), 0.5);
				    hname = "SS2gDt_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, dt);				  
				    hname = "SS2gDphi_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, dPhi*180/pigreco);
				    hname = "SS2gXcog_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, xcog);
				    hname = "SS2gYcog_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, ycog);


				    hname = "SS2gE_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, xEne, 0.5);
				    hSvc->FillHisto(hname, aEne, 0.5);
				    hname = "SS2gX_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, pos1.x(), 0.5);
				    hSvc->FillHisto(hname, pos2.x(), 0.5);
				    hname = "SS2gY_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, pos1.y(), 0.5);
				    hSvc->FillHisto(hname, pos2.y(), 0.5);
				    hname = "SS2gXEw_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, pos1.x(), xEne);
				    hSvc->FillHisto(hname, pos2.x(), aEne);
				    hname = "SS2gYEw_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, pos1.y(), xEne);
				    hSvc->FillHisto(hname, pos2.y(), aEne);
				    hname = "SS2gXYEw_passDtDphiCogDsume";
				    hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
				    hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);
				    hname = "SS2gXY_passDtDphiCogDsume";
				    hSvc->FillHisto2(hname, pos1.x(), pos1.y(), xEne);
				    hSvc->FillHisto2(hname, pos2.x(), pos2.y(), aEne);


				    n2gDsume+=1;
				    hname="SS2g_ClTime_passDtDphiCogDsume";
				    hSvc->FillHisto(hname, xTime);
				    hname = "NposInBunch_beam_passDtDphiCogDsume";
				    hSvc->FillHisto(hname,fTarget_RecoBeam->getnPOT());
				 
				    hname = "SS2gEvsR_passDtDphiCogDsume";
				    hSvc->FillHisto2(hname, pos1.Perp(), xEne,0.5);
				    hSvc->FillHisto2(hname, pos2.Perp(), aEne,0.5);
				    
				    hname = "SS2gEvsPhi_passDtDphiCogDsume";
				    hSvc->FillHisto2(hname, pos1.Phi(), xEne,0.5);
				    hSvc->FillHisto2(hname, pos2.Phi(), aEne,0.5);
				    
				  }
				}
			    }
			}
		    }

		  
		  if (fabs(aTime-xTime)<3.)
		    {
		      if (aSumCl > aSumClIn10Max)
			{
			  //igamma1In10 = hECal;
			  //igamma2In10 = clCal;
			  aSumClIn10Max = aSumCl;
			}

		      
		      double m2inv = xEne*aEne*dR*dR/3000./3000.;
		      double minv = sqrt(m2inv);
		      double rcog = sqrt(xcog*xcog+ycog*ycog);



		      /*
			std::cout<<"xEne, pos1 "<<xEne<<" "<<pos1.x()<<" "<<pos1.y()<<" "<<pos1.z()<<std::endl; 
			std::cout<<"aEne, pos2 "<<aEne<<" "<<pos2.x()<<" "<<pos2.y()<<" "<<pos2.z()<<std::endl; 
			std::cout<<"rcog = "<<rcog<<" minv = "<<minv<<std::endl;
		      */


		      hname = hprefix+"ECal2gsearchDt3_ImpactParVsMinv";
		      hSvc->FillHisto2(hname, minv, impactPar);
		      hname = hprefix+"ECal2gsearchDt3_ImpactParVsRcog";
		      hSvc->FillHisto2(hname, rcog, impactPar);
		      
		      hname =  hprefix+"ECal2gsearchDt3_ImpactParVsDPhi";
		      hSvc->FillHisto2(hname, dPhi, impactPar);
		      hname =  hprefix+"ECal2gsearchDt3_ImpactParVsDR";
		      hSvc->FillHisto2(hname, dR, impactPar);
		      hname =  hprefix+"ECal2gsearchDt3_ESumVsMinv";
		      hSvc->FillHisto2(hname, minv, aSumCl);

		      //		      if (dPhi>2.8 && fabs(impactPar)<50.)
		      if (aSumCl>300 && aSumCl<700. && minv>15. && minv<35.) 
			{
			  hname =  hprefix+"ECal2gsearchDt3_ESumVsMinv_phibcut";
			  hSvc->FillHisto2(hname, minv, aSumCl);
			  hname =  hprefix+"ECal2gsearchDt3_ImpactParVsDPhi_phibcut";
			  hSvc->FillHisto2(hname, dPhi, impactPar);
			}
		      
		      hname = "ECal2gsearchDt3_ImpactPar";////////////change 
		      hSvc->FillHisto(hname, impactPar);
		      hname = "DPhi2gammaIn10";
		      hSvc->FillHisto(hname, dPhi);
		      hname = "DR2gammaIn10";
		      hSvc->FillHisto(hname, dR);

		      hname = "ECal2gsearch_ESumDt3";
		      hSvc->FillHisto(hname, aSumCl);
		      hname = "ECal2gsearch_MinvVsDPhiDt3";
		      hSvc->FillHisto2(hname, dPhi, minv);
		      if (fabs(aTime-xTime)<2.)
			{
			  hname = "ECal2gsearch_MinvVsDPhiDt2";
			  hSvc->FillHisto2(hname, dPhi, minv);
			  if (fabs(aTime-xTime)<1.)
			    {
			      hname = "ECal2gsearch_MinvVsDPhiDt1";
			      hSvc->FillHisto2(hname, dPhi, minv);
			    }
			}
		      hname = "ECal2gsearch_MinvVsRcogDt3";
		      hSvc->FillHisto2(hname, rcog, minv);
		      hname = "ECal2gsearch_MinvDt3";
		      hSvc->FillHisto(hname, minv);
		      if (cos(dPhi)<-0.9915)
			{
			  hname = "ECal2gsearch_ESumDt3Phi75";
			  hSvc->FillHisto(hname, aSumCl);
			  hname = "ECal2gsearch_MinvVsRcogDt3Phi75";
			  hSvc->FillHisto2(hname, rcog, minv);
			  hname = "ECal2gsearch_MinvDt3Phi75";
			  hSvc->FillHisto(hname, minv);
			  if (cos(dPhi)<-0.9962)
			    {
			      hname = "ECal2gsearch_ESumDt3Phi50";
			      hSvc->FillHisto(hname, aSumCl);
			      hname = "ECal2gsearch_MinvVsRcogDt3Phi50";
			      hSvc->FillHisto2(hname, rcog, minv);
			      hname = "ECal2gsearch_MinvDt3Phi50";
			      hSvc->FillHisto(hname, minv);
			      if (cos(dPhi)<-0.999)
				{
				  hname = "ECal2gsearch_ESumDt3Phi25";
				  hSvc->FillHisto(hname, aSumCl);
				  hname = "ECal2gsearch_MinvVsRcogDt3Phi25";
				  hSvc->FillHisto2(hname, rcog, minv);
				  hname = "ECal2gsearch_MinvDt3Phi25";
				  hSvc->FillHisto(hname, minv);
				}
			    }
			}
		      
		      
		      if (isPhySym)
			{
			  // here fill dR 
			  hname = "DR2gammaIn10PhiSym";
			  hSvc->FillHisto(hname, dR);

			  hname = "ECal2gsearch_ESumDt3Phi100";
			  hSvc->FillHisto(hname, aSumCl);
			  hname = "ECal2gsearch_MinvVsRcogDt3Phi100";
			  hSvc->FillHisto2(hname, rcog, minv);
			  hname = "ECal2gsearch_MinvDt3Phi100";
			  hSvc->FillHisto(hname, minv);

			  if (fabs(dt)<2.)
			    {
			      hname = "ECal2gsearch_ESumDt2Phi100";
			      hSvc->FillHisto(hname, aSumCl);
			      hname = "ECal2gsearch_MinvVsRcogDt2Phi100";
			      hSvc->FillHisto2(hname, rcog, minv);
			      hname = "ECal2gsearch_MinvDt2Phi100";
			      hSvc->FillHisto(hname, minv);
			      if (cos(dPhi)<-0.9915)
				{
				  hname = "ECal2gsearch_ESumDt2Phi75";
				  hSvc->FillHisto(hname, aSumCl);
				  hname = "ECal2gsearch_MinvVsRcogDt2Phi75";
				  hSvc->FillHisto2(hname, rcog, minv);
				  hname = "ECal2gsearch_MinvDt2Phi75";
				  hSvc->FillHisto(hname, minv);
				  if (cos(dPhi)<-0.9962)
				    {
				      hname = "ECal2gsearch_ESumDt2Phi50";
				      hSvc->FillHisto(hname, aSumCl);
				      hname = "ECal2gsearch_MinvVsRcogDt2Phi50";
				      hSvc->FillHisto2(hname, rcog, minv);
				      hname = "ECal2gsearch_MinvDt2Phi50";
				      hSvc->FillHisto(hname, minv);
				      if (cos(dPhi)<-0.999)
					{
					  hname = "ECal2gsearch_ESumDt2Phi25";
					  hSvc->FillHisto(hname, aSumCl);
					  hname = "ECal2gsearch_MinvVsRcogDt2Phi25";
					  hSvc->FillHisto2(hname, rcog, minv);
					  hname = "ECal2gsearch_MinvDt2Phi25";
					  hSvc->FillHisto(hname, minv);
					}
				    }
				}
			      if (fabs(dt)<1.)
				{
				  hname = "ECal2gsearch_ESumDt1Phi100";
				  hSvc->FillHisto(hname, aSumCl);
				  hname = "ECal2gsearch_MinvVsRcogDt1Phi100";
				  hSvc->FillHisto2(hname, rcog, minv);
				  hname = "ECal2gsearch_MinvDt1Phi100";
				  hSvc->FillHisto(hname, minv);
				  if (cos(dPhi)<-0.9915)
				    {
				      hname = "ECal2gsearch_ESumDt1Phi75";
				      hSvc->FillHisto(hname, aSumCl);
				      hname = "ECal2gsearch_MinvVsRcogDt1Phi75";
				      hSvc->FillHisto2(hname, rcog, minv);
				      hname = "ECal2gsearch_MinvDt1Phi75";
				      hSvc->FillHisto(hname, minv);
				      if (cos(dPhi)<-0.9962)
					{
					  hname = "ECal2gsearch_ESumDt1Phi50";
					  hSvc->FillHisto(hname, aSumCl);
					  hname = "ECal2gsearch_MinvVsRcogDt1Phi50";
					  hSvc->FillHisto2(hname, rcog, minv);
					  hname = "ECal2gsearch_MinvDt1Phi50";
					  hSvc->FillHisto(hname, minv);
					  if (cos(dPhi)<-0.999)
					    {
					      hname = "ECal2gsearch_ESumDt1Phi25";
					      hSvc->FillHisto(hname, aSumCl);
					      hname = "ECal2gsearch_MinvVsRcogDt1Phi25";
					      hSvc->FillHisto2(hname, rcog, minv);
					      hname = "ECal2gsearch_MinvDt1Phi25";
					      hSvc->FillHisto(hname, minv);
					    }
					}
				    }
				}
			    }

			  if (aSumCl > aSumClIn10PhiSMax)
			    {
			      //igamma1In10PhiS = hECal;
			      //igamma2In10PhiS = clCal;
			      aSumClIn10PhiSMax = aSumCl;
			      // here fill dR for Max ESum
			      hname = "DR2gammaIn10PhiSymMaxESum";
			      hSvc->FillHisto(hname, dR);
			    }
			}
		    }		  
		}
	    }
	}

    }


  hname="SS2g_nclus50";
  hSvc->FillHisto(hname,float(nclus50));
  hname="SS2g_n2gindt10";
  hSvc->FillHisto(hname,float(n2gindt10));
  hname="SS2g_n2gindt5";
  hSvc->FillHisto(hname,float(n2gindt5));
  hname="SS2g_n2gindt2_5";
  hSvc->FillHisto(hname,float(n2gindt2_5));
  hname="SS2g_n2gindt1";
  hSvc->FillHisto(hname,float(n2gindt1));
  hname="SS2g_n2g";
  hSvc->FillHisto(hname,float(n2g));
  hname="SS2g_n2gFR";
  hSvc->FillHisto(hname,float(n2gFR));


  hname=hprefix+"energySumECalClus";
  hSvc->FillHisto(hname, eSumECalHits);
  if (fECal_ClColl->GetNElements()==1)
    {
      hname=hprefix+"energySumECalClus_1Cl";
      hSvc->FillHisto(hname, eSumCl);
    }
  else if (fECal_ClColl->GetNElements()==2)
    {
      hname=hprefix+"energySumECalClus_2Cl";
      hSvc->FillHisto(hname, eSumCl);
    }
  else if (fECal_ClColl->GetNElements()==3)
    {
      hname=hprefix+"energySumECalClus_3Cl";
      hSvc->FillHisto(hname, eSumCl);
    }
  else if (fECal_ClColl->GetNElements()>3)
    {
      hname=hprefix+"energySumECalClus_NCl";
      hSvc->FillHisto(hname, eSumCl);
    }

  


  for (int hECal=0; hECal<fECal_ClColl->GetNElements(); ++hECal)
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

  //  hname="energySumECalClus";
  //  hSvc->FillHisto(hname, eSumCl);
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
  ProcessAnalysisIO();
  return true; 
}




Bool_t EventSelection::phiSymmetricalInECal(TVector3 P1, TVector3 P2, double& distR, double& distPhi, double& b)
{
  distPhi = -999.;
  distR   = -999.;
  b       = -999.;
  P1.SetZ(0.);
  P2.SetZ(0.);
  TVector3 Pb = P2-P1;
  //double impactPar = (P1.Cross(Pb)).Perp()/Pb.Perp();
  //std::cout<<" P1.X, Y "<<P1.X()<<" "<<P1.Y()<<std::endl;
  //std::cout<<" P2.X, Y "<<P2.X()<<" "<<P2.Y()<<std::endl;
  double impPar = (P1.X()*Pb.Y()-P2.X()*Pb.Y())/Pb.Perp();
  
 
  double scalarP = P1.Dot(P2);
  double cosphi = scalarP/P1.Perp()/P2.Perp();
  double dPhi = acos(cosphi);

  distPhi = dPhi;
  distR   = Pb.Perp();
  b = impPar;
  //std::cout<<"dPhi = "<<distPhi<<" b = "<<b<<" impPar "<<impPar<<std::endl;

  //if (cosphi<-0.985 && fabs(b)<10.) return true;
  if (cosphi<-0.985) return true;
  return false;
  
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
  
    std::cout<<" 1) x,y "<<x1<<" "<<y1<<std::endl;
    std::cout<<" 2) x,y "<<x2<<" "<<y2<<std::endl;
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
  if (fVersion==2)
    {
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

      hname="ECalCl_E";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, maxX);

      hname="SACCl_E";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, maxX);


      hname="ECal2Cl_InvM";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, 50);

      hSvc->BookHisto2("MissingMass2", 100,-5000, 5000, 100,-5000, 5000);
      hSvc->BookHisto ("MissingMassPhoton1", 2000,-3000, 3000);
      hSvc->BookHisto ("MissingMassPhoton2", 2000,-3000, 3000);
      hSvc->BookHisto ("MissingMassPhoton3", 2000,-3000, 3000);
      hSvc->BookHisto ("MissingMassPhoton4", 2000,-3000, 3000);
      hSvc->BookHisto ("PVetoPhotonEnergy" , 100,0, 1000);
      hSvc->BookHisto ("MissingMassPhoton5", 2000,-3000, 3000);
      hSvc->BookHisto ("PhotonRadius"      , 100,0, 1000);

      hname="ECal2Cl_ChId1";
      hSvc->BookHisto(hname, 3000, 0, 3000);

      hname="ECal2Cl_ChId2";
      hSvc->BookHisto(hname, 3000, 0, 3000);

      hname="ECal2Cl_InvM_5MeV";
      nBinX=  750; 
     minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, 50);

      hname="ECal2Cl_InvM_100MeV";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, 50);
 
      hname="ECal2Cl_InvM_0MeV";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, 50);

      hname="ECal2Cl_InvM_D";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, 50);

      hname="ECal2Cl_D";
      hSvc->BookHisto(hname, 100, 0,100);



      hname="ECal2Cl_InvM_PVeto";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, 50);


      hname="ECal2Cl_InvM_PVeto_EVeto";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, 50);

      hname="ECal2Cl_InvM_vs_CosDeltaPhi";
      hSvc->BookHisto2(hname, 2000, -1.1, 1.1, 100, 0, 50);

      hname="ECal2Cl_InvM_vs_Phi";
      hSvc->BookHisto2(hname, 2000, -3.2, 3.2, 300, 0, 50);

      hname="ECal2Cl_InvM_vs_D";
      hSvc->BookHisto2(hname, 200, 0, 200, 300, 0, 50);

      hname="ECal2Cl_InvM_vs_DeltaTimePVeto";
      hSvc->BookHisto2(hname, 4000, -200, 200, 100, 0, 50);

      hname="ECal2Cl_InvM_vs_DeltaTimeEVeto";
      hSvc->BookHisto2(hname, 4000, -200, 200, 100, 0, 50);

      hname="ECal2Cl_InvM_vs_DeltaTheta";
      hSvc->BookHisto2(hname, 2000, -1.1, 1.1, 100, 0, 50);

      hname="ECal2Cl_InvM_vs_SumE1E2";
      hSvc->BookHisto2(hname, 200, 0, 1000, 100, 0, 50);

      hname="ECal2Cl_CosDeltaPhi";
      hSvc->BookHisto(hname, 2000, -1.1, 1.1);

      hname="ECal2Cl_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="SAC2Cl_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="ECal1Cl_PVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="ECal1Cl_HEPVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="SAC1Cl_HEPVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="SAC_21_PVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="SAC1Cl_PVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="SAC_21_PVeto_DeltaTime_Hit";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="SAC1Cl_PVeto_DeltaTime_Hit";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="SAC1Cl_E_vs_PVetoChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="SAC1Cl_E_vs_PVetoChId_OutOfTime";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="SAC1Cl_E_vs_PVetoChId_OutOfTime3";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="SAC1Cl_Y_vs_X_OutOfTime3";
      hSvc->BookHisto2(hname, 140, -70, 70, 140, -70, 70);
      hname="SAC1Cl_Y_vs_X";
      hSvc->BookHisto2(hname, 1400, -70, 70, 1400, -70, 70);

      hname="SAC1Cl_Y_vs_Z_OutOfTime3";
      hSvc->BookHisto2(hname, 90, -473, 507, 140, -70, 70);
      hname="SAC1Cl_Y_vs_Z";
      hSvc->BookHisto2(hname, 90, -473, 507, 140, -70, 70);

      hname="SAC1Cl_X_vs_Z_OutOfTime3";
      hSvc->BookHisto2(hname, 90, -473, 507, 140, -70, 70);
      hname="SAC1Cl_X_vs_Z";
      hSvc->BookHisto2(hname, 90, -473, 507, 140, -70, 70);
      hname="SAC1Cl_E_vs_PVetoChId_Xm";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);
      hname="SAC1Cl_E_vs_PVetoChId_Xp";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);
      hname="SAC1Cl_E_vs_PVetoChId_X0";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);
      hname="SAC1Cl_E_vs_PVetoChId_Xm_OutOfTime3";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);


      hname="SAC1Cl_E_vs_PVetoChId_all";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="PVetoCl_PH_vs_ChId_SACinTime";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="HEPVetoCl_PH_vs_ChId_SACinTime";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="PVetoCl_PH_vs_ChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="HEPVetoCl_PH_vs_ChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="PVetoE_vs_PVetoChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="PVetoT_vs_PVetoChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, -500, 500);

      hname="SAC1Cl_E_vs_PVetoPosz";
      hSvc->BookHisto2(hname, 90, -473, 507, 1000, 0, 1000);

      hname="SAC1Cl_E_vs_HEPVetoPosx";
      hSvc->BookHisto2(hname, 16, 792.5, 948.5, 1000, 0, 1000);

      hname="SAC1Cl_E_vs_HEPVetoPosz";
      hSvc->BookHisto2(hname, 16, 2067.31, 2122.3, 1000, 0, 1000);

      hname="SAC1Cl_E_vs_HEPVetoPosL";
      hSvc->BookHisto2(hname, 16, -5, 170, 1000, 0, 1000);

      hname="SAC1Cl_E_vs_HEPVetoChId_OutOfTime3";
      hSvc->BookHisto2(hname, 32, 0, 32, 1000, 0, 1000);

      hname="SAC1Cl_E_vs_HEPVetoPosL_OutOfTime3";
      hSvc->BookHisto2(hname, 16, -5, 170, 1000, 0, 1000);

      hname="SAC1Cl_E_vs_HEPVetoChId";
      hSvc->BookHisto2(hname, 32, 0, 32, 1000, 0, 1000);

  
      hname="SAC1Cl_E_vs_PVetoChId_Hit";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="SAC1Hit_E_vs_PVetoChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);

      hname="SAC1Hit_E_vs_PVetoChId_Hit";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);
  
      hname="ECal1Cl_E_vs_PVetoChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);
   
  
      hname="ECal1Cl_E_vs_HEPVetoChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 1000, 0, 1000);
  
      hname="ECal1Cl_E_vs_HEPVetoPosz";
      hSvc->BookHisto2(hname, 16, 2067.31, 2123, 1000, 0, 1000);
   




      //
 

      hname="SAC1Cl_EVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);
 

      hname="PVeto1Cl_EVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);
      hname="PVeto1Cl_HEPVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);
      hname="EVeto1Cl_HEPVeto_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);
      hname="PVeto2Cl_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);
      hname="HEPVeto2Cl_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);
      hname="EVeto2Cl_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);
 

      hname="PVeto_EVeto_Msum_vs_DeltaTime";
      hSvc->BookHisto2(hname, 1000, -100, 100, 1000, 0, 1000);

      hname="SAC_PVeto_EVeto_Msum_vs_DeltaTime";
      hSvc->BookHisto2(hname, 1000, -100, 100, 1000, 0, 1000);
  
      hname="PVeto1Cl_chId_vs_EVeto1Cl_chId";
      hSvc->BookHisto2(hname, 100, 0, 100, 100, 0, 100);
  
      hname="PVeto1Cl_chId_vs_EVeto1Cl_chId_OutOfTime";
      hSvc->BookHisto2(hname, 100, 0, 100, 100, 0, 100);
   
   
      hname="PVeto_EVeto_Msum_OutOfTime";
      hSvc->BookHisto(hname, 1000, 0, 1000);
   
      hname="PVeto_EVeto_Msum";
      hSvc->BookHisto(hname, 1000, 0, 1000);

   
      //





      hname="ECal1ClSAC1Cl_InvM";
      nBinX=  750;
      minX =  0.;
      hSvc->BookHisto(hname, nBinX, minX, 50);

      hname="ECal1ClSAC1Cl_D";
      hSvc->BookHisto(hname, 100, 0,100);

      hname="ECal1ClSAC1Cl_InvM_vs_CosDeltaPhi";
      hSvc->BookHisto2(hname, 2000, -1.1, 1.1, 100, 0, 50);

      hname="ECal1ClSAC1Cl_InvM_vs_Phi";
      hSvc->BookHisto2(hname, 2000, -3.2, 3.2, 300, 0, 50);

      hname="ECal1ClSAC1Cl_InvM_vs_D";
      hSvc->BookHisto2(hname, 200, 0, 200, 300, 0, 50);

      hname="ECal1ClSAC1Cl_InvM_vs_DeltaTheta";
      hSvc->BookHisto2(hname, 2000, -1.1, 1.1, 100, 0, 50);

      hname="ECal1ClSAC1Cl_InvM_vs_SumE1E2";
      hSvc->BookHisto2(hname, 200, 0, 1000, 100, 0, 50);

      hname="ECal1ClSAC1Cl_SumE1E2";
      hSvc->BookHisto(hname, 200, 0, 1000);

      hname="ECal1ClSAC1Cl_CosDeltaPhi";
      hSvc->BookHisto(hname, 2000, -1.1, 1.1);

      hname="ECal1ClSAC1Cl_DeltaTime";
      hSvc->BookHisto(hname, 4000, -200, 200);

      hname="PVeto_ClSize_vs_ChId";
      hSvc->BookHisto2(hname, 100, 0, 100, 10 , -0.5, 9.5);
  


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

      hname="CountEvents"; 
      hSvc->BookHisto(hname, 10, 0, 10);

 
      hSvc->BookHisto2("VetoMomentum_vs_z", 1100, -550, 550, 500, 1 , 500);

      hSvc->BookHisto2("SAC1Cl_E_vs_VetoMomentum" , 110, 0, 550, 1000, 1 , 1000);	 
      hSvc->BookHisto("SAC1Cl_Plus_VetoMomentum", 1000, 0, 1000);
      hSvc->BookHisto2("SAC1Cl_E_vs_VetoMomentum_OutOfTime3" , 110, 0, 550, 1000, 1 , 1000);	 
      hSvc->BookHisto("SAC1Cl_Plus_VetoMomentum_OutOfTime3", 1000, 0, 1000);
  

      hSvc->BookHisto2("HEPVeto_x_vs_ChId",100,0,100,1000,0,1000);
      hSvc->BookHisto2("HEPVeto_z_vs_ChId",100,0,100,1000,2000,2200);
      hSvc->BookHisto2("HEPVeto_L_vs_ChId",100,0,100,1000,-200,200);
      hSvc->BookHisto2("HEPVETO_Z_vs_X",1000,0,1000,2000,2000,2200);
      hSvc->BookHisto2("PVeto_x_vs_ChId",100,0,100,1000,-1000,1000);
      hSvc->BookHisto2("PVeto_z_vs_ChId",100,0,100,1000,-1000,1000);
      hSvc->BookHisto("HEPVeto_T_vs_PVeto",10000,-200,200 );
      hSvc->BookHisto2("HEPVetoPosZ_vs_PVetoPosZ",90, -473, 507,16, 2067.31, 2123);
      hSvc->BookHisto2("HEPVetoChId_vs_PVetoChId",100,0,100,32,0,32);

      hSvc->BookHisto2("ECal1Cl_E_vs_EVetoChId", 100,0,100,  1000,0,1000);
      hSvc->BookHisto2("ECal1Cl_E_vs_EVetoPosz", 90, -473, 507,  1000,0,1000);
      hSvc->BookHisto("ECal1Cl_EVeto_DeltaTime",4000,-200,200 );


      hSvc->BookHisto2("VETO_Z_vs_X",5000,-2500,2500,5000,-2500,2500);

      hSvc->BookHisto2("HEPVETO_COG",1000,0,1000,1000,0,1000);
      hSvc->BookHisto2("HEPVETO_ETA",16,0,16,100,-1,1);

      return true;

    }
  else if (fVersion==1)
   {
     InitHistosIO();
     hname ="nPOT";
     hSvc->BookHisto(hname, 3, -1.5, 1.5);
     hname="NposInBunch_beam";
     hSvc->BookHisto(hname, 500, 0., 30000.);
     hname="NposInBunch_beam_passDtDphiCogDsume";
     hSvc->BookHisto(hname, 500, 0., 30000.);

     hname="GammaSeleCutFlow"; 
     hSvc->BookHisto(hname, 31, -0.5, 30.5);
     hname="GammaSeleCutFlow"; 
     hSvc->BookHisto(hname, 31, -0.5, 30.5);
     hname="SelectionCutFlow"; 
     hSvc->BookHisto(hname, 31, -0.5, 30.5);
     hname="PreSelectCutFlow"; 
     hSvc->BookHisto(hname, 31, -0.5, 30.5);



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


     hname="SS2g_nhits";
     hSvc->BookHisto(hname, 100, 0., 400.);
     hname="SS2g_clSize";
     hSvc->BookHisto(hname, 31, -0.5, 30.5);
     hname="SS2g_clSizeVsE";
     hSvc->BookHisto2(hname, 100, 0., 500., 31, -0.5, 30.5);
     hname="SS2g_nclus";
     hSvc->BookHisto(hname, 50, 0.5, 50.5);
     hname="SS2g_nclus50";
     hSvc->BookHisto(hname, 10, 0.5, 10.5);
     hname="SS2g_n2gindt10";
     hSvc->BookHisto(hname, 10, 0.5, 10.5);
     hname="SS2g_n2gindt5";
     hSvc->BookHisto(hname, 10, 0.5, 10.5);
     hname="SS2g_n2gindt2_5";
     hSvc->BookHisto(hname, 10, 0.5, 10.5);
     hname="SS2g_n2gindt1";
     hSvc->BookHisto(hname, 10, 0.5, 10.5);
     hname="SS2g_n2g";
     hSvc->BookHisto(hname, 10, 0.5, 10.5);
     hname="SS2g_n2gFR";
     hSvc->BookHisto(hname, 10, 0.5, 10.5);

     hname="SS2g_ClTime";
     hSvc->BookHisto(hname, 100, -500, 500);
     hname="SS2g_ClTime_passDtDphiCogDsume";
     hSvc->BookHisto(hname, 100, -500, 500);


     hname = "SS2gSumE_inSR_Dt10";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_inSR_Dt10Cog30";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_inSR_Dphi160Cog30";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_inSR_Dt10Dphi160";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDtDphiCogFR0";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDtDphiCogFR45";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDtDphiCogFR90";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDtDphiCogFR135";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gSumE_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     nBinX=100.;
     minX=0.;
     maxX=350.;
     hname = "SS2gR_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gR_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gR_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gR_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gR_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     nBinY=100;
     minY=50.;
     maxY=450.;
     hname = "SS2gEvsR_passDtDphiCogDsume";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_passDtDphiCogFR";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_passDtDphiCog";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_passDtDphi";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_passDt";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu";             hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu_FRphi0";      hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu_FRphi45";      hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu_FRphi90";      hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu_FRphi135";      hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu_FRphi180";      hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu_FRphi225";      hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu_FRphi270";      hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsR_sClu_FRphi315";      hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);


     double pigreco = acos(-1.);
     nBinY=100;
     minY=50.;
     maxY=450.;
     nBinX=100.;
     maxX=pigreco+0.001;
     minX = -maxX;
     hname = "SS2gEvsPhi_passDtDphiCogDsume";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsPhi_passDtDphiCogFR";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsPhi_passDtDphiCog";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsPhi_passDtDphi";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEvsPhi_passDt";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);


     hname = "SS2gEVsE_inSR";
     hSvc->BookHisto2(hname, nBinY, minY, maxY, nBinY, minY, maxY);
     hname = "SS2gEVsE_passDtDphiCogDsume";
     hSvc->BookHisto2(hname, nBinY, minY, maxY, nBinY, minY, maxY);
     hname = "SS2gEVsE_passDtDphiCogFR";
     hSvc->BookHisto2(hname, nBinY, minY, maxY, nBinY, minY, maxY);
     hname = "SS2gEVsE_passDtDphiCog";
     hSvc->BookHisto2(hname, nBinY, minY, maxY, nBinY, minY, maxY);
     hname = "SS2gEVsE_passDtDphi";
     hSvc->BookHisto2(hname, nBinY, minY, maxY, nBinY, minY, maxY);
     hname = "SS2gEVsE_passDt";
     hSvc->BookHisto2(hname, nBinY, minY, maxY, nBinY, minY, maxY);     

     
     hname = "SS2gPhi_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gPhi_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gPhi_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gPhi_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gPhi_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);



     
     nBinX=  100;
     minX =  -50;
     maxX =   50.;
     hname = "SS2gDtVsSumE_inSR";
     hSvc->BookHisto2(hname, 200, 50., 850., nBinX, minX, maxX);
     hname = "SS2gDt_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gDt_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gDt_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gDt_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     
     
     nBinX=   181;
     minX =  -0.5;
     maxX =   180.5;
     hname = "SS2gDphiVsSumE_inSR";
     hSvc->BookHisto2(hname, 200, 50., 850., nBinX, minX, maxX);
     hname = "SS2gDphi_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gDphi_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gDphi_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gDphi_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gDphi_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     nBinX=   100;
     minX =    50;
     maxX =   450.;
     hname = "SS2gE_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gE_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gE_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gE_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gE_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     nBinX=   50;
     minX =  -400;
     maxX =   400.;
     minY =  -400;
     maxY =   400.;
     hname = "SS2gX_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gX_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gX_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gX_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gX_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gY_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gY_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gY_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gY_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gY_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     hname = "SS2gXEw_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gXEw_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gXEw_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gXEw_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gXEw_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gYEw_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gYEw_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gYEw_passDtDphiCog";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gYEw_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gYEw_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     nBinX=   100;
     nBinY=   100;
     hname = "SS2gXY_passDt";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXY_passDtDphi";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXY_passDtDphiCog";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXY_passDtDphiCogFR";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXY_passDtDphiCogDsume";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXYEw_passDtDphiCogDsume";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXYEw_passDtDphiCogFR";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXYEw_passDtDphiCog";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXYEw_passDtDphi";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXYEw_passDt";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gXY_inSR";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
     hname = "SS2gEwXY_inSR";
     hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);

     
     nBinX=  100;
     minX =  -200;
     maxX =   200.;
     hname = "SS2gXcogVsSumE_inSR";
     hSvc->BookHisto2(hname, 200, 50., 850., nBinX, minX, maxX);
     hname = "SS2gXcog_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gXcog_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gXcog_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gXcog_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     hname = "SS2gYcogVsSumE_inSR";
     hSvc->BookHisto2(hname, 200, 50., 850., nBinX, minX, maxX);
     hname = "SS2gYcog_passDtDphiCogDsume";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gYcog_passDtDphiCogFR";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gYcog_passDtDphi";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "SS2gYcog_passDt";
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     hname = "ECal2gsearch_ESumDt3";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt3Phi100";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt3Phi75";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt3Phi50";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt3Phi25";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt2";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt2Phi100";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt2Phi75";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt2Phi50";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt2Phi25";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt1";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt1Phi100";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt1Phi75";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt1Phi50";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "ECal2gsearch_ESumDt1Phi25";
     hSvc->BookHisto(hname, nBinX, minX, maxX);


     hname = "ECal2gsearch_MinvDt3";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt3Phi100";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt3Phi75";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt3Phi50";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt3Phi25";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt2";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt2Phi100";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt2Phi75";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt2Phi50";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt2Phi25";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt1";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt1Phi100";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt1Phi75";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt1Phi50";
     hSvc->BookHisto(hname, 200, 0., 50.);
     hname = "ECal2gsearch_MinvDt1Phi25";
     hSvc->BookHisto(hname, 200, 0., 50.);


     hname = "ECal2gsearch_MinvVsDPhiDt3";
     hSvc->BookHisto2(hname, 200, 0., 3.2, 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsDPhiDt2";
     hSvc->BookHisto2(hname, 200, 0., 3.2, 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsDPhiDt1";
     hSvc->BookHisto2(hname, 200, 0., 3.2, 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt3";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt3Phi100";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt3Phi75";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt3Phi50";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt3Phi25";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt2";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt2Phi100";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt2Phi75";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt2Phi50";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt2Phi25";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt1";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt1Phi100";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt1Phi75";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt1Phi50";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);
     hname = "ECal2gsearch_MinvVsRcogDt1Phi25";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, 0., 50.);


  
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

     hname = "ECal2gsearch_dPhiVsDt";
     hSvc->BookHisto2(hname, 200, -100., 100., 200, 0., 3.2);
     hname = "ECal2gsearch_dRVsDt";
     hSvc->BookHisto2(hname, 200, -100., 100., 200, 0., 500.);
     hname = "ECal2gsearchDt3_ImpactParVsDPhi";
     hSvc->BookHisto2(hname, 200, 0., 3.2, 200, -300., 300.);
     hname = "ECal2gsearchDt3_ImpactParVsDR";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, -300., 300.);
     hname = "ECal2gsearchDt3_ImpactParVsMinv";
     hSvc->BookHisto2(hname, 200, 0., 50., 200, -300., 300.);
     hname = "ECal2gsearchDt3_ImpactParVsRcog";
     hSvc->BookHisto2(hname, 200, 0., 300., 200, -300., 300.);
     hname = "ECal2gsearchDt3_ESumVsMinv";
     hSvc->BookHisto2(hname, 200, 0., 50., 200, 0., 1500.);
     hname = "ECal2gsearchDt3_ESumVsMinv_phibcut";
     hSvc->BookHisto2(hname, 200, 0., 50., 200, 0., 1500.);
     hname = "ECal2gsearchDt3_ImpactParVsDPhi_phibcut";
     hSvc->BookHisto2(hname,  200, 0., 3.2, 200, -300., 300.);
     hname = "ECal2gsearch_ImpactParVsDt";
     hSvc->BookHisto2(hname, 200, -100., 100., 200, -300., 300.);
     hname = "ECal2gsearchDt3_ImpactPar";
     hSvc->BookHisto(hname, 200, -300., 300.);


  
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
     nBinX=  600;
     minX =  0.;
     maxX =  3000.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname="energySumECalClus_1Cl";
     nBinX=  600;
     minX =  0.;
     maxX =  3000.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname="energySumECalClus_2Cl";
     nBinX=  600;
     minX =  0.;
     maxX =  3000.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname="energySumECalClus_3Cl";
     nBinX=  600;
     minX =  0.;
     maxX =  3000.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname="energySumECalClus_NCl";
     nBinX=  600;
     minX =  0.;
     maxX =  3000.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname="TimeSpreadInECalClus";
     nBinX=  200;
     minX =  -30.;
     maxX =   30.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname="TimeSpreadInECal";
     nBinX=  200;
     minX =  -300.;
     maxX =   300.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname="CellSpreadInECal";
     nBinX=  30;
     minX =   0.;
     maxX = 300.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);

     hname = "DR2gammaIn10";
     nBinX=  100;
     minX =  0.;
     maxX =  600.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "DR2gammaIn10PhiSym";
     nBinX=  100;
     minX =  0.;
     maxX =  600.;
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "DR2gammaIn10PhiSymMaxESum";
     hSvc->BookHisto(hname, nBinX, minX, maxX);
     hname = "DPhi2gammaIn10";
     minX =   0.;
     maxX =   3.2;
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

   }
  else {std::cout<<" EventSelection: version not set ....................... choose 1 for ProcessAnalysisSS / 2 for ProcessAnalysisGC"<<std::endl;}
  return true;
}

Double_t EventSelection::applyTimePVetoLinCorr(Double_t  xChId, Double_t xTime)
{
  return xTime - (32.9 + 9.*(xChId - 13.)/65.);
}

Double_t EventSelection::InvariantDECal(TVector3 V1, TVector3 V2, Double_t E1, Double_t E2){

 
  
  float_t x1 = V1.x() ;
  float_t x2 = V2.x() ;

  float_t y1 = V1.y() ;
  float_t y2 = V2.y() ;
  
  
  //float_t z1  =   V1.z()+915 ;
  //float_t z2  =   V2.z()+915 ;
   
  //float_t r1  = sqrt( x1*x1 + y1*y1 + z1*z1 ) ; 
  //float_t r2  = sqrt( x2*x2 + y2*y2 + z2*z2 ) ; 
  float_t dr  = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1); dr=sqrt(dr); 
  
  /*
  float_t Px1 = 0; if (r1 != 0) Px1 = E1 * x1 / r1 ;
  float_t Px2 = 0; if (r2 != 0) Px2 = E2 * x2 / r2 ;
  
  float_t Py1 = 0; if (r1 != 0) Py1 = E1 * y1 / r1 ;
  float_t Py2 = 0; if (r2 != 0) Py2 = E2 * y2 / r2 ;

  float_t Pz1 = 0; if (r1 != 0) Pz1 = E1 *  z1 / r1 ;
  float_t Pz2 = 0; if (r2 != 0) Pz2 = E2 *  z2 / r2 ; 
  */

  float_t Dx = 0; if(E1+E2)Dx  = (x1*E1 + x2*E2)/(E1+E2) ;
  float_t Dy = 0; if(E1+E2)Dy  = (y1*E1 + y2*E2)/(E1+E2) ;
  float_t D = sqrt(Dx*Dx+Dy*Dy);
  
  

  return Double_t (D) ;

}
Double_t EventSelection::InvariantMassECal(TVector3 V1, TVector3 V2, Double_t E1, Double_t E2){

 
  
  float_t x1 = V1.x() ;
  float_t x2 = V2.x() ;

  float_t y1 = V1.y() ;
  float_t y2 = V2.y() ;
  
  float_t z1  =   V1.z()+915 ;
  float_t z2  =   V2.z()+915 ;
   
  float_t r1  = sqrt( x1*x1 + y1*y1 + z1*z1 ) ; 
  float_t r2  = sqrt( x2*x2 + y2*y2 + z2*z2 ) ; 
  float_t dr  = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1); dr=sqrt(dr); 
  
  float_t Px1 = 0; if (r1 != 0) Px1 = E1 * x1 / r1 ;
  float_t Px2 = 0; if (r2 != 0) Px2 = E2 * x2 / r2 ;
  
  float_t Py1 = 0; if (r1 != 0) Py1 = E1 * y1 / r1 ;
  float_t Py2 = 0; if (r2 != 0) Py2 = E2 * y2 / r2 ;

  float_t Pz1 = 0; if (r1 != 0) Pz1 = E1 *  z1 / r1 ;
  float_t Pz2 = 0; if (r2 != 0) Pz2 = E2 *  z2 / r2 ; 

  float_t Minv = ( E1 + E2 ) * ( E1 + E2 ) - ( Px1 + Px2 ) * ( Px1 + Px2 ) - ( Py1 + Py2 ) * ( Py1 + Py2 ) - ( Pz1 + Pz2 ) * ( Pz1 + Pz2 );
  
  if( Minv > 0 ) { Minv = sqrt(Minv);} 

  //cout << Minv << " "<<  sqrt(E1*E2)*dr/347  <<endl;
  

  return Double_t (Minv) ;

}
Double_t  EventSelection::VetoMomentum(Double_t z){

 z= z ;
 //return ((z-14.6*0)*(z-14.6*0)+20*20)/2/20*0.3*0.5;

 return ((z+637)*(z+637)+200*200)/2/200*0.3*0.5;
}
Double_t EventSelection::PhotonRadius(TVector3 V){
 
  Double_t x = V.x();Double_t y = V.y();//Double_t z = V.z();
  Double_t radius  = sqrt(x*x+y*y);
  

 return     radius;
}

Bool_t EventSelection::Phi1Phi2ECal(TVector3 V1, TVector3 V2, double& Phi1, double& Phi2){

 
  float_t x1 = V1.x() ;
  float_t x2 = V2.x() ;

  float_t y1 = V1.y() ;
  float_t y2 = V2.y() ;
  
  float_t d1 = sqrt(x1*x1+y1*y1);
  float_t d2 = sqrt(x2*x2+y2*y2);
  
  Phi1 = 0 ; 
  if(d1) Phi1 = asin( y1 / d1 ) ;
  Phi2 = 0 ; 
  if(d2) Phi2 = asin( y2 / d2 ) ;
  

  return  true ;

}
Double_t EventSelection::CosDeltaThetaECal(TVector3 V1, TVector3 V2){

 
  float_t x1 = V1.x() ;
  float_t x2 = V2.x() ;

  float_t y1 = V1.y() ;
  float_t y2 = V2.y() ;

  float_t z1 = V1.z() + 915;
  float_t z2 = V2.z() + 915;

   
  
  float_t r1 = sqrt(x1*x1+y1*y1+z1*z1);
  float_t r2 = sqrt(x2*x2+y2*y2+z2*z2);
  
  Double_t CosDeltaTheta = 0 ; if(r1*r2!=0) CosDeltaTheta = ( x1 * x2 + y1 * y2 + z1 * z2) / ( r1 * r2 ) ;
  

  return  CosDeltaTheta ;

}
Double_t EventSelection::MissingMass(TVector3 V, Double_t E){
 double PositronE  = 490 + 0.511;
 double PositronPz = sqrt(PositronE*PositronE-0.511*0.511);
 TLorentzVector Positron (0,0,PositronPz,PositronE);
 TLorentzVector Electron (0,0,         0,    0.511);
 Double_t x = V.x();Double_t y = V.y();Double_t z = V.z();
 Double_t r = sqrt(x*x+y*y+z*z);
 double Px = E*x/r;
 double Py = E*y/r;
 double Pz = E*z/r;
 TLorentzVector Photon   (Px,Py,Pz,E);
 TLorentzVector Missing  (0,0,0,0); 
 Missing  = Positron + Electron - Photon;

 return    Missing*Missing;
}
Double_t EventSelection::CosDeltaPhiECal(TVector3 V1, TVector3 V2){

 
  float_t x1 = V1.x() ;
  float_t x2 = V2.x() ;

  float_t y1 = V1.y() ;
  float_t y2 = V2.y() ;
  
  float_t d1 = sqrt(x1*x1+y1*y1);
  float_t d2 = sqrt(x2*x2+y2*y2);
  
  Double_t CosDeltaPhi = 0 ; if(d1*d2!=0) CosDeltaPhi = ( x1 * x2 + y1 * y2 ) / ( d1 * d2 ) ;
  

  return  CosDeltaPhi ;

}

void EventSelection::SetCalibTimePVeto  (Bool_t isMC)
{
  if (isMC)
    {
      for(int i=0;i!=16;i++)fTimeOffsetPVeto[i] = -3.;
    }
  else // DATA
    {
      for(int i=0;i!=16;i++)fTimeOffsetPVeto[i] = 0.;
    }
  
  return;
}
void EventSelection::SetCalibTimeEVeto  (Bool_t isMC)
{
  if (isMC)
    {
      for(int i=0;i!=16;i++)fTimeOffsetEVeto[i] = 0;
    }
  else // DATA
    {
      for(int i=0;i!=16;i++)fTimeOffsetEVeto[i] = 0.;
    }
  
  return;
}
void EventSelection::SetCalibTimeHEPVeto(Bool_t isMC)
{
  if (isMC)
    {
      for(int i=0;i!=16;i++)fTimeOffsetHEPVeto[i] = 8.5+2.64-5.27;
    }
  else // DATA
    {
      for(int i=0;i!=16;i++)fTimeOffsetHEPVeto[i] = 0.;
    }
  
  return;
}
void EventSelection::SetCalibTimeSAC    (Bool_t isMC)
{
    if (isMC)
    {
      for(int i=0;i!=16;i++)fTimeOffsetSAC[i] = 8.5;
    }
  else // DATA
    {
      for(int i=0;i!=16;i++)fTimeOffsetSAC[i] = 0.;
    }
  
  return;
}
void EventSelection::SetCalibTimeECal   (Bool_t isMC)
{
  if (isMC)
    {
      for(int i=0;i!=3000;i++)fTimeOffsetECal[i]=6.7;
    }
  else
    {
      for(int i=0;i!=3000;i++)fTimeOffsetECal[i]=22.74*0;
    }
}

void EventSelection::ApplyCalibTimePVeto  ()
{
  TRecoVCluster* xClu;
  Int_t    xChId;
  Double_t xTime;
  for (int h=0; h<fPVeto_ClColl->GetNElements(); ++h)
    {
      xClu = fPVeto_ClColl->Element((int)h);
      if (xClu) 
	{
	  xChId = xClu->GetChannelId();
	  xTime = xClu->     GetTime();
	  xClu  ->SetTime(xTime-fTimeOffsetPVeto[xChId]);
	}
    }
  return;
}
void EventSelection::ApplyCalibTimeEVeto  ()
{
  TRecoVCluster* xClu;
  Int_t    xChId;
  Double_t xTime;
  for (int h=0; h<fEVeto_ClColl->GetNElements(); ++h)
    {
      xClu = fEVeto_ClColl->Element((int)h);
      if (xClu) 
	{
	  xChId = xClu->GetChannelId();
	  xTime = xClu->     GetTime();
	  xClu  ->SetTime(xTime-fTimeOffsetEVeto[xChId]);
	}
    }
  return;
}
void EventSelection::ApplyCalibTimeHEPVeto()
{
  TRecoVCluster* xClu;
  Int_t    xChId;
  Double_t xTime;
  for (int h=0; h<fHEPVeto_ClColl->GetNElements(); ++h)
    {
      xClu = fHEPVeto_ClColl->Element((int)h);
      if (xClu) 
	{
	  xChId = xClu->GetChannelId();
	  xTime = xClu->     GetTime();
	  xClu  ->SetTime(xTime-fTimeOffsetHEPVeto[xChId]);
	}
    }
  return;

}
void EventSelection::ApplyCalibTimeSAC    ()
{
  TRecoVCluster* xClu;
  Int_t    xChId;
  Double_t xTime;
  for (int h=0; h<fSAC_ClColl->GetNElements(); ++h)
    {
      xClu = fSAC_ClColl->Element((int)h);
      if (xClu) 
	{
	  xChId = xClu->GetChannelId();
	  xTime = xClu->     GetTime();
	  xClu  ->SetTime(xTime-fTimeOffsetSAC[xChId]);
	}
    }
  return;
}
void EventSelection::ApplyCalibTimeECal   ()
{
  TRecoVCluster* xClu;
  Int_t    xChId;
  Double_t xTime;
  for (int h=0; h<fECal_ClColl->GetNElements(); ++h)
    {
      xClu = fECal_ClColl->Element((int)h);
      if (xClu) 
	{
	  xChId = xClu->GetChannelId();
	  xTime = xClu->     GetTime();
	  xClu  ->SetTime(xTime-fTimeOffsetECal[xChId]);
	}
    }
  return;

}
void EventSelection::ApplyCalibTimeEnergyECal   (Bool_t isMC)
{
  TRecoVCluster* xClu;
  Int_t    xChId;
  Double_t xTime;
  for (int h=0; h<fECal_ClColl->GetNElements(); ++h)
    {
      xClu = fECal_ClColl->Element((int)h);
      if (xClu) 
	{
	  xChId = xClu->GetChannelId();
	  xTime = xClu->     GetTime();
	  //xClu  ->SetTime(xTime-fTimeOffsetECal[xChId]);
	  if (!isMC) xClu  ->SetEnergy(1.084*(xClu->GetEnergy()));
	}
    }
  return;

}

Bool_t EventSelection::passPreselection()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  
  Bool_t passed = false;
  std::string hname="PreSelectCutFlow";
  hSvc->FillHisto(hname,ps_cut_all);

  if (!fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) 
    {
      // std::cout<<"in passPreselection, event status bit  "<< fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<< std::endl;
      if (!fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_BEAM)) return passed;
    }
  hSvc->FillHisto(hname,ps_cut_trg);
   
  std::string hname1 = "NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT()*1.78);
  //std::cout<<"preselection...Pot control" << std::endl;
  //if (fTarget_RecoBeam->getnPOT()<12000. && fTarget_RecoBeam->getnPOT()>3500.) return passed;
  //if (fTarget_RecoBeam->getnPOT()<10000.) return passed;
  hSvc->FillHisto(hname,ps_cut_POT);
  if (fTarget_RecoBeam->getnPOT()<-9999) return passed;

  hname = "nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT()*1.78)); 

  //std::cout<<"passed ok " << std::endl;
  passed = true;
  return passed;
}




Bool_t EventSelection::ProcessAnalysisIO()
{

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
 
  Bool_t isMC=false;
  //std::cout<<"in ProcessAnalysis ... evsel ... "<<fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<std::endl;
  if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //std::cout<<"input data are simulatetd "<<std::endl;
  }
  isMC=false;
 
  // apply BTF bunch preselection 
  if (!passPreselection()) return true;



  TRecoVHit* hit=NULL;
  Int_t fNhits = fECal_hitEvent->GetNHits();
  Double_t eTot  =  0.;
  for (Int_t i=0; i<fNhits; ++i){
    hit = fECal_hitEvent->Hit(i);
    Double_t energy = hit->GetEnergy();
    hname = "ECal_HitEnergy";
    hSvc->FillHisto(hname, energy, 1.);
    eTot += energy;
  }
   hname="ECal_EHitTot";
   hSvc->FillHisto(hname, eTot, 1.);

   TRecoVCluster* clu=NULL;
   Int_t seed;
   Int_t seedId;
   Double_t seedE;
   Double_t seedT;
   Double_t eTotCl;
   Int_t clSize;
   Int_t fNclus = fECal_ClColl->GetNElements();
   hname = "ECal_NCluster";
   hSvc->FillHisto(hname, fNclus,1.);

   for (Int_t i=0; i<fNclus; ++i){
     clu    = fECal_ClColl->Element(i);
     seed   = clu->GetSeed();
     seedId = clu->GetChannelId();
     seedE  = clu->GetEnergy();
     eTotCl += seedE;
     hname = "ECal_ClusterEnergy";
     hSvc->FillHisto(hname, clu->GetEnergy(), 1.);

     seedT  = clu->GetTime();
     clSize = clu->GetNHitsInClus();
     hname = "ECal_NHitsInClus";
     hSvc->FillHisto(hname, clSize,1.);
   
    }
   hname="ECal_EClTot";
   hSvc->FillHisto(hname, eTotCl, 1.);



  Bool_t retCode = 0;
  TRecoVCluster* ecalclu=NULL;
  TRecoVCluster* ecalclu2=NULL;
  TRecoVCluster* SACclu=NULL;
  TRecoVHit* sacHit;
  //TRecoVHit* hit;
  TRecoVHit* hit2;
  TRecoVHit* targetHit;
  TRecoVCluster* SACclu2=NULL;
  TRecoVCluster* PVetoclu=NULL;
  TRecoVCluster* EVetoclu=NULL;
  TRecoVHit* pVetoHit;
  int loop=0;
  
 
  Int_t NClSAC = fSAC_ClColl->GetNElements();
  Int_t NClECal = fECal_ClColl->GetNElements();
  Int_t NClPVeto = fPVeto_ClColl->GetNElements();
  Int_t NClEVeto = fEVeto_ClColl->GetNElements();
  Int_t NHitsSac = fSAC_hitEvent->GetNHits();
  Int_t NHitsPVeto = fPVeto_hitEvent->GetNHits();
  Int_t NHitsEVeto = fEVeto_hitEvent->GetNHits();

  for(int jpveto=0; jpveto<NClPVeto; jpveto++){
    PVetoclu=fPVeto_ClColl->Element(jpveto);
    for(int jsac=0; jsac<NClSAC; jsac++){
      SACclu=fSAC_ClColl->Element(jsac);
      if(SACclu->GetChannelId()!=22) continue;
      hname="DTimePVetoSACCh22";
      hSvc->FillHisto(hname, (PVetoclu->GetTime()-SACclu->GetTime()), 1.);
      
    }
  }
  for(int jecal=0; jecal<NClECal; jecal++){
    ecalclu=fECal_ClColl->Element(jecal);
    for(int jsac=0; jsac<NClSAC; jsac++){
      SACclu=fSAC_ClColl->Element(jsac);
      if(SACclu->GetChannelId()!=22) continue;
      hname="DTimeECAlSACCh22";
      hSvc->FillHisto(hname, (ecalclu->GetTime()-SACclu->GetTime()), 1.);
      
    }
  }
  for(int jecal=0; jecal<NClECal; jecal++){
    ecalclu=fECal_ClColl->Element(jecal);
    for(int jpveto=0; jpveto<NClPVeto; jpveto++){
      PVetoclu=fPVeto_ClColl->Element(jpveto);
      if(PVetoclu->GetChannelId()!=41) continue;
      hname="DTimeECAlPVetoCh41";
      hSvc->FillHisto(hname, (ecalclu->GetTime()-PVetoclu->GetTime()), 1.);
      
    }
  }

  for(int jecal=0; jecal<NClECal; jecal++){
    ecalclu=fECal_ClColl->Element(jecal);
    for(int jeveto=0; jeveto<NClEVeto; jeveto++){
      EVetoclu=fEVeto_ClColl->Element(jeveto);
      if(EVetoclu->GetChannelId()!=41) continue;
      hname="DTimeECAlEVetoCh41";
      hSvc->FillHisto(hname, (ecalclu->GetTime()-EVetoclu->GetTime()), 1.);
      
    }
  }

  Double_t ShiftPVetoSac;
  Double_t ShiftECalSac;
  Double_t ShiftECalPVeto;
  Double_t ShiftECalEVeto;
  if(isMC){
    ShiftPVetoSac=0;
    ShiftECalSac=0;
    ShiftECalPVeto=0;
    ShiftECalEVeto=0;
  }
  else{
    ShiftPVetoSac=0;
    ShiftECalSac=0;
    ShiftECalPVeto=0;
    ShiftECalEVeto=1;
  }
  
  TVector3 pos1;
  TVector3 pos2;
  double dPhi = -999.;
  for (Int_t jecal=0; jecal<NClECal; ++jecal){
    ecalclu          = fECal_ClColl->Element(jecal);
    Double_t ClTECal = ecalclu->GetTime();
    Double_t EnClECal= ecalclu->GetEnergy();
    Int_t NGGInTheSameEventTime3ns=0;
    Int_t NGGInTheSameEventDeltaPhi10_3ns=0;
    Int_t NGGInTheSameEventDeltaPhi10_EnergyInterval_InvariantMassInterval_3ns=0;
    Int_t NGGInTheSameEventCoG2cm3ns=0;
    Double_t g1x=ecalclu->GetPosition().X();
    Double_t g1y=ecalclu->GetPosition().Y();
    Double_t g1E=ecalclu->GetEnergy();
    Double_t R_1 = sqrt(g1x*g1x+ g1y*g1y+3470*3470);
    Double_t Px_1 = g1E*g1x/ R_1;
    Double_t Py_1 = g1E*g1y/ R_1;
    Double_t Pz_1 = g1E*3470/ R_1;
    TLorentzVector P4g1F, P4eTarget, P4eBeam;
    P4g1F.SetPxPyPzE(Px_1,Py_1,Pz_1, g1E);
    Double_t EBeam=490.0;
    Double_t me=0.511;
    Double_t PzBeam=sqrt(EBeam*EBeam-me*me);
    P4eTarget.SetPxPyPzE(0,0,0, me);
    P4eBeam.SetPxPyPzE(0,0,PzBeam, EBeam);
    Double_t MissingMass=(P4eTarget+P4eBeam-P4g1F)*(P4eTarget+P4eBeam-P4g1F);
    hname="MissingMass_AllECALclu";
    hSvc->FillHisto(hname,MissingMass , 1.);
    Bool_t PassEneThr=false;
    if(g1E>50){
      PassEneThr=true;
      hname="MissingMass_AllECALclu_ThrEne50MeV";
      hSvc->FillHisto(hname,MissingMass , 1.);
    }
    Bool_t NoInTime10=true;
    Bool_t NoInTime20=true;
    for(Int_t jecal2=jecal+1; jecal2<NClECal; jecal2++){
      ecalclu2 = fECal_ClColl->Element(jecal2);
     
      if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<20.){
	NoInTime20=false;
      }
      if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<10.){
	NoInTime10=false;
      }
      if(fabs(ecalclu->GetTime()-ecalclu2->GetTime())<3.){
        NGGInTheSameEventTime3ns++;
        hname="ECAL_twoPhotonInTime3ns";
        hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
        pos1 = ecalclu->GetPosition();
        pos2 = ecalclu2->GetPosition();
        Double_t XWeighted=0.;
        Double_t YWeighted=0.;
        Double_t g2x=ecalclu2->GetPosition().X();
        Double_t g2y=ecalclu2->GetPosition().Y();
        Double_t g2E=ecalclu2->GetEnergy();
        XWeighted= (g1x*g1E+g2x*g2E)/(g1E+g2E);
        YWeighted= (g1y*g1E+g2y*g2E)/(g1E+g2E);
	Bool_t inFR1= InGoodRegionECal(ecalclu->GetChannelId()/100,ecalclu->GetChannelId()%100);
	Bool_t inFR2= InGoodRegionECal(ecalclu2->GetChannelId()/100,ecalclu2->GetChannelId()%100);
	Bool_t inFR=false;
	if(inFR1 && inFR2)inFR=true;
	if(inFR){
	  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion";
	  hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	}
        hname="ECal_xCoG_3ns";
        hSvc->FillHisto(hname,XWeighted , 1.);
        hname="ECal_yCoG_3ns";
        hSvc->FillHisto(hname,YWeighted , 1.);
        hname="ECal_y:xCog_3ns";
        hSvc->FillHisto2(hname, XWeighted, YWeighted, 1.);
        
        
        Double_t CoG= sqrt(XWeighted*XWeighted+YWeighted*YWeighted);
        Double_t R_2 = sqrt(g2x*g2x+ g2y*g2y+3470*3470);
        Double_t Px_2 = g2E*g2x/ R_2;
        Double_t Py_2 = g2E*g2y/ R_2;
        Double_t Pz_2 = g2E*3470/ R_2;
         
        TLorentzVector P4g2F,SumP;
        P4g2F.SetPxPyPzE(Px_2,Py_2,Pz_2,g2E);
        SumP=P4g1F+P4g2F;
        Double_t InvariantMass = SumP.M();
        Double_t angleBetweenTwoPhoton = P4g1F.Angle(P4g2F.Vect());
         
        Double_t ImpactParameter = fabs(ecalclu->GetPosition().Y()-ecalclu->GetPosition().X()*((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X())))/sqrt(1+((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X()))*((ecalclu2->GetPosition().Y()-ecalclu->GetPosition().Y())/(ecalclu2->GetPosition().X()-ecalclu->GetPosition().X())));
        hname="ECAL_ImpactParameter_3ns";
        hSvc->FillHisto(hname, ImpactParameter, 1.);

        Double_t thetag1 = P4g1F.Theta();
        Double_t thetag2 = P4g2F.Theta(); 
        Double_t phig1 = P4g1F.Phi();
        Double_t phig2 = P4g2F.Phi();
        
        Bool_t InInvariantMassRange = false;
        if(InvariantMass>12. && InvariantMass< 30) InInvariantMassRange=true;
        
        Bool_t InFiducialRegion=false;
        if((fabs((g1x))>90. || fabs(g1y)>90.) && (fabs((g2x))>90. || fabs(g2y)>90. )) InFiducialRegion=true;
        
        Bool_t InEnergy=false;
        if(g1E>70 && g2E>70) InEnergy=true;
        
        Bool_t TotEnergyCut=false;
        if(g1E+g2E>300 && g1E+g2E<700 ) TotEnergyCut=true;
        
        Bool_t InDeltaPhi = false;
        Double_t distance = sqrt((g1x-g2x)*(g1x-g2x)+(g1y-g2y)*(g1y-g2y));
        if(/*distance >R_1 &&*/ (phig2*phig1<0 && (fabs(phig1-phig2)-3.14)<0.1) || (phig2*phig1>0 && (fabs(phig1+phig2)-3.14)< 0.1)) InDeltaPhi= true;
        //std::cout<< "DPhi " << fabs(phig1-phig2) << " bool "<< InDeltaPhi << std::endl;
        
        bool isPhySym = phiSymmetricalInECal(pos1, pos2, dPhi);
        Bool_t dPhiIn20Degree=false; //0.35 rad
        Bool_t dPhiIn10Degree=false; //0.17 rad
        Bool_t dPhiIn5Degree=false;  //0.087 rad
        if(fabs(dPhi-3.14)<0.35) dPhiIn20Degree=true;
        if(fabs(dPhi-3.14)<0.17) dPhiIn10Degree=true;
        if(fabs(dPhi-3.14)<0.087) dPhiIn5Degree=true;
        
        if (TotEnergyCut && InInvariantMassRange && dPhiIn10Degree){
        NGGInTheSameEventDeltaPhi10_EnergyInterval_InvariantMassInterval_3ns ++;
        }
        
        
        if(dPhiIn20Degree){
          HistoInDeltaPhi20DegreeSecondMethod3ns(g1E, g2E, InvariantMass);
	  if(inFR){
	    hname="ECAL_twoPhotonInTime3ns_InFiducialRegion20Degree";
	    hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	  }
	  hname="MAPgg_3ns20Degree";
	  hSvc->FillHisto2(hname,g1x, g1y);
	  hSvc->FillHisto2(hname,g2x, g2y);
	  hname="MAPggW_3ns20Degree";
	  hSvc->FillHisto2(hname,g1x, g1y,g1E);
	  hSvc->FillHisto2(hname,g2x, g2y,g2E);
	  if(InEnergy){
	    hname="MAPgg_3ns20Degree_thrg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y);
	    hSvc->FillHisto2(hname,g2x, g2y);
	    hname="MAPggW_3ns20Degree_thrg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y,g1E);
	    hSvc->FillHisto2(hname,g2x, g2y,g2E);
	  }
	  if(TotEnergyCut){
	     hname="MAPgg_3ns20Degree_thrTotg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y);
	    hSvc->FillHisto2(hname,g2x, g2y);
	    hname="MAPggW_3ns20Degree_thrTotg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y,g1E);
	    hSvc->FillHisto2(hname,g2x, g2y,g2E);
	  }
	  if(fabs(g1y)>fabs(g2y)){
	    hname="CoGvsMaxY_3ns20Degree";
	    hSvc->FillHisto2(hname,CoG,fabs(g1y));
	  }
	  else{
	    hname="CoGvsMaxY_3ns20Degree";
	    hSvc->FillHisto2(hname,CoG,fabs(g2y));
	  }
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           HistoInDeltaPhi20DegreeSecondMethod3nsDeltaTheta(g1E, g2E, InvariantMass);
           HistoDeltaTheta_InDeltaPhi20Interval(angleBetweenTwoPhoton);
	   if(inFR){
	     hname="ECAL_twoPhotonInTime3ns_InFiducialRegion20DegreeDeltaTheta";
	     hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	   }
           
         }
        }
        if(dPhiIn10Degree){
          NGGInTheSameEventDeltaPhi10_3ns++;
          HistoInDeltaPhi10DegreeSecondMethod3ns(g1E, g2E, InvariantMass);
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           HistoInDeltaPhi10DegreeSecondMethod3nsDeltaTheta(g1E, g2E, InvariantMass);
           
          }
          if(InEnergy){
           HistoInDeltaPhi10DegreeSecondMethod3nsThrEnergy(g1E, g2E, InvariantMass);
          }
        }
        
        if(dPhiIn5Degree){
          HistoInDeltaPhi5DegreeSecondMethod3ns(g1E, g2E, InvariantMass);
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           HistoInDeltaPhi5DegreeSecondMethod3nsDeltaTheta(g1E, g2E, InvariantMass);
           
         }
        }
        
        if(InDeltaPhi){                                             //there is an error maybe in this calculation
          HistoInDeltaPhi3ns(g1E, g2E, InvariantMass);
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
            HistoInDeltaPhi3nsDeltaTheta(g1E, g2E, InvariantMass);
          }
        }
        if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
         HistoDeltaPhi_InDeltaThetaInterval(dPhi);
        }
        if(fabs(XWeighted)<10. && fabs(YWeighted)<10.){
         HistoIn1cmCoG3ns(g1E,  g2E,  InvariantMass, XWeighted, YWeighted, angleBetweenTwoPhoton, phig1, phig2, thetag1, thetag2, g1x, g2x, g1y, g2y, CoG);
	 if(inFR){
	   hname="ECAL_twoPhotonInTime3ns_InFiducialRegion1CoG";
	   hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	 }
         if(dPhiIn10Degree){
          HistoIn1cmCoG3nsDeltaPhi(g1E, g2E, InvariantMass);
	  if(inFR){
	    hname="ECAL_twoPhotonInTime3ns_InFiducialRegion1CoG10Degree";
	    hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	  }
         }
         if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
           HistoIn1cmCoG3nsDeltaTheta(g1E, g2E, InvariantMass, InDeltaPhi);
           
         }
         if(InFiducialRegion){
            HistoIn1cmCoG3nsInFiducialRegion(g1E, g2E, InvariantMass, CoG);
         }
          
          
         if(InInvariantMassRange){
            HistoIn1cmCoG3nsInvariantMassRange( g1x, g2x, g1y, g2y, g1E, g2E, thetag1, thetag2, phig1, phig2);           
         }
        }
        
        
        if(fabs(XWeighted)<20. && fabs(YWeighted)<20.){
          HistoIn2cmCoG3ns(g1E, g2E, InvariantMass, angleBetweenTwoPhoton);
	  if(inFR){
	    hname="ECAL_twoPhotonInTime3ns_InFiducialRegion2CoG";
	    hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	  }
          NGGInTheSameEventCoG2cm3ns++;
	  hname="MAPgg_3ns2CoG";
	  hSvc->FillHisto2(hname,g1x, g1y);
	  hSvc->FillHisto2(hname,g2x, g2y);
	  hname="MAPggW_3ns2CoG";
	  hSvc->FillHisto2(hname,g1x, g1y,g1E);
	  hSvc->FillHisto2(hname,g2x, g2y,g2E);
	  if(InEnergy){
	    hname="MAPgg_3ns2CoG_thrg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y);
	    hSvc->FillHisto2(hname,g2x, g2y);
	    hname="MAPggW_3ns2CoG_thrg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y,g1E);
	    hSvc->FillHisto2(hname,g2x, g2y,g2E);
	  }
	  if(TotEnergyCut){
	     hname="MAPgg_3ns2CoG_thrTotg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y);
	    hSvc->FillHisto2(hname,g2x, g2y);
	    hname="MAPggW_3ns2CoG_thrTotg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y,g1E);
	    hSvc->FillHisto2(hname,g2x, g2y,g2E);
	  }
	  if(fabs(g1y)>fabs(g2y)){
	    hname="CoGvsMaxY_3ns2CoG";
	    hSvc->FillHisto2(hname,CoG,fabs(g1y));
	  }
	  else{
	    hname="CoGvsMaxY_3ns2CoG";
	    hSvc->FillHisto2(hname,CoG,fabs(g2y));
	  }
	  if(dPhiIn20Degree){
	    if(inFR){
	      hname="ECAL_twoPhotonInTime3ns_InFiducialRegion2CoG20Degree";
	      hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	    }
	   hname="MAPgg_3ns2CoG20Degree";
	   hSvc->FillHisto2(hname,g1x, g1y);
	   hSvc->FillHisto2(hname,g2x, g2y);
	   hname="MAPggW_3ns2CoG20Degree";
	   hSvc->FillHisto2(hname,g1x, g1y,g1E);
	   hSvc->FillHisto2(hname,g2x, g2y,g2E);
	   if(InEnergy){
	    hname="MAPgg_3ns2CoG20Degree_thrg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y);
	    hSvc->FillHisto2(hname,g2x, g2y);
	    hname="MAPggW_3ns2CoG20Degree_thrg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y,g1E);
	    hSvc->FillHisto2(hname,g2x, g2y,g2E);
	  }
	  if(TotEnergyCut){
	     hname="MAPgg_3ns2CoG20Degree_thrTotg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y);
	    hSvc->FillHisto2(hname,g2x, g2y);
	    hname="MAPggW_3ns2CoG20Degree_thrTotg1g2";
	    hSvc->FillHisto2(hname,g1x, g1y,g1E);
	    hSvc->FillHisto2(hname,g2x, g2y,g2E);
	  }
	   if(fabs(g1y)>fabs(g2y)){
	     hname="CoGvsMaxY_3ns2CoG20Degree";
	     hSvc->FillHisto2(hname,CoG,fabs(g1y));
	   }
	   else{
	     hname="CoGvsMaxY_3ns2CoG20Degree";
	     hSvc->FillHisto2(hname,CoG,fabs(g2y));
	   }
	  }
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
            HistoIn2cmCoG3nsDeltaTheta(g1E, g2E, InvariantMass);
	    if(inFR){
	      hname="ECAL_twoPhotonInTime3ns_InFiducialRegion2CoGDeltaTheta";
	      hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	    }
            if(dPhiIn10Degree){
            HistoIn2cmCoG3nsDeltaThetaDeltaPhi(g1E, g2E, InvariantMass);
	    if(inFR){
	      hname="ECAL_twoPhotonInTime3ns_InFiducialRegion2CoGDeltaTheta10Degree";
	      hSvc->FillHisto(hname,(ecalclu->GetEnergy()+ecalclu2->GetEnergy()) , 1.);
	    }
            }
          }
        }
        
        if(fabs(XWeighted)<30. && fabs(YWeighted)<30.){
          HistoIn3cmCoG3ns(g1E, g2E, InvariantMass);
          if(angleBetweenTwoPhoton>0.08 && angleBetweenTwoPhoton<0.12){
            HistoIn3cmCoG3nsDeltaTheta(g1E, g2E, InvariantMass);
            if(dPhiIn10Degree){
              HistoIn3cmCoG3nsDeltaThetaDeltaPhi(g1E, g2E, InvariantMass);
            }
          }
        }
        
        if(fabs(XWeighted)<50. && fabs(YWeighted)<50.){
         HistoIn5cmCoG3ns(g1E, g2E, InvariantMass);
        }
      }
    
 }
  hname="NGammaGammaForEvent_TimeRequest"; 
  hSvc->FillHisto(hname,NGGInTheSameEventTime3ns , 1.);
  hname="NGammaGammaForEvent_TimeRequest_DeltaPhi10Degree"; 
  hSvc->FillHisto(hname,NGGInTheSameEventDeltaPhi10_3ns , 1.);
  hname="NGammaGammaForEvent_TimeRequest_DeltaPhi10Degree_EnergyInterval_InvariantMassInterval"; 
  hSvc->FillHisto(hname,NGGInTheSameEventDeltaPhi10_EnergyInterval_InvariantMassInterval_3ns , 1.);
  hname="NGammaGammaForEvent_TimeRequest_CoG2cm";
  hSvc->FillHisto(hname,NGGInTheSameEventCoG2cm3ns , 1.);

  if(NoInTime20 && PassEneThr){
    hname="MissingMass_NoClInTime20ns_ThrEne50MeV";
    hSvc->FillHisto(hname,MissingMass, 1.);
    Bool_t CoincidencePVeto=true;
    Bool_t CoincidenceSAC=true;
    for(int jpveto=0; jpveto<NClPVeto; jpveto++){
      PVetoclu=fPVeto_ClColl->Element(jpveto);
      Double_t cluTime=PVetoclu->GetTime();
      Double_t cluZpos=PVetoclu->GetPosition().Z();
      Double_t cluXpos=PVetoclu->GetPosition().X();
      if(fabs(ClTECal-cluTime-ShiftECalPVeto)<2.){
	Double_t momentumPositron=CalculateMomentumPositron(cluZpos,cluXpos);
	if(fabs(momentumPositron+g1E-490)<50){
	  CoincidencePVeto=false;
	}
      }
    }
 
    for(int jsac=0; jsac<NClSAC; jsac++){
      SACclu=fSAC_ClColl->Element(jsac);
      Double_t cluTime=SACclu->GetTime();
      if(fabs(ClTECal-cluTime-ShiftECalSac)<2. && SACclu->GetEnergy()>50){
	CoincidenceSAC=false;
      }
    }
   if(CoincidencePVeto){
      hname="MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto";
      hSvc->FillHisto(hname,MissingMass, 1.);
      if(CoincidenceSAC){
	hname="MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC";
	hSvc->FillHisto(hname,MissingMass, 1.);
      }
    }
  }
  if(NoInTime10 && PassEneThr){
    hname="MissingMass_NoClInTime10ns_ThrEne50MeV";
    hSvc->FillHisto(hname,MissingMass, 1.);
    Bool_t CoincidencePVeto=true;
    Bool_t CoincidenceSAC=true;
    Bool_t energyRequestSAC=true;
    for(int jpveto=0; jpveto<NClPVeto; jpveto++){
      PVetoclu=fPVeto_ClColl->Element(jpveto);
      Double_t cluTime=PVetoclu->GetTime();
      Double_t cluZpos=PVetoclu->GetPosition().Z();
      Double_t cluXpos=PVetoclu->GetPosition().X();
      if(fabs(ClTECal-cluTime-ShiftECalPVeto)<2.){
	Double_t momentumPositron=CalculateMomentumPositron(cluZpos,cluXpos);
	if(fabs(momentumPositron+g1E-490)<50){
	  CoincidencePVeto=false;
	}
      }
    }
 
    for(int jsac=0; jsac<NClSAC; jsac++){
      SACclu=fSAC_ClColl->Element(jsac);
      Double_t cluTime=SACclu->GetTime();
      Double_t cluEne=SACclu->GetEnergy();
      // if(cluEne>50 ){
      //	  energyRequestSAC=false;
      if(fabs(ClTECal-cluTime-ShiftECalSac)<2. && SACclu->GetEnergy()>50){
	    CoincidenceSAC=false;
	  }
	  // }
    }
   if(CoincidencePVeto){
      hname="MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto";
      hSvc->FillHisto(hname,MissingMass, 1.);
      if(CoincidenceSAC){
	hname="MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC";
	hSvc->FillHisto(hname,MissingMass, 1.);
      }
    }

  }


  }
   return retCode;
 
}


Double_t EventSelection::CalculateMomentumPositron(Double_t z,Double_t x){
  return ((z+631.5)*(z+631.5)+x*x)/2/x*0.3*0.3986;
}


Bool_t EventSelection::phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi)
{
 distPhi = -999.;
 //distR   = -999.;
 //b       = -999.;
 P1.SetZ(0.);
 P2.SetZ(0.);
 TVector3 Pb = P2-P1;
 double impactPar = (P1.Cross(Pb)).Perp()/Pb.Perp();
 //std::cout<<" P1.X, Y "<<P1.X()<<" "<<P1.Y()<<std::endl;
 //std::cout<<" P2.X, Y "<<P2.X()<<" "<<P2.Y()<<std::endl;
 double impPar = (P1.X()*Pb.Y()-P2.X()*Pb.Y())/Pb.Perp();


 double scalarP = P1.Dot(P2);
 double cosphi = scalarP/P1.Perp()/P2.Perp();
 double dPhi = acos(cosphi);

 distPhi = dPhi;
 //distR   = Pb.Perp();
 //b = impPar;
 //std::cout<<"dPhi = "<<distPhi<<" b = "<<b<<" impPar "<<impPar<<std::endl;

 //if (cosphi<-0.985 && fabs(b)<10.) return true;
 if (cosphi<-0.985) return true; //phi > 170.06 degree
 return false;

}





void EventSelection::HistoInDeltaPhi3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns";
  hSvc->FillHisto(hname, InvariantMass, 1.);
   
}

void EventSelection::HistoInDeltaPhi3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi_withAngleCut";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns_withAngleCut";
  hSvc->FillHisto(hname, InvariantMass, 1.);
   
}

void EventSelection::HistoIn1cmCoG3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass,Double_t XWeighted,Double_t YWeighted,Double_t angleBetweenTwoPhoton,Double_t phig1,Double_t phig2,Double_t thetag1,Double_t thetag2,Double_t g1x,Double_t g2x,Double_t g1y,Double_t g2y , Double_t CoG ){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns1cm";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECal_xCoG_in3ns1cm";
  hSvc->FillHisto(hname,XWeighted , 1.);
  hname="ECal_yCoG_in3ns1cm";
  hSvc->FillHisto(hname,YWeighted , 1.);
  hname="ECal_InvariantMassLorentz_3ns1cm";
  hSvc->FillHisto(hname, InvariantMass, 1.);
  hname="ECal_CoGVsInvariantMassLorentz_3ns1cm";
  hSvc->FillHisto2(hname, InvariantMass, CoG,  1.);
  hname="ECal_angleBetweenTwoPhotonIn3ns1cm";
  hSvc->FillHisto(hname, angleBetweenTwoPhoton, 1.);
  //hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns1cm";
  //hSvc->FillHisto2(hname, g1E+g2E,  angleBetweenTwoPhoton, 1.);
  hname="ECAL_Phi1VsInvariantMass_3ns1cm";
  hSvc->FillHisto2(hname, InvariantMass,  phig1, 1.);
  hname="ECAL_Phi2VsInvariantMass_3ns1cm";
  hSvc->FillHisto2(hname, InvariantMass,  phig2, 1.);
  hname="ECAL_Theta1VsInvariantMass_3ns1cm";
  hSvc->FillHisto2(hname, InvariantMass,  thetag1, 1.);
  hname="ECAL_Theta2VsInvariantMass_3ns1cm";
  hSvc->FillHisto2(hname, InvariantMass,  thetag1, 1.);
   
  hname="ECAL_XGamma1_3ns1cm";
  hSvc->FillHisto(hname, g1x, 1.);
  hname="ECAL_XGamma2_3ns1cm";
  hSvc->FillHisto(hname, g2x, 1.);
  hname="ECAL_YGamma1_3ns1cm";
  hSvc->FillHisto(hname, g1y, 1.);
  hname="ECAL_YGamma2_3ns1cm";
  hSvc->FillHisto(hname, g2y, 1.);
   
  hname="ECAL_EGamma2VSEGamma1_3ns1cm";
  hSvc->FillHisto2(hname, g1E , g2E , 1.);
  hname="ECAL_Theta1VSEGamma1_3ns1cm";
  hSvc->FillHisto2(hname, g1E , thetag1 , 1.);
  hname="ECAL_Theta2VSEGamma2_3ns1cm";
  hSvc->FillHisto2(hname, g2E , thetag2 , 1.);
  hname="ECAL_Theta2VSTheta1_3ns1cm";
  hSvc->FillHisto2(hname,  thetag1 , thetag2, 1.);
  hname="ECAL_Phi1VSEGamma1_3ns1cm";
  hSvc->FillHisto2(hname, g1E , phig1 , 1.);
  hname="ECAL_Phi2VSEGamma2_3ns1cm";
  hSvc->FillHisto2(hname, g2E , phig2 , 1.);
  hname="ECAL_Phi2VSPhi1_3ns1cm";
  hSvc->FillHisto2(hname,  phig1 , phig2, 1.);
   
}


void EventSelection::HistoIn1cmCoG3nsDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns1cm_InDeltaPhi";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns1cm";
  hSvc->FillHisto(hname, InvariantMass, 1.);
   
}


void EventSelection::HistoIn1cmCoG3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass, Bool_t InDeltaPhi){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECal_InvariantMassLorentz_3ns1cm_withAngleCut";
  hSvc->FillHisto(hname, InvariantMass, 1.);
  if(InDeltaPhi){
    hname="ECAL_gravTwoPhoton3ns1cm_InDeltaPhi_withAngleCut";
    hSvc->FillHisto(hname,g1E+g2E , 1.);
    hname="ECAL_InvariantMass_InDeltaPhi_3ns1cm_withAngleCut";
    hSvc->FillHisto(hname, InvariantMass, 1.);
  }
   
}


void EventSelection::HistoIn1cmCoG3nsInFiducialRegion(Double_t g1E, Double_t g2E, Double_t InvariantMass, Double_t CoG){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_InvariantMassCog_GInFiducialRegion_3ns1cm";
  hSvc->FillHisto2(hname, InvariantMass, CoG , 1.);
  hname="InvariantMass_GInFiducialRegion_3ns1cm";
  hSvc->FillHisto(hname, InvariantMass, 1.);
   
}


void EventSelection::HistoIn1cmCoG3nsInvariantMassRange(Double_t g1E, Double_t g2E,Double_t g1x, Double_t g2x, Double_t g1y, Double_t g2y, Double_t thetag1, Double_t thetag2, Double_t phig1, Double_t phig2){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_XGamma1_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto(hname, g1x, 1.);
  hname="ECAL_XGamma2_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto(hname, g2x, 1.);
  hname="ECAL_YGamma1_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto(hname, g1y, 1.);
  hname="ECAL_YGamma2_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto(hname, g2y, 1.);
  hname="ECAL_EGamma2VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto2(hname, g1E , g2E , 1.);
  hname="ECAL_Theta1VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto2(hname, g1E , thetag1 , 1.);
  hname="ECAL_Theta2VSEGamma2_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto2(hname, g2E , thetag2 , 1.);
  hname="ECAL_Theta2VSTheta1_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto2(hname,  thetag1 , thetag2, 1.);
  hname="ECAL_Phi1VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto2(hname, g1E , phig1 , 1.);
  hname="ECAL_Phi2VSEGamma2_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto2(hname, g2E , phig2 , 1.);
  hname="ECAL_Phi2VSPhi1_3ns1cm_InInvariantMassRange";
  hSvc->FillHisto2(hname,  phig1 , phig2, 1.);
   
}


void EventSelection::HistoIn2cmCoG3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass, Double_t angleBetweenTwoPhoton){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns2cm";
  hSvc->FillHisto(hname,g1E+g2E , 1.);      
  hname="ECal_InvariantMassLorentz_3ns2cm";
  hSvc->FillHisto(hname, InvariantMass, 1.);
  hname="ECal_angleBetweenTwoPhotonIn3ns2cm";
  hSvc->FillHisto(hname, angleBetweenTwoPhoton, 1.);
  //hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns2cm";
  //hSvc->FillHisto2(hname, g1E+g2E,  angleBetweenTwoPhoton, 1.);
   
}


void EventSelection::HistoIn2cmCoG3nsDeltaThetaDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns2cm_InDeltaPhi_withAngleCut";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECal_InvariantMassLorentz_3ns2cm_InDeltaPhi_withAngleCut";
  hSvc->FillHisto(hname, InvariantMass, 1.);
   
}


void EventSelection::HistoIn2cmCoG3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns2cm_withAngleCut";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECal_InvariantMassLorentz_3ns2cm_withAngleCut";
  hSvc->FillHisto(hname, InvariantMass, 1.);
   
}


void EventSelection::HistoIn3cmCoG3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns3cm";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
   
}


void EventSelection::HistoIn3cmCoG3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns3cm_withAngleCut";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECal_InvariantMassLorentz_3ns3cm_withAngleCut";
  hSvc->FillHisto(hname, InvariantMass, 1.);
   
}

void EventSelection::HistoIn3cmCoG3nsDeltaThetaDeltaPhi(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns3cm_InDeltaPhi_withAngleCut";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECal_InvariantMassLorentz_3ns3cm_InDeltaPhi_withAngleCut";
  hSvc->FillHisto(hname, InvariantMass, 1.);
   
}


void EventSelection::HistoIn5cmCoG3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns5cm";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
   
}

void EventSelection::HistoInDeltaPhi20DegreeSecondMethod3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi20Degree2Method";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi20Degree2Method_3ns";
  hSvc->FillHisto(hname, InvariantMass, 1.);
}

void EventSelection::HistoInDeltaPhi10DegreeSecondMethod3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi10Degree2Method";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi10Degree2Method_3ns";
  hSvc->FillHisto(hname, InvariantMass, 1.);
}

void EventSelection::HistoInDeltaPhi5DegreeSecondMethod3ns(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi5Degree2Method";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi5Degree2Method_3ns";
  hSvc->FillHisto(hname, InvariantMass, 1.);
}

void EventSelection::HistoInDeltaPhi20DegreeSecondMethod3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi20Degree2Method_DeltaTheta";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi20Degree2Method_DeltaTheta_3ns";
  hSvc->FillHisto(hname, InvariantMass, 1.);
}

void EventSelection::HistoInDeltaPhi10DegreeSecondMethod3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi10Degree2Method_DeltaTheta";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi10Degree2Method_DeltaTheta_3ns";
  hSvc->FillHisto(hname, InvariantMass, 1.);
}

void EventSelection::HistoInDeltaPhi5DegreeSecondMethod3nsDeltaTheta(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi5Degree2Method_DeltaTheta";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
  hname="ECAL_InvariantMass_InDeltaPhi5Degree2Method_DeltaTheta_3ns";
  hSvc->FillHisto(hname, InvariantMass, 1.);
}

void EventSelection::HistoInDeltaPhi10DegreeSecondMethod3nsThrEnergy(Double_t g1E, Double_t g2E, Double_t InvariantMass){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi10Degree2Method_ThrEnergy";
  hSvc->FillHisto(hname,g1E+g2E , 1.);
 // hname="ECAL_InvariantMass_InDeltaPhi10Degree2Method_3ns_ThrEnergy70MeV";
 // hSvc->FillHisto(hname, InvariantMass, 1.);
}

void EventSelection::HistoDeltaTheta_InDeltaPhi20Interval(Double_t angleBetweenTwoPhoton){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_DeltaThetaDistributions_TwoPhoton3ns_InDeltaPhi20Degree2Method";
  hSvc->FillHisto(hname, angleBetweenTwoPhoton , 1.);
}

void EventSelection::HistoDeltaPhi_InDeltaThetaInterval(Double_t dphi){
  std::string hname;
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  hname="ECAL_DeltaPhiDistributions_TwoPhoton3ns_InDeltaTheta";
  hSvc->FillHisto(hname,dphi , 1.);
}






Bool_t EventSelection::InitHistosIO()
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  std::string hname;
  int nBin=500;
  int min=0;
  int max=3000;
  hname = "ECal_SeedEnergy";
  hSvc->BookHisto(hname,nBin,min, max);
  hname = "ECal_ClusterEnergy";
  hSvc->BookHisto(hname,nBin,min, max);
  hname = "ECal_HitEnergy";
  hSvc->BookHisto(hname,nBin,min, max);
   

  nBin=600;
  min=0;
  max=10000;
  hname="ECal_EHitTot";
  hSvc->BookHisto(hname,nBin,min, max);
  hname="ECal_EClTot";
  hSvc->BookHisto(hname,nBin,min, max);

  min=-0.5;
  max= 40.5;
  nBin=41;
  hname = "ECal_NHitsInClus";
  hSvc->BookHisto(hname,nBin,min, max);

   
    
    
  //Histo for molteplicity studies
  min=-0.5;
  max= 35.5;
  nBin=36;
  hname = "ECal_NCluster";
  hSvc->BookHisto(hname,nBin,min, max);
    

  
  int binX=11;   
  int minX=-0.5;
  int maxX=10.5;

  hname="NGammaGammaForEvent_TimeRequest"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="NGammaGammaForEvent_TimeRequest_DeltaPhi10Degree"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="NGammaGammaForEvent_TimeRequest_DeltaPhi10Degree_EnergyInterval_InvariantMassInterval"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="NGammaGammaForEvent_TimeRequest_CoG2cm"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  binX=500;   
  minX=0;
  maxX=2000;
  int binY=40;
  int minY=0;
  int maxY=40;

  hname="AnnihilationGammaEcalGammaSac"; 
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi20Degree2Method";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi10Degree2Method";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi10Degree2Method_ThrEnergy";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi5Degree2Method";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi20Degree2Method_DeltaTheta";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi10Degree2Method_DeltaTheta";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns_InDeltaPhi5Degree2Method_DeltaTheta";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns1cm_InDeltaPhi";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns1cm_InDeltaPhi_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns2cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns2cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns2cm_InDeltaPhi_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns3cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns3cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns3cm_InDeltaPhi_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_gravTwoPhoton3ns5cm"; 
  hSvc->BookHisto(hname, binX, minX, maxX);

  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion20Degree";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion20DegreeDeltaTheta";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion1CoG";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion1CoG10Degree";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion2CoG";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion2CoG20Degree";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion2CoGDeltaTheta";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_twoPhotonInTime3ns_InFiducialRegion2CoGDeltaTheta10Degree";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  
  binX=601;   
  minX=-300.5;
  maxX=300.5;
  hname="ECal_xCoG_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_yCoG_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_y:xCog_3ns";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  hname="ECal_xCoG_in3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_yCoG_in3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_ImpactParameter_3ns";
  hSvc->BookHisto(hname, binX, -0.5, maxX);
 
  
  binX=101;   
  minX=-0.5;
  maxX=70.5;
  binY=70;   
  minY=-0.5;
  maxY=30.5;
  hname="InvariantMass_GInFiducialRegion_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMassCog_GInFiducialRegion_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi20Degree2Method_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi10Degree2Method_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi5Degree2Method_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi20Degree2Method_DeltaTheta_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi10Degree2Method_DeltaTheta_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi5Degree2Method_DeltaTheta_3ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECAL_InvariantMass_InDeltaPhi_3ns1cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns1cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_CoGVsInvariantMassLorentz_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECal_InvariantMassLorentz_3ns1cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns2cm";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns2cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns2cm_InDeltaPhi_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns3cm_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="ECal_InvariantMassLorentz_3ns3cm_InDeltaPhi_withAngleCut";
  hSvc->BookHisto(hname, binX, minX, maxX);

  
  
  binX=501;   
  minX=-0.5;
  maxX=2000.5;
  binY=700;   
  minY=-0.5;
  maxY=0.2;
  
  hname="ECal_angleBetweenTwoPhotonIn3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  //hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns1cm";
  //hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECal_angleBetweenTwoPhotonIn3ns2cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  //hname ="ECal_angleBetweenTwoPhotonVSEnergyIn3ns2cm";
  //hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  
  binY=400;   
  minY=-300.5;
  maxY=300.5;
  
  hname="ECAL_XGamma1_3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma2_3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma1_3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma2_3ns1cm";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_XGamma2_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_YGamma2_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto(hname, binY, minY, maxY);
 
  hname="MAPgg_3ns20Degree";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns20Degree";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPgg_3ns2CoG";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns2CoG";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPgg_3ns2CoG20Degree";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns2CoG20Degree";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);

  hname="MAPgg_3ns20Degree_thrg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns20Degree_thrg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPgg_3ns20Degree_thrTotg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns20Degree_thrTotg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPgg_3ns2CoG_thrg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns2CoG_thrg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPgg_3ns2CoG_thrTotg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns2CoG_thrTotg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPgg_3ns2CoG20Degree_thrg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns2CoG20Degree_thrg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPgg_3ns2CoG20Degree_thrTotg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
  hname="MAPggW_3ns2CoG20Degree_thrTotg1g2";
  hSvc->BookHisto2(hname, binY, minY, maxY,binY,minY,maxY);
	

  binX=400;   
  minX=-0.5;
  maxX=300.5;
  binY=1000;   
  minY=-0.5;
  maxY=300.5;
  hname="CoGvsMaxY_3ns20Degree";
  hSvc->BookHisto2(hname, binX, minX, maxX,binY,minY,maxY);
  hname="CoGvsMaxY_3ns2CoG";
  hSvc->BookHisto2(hname, binX, minX, maxX,binY,minY,maxY);
  hname="CoGvsMaxY_3ns2CoG20Degree";
  hSvc->BookHisto2(hname, binX, minX, maxX,binY,minY,maxY);

  
  binX=501;   
  minX=-0.5;
  maxX=500.5;
  binY=200;   
  minY=-0.5;
  maxY=0.1;  
  hname="ECAL_EGamma2VSEGamma1_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  hname="ECAL_Theta1VSEGamma1_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSEGamma2_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSTheta1_3ns1cm";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  binY=500;   
  minY=-6.16;
  maxY=6.16; 
  hname="ECAL_Phi1VSEGamma1_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSEGamma2_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSPhi1_3ns1cm";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  
  hname="ECAL_DeltaThetaDistributions_TwoPhoton3ns_InDeltaPhi20Degree2Method";
  hSvc->BookHisto(hname, binY, minY, maxY);
  hname="ECAL_DeltaPhiDistributions_TwoPhoton3ns_InDeltaTheta";
  hSvc->BookHisto(hname, binY, minY, maxY);
  
  hname="ECAL_EGamma2VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binX, minX, maxX);
  binY=200;   
  minY=-0.5;
  maxY=0.1;
  hname="ECAL_Theta1VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSEGamma2_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VSTheta1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);
  binY=500;   
  minY=-6.16;
  maxY=6.16;
  hname="ECAL_Phi1VSEGamma1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSEGamma2_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VSPhi1_3ns1cm_InInvariantMassRange";
  hSvc->BookHisto2(hname, binY, minY, maxY, binY, minY, maxY);


  
  binX=71;   
  minX=-0.5;
  maxX=30.5;
  binY=500;   
  minY=-6.16;
  maxY=6.16;
  hname="ECAL_Phi1VsInvariantMass_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Phi2VsInvariantMass_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  binY=200;   
  minY=-0.5;
  maxY=0.1; 
  hname="ECAL_Theta1VsInvariantMass_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  hname="ECAL_Theta2VsInvariantMass_3ns1cm";
  hSvc->BookHisto2(hname, binX, minX, maxX, binY, minY, maxY);
  
  
  binX=1001;   
  minX=-500.5;
  maxX=500.5;
;
  hname="DTimePVetoSACCh22";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="DTimeECAlSACCh22";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="DTimeECAlPVetoCh41";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="DTimeECAlEVetoCh41";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  
  binX=500;   
  minX=-200.5;
  maxX=600.5;  
  hname="MissingMass_AllECALclu";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_AllECALclu_ThrEne50MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime20ns_ThrEne50MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime10ns_ThrEne50MeV";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="SAC_evSel_SinglePhotonAnnihilationEnergy_TimeCoincidenceRequest2ns";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime20ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto";
  hSvc->BookHisto(hname, binX, minX, maxX);
  hname="MissingMass_NoClInTime10ns_ThrEne50MeV_NoCoincidencePVeto_NoCoincidenceSAC";
  hSvc->BookHisto(hname, binX, minX, maxX);
  
  return true;
}



Bool_t EventSelection::InGoodRegionECal(Int_t x, Int_t y){
  Bool_t inFR=false;
  if((x==2 || x== 26 )&& y>8 && y<20 ) inFR=true;
  if((x==3 || x== 25 )&& y>6 && y<22 ) inFR=true;
  if((x==4 || x== 24 )&& y>5 && y<23 ) inFR=true;
  if((x==5 || x== 6 || x==22 || x==23 )&& y>4 && y<24 ) inFR=true;
  if((x==7 || x== 8 || x==20 || x==21 )&& y>7 && y<25 ) inFR=true;
  if((x==9 || x== 10 || x==11 || x==17 || x==18 || x==19 )&& y>2 && y<26 ) inFR=true;
  if(x>9 && x<19 && y>9 && y<19) inFR=false; //central square
  return inFR;                                                                                                                                                  

}
