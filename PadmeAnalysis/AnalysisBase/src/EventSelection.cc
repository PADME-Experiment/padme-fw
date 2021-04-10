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
  else if (fProcessingMode==3)
 {
  return InitHistosAnalysis();
  }
  else if ( fProcessingMode > 3 || fProcessingMode < 0 )
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
  /*
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
  else if (fProcessingMode==3)
    {
      return ProcessAnalysisTimeCalib();
    }
  else if ( fProcessingMode > 3 || fProcessingMode < 0 )
    {
      std::cout<<"Error:: fProcessingModel = "<<fProcessingMode<<" out of range "<<std::endl;
      return true;
    }*/
  return ProcessAnalysis();
}

Bool_t EventSelection::ProcessAnalysis()
{
 // CalibrateTimeAndEnergy();

 /* if (fVersion==1)
    {
      return ProcessAnalysisSS();
    }*/
  if (fVersion==3)
    {
      return ProcessAnalysisTimeCalib();
    }
  else {std::cout<<" EventSelection: version not set ....................... choose 1 for ProcessAnalysisSS / 2 for ProcessAnalysisGC"<<std::endl;}
  return true;
}

Bool_t EventSelection::ProcessAnalysisTimeCalib()
{  
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit* xHit;
  TRecoVHit* yHit;
  TRecoVCluster* xClu;
  TRecoVCluster* aClu;
  TRecoVCluster* yClu;
  Double_t xTime, yTime, xEne, yEne, aEne, aTime;
  Int_t xChId, yChId, aChId;

  char name[60];

  std::string hname;

//PVetoSACHits

  
  for (unsigned int hPVeto=0; hPVeto<fPVeto_hitEvent->GetNHits(); ++hPVeto)
    {
      xHit = fPVeto_hitEvent->Hit(hPVeto);
      xTime= xHit->GetTime();
      xChId= xHit->GetChannelId();
      hname="timePVetoVsCh_Hits";
      hSvc->FillHisto2(hname, xTime, (float)xChId);
      	
      for (unsigned int hSAC=0; hSAC<fSAC_hitEvent->GetNHits(); ++hSAC)
	{
	  yHit = fSAC_hitEvent->Hit(hSAC);
	  if (yHit->GetChannelId()!=22) continue;
	  yTime= yHit->GetTime();
	  hname="DtimePVetoVsSAC22_Hits";
          hSvc->FillHisto(hname, xTime-yTime);
          hname="DiffTimeSACVsPVeto_HitsVsChId";
          hSvc->FillHisto2(hname, xChId, xTime-yTime, 1.);  
          for (int k=0; k<90; k++)
             {
              if(k==xChId)
               {
                sprintf(name,"DiffSAC22_PVeto%d",k);
                 if(yHit->GetChannelId()==22) hSvc->FillHisto(name, xTime-yTime);
                 }
             }           
	}
}
 //Cluster  PVeto SAC
  for (unsigned int hPVeto=0; hPVeto<fPVeto_ClColl->GetNElements(); ++hPVeto)
    {
      xClu = fPVeto_ClColl->Element(hPVeto);
      xTime= xClu->GetTime();
      xChId= xClu->GetChannelId();
  

      for (unsigned int hSAC=0; hSAC<fSAC_ClColl->GetNElements(); ++hSAC)
	{
	  yClu = fSAC_ClColl->Element(hSAC);
	  yTime= yClu->GetTime();
	  yChId= yClu->GetChannelId();
	  yEne = yClu->GetEnergy();

          float diffPVeto=yTime-xTime;
   
          hname="DiffTimeSACVsPVeto_Clus";
          hSvc->FillHisto(hname, diffPVeto, 1.);
          hname="DiffTimeSACVsPVeto_ClusVsChId";
          hSvc->FillHisto2(hname, xChId, diffPVeto, 1.);
          //hSvc->FillHisto(hname, yTime-xTime, 1.);
                       
	}//end loop on SAC clusters

       
    }//PVetoCluster
     

  //EVetoSACHits
  for (unsigned int hEVeto=0; hEVeto<fEVeto_hitEvent->GetNHits(); ++hEVeto)
    {
      xHit = fEVeto_hitEvent->Hit(hEVeto);
      xTime= xHit->GetTime();
      xChId= xHit->GetChannelId();
      
      
      for (unsigned int hSAC=0; hSAC<fSAC_hitEvent->GetNHits(); ++hSAC)
	{
	  yHit = fSAC_hitEvent->Hit(hSAC);
	  if (yHit->GetChannelId()!=22) continue;
	  yTime= yHit->GetTime();        
          hname="DtimeEVetoVsSAC22_Hits";
          hSvc->FillHisto(hname, xTime-yTime);
          hname="DiffTimeSACVsEVeto_HitsVsChId";
          hSvc->FillHisto2(hname, xChId, xTime-yTime, 1.);
   	}  
     } 

 //HEPVetoSAC Hits
 for (unsigned int hHEPVeto=0; hHEPVeto<fHEPVeto_hitEvent->GetNHits(); ++hHEPVeto)
    {
      xHit = fHEPVeto_hitEvent->Hit(hHEPVeto);
      xTime= xHit->GetTime();
      xChId= xHit->GetChannelId();
      
      
      for (unsigned int hSAC=0; hSAC<fSAC_hitEvent->GetNHits(); ++hSAC)
	{
	  yHit = fSAC_hitEvent->Hit(hSAC);
	  
	  yTime= yHit->GetTime();

          for (int k=0; k<32; k++)
             {
              if(k==xChId)
               {
               sprintf(name,"DiffSAC22_HEPVeto%d",k);
               if(yHit->GetChannelId()==22) hSvc->FillHisto(name, xTime-yTime);
               
                }
              }
	  
          hname="DtimeHEPVetoVsSAC22_Hits";
          if(yHit->GetChannelId()==22) hSvc->FillHisto(hname, xTime-yTime);
	}  
}

   //Cluster  HEPVeto SAC
  for (unsigned int hHEPVeto=0; hHEPVeto<fHEPVeto_ClColl->GetNElements(); ++hHEPVeto)
    {
      xClu = fHEPVeto_ClColl->Element(hHEPVeto);
      xTime= xClu->GetTime();
      xChId= xClu->GetChannelId();
      xEne=  xClu->GetEnergy();
      
      for (unsigned int hSAC=0; hSAC<fSAC_ClColl->GetNElements(); ++hSAC)
	{
	  yClu = fSAC_ClColl->Element(hSAC);
	  yTime= yClu->GetTime();
	  yChId= yClu->GetChannelId();
	  yEne = yClu->GetEnergy();
	  
	  hname="timeSACVsHEPVeto_Clus";
	  hSvc->FillHisto2(hname, xTime, yTime);

          hname="DiffTimeSACVsHEPVeto_Clus";
          hSvc->FillHisto(hname, yTime-xTime, 1.);        
        
	  double diffHEPVeto;
          
          char name[60];
   /*       for (int ichid=0; ichid<32; ichid++)
         {
          
          if(ichid==xChId) 
           { 
             diffHEPVeto=yTime-xTime;
             sprintf(name,"DiffClSACVsHEPVeto_ChId%d",ichid); 
             hSvc->FillHisto(name, diffHEPVeto, 1.);
            }
         }*/
            
        } //endSACClusters
}//endHEPVetoClusters

   for (unsigned int hPVeto=0; hPVeto<fPVeto_hitEvent->GetNHits(); ++hPVeto)
    {
      xHit = fPVeto_hitEvent->Hit(hPVeto);
      xTime= xHit->GetTime();
      xChId= xHit->GetChannelId();
      hname="timePVetoVsCh_Hits";
      hSvc->FillHisto2(hname, xTime, (float)xChId);
      	
      for (unsigned int hECal=0; hECal<fECal_hitEvent->GetNHits(); ++hECal)
	{
         
          yHit = fECal_hitEvent->Hit(hECal);
          yTime= yHit->GetTime();
          hname="DtimeECalVsPVeto41_Hits";
          if(xHit->GetChannelId()==41) hSvc->FillHisto(hname, xTime-yTime); 
         
     /*    for (int x=0; x<28; k++)
             {
            for (int y=0; y<28; k++)
             {
              if(x==yChId/100 && ==yChId%100)
               {
               sprintf(name,"DiffPVeto41_ECal%d",k);
               if(xHit->GetChannelId()==41) hSvc->FillHisto(name, xTime-yTime);
               
                }
              }*/
         
         
          }
    }
     for (unsigned int hECal=0; hECal<fECal_hitEvent->GetNHits(); ++hECal)
    {
      xHit = fECal_hitEvent->Hit(hECal);
      xTime= xHit->GetTime();
      xChId= xHit->GetChannelId();
     
      
      for (unsigned int hSAC=0; hSAC<fSAC_hitEvent->GetNHits(); ++hSAC)
	{
	  yHit = fSAC_hitEvent->Hit(hSAC);
	  if (yHit->GetChannelId()!=22) continue;

	  yTime= yHit->GetTime();

          hname="DtimeECALVsSAC22_Hits";
          hSvc->FillHisto(hname, yTime-xTime, 1.);

        }
   }

    ///PVetoECalClusters
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

          hname="DiffTimeECALVsPVeto_Clus";
          hSvc->FillHisto(hname, yTime-xTime, 1.);	
	  }        

	}

   return true;

}
Bool_t EventSelection::InitHistosAnalysis()
{

 HistoSvc* hSvc =  HistoSvc::GetInstance();


  std::string hname;


  char name[60];
  
  for(int i = 0;i<90;i++) {
    
    sprintf(name,"DiffSAC22_PVeto%d",i);
    hSvc->BookHisto(name, 200, -50, 50);
    //sprintf(name,"DiffSAC22_EVeto%d",i);
    //hSvc->BookHisto(name, 200, -50, 50);
    //sprintf(name,"DiffClSACVsPVeto_ChId%d",i); 
    //hSvc->BookHisto(name, 200, -50, 50);
  }

  for(int i = 0;i<32;i++) {
    
    sprintf(name,"DiffSAC22_HEPVeto%d",i);
    hSvc->BookHisto(name, 200, -50, 50);
   // sprintf(name,"DiffClSACVsHEPVeto_ChId%d",i); 
   // hSvc->BookHisto(name, 200, -50, 50);

  }
 
 /* for(int i = 0;i<616;i++) {
  sprintf(name,"DiffPVeto41_ECal%d",i);
  hSvc->BookHisto(name, 200, -50, 50);
  }*/

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
  hname="DtimePVetoVsSAC22_Hits";
  hSvc->BookHisto(hname, 200, -20., 20.);
  hname="DtimeECalVsPVeto41_Hits";
  hSvc->BookHisto(hname, 200, -20., 20.);
  hname="DtimeHEPVetoVsSAC22_Hits";
  hSvc->BookHisto(hname, 200, -20., 20.);
  hname="DtimeECALVsSAC22_Hits";
  hSvc->BookHisto(hname, 200, -20., 20.);
  hname="DtimeEVetoVsSAC22_Hits";
   hSvc->BookHisto(hname, 200, -20., 20.);
  hname="timeECalVsPVeto_Clus";
  nBinX=  200;
  nBinY=  200;
  minX = -300;
  maxX =  300;
  minY = -300;
  maxY =  300;
  hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
  hname="timeSACVsPVeto_Clus";
  hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);
  hname="timeSACVsHEPVeto_Clus";
  hSvc->BookHisto2(hname, nBinX, minX, maxX, nBinY, minY, maxY);

  nBinX =  200;
  minX  =  -20;
  maxX  =   20;
  hname="DiffTimeSACVsPVeto_Clus";
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="DiffTimeECALVsPVeto_Clus";
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="DiffTimeSACVsHEPVeto_Clus";
  hSvc->BookHisto(hname, nBinX, minX, maxX);
  hname="DiffTimeECALVsHEPVeto_Clus";
  hSvc->BookHisto(hname, nBinX, minX, maxX);

  hname="DiffTimeSACVsPVeto_ClusVsChId";
  hSvc->BookHisto2(hname, 90, 0, 90, nBinX, minX, maxX);
  hname="DiffTimeSACVsPVeto_HitsVsChId";
  hSvc->BookHisto2(hname, 90, 0, 90, nBinX, minX, maxX);
  hname="DiffTimeSACVsEVeto_HitsVsChId";
  hSvc->BookHisto2(hname, 90, 0, 90, nBinX, minX, maxX);

   

  return true;

}
/*

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
     hname="BTFtrigger"; 
     hSvc->BookHisto(hname, 130, 0., 129);
     hname="CRtrigger"; 
     hSvc->BookHisto(hname, 130, 0., 129);

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

  std::string hname;
 // std::string hname="PreSelectCutFlow";
 // hSvc->FillHisto(hname,ps_cut_all);

 
 // if(fRecoEvent->GetTriggerMask()==1) {  hname="BTFtrigger"; hSvc->FillHisto(hname, fRecoEvent->GetTriggerMask(),1.);}
 // if(fRecoEvent->GetTriggerMask()==2) {  hname="CRtrigger"; hSvc->FillHisto(hname, fRecoEvent->GetTriggerMask(), 1. );}
   
  
  if(fRecoEvent->GetTriggerMask()==1) {  hSvc->FillHisto("BTFtrigger", fRecoEvent->GetTriggerMask(),1.);}
  if(fRecoEvent->GetTriggerMask()==2) {  hSvc->FillHisto("CRtrigger", fRecoEvent->GetTriggerMask(), 1. );}
   
  

  if (!fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) 
    {
      // std::cout<<"in passPreselection, event status bit  "<< fRecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<< std::endl;
      if (!fRecoEvent->GetTriggerMaskBit(TRECOEVENT_TRIGMASKBIT_BEAM)) return passed;
    }
  hSvc->FillHisto(hname,ps_cut_trg);

  
  std::string hname1 = "NposInBunch_beam";
  hSvc->FillHisto(hname1,fTarget_RecoBeam->getnPOT());
  //std::cout<<"preselection...Pot control" << std::endl;
  //if (fTarget_RecoBeam->getnPOT()<12000. && fTarget_RecoBeam->getnPOT()>3500.) return passed;
  //if (fTarget_RecoBeam->getnPOT()<10000.) return passed;
  hSvc->FillHisto(hname,ps_cut_POT);
  
  hname = "nPOT";
  hSvc->FillHisto(hname,0,float(fTarget_RecoBeam->getnPOT())); 

  //std::cout<<"passed ok " << std::endl;
  passed = true;
  return passed;
}


*/


