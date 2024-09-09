// Written by M. Raggi   28/03/2022 
// Added GetInstance by M. Raggi 2/06/2022
#include "MCTruthECal.hh"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "TMath.h"
#include "TString.h"
#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>



//Added the Finstance to allow to use the information in all the enalyzers
MCTruthECal* MCTruthECal::fInstance = 0;
MCTruthECal* MCTruthECal::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new MCTruthECal(); }
  return fInstance;
}

MCTruthECal::MCTruthECal(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating MCTruthECal\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

MCTruthECal::~MCTruthECal(){
  delete fCfgParser;
  //delete fRndm;
}

Bool_t MCTruthECal::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing MCTruthECal\n");
  fEvent = event;
  fECal_clEvent = fEvent->ECalRecoCl; 
  fGeneralInfo = GeneralInfo::GetInstance();
  fXMin = -21.*(14+0.5);
  fXMax = 21.*(14+0.5);
  fXW = 21; // mm
  fNXBins = (fXMax-fXMin)/fXW;
  fYMin = -21.*(14+0.5);
  fYMax = 21.*(14+0.5);
  fYW = 21; // mm
  fNYBins = (fYMax-fYMin)/fYW;

  TString mapname = "Analyzers/ECal_map_2.txt";
  

  std::ifstream MapFile(mapname.Data());
  std::string line;
  if(MapFile.is_open()){
    int SU,px,py,BN,CH;
    while(getline(MapFile,line)){
      std::stringstream(line) >> SU >> px >> py >> BN >> CH;
      fCellMap[100*px+py] = 1;
      
      }
  }else{
    std::cout<<"file 'Analyzers/ECal_map_2.txt' not found -->check your paths"<<std::endl;
    exit(1);
  }

  InitHistos();
  hBrem->GetXaxis()->SetLimits(0,400);
  heIoni->GetXaxis()->SetLimits(0,400);
  hAnnihil->GetXaxis()->SetLimits(0,400);
  return true;
}

Bool_t MCTruthECal::InitHistos(){
  //MCTruthECal directory will contain all histograms related to this analysis
  //Histograms for MCTruthECal information
  Int_t    NBinE=600.;
  Double_t MaxE=NBinE;
  static int NprocessAvailable = 5;
  TString processIDs[NprocessAvailable]={"eIoni", "eBrem", "annihil", "Bhabha", "NoVtx"};

  fHS->CreateList("MCTruthECal");
  fHS->BookHistoList("MCTruthECal","DTCluVtx",100,-10.,10.); 
  fHS->BookHistoList("MCTruthECal","NPcleOut",10,0,10);	
  fHS->BookHistoList("MCTruthECal","ProcessID",4,-0.5,3.5);	
  fHS->BookHistoList("MCTruthECal","dRCluVtx",1200,0.,TMath::Sqrt(((fXMax+fXMin)*(fXMax+fXMin))+((fYMax+fYMin)*(fYMax+fYMin))));
  fHS->BookHistoList("MCTruthECal","dXCluVtx",1200,-2.*fXMax,2.*fXMax);
  fHS->BookHistoList("MCTruthECal","dYCluVtx",1200,-2.*fYMax,2.*fYMax);
  fHS->BookHisto2List("MCTruthECal","dXvsPxovPzCluVtx",600,-3,3, 1200,-2.*fYMax,2.*fYMax);
  fHS->BookHisto2List("MCTruthECal","dYvsPyovPzCluVtx",600,-3,3, 1200,-2.*fYMax,2.*fYMax);
  fHS->BookHistoList("MCTruthECal","EPcle",100,0,400);
  fHS->BookHistoList("MCTruthECal","EPcleAss",100,0,400);
  fHS->BookHisto2List("MCTruthECal",Form("DEvsEtrue"),100, 0,300, 100, -100,100 );


  fHS->BookHistoList("MCTruthECal","EPcleTag",200,0,400);
  fHS->BookHistoList("MCTruthECal","EPcleTagandProbe",200,0,400);
  fHS->BookHistoList("MCTruthECal","EbeamE1E0",400,-400,400);
  fHS->BookHistoList("MCTruthECal","EbeamE1E0Ass",400,-400,400);
  
  fHS->BookHistoList("MCTruthECal","EbeamE1E0Ass_eBrem",200,-50,50);
  fHS->BookHistoList("MCTruthECal","EbeamE1E0Ass_annihil",200,-50,50);
  fHS->BookHistoList("MCTruthECal","EbeamE1E0Ass_eIoni",200,-50,50);
  fHS->BookHistoList("MCTruthECal","EbeamE1E0Ass_Bhabha",200,-50,50);

  fHS->BookHistoList("MCTruthECal","EPcle_eBrem",100,0,400);
  fHS->BookHistoList("MCTruthECal","EPcleAss_eBrem",100,0,400);
  fHS->BookHistoList("MCTruthECal","EPcle_annihil",100,0,400);
  fHS->BookHistoList("MCTruthECal","EPcleAss_annihil",100,0,400);
  fHS->BookHistoList("MCTruthECal","EPcle_eIoni",100,0,400);
  fHS->BookHistoList("MCTruthECal","EPcleAss_eIoni",100,0,400);
  fHS->BookHistoList("MCTruthECal","EPcle_Bhabha",100,0,400);
  fHS->BookHistoList("MCTruthECal","EPcleAss_Bhabha",100,0,400);
  
  fHS->BookHisto2List("MCTruthECal","XYmap",30,0,30,30,0,30);
  fHS->BookHisto2List("MCTruthECal","XYmapEw",30,0,30,30,0,30);
  fHS->BookHisto2List("MCTruthECal","XYmapAss",30,0,30,30,0,30);
  fHS->BookHisto2List("MCTruthECal","XYmapEwAss",30,0,30,30,0,30);
  fHS->BookHistoList("MCTruthECal","dECluVtx",2000,-fGeneralInfo->GetBeamEnergy(),fGeneralInfo->GetBeamEnergy());
  fHS->BookHisto2List("MCTruthECal","dEvsECluVtx",1000,0,fGeneralInfo->GetBeamEnergy(),1000,-fGeneralInfo->GetBeamEnergy(),fGeneralInfo->GetBeamEnergy());
  hAnnihil = fHS->BookHisto2List("MCTruthECal","dEvsECluVtx_annihil",1000,0,400,1000,-400,400);
  fHS->BookHisto2List("MCTruthECal","dEvsECluVtx_Bhabha",1000,0,400,1000,-400,400);
  hBrem    = fHS->BookHisto2List("MCTruthECal","dEvsECluVtx_eBrem",1000,0,400,1000,-400,400);
  heIoni   = fHS->BookHisto2List("MCTruthECal","dEvsECluVtx_eIoni",1000,0,400,1000,-400,400);

  fHS->BookHistoList("MCTruthECal",Form("DeltaCOGX_annihil"),600, -300, 300);
  fHS->BookHistoList("MCTruthECal",Form("DeltaCOGY_annihil"), 600, -300, 300);
  fHS->BookHisto2List("MCTruthECal",Form("DeltaCOGXvsCOGY_annihil") ,600, -300, 300 ,600, -300, 300);

  fHS->BookHistoList("MCTruthECal",Form("DeltaCOGX_eBrem"),600, -300, 300);
  fHS->BookHistoList("MCTruthECal",Form("DeltaCOGY_eBrem"), 600, -300, 300);
  fHS->BookHisto2List("MCTruthECal",Form("DeltaCOGXvsCOGY_eBrem") ,600, -300, 300 ,600, -300, 300);

  fHS->BookHistoList("MCTruthECal",Form("DeltaCOGX_eIoni"),600, -300, 300);
  fHS->BookHistoList("MCTruthECal",Form("DeltaCOGY_eIoni"), 600, -300, 300);
  fHS->BookHisto2List("MCTruthECal",Form("DeltaCOGXvsCOGY_eIoni") ,600, -300, 300 ,600, -300, 300);

  fHS->BookHistoList("MCTruthECal",Form("DeltaCOGX_Bhabha"),600, -300, 300);
  fHS->BookHistoList("MCTruthECal",Form("DeltaCOGY_Bhabha"), 600, -300, 300);
  fHS->BookHisto2List("MCTruthECal",Form("DeltaCOGXvsCOGY_Bhabha") ,600, -300, 300 ,600, -300, 300);


  fHS->BookHisto2List("MCTruthECal","Chi2PositionvsEPcle_annihil",2000,0, 400, 1000, 0, 100);
  fHS->BookHisto2List("MCTruthECal","Chi2PositionvsEPcle_Bhabha",2000,0, 400, 1000, 0, 100);
  fHS->BookHisto2List("MCTruthECal","Chi2PositionvsEPcle_eIoni",2000,0, 400, 1000, 0, 100);
  fHS->BookHisto2List("MCTruthECal","Chi2PositionvsEPcle_eBrem",2000,0, 400, 1000, 0, 100);
  fHS->BookHisto2List("MCTruthECal","Chi2PositionvsEPcle",2000,0, 400, 1000, 0, 100);

  fHS->BookHistoList("MCTruthECal","Chi2Position",1000, 0, 100);
  fHS->BookHistoList("MCTruthECal","NPcleClu", 5, -2.5, 2.5);
  fHS->BookHistoList("MCTruthECal","NCluVtx", 10, 0, 10);
  fHS->BookHistoList("MCTruthECal","dXAss",1200,-2.*fXMax,2.*fXMax);
  fHS->BookHistoList("MCTruthECal","dYAss",1200,-2.*fYMax,2.*fYMax);
 
  return true;
}

Bool_t MCTruthECal::Process(){
  // MCTruthECal analysis
  
  // particle codes: 22 photon 11 electrons positron -11
  if(fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)){
    //std::cout<<"Event: "<<fEvent->RecoEvent->GetEventNumber()<<std::endl;
    CorrelateVtxClu();
    return true;
  }
}



Bool_t MCTruthECal::CorrelateVtxClu(){
  VtxCluCorr.clear(); //Vtx-Clu 1-N correspondence
  CluVtxCorr.clear(); //Cluster-Vtx 1-1 correspondence
  VtxPcleCluCorr.clear();
  
  TVector3 cluPos, VtxPos;

  TRecoVCluster* clu;
  double cluEnergy;
  double mcIPartE;
  double VtxTime;
  double cluTime =-99;
  int NCluVtx    =0;
  double cVal = 30.; //cm/ns

  double TOFoffset = 13.6; //ns --> TOF offset moving towards the calorimeter
  const double DTlow     = -0.09;//4.0; //from fit on DT
  const double DTup      = 7.27;//6.0; //from fit on DT
  const double muX       =  1.442; //0.868;//1.389; //from fit on DX
  const double muY       = -0.0731;//-0.100; //from fit on DY
  const double sigmaX    = 10.57;//7.232; //from fit on DX
  const double sigmaY    = 10.5;//7.632; //from fit on DY

  std::vector<std::vector<Int_t>> VtxVector;
  // Int_t CluEmpty[2]={-1,1};

  //Loop on vertex
  for(Int_t iV = 0; iV < fEvent->MCTruthEvent->GetNVertices(); iV++) {
      std::vector<Int_t> CluforiV;
      VtxVector.push_back(CluforiV);
      // VtxPcleCluCorr.push_back(CluEmpty);

      Int_t CluPcleOut[2]={-1,-1}; //2 is the maximum number of plces out from a vertex, initialized to -1 so that when a pcle is associated with a cluster the value becomes 1
      Int_t CluPcleOutFlag[2]={-1,-1}; //2 is the maximum number of plces out from a vertex, initialized to -1 so that when a pcle is associated with a cluster the value becomes 1
      Int_t Extrapolation[2]={-1,-1};
      Double_t EPcleOut[2]={-1,-1};
      TVector3 cluPosOut[2];
      Double_t ECluOut[2] ={-1,-1};
      mcVtx = fEvent->MCTruthEvent->Vertex(iV);
      
      VtxPos = mcVtx->GetPosition();
      //add
      fHS->FillHistoList("MCTruthECal","NPcleOut",mcVtx->GetNParticleOut(),1.);	
       if(mcVtx->GetProcess() =="eBrem"){
          fHS->FillHistoList("MCTruthECal","ProcessID",0.,1.);
          }else if(mcVtx->GetProcess() =="eIoni"){ 
          fHS->FillHistoList("MCTruthECal","ProcessID",1.,1.);
          }else if(mcVtx->GetProcess()=="annihil"){
          fHS->FillHistoList("MCTruthECal","ProcessID",2.,1.);
          }else{
            fHS->FillHistoList("MCTruthECal","ProcessID",3.,1.);	
          } 
          
      //pcleOut, x e y all'ecal
      //loop on vtx pcle out
      for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {

          TMCParticle* mcOPart = mcVtx->ParticleOut(iO);
        
          Double_t pcleE=mcOPart->GetEnergy();
          TVector3 pclePos = VtxPos;

          TVector3 pcleMom =mcOPart->GetMomentum();
          
          TVector3 VtxPosAtCalo;
          VtxPosAtCalo.SetZ(fGeneralInfo->GetCOG().Z()-72.8); //removed 6.5X0 faccia calorimetro
          VtxPosAtCalo.SetX(pclePos.X()+((pcleMom.X()/pcleMom.Z())*(VtxPosAtCalo.Z()-pclePos.Z())));
          VtxPosAtCalo.SetY(pclePos.Y()+((pcleMom.Y()/pcleMom.Z())*(VtxPosAtCalo.Z()-pclePos.Z())));
       
          int icellX = VtxPosAtCalo.X()/cellSize+0.5 + ncells/2;
          int icellY = VtxPosAtCalo.Y()/cellSize+0.5 + ncells/2;
          
          if(icellX>ncells || icellX<0) continue;
          if(icellY>ncells || icellY<0) continue;
  

          // Double_t PhiPcle0 = TMath::ATan2(VtxPosAtCalo.Y(),VtxPosAtCalo.X());

          //if(abs((abs(PhiPcle0)-TMath::Pi()/2))<TMath::Pi()/6) continue;

          // if (icellY > 26) continue; //magnet shadow for the 1st clu
          // if (icellY < 4) continue; //magnet shadow for the 1st clu
          

          if(fCellMap[100*icellX+icellY]!=1) continue; //controlla mappa calorimetro

          // if (VtxPosAtCalo.Perp() < fGeneralInfo->GetRadiusMin()) continue; // cluster should be within the radius range of the 2gamma cluster pair //for tag and probe comparison
          // if (VtxPosAtCalo.Perp() > fGeneralInfo->GetRadiusMax()) continue; // cluster should be within the radius range of the 2gamma cluster pair
          

          // if (pcleE < fGeneralInfo->GetEnergyMin()) continue; // cluster should be within the energy range of the 2gamma cluster pair //for tag and probe comparison
          // if (pcleE > fGeneralInfo->GetEnergyMax()) continue; // cluster should be within the energy range of the 2gamma cluster pair
          
          
          
          Extrapolation[iO]=1;
          EPcleOut[iO]=pcleE;
          

          fHS->FillHisto2List("MCTruthECal","XYmap",icellX,icellY, 1.);
          fHS->FillHisto2List("MCTruthECal","XYmapEw",icellX,icellY, pcleE);
          fHS->FillHistoList("MCTruthECal","EPcle",pcleE, 1.);
          fHS->FillHistoList("MCTruthECal",Form("EPcle_%s",mcVtx->GetProcess().Data()),pcleE);
          Double_t Rpcle = TMath::Sqrt((VtxPosAtCalo.X()*VtxPosAtCalo.X())+(VtxPosAtCalo.Y()*VtxPosAtCalo.Y()));

          TOFoffset = TMath::Sqrt(((Rpcle*Rpcle)+(fGeneralInfo->GetCOG().Z()*fGeneralInfo->GetCOG().Z())))/(10*cVal); // ns -->c is in cm/ns, R is in mm and 
          //std::cout<<"Rpcle: "<<Rpcle<<" fGeneralInfo->GetCOG().Z(): "<<fGeneralInfo->GetCOG().Z()<<" TOFoffset:"<<TOFoffset<<std::endl;
          VtxTime   = mcVtx->GetTime()+TOFoffset; 
          fHS->FillHistoList("MCTruthECal","DTCluVtx",cluTime-VtxTime,1.);

            //Loop on clu
          for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
              NCluVtx =0;
              clu = fECal_clEvent->Element((int)h1);
              cluEnergy = clu->GetEnergy();
              cluTime = clu->GetTime();
              cluPos.SetXYZ(
                  clu->GetPosition().X(),
                  clu->GetPosition().Y(),fGeneralInfo->GetCOG().Z()); 
              cluPosOut[h1] = cluPos;
                  if((cluTime-VtxTime)>DTlow && (cluTime-VtxTime)<DTup){        

                    Double_t DeltaR = TMath::Sqrt(((VtxPosAtCalo.X()-cluPos.X())*(VtxPosAtCalo.X()-cluPos.X()))+((VtxPosAtCalo.Y()-cluPos.Y())*(VtxPosAtCalo.Y()-cluPos.Y())));
                    Double_t DeltaX = VtxPosAtCalo.X()-cluPos.X();
                    Double_t DeltaY = VtxPosAtCalo.Y()-cluPos.Y();

                    fHS->FillHistoList("MCTruthECal","dXCluVtx",DeltaX,1.);
                    fHS->FillHistoList("MCTruthECal","dYCluVtx",DeltaY,1.);
                    fHS->FillHistoList("MCTruthECal","dRCluVtx",DeltaR,1.);
                    fHS->FillHisto2List("MCTruthECal","dXvsPxovPzCluVtx",pcleMom.X()/pcleMom.Z() ,DeltaX,1.);
                    fHS->FillHisto2List("MCTruthECal","dYvsPyovPzCluVtx", pcleMom.Y()/pcleMom.Z(),DeltaY,1.);
                    fHS->FillHistoList("MCTruthECal","dECluVtx",pcleE-cluEnergy,1.);
                    fHS->FillHisto2List("MCTruthECal","dEvsECluVtx",pcleE, pcleE-cluEnergy, 1.);
                    fHS->FillHisto2List("MCTruthECal",Form("dEvsECluVtx_%s",mcVtx->GetProcess().Data()),pcleE, pcleE-cluEnergy, 1.);
                    
                    double Chi2X= (muX-DeltaX)/sigmaX;
                    double Chi2Y= (muY-DeltaY)/sigmaY;

                    fHS->FillHistoList("MCTruthECal","Chi2Position",TMath::Sqrt((Chi2X*Chi2X)+(Chi2Y*Chi2Y)), 1.);
                    fHS->FillHisto2List("MCTruthECal","Chi2PositionvsEPcle",pcleE, TMath::Sqrt((Chi2X*Chi2X)+(Chi2Y*Chi2Y)), 1.);
                    fHS->FillHisto2List("MCTruthECal",Form("Chi2PositionvsEPcle_%s",mcVtx->GetProcess().Data()),pcleE, TMath::Sqrt((Chi2X*Chi2X)+(Chi2Y*Chi2Y)), 1.);
                    
                    if(TMath::Sqrt((Chi2X*Chi2X)+(Chi2Y*Chi2Y))<6){
                      NCluVtx +=1;
                      if(NCluVtx<2){ //prendo il primo che soddisfa il Chi2 nel range
                        CluVtxCorr.insert({h1, iV});
                        //VtxVector.at(iV).push_back(h1);
                        ECluOut[iO]= cluEnergy;
                        VtxVector.at(iV).push_back(h1);
                        CluPcleOutFlag[iO]= 1; //flagga il cluster associato
                        CluPcleOut[iO]= h1; //salva indice cluster degli associati
                        fHS->FillHistoList("MCTruthECal","dXAss",DeltaX,1.);
                        fHS->FillHistoList("MCTruthECal","dYAss",DeltaY,1.);

                      } //chiude NCluVtx
                    } //chiude if Chi2
              } //chiude DT             
            } //chiude for clu
           
            //VtxPcleCluCorr[iV] = CluPcleOut;
            if(CluPcleOutFlag[iO]==1) {
              fHS->FillHistoList("MCTruthECal","EPcleAss",pcleE);
              fHS->FillHisto2List("MCTruthECal","XYmapAss",icellX,icellY, 1.);
              fHS->FillHisto2List("MCTruthECal","XYmapEwAss",icellX,icellY, pcleE);
              fHS->FillHistoList("MCTruthECal",Form("EPcleAss_%s",mcVtx->GetProcess().Data()),pcleE);
              fHS->FillHisto2List("MCTruthECal",Form("DEvsEtrue"),pcleE, ECluOut[iO]-pcleE,1.);

            }
            fHS->FillHistoList("MCTruthECal","NPcleClu",CluPcleOutFlag[iO]);
            fHS->FillHistoList("MCTruthECal","NCluVtx",NCluVtx);

          } //chiude Pcle
          if((Extrapolation[0]==1 && Extrapolation[1]==1) && CluPcleOutFlag[0]==1) {
            fHS->FillHistoList("MCTruthECal","EPcleTag",fGeneralInfo->GetBeamEnergy()-EPcleOut[0]);
            fHS->FillHistoList("MCTruthECal","EbeamE1E0",fGeneralInfo->GetBeamEnergy()-EPcleOut[0]-EPcleOut[1]);

            if(CluPcleOutFlag[1]==1){
                fHS->FillHistoList("MCTruthECal","EPcleTagandProbe",fGeneralInfo->GetBeamEnergy()-EPcleOut[0]);
                fHS->FillHistoList("MCTruthECal","EbeamE1E0Ass",fGeneralInfo->GetBeamEnergy()-EPcleOut[0]-EPcleOut[1]);
                fHS->FillHistoList("MCTruthECal",Form("EbeamE1E0Ass_%s",mcVtx->GetProcess().Data()),fGeneralInfo->GetBeamEnergy()-EPcleOut[0]-EPcleOut[1]);
                //cog

                TVector3 cog0= cluPosOut[0];
                cog0*= EPcleOut[0];
                TVector3 cog1 =cluPosOut[1];
                cog1*= EPcleOut[1];
                TVector3 cog = cog0+cog1;
                cog*= 1/(EPcleOut[0]+EPcleOut[1]);

                fHS->FillHistoList("MCTruthECal",Form("DeltaCOGX_%s",mcVtx->GetProcess().Data()), cog.X()- fGeneralInfo->GetCOG().X(), 1.);
                fHS->FillHistoList("MCTruthECal",Form("DeltaCOGY_%s",mcVtx->GetProcess().Data()),  cog.Y()- fGeneralInfo->GetCOG().Y(), 1.);
                fHS->FillHisto2List("MCTruthECal",Form("DeltaCOGXvsCOGY_%s",mcVtx->GetProcess().Data()), cog.X()- fGeneralInfo->GetCOG().X(), cog.Y()- fGeneralInfo->GetCOG().Y(), 1.);

                // if(fGeneralInfo->GetBeamEnergy()-EPcleOut[0]-EPcleOut[1]> 100){
                //   std::cout<<"EPcleOut[0]: "<<EPcleOut[0]<<" EPcleOut[1]: "<<EPcleOut[1]<<" Process:"<<mcVtx->GetProcess().Data()<<" ECluOut[0]: "<<ECluOut[0]<<" ECluOut[1]: "<<ECluOut[1]<<std::endl;
                // }

              }
          }
           //VtxPcleCluCorr.insert({iV, CluPcleOut});
            VtxPcleCluCorr.insert({iV, make_pair(CluPcleOut[0], CluPcleOut[1])});
      } //chiude Vtx
      
      // VtxCluCorr[iV]= VtxVector.at(iV);
// for(Int_t iV = 0; iV < fEvent->MCTruthEvent->GetNVertices(); iV++) {
//   mcVtx = fEvent->MCTruthEvent->Vertex(iV);
//   TVector3 VtxPosAtCalo;
//   VtxPosAtCalo.SetZ(fGeneralInfo->GetCOG().Z()-72.8); //removed 6.5X0 faccia calorimetro
  
//   //Double_t Rpcle = TMath::Sqrt((VtxPosAtCalo.X()*VtxPosAtCalo.X())+(VtxPosAtCalo.Y()*VtxPosAtCalo.Y()));

//   TOFoffset = (fGeneralInfo->GetCOG().Z()-72.8)/(10*cVal); // ns -->c is in cm/ns, R is in mm and 

//   std::cout<<"Vertex id:"<<iV<<" Time: "<<mcVtx->GetTime()+TOFoffset<<" Pos:"<<mcVtx->GetPosition().X()<<","<<mcVtx->GetPosition().Y()<<","<<mcVtx->GetPosition().Z()<<std::endl;
// }

//  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
//    clu = fECal_clEvent->Element((int)h1);
//       std::cout<<"Clu id: "<<h1<<" Vtx ass"<<GetVtxFromCluID(h1)<<" Time: "<<clu->GetTime()<<std::endl;
//     }

  return true;



}


Int_t MCTruthECal::GetVtxFromCluID(Int_t CluId){
    map<Int_t, Int_t>::iterator it;
    it = CluVtxCorr.find(CluId);
    if(it!= CluVtxCorr.end()){
      return it->second;
    }else{
      return -1;
    }


    // return CluVtxCorr[CluId];
  
  }
  
// std::vector<Int_t> MCTruthECal::GetCluFromVtxID(Int_t VtxId){
//   if(VtxId< VtxCluCorr.size()) return VtxCluCorr[VtxId];
//   }

// Int_t* MCTruthECal::GetCluPcleCorr(Int_t VtxId){

//     return VtxPcleCluCorr[VtxId];
  
// }

std::pair<Int_t,Int_t> MCTruthECal::GetCluPcleCorr(Int_t VtxId){

    std::map<Int_t, std::pair<Int_t,Int_t>>::iterator it;
    it = VtxPcleCluCorr.find(VtxId);
    if(it!= VtxPcleCluCorr.end()){
      return it->second;
    }else{
      return {-1,-1};
    }

    // return VtxPcleCluCorr[VtxId];
  
}


std::map<Int_t, std::pair<Int_t,Int_t>> MCTruthECal::GetCluPcleCorr_all(){

    return VtxPcleCluCorr;
  
}

// void MCTruthECal::GetCluPcleCorr(Int_t VtxId, Int_t &Val1, Int_t &Val2){
//     std::cout<<VtxId<<std::endl;
//     Val1 = VtxPcleCluCorr[VtxId][0];
//     Val2 = VtxPcleCluCorr[VtxId][1];
  
// }



Bool_t MCTruthECal::Finalize()
{
  
  if (fVerbose) printf("---> Finalizing MCTruthECal\n");
  return 1;
}



