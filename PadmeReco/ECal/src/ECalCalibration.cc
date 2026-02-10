// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
// Modified by G. Piperno (gabriele.piperno@roma1.infn.it) 2019-10-10 
// --------------------------------------------------------------
#include "Riostream.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ECalCalibration.hh"
#include "TCanvas.h"
#include "TRecoVHit.hh"
#include "TRawEvent.hh"

#include <string>

 
ECalCalibration::ECalCalibration()
  : PadmeVCalibration()
{
  std::cout<<"ECalCalibration being created ............"<<std::endl ;
 
}



ECalCalibration::~ECalCalibration()
{
  fT0Map.clear();
  fCalibMap.clear();
  fCalibMapRun.clear();
}



void ECalCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){

  PadmeVCalibration::Init(cfg, chIdMgr );
  
  fUseCalibE   = (int)cfg->GetParOrDefault("EnergyCalibration","UseCalibration",1);
  fGlobEnScale = (double)cfg->GetParOrDefault("EnergyCalibration","AveragepCMeV",15.);
  fOverrideEScale   = (int)cfg->GetParOrDefault("EnergyCalibration","OverrideEScale",1);
  fGlobHitEnScaleOverrideData = (double)cfg->GetParOrDefault("EnergyCalibration","HitGlobalScaleOverrideData",1.);
  fGlobHitEnScaleMC   = (double)cfg->GetParOrDefault("EnergyCalibration","HitGlobalScaleMC",1.);
  fCalibList = (std::string)cfg->GetParOrDefault("EnergyCalibration","EnergyCalibIntervalsList","ECalEnergyCalibTimeIntervals.txt");
  fCalibRunList = (std::string)cfg->GetParOrDefault("EnergyCalibration","EnergyCalibRunList","ECalEnergyCalibRunList.txt");
  fCalibVersion = (std::string)cfg->GetParOrDefault("EnergyCalibration","CalibVersion","8");
  //std::cout<<" ma giarda un po' "<<fCalibVersion<<std::endl;
  fUseCalibT   = (int)cfg->GetParOrDefault("TimeAlignment","UseTimeAlignment",1);


  // Energy calibration //

  if(fUseCalibE){

    fMuonDepositedEnergy=17.5;
    //    fGlobEnScale=15;

    /*
  fUseCalibT   = (int)cfg->GetParOrDefault("TimeAlignment","UseTimeAlignment",0);
  fCalibVersion = (int)cfg->GetParOrDefault("EnergyCalibration","CalibVersion",3);
  // Energy calibration 
  
  if(fUseCalibE) {
    char fname[256];
    sprintf(fname,"config/Calibration/ECalEnergyCalibration_%d.dat",fCalibVersion);
    ECalib.open(fname);
  }
  // if(fUseCalibE==1) ECalib.open("config/Calibration/ECalCalibConst.txt");
  // if(fUseCalibE==2) ECalib.open("config/Calibration/equalization_constants2.dat");
  */
  
    if(fCalibVersion=="0") { // calib file from event time

      fCalibHandler = ECalCalibHandler::GetInstance();
      // set energy calibration list file
      char path[256];
      sprintf(path,"config/Calibration/%s",fCalibList.c_str());
      fCalibHandler->SetFileList(path);
      std::cout<<"List file in use: "<<fCalibHandler->GetFileList()<<std::endl;
      // calibration initialisation
      fCalibHandler->Initialise();
      
    } else if(fCalibVersion!="0" &&fCalibVersion!="8") { // same calib file for all the events
      
      char fname[256];
      //      sprintf(fname,"config/Calibration/%s",fCalibVersion.c_str());
      sprintf(fname,"config/Calibration/ECalEnergyCalibration_%s.dat",fCalibVersion.c_str());
      ECalib.open(fname);
      std::cout<<"Using a USER SELECTED energy calibration file for ECal: "
	       <<fname<<std::endl;
      
      if(!ECalib.is_open()){
        std::cout<<"ERROR: Cannot find ECal  file "<<"**************"<<std::endl;
        exit(1);
      }
    }
  }
  // Time offsets calibration 
  if(fUseCalibT==1) TCalib.open("config/Calibration/ECalTimeOffSets.txt");
  if(fUseCalibT==1 && !TCalib.is_open()){ 
    std::cout<<"ERROR: Cannot find ECal time offset file "<<"**************"<<std::endl;
  }


  if((fUseCalibE>0 && fCalibVersion!="0") || fUseCalibT==1) ReadCalibConstant();

  fRunDependentScale = new TGraph();
  fRunDependentScale->SetName("RunDependentECalEnScale");
  std::ifstream RunDependentCalib; 
  std::string line;

  RunDependentCalib.open("config/Calibration/ECalRunDependent.dat");
  Int_t runid;
  Double_t enScale;
  while(getline(RunDependentCalib,line)){
    std::stringstream(line) >> runid >> enScale; 
    //      std::cout <<" "<<runid <<" "<< enScale << std::endl;
    fRunDependentScale->SetPoint(fRunDependentScale->GetN(),runid,enScale);
  }
  RunDependentCalib.close();
}



void ECalCalibration::ReadCalibConstant()
{
  double MIPCharge,TimeOffSet;
  int NBD,CID;
  int row,col;


  //Read Energy calibration constants if a specific file has been selected
  if(fCalibVersion!="0" && fCalibVersion!="8"){
    if(ECalib.is_open()){
      for(int i=0;i<616;i++){
	ECalib >> row >> col >> NBD >> CID >> MIPCharge; 
  //std::cout <<" "<<row <<" "<< col << " "<< NBD << " "<<CID << " "<<MIPCharge<<" "<<std::endl;;   //reads Piperno informations need cross-check
	fCalibMap[std::make_pair(NBD,CID)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
	//fCalibMap[std::make_pair(row,col)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
	//std::cout<<i<<" channel ID "<<CID<<" NBD "<<NBD<<" "<<fCalibMap[std::make_pair(NBD,CID)]<<std::endl;
      }
      ECalib.close();
    } else{ 
      std::cout << "================ WARNING!!! No ECal energy calibration file available "<<std::endl;
    } 
  }else if(fCalibVersion=="8"){ //Handling of Run IV partially broken units alog the data-taking EDM
      char nameList[256];
      //      sprintf(fname,"config/Calibration/%s",fCalibVersion.c_str());
      sprintf(nameList,"config/Calibration/%s",fCalibRunList.c_str());
      ECalibList.open(nameList); 
      std::string line;

      if(ECalibList.is_open()){
         while(getline(ECalibList,line)){
          int run=0;
          ECalibList >> run;
          fRunList.push_back(run); 

        }
        sort(fRunList.begin(), fRunList.end()); //prior sorting of the input runs to be sure it follows the time flow
        
        ECalibList.close();
        for(UInt_t iR =0; iR < fRunList.size(); iR++){
          char nameiR[256];
          sprintf(nameiR,"config/Calibration/ECalEnergyCalibration_8_%d.dat",fRunList.at(iR));
          ECalib.open(nameiR);
          if(ECalib.is_open()){
            std::map < std::pair<int,int>,double> CalibMapTemp;
            for(int i=0;i<616;i++){
              ECalib >> row >> col >> NBD >> CID >> MIPCharge; 
              //std::cout <<" "<<row <<" "<< col << " "<< NBD << " "<<CID << " "<<MIPCharge<<" "<<std::endl;;   //reads Piperno informations need cross-check
              CalibMapTemp[std::make_pair(NBD,CID)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
              //fCalibMap[std::make_pair(row,col)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
              //std::cout<<i<<" channel ID "<<CID<<" NBD "<<NBD<<" "<<fCalibMap[std::make_pair(NBD,CID)]<<std::endl;
              }
            fCalibMapRun[fRunList.at(iR)]= CalibMapTemp;
            ECalib.close(); 
            

        }else{ 
          std::cerr<<" ERROR: ECAL Calib version 8 needs a calibration file containing the run number"<< fRunList.at(iR)<< " to work when units broke!!! Exiting..."<<std::endl;
          exit(1);
        }
      }
      for (const auto& runIt : fCalibMapRun) {
          std::cout << "Run " << runIt.first << ":\n";

          const auto& calibMap = runIt.second;
          for (const auto& chIt : calibMap) {
              std::cout << "  (BD=" << chIt.first.first
                        << ", ChID=" << chIt.first.second
                        << ") -> " << chIt.second << '\n';
          }
      }
      }else{
          std::cerr<<" ERROR: ECAL Calib version 8 needs a file containing the run numbers when units broke!!! Exiting..."<<std::endl;
          exit(1);
      }



  }

  //Read Time calibration constants
  if(TCalib.is_open()){
    for(int i=0;i<616;i++){
      TCalib >> row >> col >> NBD >> CID>>TimeOffSet;
      fT0Map[std::make_pair(NBD,CID)] = TimeOffSet;
      //fCalibMap[std::make_pair(row,col)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
      //std::cout<<i<<" channel ID "<<CID<<" NBD "<<NBD<<" "<<fT0Map[std::make_pair(NBD,CID)]<<std::endl;
    }
    TCalib.close();
  } else{ 
    std::cout << "================ WARNING!!! No ECal time calibration file available "<<std::endl;
  } 
  
}

void ECalCalibration::PerformMCCalibration(std::vector<TRecoVHit *> &Hits){
  for(unsigned int iHit = 0;iHit < Hits.size();++iHit){
    // Energy calibration //
    if (fUseCalibE > 0){
      //int ich = Hits[iHit]->GetChannelId(); //need to convert into BDID e CHID
      // unsigned int BD   = Hits[iHit]->getBDid(); 
      // unsigned int ChID = Hits[iHit]->getCHid();

      fHitE   = Hits[iHit]->GetEnergy();
      fHitECalibrated = fHitE/fGlobHitEnScaleMC;
      Hits[iHit]->SetEnergy(fHitECalibrated);
    }
  }
}
 
void ECalCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits, TRawEvent* rawEv)
{
  if(fOverrideEScale) fGlobHitEnScaleData = fGlobHitEnScaleOverrideData;
  else {
    if (fRunDependentScale->GetN()) {
      fGlobHitEnScaleData = fRunDependentScale->Eval(rawEv->GetRunNumber());
    }
    else {
      std::cout << "ECalCalibration >> PerformCalibration ERROR! Cannot retrieve run-dependent energy scale" << std::endl;
      exit(1);
    }
  }

  static int PRINTED = 0; 
  for(unsigned int iHit = 0;iHit < Hits.size();++iHit){
    // Energy calibration //
    if (fUseCalibE > 0){

      int ich = Hits[iHit]->GetChannelId(); //need to convert into BDID e CHID
      unsigned int BD   = Hits[iHit]->getBDid(); 
      unsigned int ChID = Hits[iHit]->getCHid();

      fHitE   = Hits[iHit]->GetEnergy();

      // Correcting for different crystals response
      if(fCalibVersion=="0"){
          TTimeStamp time = rawEv->GetEventAbsTime();
          /*
          std::cout<<"Hit "<<iHit<<", time: "<<time<<" (day "<<time.GetDate()
            <<", hour "<<time.GetTime()<<")"<<std::endl;
          */
          fCalibHandler->SetTimeInterval(time);
          fHitECalibrated = fHitE*(fCalibHandler->GetCalibVal(BD,ChID))*fMuonDepositedEnergy*fGlobEnScale/fGlobHitEnScaleData;
          Hits[iHit]->SetEnergy(fHitECalibrated);
          //std::cout<<"channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<" "<<fHitECalibrated<<" "<<(fCalibHandler->GetCalibVal(BD,ChID))*fMuonDepositedEnergy*fGlobEnScale<<std::endl;
          
        } else if(fCalibVersion!="0" && fCalibVersion!="8"){ // fCalibVersion != "0"
            if(fCalibMap[std::make_pair(BD,ChID)]!=0){ 
              fHitECalibrated = fHitE/fCalibMap[std::make_pair(BD,ChID)]/fGlobHitEnScaleData;
              Hits[iHit]->SetEnergy(fHitECalibrated);
            }else{
                std::cout<<"Missing ECal energy calibration for channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<std::endl;
                PRINTED++;
              }
	  //std::cout<<"channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<" "<<fHitECalibrated<<" "<<1./fCalibMap[std::make_pair(BD,ChID)]<<std::endl;
        }else if(fCalibVersion=="8"){
              int runN = rawEv->GetRunNumber();
              
              auto it = fCalibMapRun.upper_bound(runN); //finds the closest run the map above it
              //std::cout<<"it: "<<it->first<<std::endl;
              if (it != fCalibMapRun.begin()) { //checks if i'm not already at the map beginning
                
                --it; //goes to the run immediately before 
                std::map < std::pair<int,int>,double> CalibMap = it->second;
                if(CalibMap[std::make_pair(BD,ChID)]!=0){ 
                  fHitECalibrated = fHitE/CalibMap[std::make_pair(BD,ChID)]/fGlobHitEnScaleData;
                  Hits[iHit]->SetEnergy(fHitECalibrated);
                  //std::cout<<"BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<" "<<fHitECalibrated<<" "<<1./CalibMap[std::make_pair(BD,ChID)]<<" runN: "<<runN<<" runCalib: "<<it->first<< std::endl;
              }
            }
        }
      }
     
    // Time calibration //    
    if (fUseCalibT == 1){
      // Correcting for time offestets in between channels
      fHitT          = Hits[iHit]->GetTime();
      fHitTCorrected = fHitT-fT0Map[std::make_pair(fBID,fChID)];
      std::cout<<" HitT "<<fHitT<<" "<<fHitTCorrected<<std::endl;
      Hits[iHit]->SetTime(fHitTCorrected);
    }
    else{
    fHitT          = Hits[iHit]->GetTime();
    fHitTCorrected = fHitT-fCommonT0;
    //std::cout<<" HitT "<<fHitT<<" "<<fHitTCorrected<<std::endl;
    Hits[iHit]->SetTime(fHitTCorrected);
    }
  } 

}
