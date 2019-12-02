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
  }
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
  ApplyCalibTimeSAC    ();
  ApplyCalibTimeEnergyECal   (isMC);
  return;
  
}
Bool_t EventSelection::ProcessAnalysis()
{
  CalibrateTimeAndEnergy();

  if (fVersion==1)
    {
      return ProcessAnalysisSS();
    }
  else if (fVersion==2)
    {
      return ProcessAnalysisGC();
    }
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
  //Int_t xChId1, 
  Int_t aChId;
  
      
  Double_t eSumECalHits=0.;
  Double_t xEnergy=0;
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
  //int chSubMax=-1;  
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
  return true; 
}


Bool_t EventSelection::Proc_HEPVeto_vs_PVeto(Bool_t isMC)
{

  HistoSvc* hSvc =  HistoSvc::GetInstance();
  
  // HEPVeto vs HEPVeto and HEPVeto vs PVeto 

  TRecoVCluster*  xCluHEPVeto ;
  TRecoVCluster*  xCluHEPVeto1;
  TRecoVCluster*    xCluPVeto ;

  Double_t xEneHEPVeto, xEneHEPVeto1, xTimeHEPVeto, xTimeHEPVeto1;
  Double_t   xEnePVeto,                 xTimePVeto; 

  Int_t  xChIdHEPVeto, xChIdHEPVeto1;
  Int_t    xChIdPVeto;
  
  TVector3   PosHEP,   PosHEP1 ;
  TVector3 PosPVeto;

  //HEPVeto Cluster1                
  for (int hHEPVeto=0; hHEPVeto<fHEPVeto_ClColl->GetNElements(); ++hHEPVeto)
    {
      xCluHEPVeto = fHEPVeto_ClColl->Element((int)hHEPVeto);
      if (xCluHEPVeto) 
	{
	       
	  xChIdHEPVeto = xCluHEPVeto->GetChannelId();
	  xTimeHEPVeto = xCluHEPVeto->     GetTime(); 
	  xEneHEPVeto  = xCluHEPVeto->   GetEnergy();
	  //xTimeHEPVeto -= fTimeOffsetHEPVeto[xChIdHEPVeto];
	  PosHEP = xCluHEPVeto-> GetPosition()        ; 
	  //if(fabs(xTime-xTimeHEPVeto-7*0+15*0+1)<2)hSvc->FillHisto2("ECal1Cl_E_vs_HEPVetoChId", xChIdHEPVeto,  xEne);
	  //if(fabs(xTime-xTimeHEPVeto-7*0+15*0+1)<2)hSvc->FillHisto2("ECal1Cl_E_vs_HEPVetoPosz", PosHEP.z(),  xEne);
	  Double_t L = (PosHEP.x() - 792.5)*(PosHEP.x() - 792.5)+(PosHEP.z() - 2122.3)*(PosHEP.z() - 2122.3);
	  L=sqrt(L);
	  hSvc->FillHisto2("HEPVeto_x_vs_ChId",xChIdHEPVeto,PosHEP.X());
	  hSvc->FillHisto2("HEPVETO_Z_vs_X"   ,PosHEP.X(),PosHEP.Z());
	  hSvc->FillHisto2("HEPVeto_z_vs_ChId",xChIdHEPVeto,PosHEP.Z());
	  hSvc->FillHisto2("HEPVeto_L_vs_ChId",xChIdHEPVeto,         L);
	  hSvc->FillHisto2("VETO_Z_vs_X",PosHEP.X(),PosHEP.Z());

	  //search HEPVeto Cluster 2
	  for (int hHEPVeto1=hHEPVeto+1; hHEPVeto1<fHEPVeto_ClColl->GetNElements(); ++hHEPVeto1)
	    {
	      xCluHEPVeto1 = fHEPVeto_ClColl->Element((int)hHEPVeto1);
	      if (xCluHEPVeto1) 
		{
                 
		  xChIdHEPVeto1 = xCluHEPVeto1->GetChannelId();
		  xTimeHEPVeto1 = xCluHEPVeto1->     GetTime(); 
		  xEneHEPVeto1  = xCluHEPVeto1->   GetEnergy();
		  //xTimeHEPVeto1 -= fTimeOffsetHEPVeto[xChIdHEPVeto1]; 

 		  //if(xChIdHEPVeto<16 && xChIdHEPVeto1 << 16)hSvc->FillHisto("HEPVeto2Cl_DeltaTime", xTimeHEPVeto1-xTimeHEPVeto );                 
 		  if(xChIdHEPVeto < 16 && xChIdHEPVeto1 < 16)hSvc->FillHisto("HEPVeto2Cl_DeltaTime", xTimeHEPVeto1-xTimeHEPVeto );                 

		  if(fabs(xTimeHEPVeto1-xTimeHEPVeto)<1){ 
		    if(xChIdHEPVeto1-xChIdHEPVeto==16){
		      hSvc->FillHisto2("HEPVETO_COG",xEneHEPVeto,xEneHEPVeto1);
		      float etafun = 0 ;
		      if(xEneHEPVeto+xEneHEPVeto1>0) etafun = (xEneHEPVeto-xEneHEPVeto1)/(xEneHEPVeto+xEneHEPVeto1);
		      hSvc->FillHisto2("HEPVETO_ETA",xChIdHEPVeto,etafun); 
		    }
		  }
  
		}
	    }

	  //search PVeto in time
	  for (int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
	    {
	      xCluPVeto = fPVeto_ClColl->Element((int)hPVeto);
	      if (xCluPVeto) 
		{
	       
	       

		  xChIdPVeto       = xCluPVeto->GetChannelId()   ;
		  xTimePVeto       = xCluPVeto->     GetTime()   ; //xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto]; 
		  PosPVeto         = xCluPVeto-> GetPosition()   ; 
		  xEnePVeto        = xCluPVeto->   GetEnergy()   ;

                 
		  if(isMC){hSvc->FillHisto("HEPVeto_T_vs_PVeto",xTimeHEPVeto-xTimePVeto);} else{hSvc->FillHisto("HEPVeto_T_vs_PVeto",xTimeHEPVeto-xTimePVeto); }

		  if((fabs(xTimeHEPVeto-xTimePVeto)<1)&&isMC==0){
		    hSvc->FillHisto2("HEPVetoChId_vs_PVetoChId",xChIdPVeto,xChIdHEPVeto);

		    hSvc->FillHisto2("HEPVetoPosZ_vs_PVetoPosZ",PosPVeto.Z(),PosHEP.Z());
		  }
		  if((fabs(xTimeHEPVeto-xTimePVeto )<1)&&isMC==1){
		    hSvc->FillHisto2("HEPVetoChId_vs_PVetoChId",xChIdPVeto,xChIdHEPVeto);

		    hSvc->FillHisto2("HEPVetoPosZ_vs_PVetoPosZ",PosPVeto.Z(),PosHEP.Z());
		  }
		}
	    }
	}
    }// end of HEPVeto plots
  return true;
}


Bool_t EventSelection::Proc_SAChit_vs_PVeto(Bool_t isMC)
{
  TRecoVHit*             xHit;
  TRecoVHit*             yHit;
  TRecoVCluster*         xClu;
  //TRecoVCluster*         aClu;
  //TRecoVCluster*         yClu;
  TRecoVCluster*    xCluPVeto;
  TRecoVCluster*   xCluPVeto1;
  TRecoVCluster*   xCluEVeto1;
  TRecoVCluster*    xCluEVeto;
  TRecoVCluster*  xCluHEPVeto;
  //TRecoVCluster*  xCluHEPVeto1;
  TRecoVCluster*      xCluSAC1;
  Double_t xTime, xEne, xEnePVeto, xTimePVeto, xTimePVeto1,xTimeEVeto, xTimeEVeto1,xTimeHEPVeto, xEneEVeto, xTimeSAC1;//, yTime, yEne, aEne, aTime, xTimeHEPVeto1,  xTimeLinCorr,
  Int_t xChId, yChId, yTime;

  TRecoVCluster* xClu1;
  //TRecoVCluster* aClu1;
  //TRecoVCluster* yClu1;
  Double_t xTime1, xEne1;//, xEneHEPVeto;//, xEneHEPVeto1;
  //Double_t xTimeLinCorr1, aTime1, aEne1, yEne1, yTime1;
  Int_t xChId1, xChIdPVeto, xChIdPVeto1, xChIdHEPVeto, xChIdEVeto, xChIdEVeto1, xChIdSAC1;//xChIdHEPVeto1, 
  //Int_t yChId1, aChId1;

  TVector3 Pos     ;
  TVector3 Pos1    ;
  TVector3 PosHEP  ;
  TVector3 PosHEP1 ;
  TVector3 PosPVeto;
  TVector3 PosEVeto;
  TVector3 PosSAC  ;

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  //SAC Hit 1
  for (int hSAC=0; hSAC<fSAC_hitEvent->GetNHits(); ++hSAC)
    {
      yHit  = fSAC_hitEvent->Hit(hSAC);
      yChId = yHit->GetChannelId()    ;
      if (yHit->GetChannelId()!=21) continue;

      yTime= yHit->GetTime()  ; //yTime -= fTimeOffsetSAC[yChId];
      xEne = yHit->GetEnergy() ; 
	  
      //Search for PVeto cluster in time          
      for (int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
	{
	  xCluPVeto = fPVeto_ClColl->Element((int)hPVeto);
	  if (xCluPVeto) 
	    {
	       
	      xChIdPVeto = xCluPVeto->GetChannelId();
	      xTimePVeto = xCluPVeto->     GetTime(); //xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto];
	      //ss modified hSvc->FillHisto("SAC_21_PVeto_DeltaTime",xTime-yTime);
	      hSvc->FillHisto("SAC_21_PVeto_DeltaTime",xTimePVeto-yTime);
	      if(fabs(xTimePVeto-yTime)<1)hSvc->FillHisto2("SAC1Hit_E_vs_PVetoChId", xChIdPVeto,  xEne);
	      //ss modified if(fabs(xTimePVeto-yTime)<1)hSvc->FillHisto2("SAC1Hit_E_vs_PVetoChId", xChIdPVeto,  xEne);
	    }

	}
      //Search for PVeto hit in time                
      for (int hPVeto=0; hPVeto<fPVeto_hitEvent->GetNHits(); ++hPVeto)
	{
	  xHit = fPVeto_hitEvent->Hit((int)hPVeto);
	  if (xHit) 
	    {
	       
	      xChIdPVeto = xHit->GetChannelId();
	      xTimePVeto = xHit->     GetTime(); //xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto];
	      //ss modified hSvc->FillHisto("SAC_21_PVeto_DeltaTime_Hit",xTime-yTime);
	      hSvc->FillHisto("SAC_21_PVeto_DeltaTime_Hit",xTimePVeto-yTime);
	      if(fabs(xTimePVeto-yTime)<1)hSvc->FillHisto2("SAC1Hit_E_vs_PVetoChId_Hit", xChIdPVeto,  xEne);
	    }

	}
    }

  return true;

}

Bool_t EventSelection::Proc_SACclu_vs_all(Bool_t isMC)
{
  TRecoVHit*             xHit;
  TRecoVHit*             yHit;
  TRecoVCluster*         xClu;
  //TRecoVCluster*         aClu;
  //TRecoVCluster*         yClu;
  TRecoVCluster*    xCluPVeto;
  TRecoVCluster*   xCluPVeto1;
  TRecoVCluster*   xCluEVeto1;
  TRecoVCluster*    xCluEVeto;
  TRecoVCluster*  xCluHEPVeto;
  //TRecoVCluster*  xCluHEPVeto1;
  TRecoVCluster*      xCluSAC1;
  Double_t xTime, xEne, xEnePVeto, xTimePVeto, xTimePVeto1,xTimeEVeto, xTimeEVeto1,xTimeHEPVeto, xEneEVeto, xTimeSAC1;//, yTime, yEne, aEne, aTime, xTimeHEPVeto1,  xTimeLinCorr,
  Int_t xChId, yChId, yTime;

  TRecoVCluster* xClu1;
  //TRecoVCluster* aClu1;
  //TRecoVCluster* yClu1;
  Double_t xTime1, xEne1;//, xEneHEPVeto;//, xEneHEPVeto1;
  //Double_t xTimeLinCorr1, aTime1, aEne1, yEne1, yTime1;
  Int_t xChId1, xChIdPVeto, xChIdPVeto1, xChIdHEPVeto, xChIdEVeto, xChIdEVeto1, xChIdSAC1;//xChIdHEPVeto1, 
  //Int_t yChId1, aChId1;

  TVector3 Pos     ;
  TVector3 Pos1    ;
  TVector3 PosHEP  ;
  TVector3 PosHEP1 ;
  TVector3 PosPVeto;
  TVector3 PosEVeto;
  TVector3 PosSAC  ;

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  //std::cout<<" in proc_"<<std::endl;
  //SAC Cluster 1
  for (int hSAC=0; hSAC<fSAC_ClColl->GetNElements(); ++hSAC)
    {
      //std::cout<<" cluster n. hSAC = "<<hSAC<<" pointer to collection = "<<(long)fSAC_ClColl<<" out of "<<fSAC_ClColl->GetNElements()<<std::endl;
      xClu = fSAC_ClColl->Element((int)hSAC);
      if (xClu) 
	{
	  xChId  = xClu->GetChannelId()        ;
	  xEne   = xClu->   GetEnergy()        ; 
	  xTime  = xClu->     GetTime()        ; //xTime -= fTimeOffsetSAC[xChId];
	  PosSAC = xClu-> GetPosition()   ;
	  hSvc->FillHisto("SACCl_E", xEne ); 





	  //SAC Cluster 2 
                
	  for (int hSAC1=hSAC+1; hSAC1<fSAC_ClColl->GetNElements(); ++hSAC1)
	    {
	      xCluSAC1 = fSAC_ClColl->Element((int)hSAC1);
	      if (xCluSAC1) 
		{
		  xChIdSAC1       = xCluSAC1->  GetChannelId() ;
		  xTimeSAC1       = xCluSAC1->     GetTime()   ; //xTimeSAC1 -= fTimeOffsetSAC[xChIdSAC1];
                
		  hSvc->FillHisto("SAC2Cl_DeltaTime",xTime-xTimeSAC1); 
                
		}

	    }





	  //Search for EVeto in time
                
	  for (int hEVeto=0; hEVeto<fEVeto_ClColl->GetNElements(); ++hEVeto)
	    {
	      xCluEVeto = fEVeto_ClColl->Element((int)hEVeto);
	      if (xCluEVeto) 
		{
	       
		  xChIdEVeto       = xCluEVeto->GetChannelId()   ;
		  xTimeEVeto       = xCluEVeto->     GetTime()   ; //xTimeEVeto -= fTimeOffsetEVeto[xChIdEVeto];
		  Pos              = xCluEVeto-> GetPosition()   ;
		  xEneEVeto        = xCluEVeto->   GetEnergy()   ;
                
                
		  hSvc->FillHisto("SAC1Cl_EVeto_DeltaTime",xTime-xTimeEVeto); 
                
		}

	    }





	  //Search for PVeto in time
                
	  for (int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
	    {
	      xCluPVeto = fPVeto_ClColl->Element((int)hPVeto);
	      if (xCluPVeto) 
		{
	       
		  xChIdPVeto       = xCluPVeto->GetChannelId()   ;
		  xTimePVeto       = xCluPVeto->     GetTime()   ; //xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto];
		  Pos              = xCluPVeto-> GetPosition()   ;
		  xEnePVeto        = xCluPVeto->   GetEnergy()   ;
                
		  //xTime = xTime + Double_t(4.0);
		  hSvc->FillHisto("SAC1Cl_PVeto_DeltaTime",xTime-xTimePVeto);
		  Double_t Momentum = VetoMomentum(      Pos.z()    );  hSvc->FillHisto2("VetoMomentum_vs_z",    Pos.z(),     Momentum     );	
		  if(fabs(xTime-xTimePVeto)<1){
		    hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId"     , xChIdPVeto,         xEne);
		    //std::cout<<" do we come here "<<std::endl;
		    hSvc->FillHisto2("PVetoE_vs_PVetoChId"       , xChIdPVeto,    xEnePVeto);
		    hSvc->FillHisto2("PVetoT_vs_PVetoChId"       , xChIdPVeto,   xTimePVeto);
		    hSvc->FillHisto2("SAC1Cl_E_vs_PVetoPosz"     ,    Pos.z(),         xEne);
		    hSvc->FillHisto2("SAC1Cl_Y_vs_X"             ,    PosSAC.x(),      PosSAC.y());
		    hSvc->FillHisto2("SAC1Cl_Y_vs_Z"             ,    Pos.z(),      PosSAC.y());
		    hSvc->FillHisto2("SAC1Cl_X_vs_Z"             ,    Pos.z(),      PosSAC.x());
		    hSvc->FillHisto2("PVetoCl_PH_vs_ChId_SACinTime", xChIdPVeto,    xEnePVeto);	             
		    hSvc->FillHisto2("SAC1Cl_E_vs_VetoMomentum",     Momentum ,   xEne  );	 
		    hSvc->FillHisto("SAC1Cl_Plus_VetoMomentum",     Momentum +   xEne  );	
		    if(PosSAC.x()<0.15&&PosSAC.x()>-0.15) hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_X0"       , xChIdPVeto,    xEne);
		
		    if(PosSAC.x()>0.15) hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_Xp"       , xChIdPVeto,    xEne);
		
		    if(PosSAC.x()<-0.15) hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_Xm"       , xChIdPVeto,    xEne);
		  }
		  else if(   (fabs(xTime-xTimePVeto)>2)  &&    (fabs(xTime-xTimePVeto)<3) ){
		    hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_OutOfTime"     , xChIdPVeto,         xEne); 		 
		  }
		  else if(   (fabs(xTime-xTimePVeto)>-3)  &&    (fabs(xTime-xTimePVeto)<-2)){
		    hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_OutOfTime"     , xChIdPVeto,         xEne); 		 
		  }
		  else if(   (fabs(xTime-xTimePVeto)>2+1)  &&    (fabs(xTime-xTimePVeto)<3+1) ){
		    hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_OutOfTime3"     , xChIdPVeto,         xEne); 
		    hSvc->FillHisto2("SAC1Cl_E_vs_VetoMomentum_OutOfTime3",     Momentum ,   xEne  );	 
		    hSvc->FillHisto("SAC1Cl_Plus_VetoMomentum_OutOfTime3",     Momentum +   xEne  );
		    hSvc->FillHisto2("SAC1Cl_Y_vs_X_OutOfTime3"             ,    PosSAC.x(),      PosSAC.y());
		    hSvc->FillHisto2("SAC1Cl_Y_vs_Z_OutOfTime3"             ,    Pos.z(),      PosSAC.y());
		    hSvc->FillHisto2("SAC1Cl_X_vs_Z_OutOfTime3"             ,    Pos.z(),      PosSAC.x());
		    if(PosSAC.x()<-0.15) hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_Xm_OutOfTime3"       , xChIdPVeto,    xEne);	 		 
		  }
		  else if(   (fabs(xTime-xTimePVeto)>-3-1)  &&    (fabs(xTime-xTimePVeto)<-2-1)){
		    hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_OutOfTime3"     , xChIdPVeto,         xEne); 
		    hSvc->FillHisto2("SAC1Cl_E_vs_VetoMomentum_OutOfTime3",     Momentum ,   xEne  );	 
		    hSvc->FillHisto("SAC1Cl_Plus_VetoMomentum_OutOfTime3",     Momentum +   xEne  );
		    //ss modified hSvc->FillHisto2("SAC1Cl_Y_vs_X_OutOfTime3"             ,    Pos.x(),      Pos.z());
		    hSvc->FillHisto2("SAC1Cl_Y_vs_X_OutOfTime3"             ,    Pos.x(),      PosSAC.y());
		    hSvc->FillHisto2("SAC1Cl_Y_vs_Z_OutOfTime3"             ,    Pos.z(),      PosSAC.y());
		    hSvc->FillHisto2("SAC1Cl_X_vs_Z_OutOfTime3"             ,    Pos.z(),      PosSAC.x());
		    if(PosSAC.x()<-0.15) hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_Xm_OutOfTime3"       , xChIdPVeto,    xEne);	 		 
		  }
                
		}

	    }


	  //Search for HEPVeto in time
                
	  for (int hPVeto=0; hPVeto<fHEPVeto_ClColl->GetNElements(); ++hPVeto)
	    {
	      xCluPVeto = fHEPVeto_ClColl->Element((int)hPVeto);
	      if (xCluPVeto) 
		{
	       
		  xChIdPVeto = xCluPVeto->GetChannelId();
		  xTimePVeto = xCluPVeto->     GetTime(); //xTimePVeto -= fTimeOffsetHEPVeto[xChIdPVeto];
		  PosHEP     =xCluPVeto-> GetPosition()   ; 
               



		  Double_t L = (PosHEP.x() - 792.5)*(PosHEP.x() - 792.5)+(PosHEP.z() - 2122.3)*(PosHEP.z() - 2122.3);
		  L=sqrt(L);

		  if(isMC){
		    hSvc->FillHisto("SAC1Cl_HEPVeto_DeltaTime",xTime-xTimePVeto);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosx", PosHEP.x(),  xEne);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosz", PosHEP.z(),  xEne);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosL",          L,  xEne);                 
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoChId", xChIdPVeto,  xEne);
		    //ss modified if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("HEPVetoCl_PH_vs_ChId_SACinTime", xChIdPVeto,    xEnePVeto);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("HEPVetoCl_PH_vs_ChId_SACinTime", xChIdPVeto,    xEne);
		    if(fabs(xTime-xTimePVeto)>3&&fabs(xTime-xTimePVeto)<4)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosL_OutOfTime3",          L,  xEne);
		    if(fabs(xTime-xTimePVeto)>-4&&fabs(xTime-xTimePVeto)<-3)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosL_OutOfTime3",          L,  xEne); 
                
		    if(fabs(xTime-xTimePVeto)>3&&fabs(xTime-xTimePVeto)<4)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoChId_OutOfTime3",xChIdPVeto,  xEne);
		    if(fabs(xTime-xTimePVeto)>-4&&fabs(xTime-xTimePVeto)<-3)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoChId_OutOfTime3", xChIdPVeto,  xEne); 
		  }
		  else{
		    hSvc->FillHisto("SAC1Cl_HEPVeto_DeltaTime",xTime-xTimePVeto);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosx", PosHEP.x(),  xEne);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosz", PosHEP.z(),  xEne);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosL",	     L,  xEne);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoChId", xChIdPVeto,  xEne);
		    //ss modified if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("HEPVetoCl_PH_vs_ChId_SACinTime", xChIdPVeto,	xEnePVeto);
		    if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("HEPVetoCl_PH_vs_ChId_SACinTime", xChIdPVeto,	xEne);
		    if(fabs(xTime-xTimePVeto)>3&&fabs(xTime-xTimePVeto)<4)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoChId_OutOfTime3", xChIdPVeto,  xEne);
		    if(fabs(xTime-xTimePVeto)>-4&&fabs(xTime-xTimePVeto)<-3)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoChId_OutOfTime3", xChIdPVeto,  xEne);
		    if(fabs(xTime-xTimePVeto)>3&&fabs(xTime-xTimePVeto)<4)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosL_OutOfTime3", 	 L,  xEne);
		    if(fabs(xTime-xTimePVeto)>-4&&fabs(xTime-xTimePVeto)<-3)hSvc->FillHisto2("SAC1Cl_E_vs_HEPVetoPosL_OutOfTime3",          L,  xEne);
		  }
		}
	    } // end of loop on HEPVeto Clusters
	  
	  //Search for PVeto hit in time      
	  for (int hPVeto=0; hPVeto<fPVeto_hitEvent->GetNHits(); ++hPVeto)
	    {
	      xHit = fPVeto_hitEvent->Hit((int)hPVeto);
	      if (xHit) 
		{
	       
		  xChIdPVeto = xHit->GetChannelId();
		  xTimePVeto = xHit->     GetTime(); //xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto];
		  hSvc->FillHisto("SAC1Cl_PVeto_DeltaTime_Hit",xTime-xTimePVeto);
		  if(fabs(xTime-xTimePVeto)<1)hSvc->FillHisto2("SAC1Cl_E_vs_PVetoChId_Hit", xChIdPVeto,  xEne);
		}
	    }//end of loop on PVeto Hits
	}
    }// end of loop on SAC Clusters
  return true;
}


Bool_t EventSelection::Proc_ECalclu_vs_all(Bool_t isMC)
{
  HistoSvc* hSvc =  HistoSvc::GetInstance();


  TRecoVHit*             xHit;
  TRecoVHit*             yHit;
  TRecoVCluster*         xClu;
  //TRecoVCluster*         aClu;
  //TRecoVCluster*         yClu;
  TRecoVCluster*    xCluPVeto;
  TRecoVCluster*   xCluPVeto1;
  TRecoVCluster*   xCluEVeto1;
  TRecoVCluster*    xCluEVeto;
  TRecoVCluster*  xCluHEPVeto;
  //TRecoVCluster*  xCluHEPVeto1;
  TRecoVCluster*      xCluSAC1;
  Double_t xTime, xEne, xEnePVeto, xTimePVeto, xTimePVeto1,xTimeEVeto, xTimeEVeto1,xTimeHEPVeto, xEneEVeto, xTimeSAC1;//, yTime, yEne, aEne, aTime, xTimeHEPVeto1,  xTimeLinCorr,
  Int_t xChId, yChId, yTime;

  TRecoVCluster* xClu1;
  //TRecoVCluster* aClu1;
  //TRecoVCluster* yClu1;
  Double_t xTime1, xEne1;//, xEneHEPVeto;//, xEneHEPVeto1;
  //Double_t xTimeLinCorr1, aTime1, aEne1, yEne1, yTime1;
  Int_t xChId1, xChIdPVeto, xChIdPVeto1, xChIdHEPVeto, xChIdEVeto, xChIdEVeto1, xChIdSAC1;//xChIdHEPVeto1, 
  //Int_t yChId1, aChId1;

  TVector3 Pos     ;
  TVector3 Pos1    ;
  TVector3 PosHEP  ;
  TVector3 PosHEP1 ;
  TVector3 PosPVeto;
  TVector3 PosEVeto;
  TVector3 PosSAC  ;

  //ECal Cluster 1
  for (int hECal=0; hECal<fECal_ClColl->GetNElements(); ++hECal)
    {
      //std::cout<<" cluster n. hECal = "<<hECal<<" pointer to collection = "<<(long)fECal_ClColl<<std::endl;
      xClu = fECal_ClColl->Element((int)hECal);
      if (xClu) 
	{
	  xEne   = xClu->   GetEnergy()        ;
	  xChId  = xClu->GetChannelId()        ;
	  xTime  = xClu->     GetTime()        ;//xTime -= fTimeOffsetECal[xChId];
	  Pos    = xClu-> GetPosition()        ; 

	  hSvc->FillHisto("ECalCl_E", xEne );

          

	  //Search for PVeto in time
	  bool PVetoInTimePhoton    = false ;
	  bool PVetoPhotonEnergyCut = false ;  
	  bool PhotonRadiusCut      = false ;   
	  for (int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
	    {
	      xCluPVeto = fPVeto_ClColl->Element((int)hPVeto);
	      if (xCluPVeto) 
		{
	       
		  PosPVeto              = xCluPVeto-> GetPosition()   ; 
		  xChIdPVeto = xCluPVeto->GetChannelId();
		  xTimePVeto = xCluPVeto->     GetTime(); //xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto];
		  hSvc->FillHisto("ECal1Cl_PVeto_DeltaTime",xTime-xTimePVeto);
		  if(fabs(xTime-xTimePVeto)<1) {
		    hSvc->FillHisto2 ("ECal1Cl_E_vs_PVetoChId",xChIdPVeto,xEne);
		    PVetoInTimePhoton = true ;
		    Double_t Momentum = VetoMomentum(      PosPVeto.z()    );
		    hSvc->FillHisto ("PVetoPhotonEnergy",Momentum+xEne); 
		    if((Momentum+xEne)>440 && (Momentum+xEne)<590)PVetoPhotonEnergyCut = true ; 
		  }
		}

	    }
            

	  Double_t MissingMass2_1 = MissingMass (Pos,   xEne);
	  Double_t Radius         = PhotonRadius(Pos        );
             

	  hSvc->FillHisto ("MissingMassPhoton1",MissingMass2_1); 
	  hSvc->FillHisto ("PhotonRadius",Radius); 

	  if(xEne>25)   hSvc->FillHisto ("MissingMassPhoton2",MissingMass2_1); 
	  if(xEne>25&&!(PVetoInTimePhoton)) {
	    hSvc->FillHisto ("MissingMassPhoton3",MissingMass2_1); 
                
	  }
	  if(xEne>25&&!(PVetoInTimePhoton&&PVetoPhotonEnergyCut))   hSvc->FillHisto ("MissingMassPhoton4",MissingMass2_1);   

	  if(Radius> 94.5*0+150 && Radius < 262.5*0+250) PhotonRadiusCut      = true; 

	  if(xEne>25&&!(PVetoInTimePhoton&&PVetoPhotonEnergyCut)&&PhotonRadiusCut )   hSvc->FillHisto ("MissingMassPhoton5",MissingMass2_1);   

	  //Search for HEPVeto in time
                
	  for (int hPVeto=0; hPVeto<fHEPVeto_ClColl->GetNElements(); ++hPVeto)
	    {
	      xCluPVeto = fHEPVeto_ClColl->Element((int)hPVeto);
	      if (xCluPVeto) 
		{
	       
		  xChIdPVeto = xCluPVeto->GetChannelId();
		  xTimePVeto = xCluPVeto->     GetTime(); //xTimePVeto -= fTimeOffsetHEPVeto[xChIdPVeto];              
		  hSvc->FillHisto("ECal1Cl_HEPVeto_DeltaTime",xTime-xTimePVeto);
		  PosHEP = xCluPVeto-> GetPosition()        ; 
		  if(fabs(xTime-xTimePVeto-7*0+15*0+1)<2)hSvc->FillHisto2("ECal1Cl_E_vs_HEPVetoChId", xChIdPVeto,  xEne);
		  if(fabs(xTime-xTimePVeto-7*0+15*0+1)<2)hSvc->FillHisto2("ECal1Cl_E_vs_HEPVetoPosz", PosHEP.z(),  xEne);
		}

	    }      
 


	  //Search for EVeto in time
                
	  for (int hEVeto=0; hEVeto<fEVeto_ClColl->GetNElements(); ++hEVeto)
	    {
	      xCluEVeto = fEVeto_ClColl->Element((int)hEVeto);
	      if (xCluEVeto) 
		{
	       
		  xChIdEVeto = xCluEVeto->GetChannelId();
		  xTimeEVeto = xCluEVeto->     GetTime(); //xTimeEVeto -=fTimeOffsetEVeto[xChIdEVeto];
		  hSvc->FillHisto("ECal1Cl_EVeto_DeltaTime",xTime-xTimeEVeto);
		  PosEVeto = xCluEVeto-> GetPosition()        ; 
		  if(fabs(xTime-xTimeEVeto)<2)hSvc->FillHisto2("ECal1Cl_E_vs_EVetoChId",   xChIdEVeto,  xEne);
		  if(fabs(xTime-xTimeEVeto)<2)hSvc->FillHisto2("ECal1Cl_E_vs_EVetoPosz", PosEVeto.z(),  xEne);
		}

	    }      
 


	  //ECal Cluster 2
	  for (int hECal1=hECal+1; hECal1<fECal_ClColl->GetNElements(); ++hECal1)
	    {
	      xClu1 = fECal_ClColl->Element((int)hECal1);
	      if (xClu1) 
		{
		  xEne1  = xClu1->   GetEnergy()        ;
		  xChId1 = xClu1->GetChannelId()        ;
		  xTime1 = xClu1->     GetTime()        ; //xTime1 -= fTimeOffsetECal[xChId1];
		  Pos1   = xClu1-> GetPosition()        ; 

            
            
		  Double_t DeltaTime =  xTime1 - xTime;
     
		  hSvc->FillHisto("ECal2Cl_DeltaTime", DeltaTime );

		  Double_t CosDeltaPhi   =  CosDeltaPhiECal  (Pos, Pos1); 

		  Double_t CosDeltaTheta =  CosDeltaThetaECal(Pos, Pos1);

		  Double_t DeltaTheta =  acos(CosDeltaTheta);
     
		  hSvc->FillHisto("ECal2Cl_CosDeltaPhi", CosDeltaPhi );
            
		  Double_t Minv =  InvariantMassECal(Pos, Pos1, xEne, xEne1);

		  Double_t D    =  InvariantDECal(Pos, Pos1, xEne, xEne1);

		  double Phi1, Phi2;
		  Phi1Phi2ECal( Pos, Pos1, Phi1, Phi2);
             
            
		  Double_t MissingMass2_2 = MissingMass(Pos1, xEne1);
		  // cout << MissingMass2_1 <<  " " << Pos.x() << " " <<  Pos.y()<<" " << Pos.z() <<" " << xEne <<endl;

          


		  //Search for PVeto in time
		  bool PVetoInTime = false ;   
		  for (int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
		    {
		      xCluPVeto = fPVeto_ClColl->Element((int)hPVeto);
		      if (xCluPVeto) 
			{
	       
			  xChIdPVeto = xCluPVeto->GetChannelId();
			  xTimePVeto = xCluPVeto->     GetTime(); // xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto];

			  if(fabs(xTime-xTimePVeto)<1) PVetoInTime = true ;	
			  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003  )
			    hSvc->FillHisto2("ECal2Cl_InvM_vs_DeltaTimePVeto", xTime-xTimePVeto, Minv );
			}     
		      //ss moved in the loop above if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003  )
		      //ss moved in the loop above   hSvc->FillHisto2("ECal2Cl_InvM_vs_DeltaTimePVeto", xTime-xTimePVeto, Minv );

		    }
		  //Search for EVeto in time
		  bool EVetoInTime = false ;   
		  for (int hEVeto=0; hEVeto<fEVeto_ClColl->GetNElements(); ++hEVeto)
		    {
		      xCluEVeto = fEVeto_ClColl->Element((int)hEVeto);
		      if (xCluEVeto) 
			{
	       
			  xChIdEVeto = xCluEVeto->GetChannelId();
			  xTimeEVeto = xCluEVeto->     GetTime(); //xTimeEVeto -= fTimeOffsetEVeto[xChIdEVeto];
			  if(fabs(xTime-xTimeEVeto)<1) EVetoInTime = true ;
			  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003  )
			    hSvc->FillHisto2("ECal2Cl_InvM_vs_DeltaTimeEVeto", xTime-xTimeEVeto, Minv );
			}     
		      //ss_moved in the loop above if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003  )
		      //ss_moved in the loop above   hSvc->FillHisto2("ECal2Cl_InvM_vs_DeltaTimeEVeto", xTime-xTimeEVeto, Minv );

		    }     
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 ){
		    hSvc->FillHisto ("ECal2Cl_InvM"                            , Minv );
		    if(Minv>15)hSvc->FillHisto2 ("MissingMass2",MissingMass2_1,MissingMass2_2);
		  }    
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )
		    hSvc->FillHisto ("ECal2Cl_ChId1",          xChId                 );     
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )
		    hSvc->FillHisto ("ECal2Cl_ChId2",          xChId1                 ); 
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )  
		    //cout << fRecoEvent->GetEventNumber() << " " << Minv << " " << Pos.X()<< " "  << Pos.Y()<< " " << Pos1.X()<< " "  << Pos1.Y() <<" " << xChId << " "  << xChId1 <<" " << xEne << " " << xEne1<<endl;  
		    if(xEne>100. && xEne1>100. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )
		      hSvc->FillHisto ("ECal2Cl_InvM_100MeV"                            , Minv ); ;  
		  if(xEne>5. && xEne1>5. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )
		    hSvc->FillHisto ("ECal2Cl_InvM_5MeV"                            , Minv ); ;   
		  if(fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )
		    hSvc->FillHisto ("ECal2Cl_InvM_0MeV"                            , Minv );   
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && D<10 )
		    hSvc->FillHisto ("ECal2Cl_InvM_D"                            , Minv );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 && !PVetoInTime)
		    hSvc->FillHisto ("ECal2Cl_InvM_PVeto"                            , Minv );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 && !PVetoInTime&& !EVetoInTime)
		    hSvc->FillHisto ("ECal2Cl_InvM_PVeto_EVeto"                            , Minv );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003  )
		    hSvc->FillHisto2("ECal2Cl_InvM_vs_Phi", Phi1, Minv ); 
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003  )
		    hSvc->FillHisto2("ECal2Cl_InvM_vs_Phi", Phi2, Minv );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1.  && fabs(CosDeltaPhi+1)<0.003 )
		    hSvc->FillHisto2("ECal2Cl_InvM_vs_DeltaTheta", DeltaTheta, Minv );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )
		    hSvc->FillHisto2("ECal2Cl_InvM_vs_SumE1E2"    ,  xEne+xEne1, Minv );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )
		    hSvc->FillHisto("ECal2Cl_D", D );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.003 )
		    hSvc->FillHisto2("ECal2Cl_InvM_vs_D", D , Minv);
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1.)
		    hSvc->FillHisto2("ECal2Cl_InvM_vs_CosDeltaPhi",CosDeltaPhi,Minv);


		}
	    }

	  //ECal Cluster 1 and SAC Cluster 2
	  //SAC Cluster 2
	  for (int hSAC=0; hSAC<fSAC_ClColl->GetNElements(); ++hSAC)
	    {
	      xClu1 = fSAC_ClColl->Element((int)hSAC);
	      if (xClu1) 
		{
	     
            
		  xChId1 = xClu1->GetChannelId()        ;
		  xEne1  = xClu1->   GetEnergy()        ;
		  xTime1 = xClu1->     GetTime()        ; //xTime1 -= fTimeOffsetSAC[xChId1];
		  Pos1   = xClu1-> GetPosition()        ; 

             
            
		  Double_t DeltaTime =  xTime1 - xTime;
     
		  hSvc->FillHisto("ECal1ClSAC1Cl_DeltaTime", DeltaTime );

		  Double_t CosDeltaPhi   =  CosDeltaPhiECal  (Pos, Pos1); 

		  Double_t CosDeltaTheta =  CosDeltaThetaECal(Pos, Pos1);

		  Double_t DeltaTheta =  acos(CosDeltaTheta);
     
		  hSvc->FillHisto("ECal1ClSAC1Cl_CosDeltaPhi", CosDeltaPhi );
            
		  Double_t Minv =  InvariantMassECal(Pos, Pos1, xEne, xEne1);

             

		  Double_t D    =  InvariantDECal(Pos, Pos1, xEne, xEne1);

		  double Phi1, Phi2;
		  Phi1Phi2ECal( Pos, Pos1, Phi1, Phi2);
             
 
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.001 )
		    hSvc->FillHisto ("ECal1ClSAC1Cl_InvM"                            , Minv );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.001  )
		    hSvc->FillHisto2("ECal1ClSAC1Cl_InvM_vs_Phi", Phi1, Minv ); 
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.001  )
		    hSvc->FillHisto2("ECal1ClSAC1Cl_InvM_vs_Phi", Phi2, Minv );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1.  && fabs(CosDeltaPhi+1)<0.001 )
		    hSvc->FillHisto2("ECal1ClSAC1Cl_InvM_vs_DeltaTheta", DeltaTheta, Minv );

		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1.  )
		    hSvc->FillHisto2("ECal1ClSAC1Cl_InvM_vs_SumE1E2"    ,  xEne+xEne1, Minv );

		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.001 )
		    hSvc->FillHisto("ECal1ClSAC1Cl_SumE1E2"    ,  xEne+xEne1 );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.001 )
		    hSvc->FillHisto("ECal1ClSAC1Cl_D", D );
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1. && fabs(CosDeltaPhi+1)<0.001 )
		    hSvc->FillHisto2("ECal1ClSAC1Cl_InvM_vs_D", D , Minv);
		  if(xEne>25. && xEne1>25. && fabs(DeltaTime)<1.)
		    hSvc->FillHisto2("ECal1ClSAC1Cl_InvM_vs_CosDeltaPhi",CosDeltaPhi,Minv);


		}
	    }
	  
	}
    }
  //ECal Cluster 1 //end

  return true;
}

Bool_t EventSelection::Proc_PVetoclu_vs_AllVetos(Bool_t isMC)
 {
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit*             xHit;
  TRecoVHit*             yHit;
  TRecoVCluster*         xClu;
  //TRecoVCluster*         aClu;
  //TRecoVCluster*         yClu;
  TRecoVCluster*    xCluPVeto;
  TRecoVCluster*   xCluPVeto1;
  TRecoVCluster*   xCluEVeto1;
  TRecoVCluster*    xCluEVeto;
  TRecoVCluster*  xCluHEPVeto;
  //TRecoVCluster*  xCluHEPVeto1;
  Double_t xTime, xEne, xEnePVeto, xTimePVeto, xTimePVeto1,xTimeEVeto, xTimeEVeto1,xTimeHEPVeto, xEneEVeto, xTimeSAC1;//, yTime, yEne, aEne, aTime, xTimeHEPVeto1,  xTimeLinCorr,
  Int_t xChId, yChId, yTime;

  TRecoVCluster* xClu1;
  //TRecoVCluster* aClu1;
  //TRecoVCluster* yClu1;
  Double_t xTime1, xEne1;//, xEneHEPVeto;//, xEneHEPVeto1;
  //Double_t xTimeLinCorr1, aTime1, aEne1, yEne1, yTime1;
  Int_t xChId1, xChIdPVeto, xChIdPVeto1, xChIdHEPVeto, xChIdEVeto, xChIdEVeto1, xChIdSAC1;//xChIdHEPVeto1, 
  //Int_t yChId1, aChId1;

  TVector3 Pos     ;
  TVector3 Pos1    ;
  TVector3 PosHEP  ;
  TVector3 PosHEP1 ;
  TVector3 PosPVeto;
  TVector3 PosEVeto;

   for (int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
    {
      xCluPVeto = fPVeto_ClColl->Element((int)hPVeto);
      if (xCluPVeto) 
	{
	       
	       

	  xChIdPVeto       = xCluPVeto->GetChannelId()   ;
	  xTimePVeto       = xCluPVeto->     GetTime()   ;  //xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto];
	  PosPVeto = xCluPVeto-> GetPosition()        ; 
		 
	  xEnePVeto        = xCluPVeto->   GetEnergy()   ;
	  
          hSvc->FillHisto2("PVetoCl_PH_vs_ChId", xChIdPVeto,    xEnePVeto);  

	  float ClusterSizePVeto = xCluPVeto->GetNHitsInClus();
	       
	  hSvc->FillHisto2("PVeto_ClSize_vs_ChId", xChIdPVeto ,  ClusterSizePVeto); 
                
	  hSvc->FillHisto2("PVeto_x_vs_ChId",xChIdPVeto,PosPVeto.X()); 
	  hSvc->FillHisto2("PVeto_z_vs_ChId",xChIdPVeto,PosPVeto.Z());
	  hSvc->FillHisto2("VETO_Z_vs_X",PosPVeto.X(),PosPVeto.Z());


	  Double_t MomentumPVeto = VetoMomentum(      PosPVeto.z()    );  	

                 

	  //Search for EVeto in time
                
	  for (int hEVeto=0; hEVeto<fEVeto_ClColl->GetNElements(); ++hEVeto)
	    {
	      xCluEVeto = fEVeto_ClColl->Element((int)hEVeto);
	      if (xCluEVeto) 
		{
	       
		  xChIdEVeto       = xCluEVeto->GetChannelId()   ;
		  xTimeEVeto       = xCluEVeto->     GetTime()   ; //xTimeEVeto -= fTimeOffsetEVeto[xChIdEVeto];
		  PosEVeto         = xCluEVeto-> GetPosition()   ;
		  xEneEVeto        = xCluEVeto->   GetEnergy()   ;

		  Double_t MomentumEVeto = VetoMomentum(      PosEVeto.z()    );  	
                
                
		  hSvc->FillHisto("PVeto1Cl_EVeto_DeltaTime",xTimePVeto-xTimeEVeto); 
		  hSvc->FillHisto2("PVeto_EVeto_Msum_vs_DeltaTime",xTimePVeto-xTimeEVeto, MomentumPVeto+MomentumEVeto);

		  if(fabs(xTimePVeto-xTimeEVeto)<5)hSvc->FillHisto2("PVeto1Cl_chId_vs_EVeto1Cl_chId",xChIdEVeto,xChIdPVeto);
		  if(fabs(xTimePVeto-xTimeEVeto)>15&&fabs(xTimePVeto-xTimeEVeto)<20)hSvc->FillHisto2("PVeto1Cl_chId_vs_EVeto1Cl_chId_OutOfTime",xChIdEVeto,xChIdPVeto);
		  if(fabs(xTimePVeto-xTimeEVeto)>-20&&fabs(xTimePVeto-xTimeEVeto)<-15)hSvc->FillHisto2("PVeto1Cl_chId_vs_EVeto1Cl_chId_OutOfTime",xChIdEVeto,xChIdPVeto);

		  if(fabs(xTimePVeto-xTimeEVeto)<5)hSvc->FillHisto("PVeto_EVeto_Msum",MomentumPVeto+MomentumEVeto);
		  if(fabs(xTimePVeto-xTimeEVeto)>15&&fabs(xTimePVeto-xTimeEVeto)<20) hSvc->FillHisto("PVeto_EVeto_Msum_OutOfTime",MomentumPVeto+MomentumEVeto);
		  if(fabs(xTimePVeto-xTimeEVeto)>-20&&fabs(xTimePVeto-xTimeEVeto)<-15) hSvc->FillHisto("PVeto_EVeto_Msum_OutOfTime",MomentumPVeto+MomentumEVeto);
                        
		  if(fabs(xTimePVeto-xTimeEVeto)<5){
		    for (int hSAC=0; hSAC<fSAC_ClColl->GetNElements(); ++hSAC)
		      {
			//std::cout<<" cluster n. hSAC = "<<hSAC<<" pointer to collection = "<<(long)fSAC_ClColl<<std::endl;
			xClu = fSAC_ClColl->Element((int)hSAC);
			if (xClu) 
			  {
			    xChId  = xClu->GetChannelId()        ;
			    xEne   = xClu->   GetEnergy()        ; 
			    xTime  = xClu->     GetTime()        ; //xTime -= fTimeOffsetSAC[xChId];
                                 
			    hSvc->FillHisto2("SAC_PVeto_EVeto_Msum_vs_DeltaTime",xTimePVeto-xTime, MomentumPVeto+MomentumEVeto+xEne);

			  }
		      }}


                
		}

	    }


                 

	  //Search for Cluster 2 PVeto in time              
	  for (int hPVeto1=hPVeto+1; hPVeto1<fPVeto_ClColl->GetNElements(); ++hPVeto1)
	    {
	      xCluPVeto1 = fPVeto_ClColl->Element((int)hPVeto1);
	      if (xCluPVeto1) 
		{
		  xChIdPVeto1       = xCluPVeto1->GetChannelId()   ;
		  xTimePVeto1       = xCluPVeto1->     GetTime()   ; //xTimePVeto1 -= fTimeOffsetPVeto[xChIdPVeto1];
			 
		  hSvc->FillHisto("PVeto2Cl_DeltaTime",xTimePVeto-xTimePVeto1);                    
                
		}

	    }

                 

          	 
	  //Search for HEPVeto in time
	  for (int hHEPVeto=0; hHEPVeto<fHEPVeto_ClColl->GetNElements(); ++hHEPVeto)
	    {
	      xCluHEPVeto = fHEPVeto_ClColl->Element((int)hHEPVeto);
	      if (xCluHEPVeto) 
		{
		  xChIdHEPVeto       = xCluHEPVeto->GetChannelId()   ;
		  xTimeHEPVeto       = xCluHEPVeto->     GetTime()   ; //xTimeHEPVeto -= fTimeOffsetHEPVeto[xChIdHEPVeto];
			 
		  hSvc->FillHisto("PVeto1Cl_HEPVeto_DeltaTime",xTimePVeto-xTimeHEPVeto);                    
                
		}

	    }



	}

    }

   return true;
}


Bool_t EventSelection::Proc_EVetoclu_vs_HEPVeto(Bool_t isMC)
{
 HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit*             xHit;
  TRecoVHit*             yHit;
  TRecoVCluster*         xClu;
  //TRecoVCluster*         aClu;
  //TRecoVCluster*         yClu;
  TRecoVCluster*    xCluPVeto;
  TRecoVCluster*   xCluPVeto1;
  TRecoVCluster*   xCluEVeto1;
  TRecoVCluster*    xCluEVeto;
  TRecoVCluster*  xCluHEPVeto;
  //TRecoVCluster*  xCluHEPVeto1;
  Double_t xTime, xEne, xEnePVeto, xTimePVeto, xTimePVeto1,xTimeEVeto, xTimeEVeto1,xTimeHEPVeto, xEneEVeto, xTimeSAC1;//, yTime, yEne, aEne, aTime, xTimeHEPVeto1,  xTimeLinCorr,
  Int_t xChId, yChId, yTime;

  TRecoVCluster* xClu1;
  //TRecoVCluster* aClu1;
  //TRecoVCluster* yClu1;
  Double_t xTime1, xEne1;//, xEneHEPVeto;//, xEneHEPVeto1;
  //Double_t xTimeLinCorr1, aTime1, aEne1, yEne1, yTime1;
  Int_t xChId1, xChIdPVeto, xChIdPVeto1, xChIdHEPVeto, xChIdEVeto, xChIdEVeto1, xChIdSAC1;//xChIdHEPVeto1, 
  //Int_t yChId1, aChId1;

  TVector3 Pos     ;
  TVector3 Pos1    ;
  TVector3 PosHEP  ;
  TVector3 PosHEP1 ;
  TVector3 PosPVeto;
  TVector3 PosEVeto;

  for (int hEVeto=0; hEVeto<fEVeto_ClColl->GetNElements(); ++hEVeto)
    {
      xCluEVeto = fEVeto_ClColl->Element((int)hEVeto);
      if (xCluEVeto) 
	{
	       
	  xChIdEVeto       = xCluEVeto->GetChannelId()   ;
	  xTimeEVeto       = xCluEVeto->     GetTime()   ; //xTimeEVeto -= fTimeOffsetEVeto[xChIdEVeto];
	  PosEVeto         = xCluEVeto-> GetPosition()   ;
	  xEneEVeto        = xCluEVeto->   GetEnergy()   ;

	  hSvc->FillHisto2("VETO_Z_vs_X",PosEVeto.X(),PosEVeto.Z());


                 

	  //Search for Cluster 2 EVeto in time              
	  for (int hEVeto1=hEVeto+1; hEVeto1<fEVeto_ClColl->GetNElements(); ++hEVeto1)
	    {
	      xCluEVeto1 = fEVeto_ClColl->Element((int)hEVeto1);
	      if (xCluEVeto1) 
		{
		  xChIdEVeto1       = xCluEVeto1->GetChannelId()   ;
		  xTimeEVeto1       = xCluEVeto1->     GetTime()   ; //xTimeEVeto1 -= fTimeOffsetEVeto[xChIdEVeto1];
			 
		  hSvc->FillHisto("EVeto2Cl_DeltaTime",xTimeEVeto-xTimeEVeto1);                    
                
		}

	    }

                 

          	 
	  //Search for HEPVeto in time
	  for (int hHEPVeto=0; hHEPVeto<fHEPVeto_ClColl->GetNElements(); ++hHEPVeto)
	    {
	      xCluHEPVeto = fHEPVeto_ClColl->Element((int)hHEPVeto);
	      if (xCluHEPVeto) 
		{
		  xChIdHEPVeto       = xCluHEPVeto->GetChannelId()   ; 
		  xTimeHEPVeto       = xCluHEPVeto->     GetTime()   ; //xTimeHEPVeto -= fTimeOffsetHEPVeto[xChIdHEPVeto];
			 
		  hSvc->FillHisto("EVeto1Cl_HEPVeto_DeltaTime",xTimeEVeto-xTimeHEPVeto);                    
                
		}

	    }
	}
    }
  return true;
}

Bool_t EventSelection::ProcessAnalysisGC()
{  

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  Bool_t isMC=false;
  //std::cout<<"in ProcessAnalysis ... evsel ... "<<fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<std::endl;
  if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //std::cout<<"input data are simulatetd "<<std::endl;
  }
  //std::cout<<"input data are not simulatetd "<<std::endl;
  
  TRecoVHit*             xHit;
  TRecoVHit*             yHit;
  TRecoVCluster*         xClu;
  //TRecoVCluster*         aClu;
  //TRecoVCluster*         yClu;
  TRecoVCluster*    xCluPVeto;
  TRecoVCluster*   xCluPVeto1;
  TRecoVCluster*   xCluEVeto1;
  TRecoVCluster*    xCluEVeto;
  TRecoVCluster*  xCluHEPVeto;
  //TRecoVCluster*  xCluHEPVeto1;
  TRecoVCluster*      xCluSAC1;
  Double_t xTime, xEne, xEnePVeto, xTimePVeto, xTimePVeto1,xTimeEVeto, xTimeEVeto1,xTimeHEPVeto, xEneEVeto, xTimeSAC1;//, yTime, yEne, aEne, aTime, xTimeHEPVeto1,  xTimeLinCorr,
  Int_t xChId, yChId, yTime;

  TRecoVCluster* xClu1;
  //TRecoVCluster* aClu1;
  //TRecoVCluster* yClu1;
  Double_t xTime1, xEne1;//, xEneHEPVeto;//, xEneHEPVeto1;
  //Double_t xTimeLinCorr1, aTime1, aEne1, yEne1, yTime1;
  Int_t xChId1, xChIdPVeto, xChIdPVeto1, xChIdHEPVeto, xChIdEVeto, xChIdEVeto1, xChIdSAC1;//xChIdHEPVeto1, 
  //Int_t yChId1, aChId1;

  TVector3 Pos     ;
  TVector3 Pos1    ;
  TVector3 PosHEP  ;
  TVector3 PosHEP1 ;
  TVector3 PosPVeto;
  TVector3 PosEVeto;
  TVector3 PosSAC  ;

  //if (fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) std::cout<<"input data are simulatetd "<<std::endl;
  //else std::cout<<"input data are NOT simulated "<<std::endl;
      
  //      Double_t eSumECalHits=0.;
  //      Double_t xEnergy=0;
  std::string hname;


  
  hSvc->FillHisto("CountEvents", 5 ); 


  //EVeto Cluster 1
  /*
    for (int hEVeto=0; hEVeto<fEVeto_ClColl->GetNElements(); ++hEVeto)
    {
    //std::cout<<" cluster n. hEVeto = "<<hEVeto<<" pointer to collection = "<<(long)fEVeto_ClColl<<std::endl;
    xClu = fEVeto_ClColl->Element((int)hEVeto);
    if (xClu) 
    {
    xChId  = xClu->GetChannelId()        ;
    xEne   = xClu->   GetEnergy()        ; 
    xTime  = xClu->     GetTime()        ; if(MC){xTime -= 8.5;} else{ xTime -= fTimeOffsetEVeto[xChId];}
          
    hSvc->FillHisto("EVetoCl_E", xEne ); 

    //Search for PVeto in time
                
    for (int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
    {
    xCluPVeto = fPVeto_ClColl->Element((int)hPVeto);
    if (xCluPVeto) 
    {
	       
    xChIdPVeto = xCluPVeto->GetChannelId()   ;
    xTimePVeto = xCluPVeto->     GetTime()   ; if(MC){;} else{xTimePVeto -= fTimeOffsetPVeto[xChIdPVeto];}
    Pos         =xCluPVeto-> GetPosition()   ;
    hSvc->FillHisto("EVeto1Cl_PVeto_DeltaTime",xTime-xTimePVeto);

    if(fabs(xTime-xTimePVeto+1.8*0+1.*0-6.7*0)<1){
    hSvc->FillHisto2("EVeto1Cl_E_vs_PVetoChId", xChIdPVeto,  xEne);
    hSvc->FillHisto2("EVeto1Cl_E_vs_PVetoPosz",    Pos.z(),  xEne);		 
    }
    }

    }


    //Search for HEPVeto in time
                
    for (int hPVeto=0; hPVeto<fHEPVeto_ClColl->GetNElements(); ++hPVeto)
    {
    xCluPVeto = fHEPVeto_ClColl->Element((int)hPVeto);
    if (xCluPVeto) 
    {
	       
    xChIdPVeto = xCluPVeto->GetChannelId();
    xTimePVeto = xCluPVeto->     GetTime(); if(MC){xTimePVeto -= 8.5+2.64-5.27;} else{xTimePVeto -= fTimeOffsetHEPVeto[xChIdPVeto];}
    PosHEP     =xCluPVeto-> GetPosition()   ;                
    hSvc->FillHisto("EVeto_HEPVeto_DeltaTime",xTime-xTimePVeto);
    if(fabs(xTime-xTimePVeto-7*0+15*0)<1)hSvc->FillHisto2("EVeto1Cl_E_vs_HEPVetoPosz", PosHEP.z(),  xEne);
    }

    } 



    }
    }

  */
  //cout << fRecoEvent->GetEventNumber() << endl;


  // HEPVeto cluster vs HEPVeto and vs PVeto 
  Proc_HEPVeto_vs_PVeto(isMC);

  // SAC cluster vs SAC and vs all 
  Proc_SACclu_vs_all(isMC);

  // SAC hit vs PVeto hit and cluster 
  Proc_SAChit_vs_PVeto(isMC);

  // ECal cluster vs all
  Proc_ECalclu_vs_all(isMC);

  // PVeto Clusters
  Proc_PVetoclu_vs_AllVetos(isMC);
                

  //EVeto
  Proc_EVetoclu_vs_HEPVeto(isMC);
                
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
	  xClu  ->SetTime(xTime-fTimeOffsetECal[xChId]);
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
      if (!fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_BEAM)) return passed;
    }
  hSvc->FillHisto(hname,ps_cut_trg);
      
  std::string hname1 = "NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT());
  
  if (fTarget_RecoBeam->getnPOT()<10000.) return passed;
  hSvc->FillHisto(hname,ps_cut_POT);
  
  hname = "nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT())); 


  passed = true;
  return passed;
}
