#include "ThreeGammaAnalysis.hh"
#include "HistoSvc.hh"
#include <iostream>

static ThreeGammaAnalysis ThisAnalyser("ThreeGammaAnalysis");

ThreeGammaAnalysis::ThreeGammaAnalysis(const char *s,int valid, int verb)
  : PadmeVAnalyser(s,valid,verb)
{;}

ThreeGammaAnalysis::ThreeGammaAnalysis(const char *s):
  PadmeVAnalyser(s)
{;}

Bool_t ThreeGammaAnalysis::InitHistos(){
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->makeFileDir(GetName());
  
  hSvcVal->BookHisto(this->GetName()+"_ECal_ClusterElements",100,0.0,50.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_ClusterEnergy",100,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2ClusterEnergy",100,0.0,1000.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_2ClusterEnergy_TimeCut",100,0.0,1000.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_ClusterX",100,-400.0,400.0);
  hSvcVal->BookHisto(this->GetName()+"_ECal_ClusterY",100,-400.0,400.0);
  hSvcVal->BookHisto2(this->GetName()+"_ECal_ClusterXY",100,-400.0,400.0,100,-400.0,400.0);

  hSvcVal->BookHisto(this->GetName()+"_ECal_ClusterTime",100,0.0,200.0);
  return true;
}


Bool_t ThreeGammaAnalysis::Process(){
  HistoSvc* hSvc =  HistoSvc::GetInstance();

  if(! evt->ECalRecoCl){
    return fResult = false;
  }
  // NUMBER OF HITS
  int NElements = evt->ECalRecoCl->GetNElements();
  hSvc->FillHisto(this->GetName()+"_ECal_ClusterElements",NElements);

  // loop over clusters
  for(int icl = 0; icl < NElements; icl++){
    TRecoVCluster *ECalClus = evt->ECalRecoCl->Element(icl);
    
    // ENERGY
    double clusEnergy = ECalClus->GetEnergy();
    hSvc->FillHisto(this->GetName()+"_ECal_ClusterEnergy",clusEnergy);

    // TIME
    double clusTime = ECalClus->GetTime();
    hSvc->FillHisto(this->GetName()+"_ECal_ClusterTime",clusTime);

    // POSITION
    TVector3 clusPos = ECalClus->GetPosition();
    hSvc->FillHisto(this->GetName()+"_ECal_ClusterX",clusPos[0]);
    hSvc->FillHisto(this->GetName()+"_ECal_ClusterY",clusPos[1]);
    hSvc->FillHisto2(this->GetName()+"_ECal_ClusterXY",clusPos[0],clusPos[1]);

    // loop over second cluster -- calculate E1 + E2
    for(int icl2 = icl+1; icl2 < NElements; icl2++){
      TRecoVCluster *ECalClus2 = evt->ECalRecoCl->Element(icl2);
      double clus2Energy = ECalClus2->GetEnergy();
      double EnergySum = clusEnergy + clus2Energy;
      hSvc->FillHisto(this->GetName()+"_ECal_2ClusterEnergy",EnergySum);

      // apply time cut
      double clus2Time = ECalClus2->GetTime();
      if(abs(clusTime - clus2Time) < 10){
        hSvc->FillHisto(this->GetName()+"_ECal_2ClusterEnergy_TimeCut",EnergySum);
      }
    }

  }


  fResult = true;
  return fResult;
}
