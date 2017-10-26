// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "TargetReconstruction.hh"
#include "TargetRecoEvent.hh"
#include "TargetRecoHit.hh"

#include "TargetParameters.hh"

#include "TTargetMCEvent.hh"
#include "TTargetMCHit.hh"
#include "TTargetMCDigi.hh"

#include <stdlib.h>
#include <cmath>  


TargetReconstruction::TargetReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "Target", ConfigFileName)
{
  //fRecoEvent = new TRecoTargetEvent();
  //  fRecoEvent = new TargetRecoEvent();
  //ParseConfFile(ConfigFileName);
}

TargetReconstruction::~TargetReconstruction()
{;}

void TargetReconstruction::Init(PadmeVReconstruction* MainReco)
{

  //common part for all the subdetectors
  PadmeVReconstruction::Init(MainReco);

}

// Read Target reconstruction parameters from a configuration file
void TargetReconstruction::ParseConfFile(TString ConfFileName) {

  std::ifstream confFile(ConfFileName.Data());
  if (!confFile.is_open()) {
    perror(ConfFileName);
    exit(1);
  }

  TString Line;
  while (Line.ReadLine(confFile)) {
    if (Line.BeginsWith("#")) continue;
  }
  confFile.close();
}


void TargetReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TTargetMCEvent* tTargetEvent = (TTargetMCEvent*)tEvent;
  std::cout << "--- TargetReconstruction --- run/event/#hits/#digi " << tTargetEvent->GetRunNumber() << " " << tTargetEvent->GetEventNumber() << " " << tTargetEvent->GetNHits() << " " << tTargetEvent->GetNDigi() << std::endl;

  MakeTargetRecoHitCollection(tTargetEvent);

  MakeTargetRecoEvent();
}



void TargetReconstruction::MakeTargetRecoHitCollection(TTargetMCEvent* tTargetEvent)
{

  TargetParameters* param = TargetParameters::GetInstance();
  double sigmaQi = param->GetSigmaQi();
  double timeerror = param->GetTimeError();

  bool doEff = param->GetTargetEfficiencyEnabled();

  if(doEff){
    double effi=0.;
    double effi0=0.;
    double bad=0.;
    
    // compute efficiency {{{{{{{
    for (Int_t iH=0; iH<tTargetEvent->GetNHits(); iH++) {
      TTargetMCHit* hit = (TTargetMCHit*)tTargetEvent->Hit(iH);
      int hitval=hit->GetHitVal();
      if(hitval<0)
	bad++;
      else
	if(hitval==2)
	  effi++;
	else
	  effi0++;
      //  hit->Print();
    }
    effi /= tTargetEvent->GetNHits();
    effi0 /= tTargetEvent->GetNHits();
    bad /= tTargetEvent->GetNHits();
    std::cout<< "- Target Efficiency: "<<effi<<" lost: "<<effi0<<" bad: "<<bad<<std::endl;
    
  }// 

  // ERROR ON BEAM CENTER
  // sigmax = sqrt(sum [ (x_i-X)/Qtot)^2 * sigmaQi^2 ])
  //double sigmaQi = (1e-9); // 1 nC
  ////2PARS

  fTargetRecoHitCollection =  new std::vector<TargetRecoHit*>;
  for (Int_t iD=0; iD<tTargetEvent->GetNDigi(); iD++) {
    //
    TTargetMCDigi* digi = (TTargetMCDigi*)tTargetEvent->Digi(iD);
    digi->Print();
    
    TargetRecoHit* lRecoHit = new TargetRecoHit(digi->GetChannelId(),digi->GetTime(),timeerror, digi->GetCharge(), sigmaQi);
    fTargetRecoHitCollection->push_back(lRecoHit);
  }

  std::cout<< "TargetReconstruction::MakeTargetRecoHitCollection: size of reco hit collection is "<<fTargetRecoHitCollection->size()<<" sigma"<<sigmaQi<<std::endl;
}





void TargetReconstruction::MakeTargetRecoEvent()
{

  //  double evtime=100.;
  double baryx=0.;
  double baryy=0.;
  double xerr=0.;
  double yerr=0.;
  double baryxerr=0.;
  double baryyerr=0.;
  double weix=0.;
  double weiy=0.;
  double weixerr=0.;
  double weiyerr=0.;
  double targetTime = 1000000.;
  double timeerror = 0.;

  // geometry {
  // CHANNEL CENTERS for charge weighted position
  double offset =-7500;   // for nchx=16 offset=-first-(nchx/2-1)*pitch
  double pitch  = 1000.; // um
  int nchx=16;
  //int nchh=nchx-1;
  double chc[nchx];
  for (int j=0;j<nchx;j++){
    chc[j]=offset+j*pitch; //channel center X (or Y) coord [um] from center of target
  }
  //}


  fRecoEvent = new TargetRecoEvent();
  std::vector<TargetRecoHit*>::const_iterator iRH = fTargetRecoHitCollection->begin();
  for (;iRH!=fTargetRecoHitCollection->end();++iRH){

    // target time is the smallest time among the target reco hits
    if ((*iRH)->getTime()<targetTime) targetTime=(*iRH)->getTime();

    timeerror=(*iRH)->getTimeError();

    int chId =(*iRH)->getChannelId();
    if (chId<nchx){
      baryx += chc[chId]*(*iRH)->getCharge();
      weix  += (*iRH)->getCharge();
    }
    else {
      chId=chId-nchx;
      baryy += chc[chId]*(*iRH)->getCharge();
      weiy  += (*iRH)->getCharge();
    }
  }
  baryx /= weix;
  baryy /= weiy;

  //

  std::vector<TargetRecoHit*>::const_iterator iRH2 = fTargetRecoHitCollection->begin();
  for (;iRH2!=fTargetRecoHitCollection->end();++iRH2){

    int chId =(*iRH2)->getChannelId();
    double err2 = (*iRH2)->getChargeError()*(*iRH2)->getChargeError();

    /*
    xerr += (chId<=nchh?(chc[chId]-baryx):0)*(chId<=nchh?(chc[chId]-baryx):0) * err2;
    yerr += (chId>nchh?(chc[chId]-baryy):0)*(chId>nchh?(chc[chId]-baryy):0) * err2;
    */

    
    if (chId<nchx){
      xerr += ( chc[chId] - baryx )*( chc[chId] - baryx )*err2;
      weixerr += err2;
    }
    else {
      chId=chId-nchx;
      yerr += ( chc[chId] - baryy )*( chc[chId] - baryy )*err2;
      weiyerr += err2;
    }
    

  }
  baryxerr = sqrt(xerr)/weix;
  baryyerr = sqrt(yerr)/weiy;
  weixerr = sqrt(weixerr);
  weiyerr = sqrt(weiyerr);

  std::cout << "- Beamcenter X Y: " << baryx <<" +- "<<baryxerr<< " " 
	    << baryy<<" +- "<<baryyerr
	    <<" evtime: "<<std::setprecision(4)<< targetTime << std::endl;



  fRecoEvent->setX(baryx);
  fRecoEvent->setY(baryy);
  fRecoEvent->setXError(baryxerr);
  fRecoEvent->setYError(baryyerr);
  fRecoEvent->setChargeX(weix);
  fRecoEvent->setChargeY(weiy);
  fRecoEvent->setChargeXErr(weixerr);
  fRecoEvent->setChargeYErr(weiyerr);
  fRecoEvent->setTime(targetTime);
  fRecoEvent->setTimeErr(timeerror);
 


 
  std::cout << "TargetReconstruction::MakeTargetRecoEvent " << fRecoEvent->getX()<<" +- "<<fRecoEvent->getXError()
	    << " " << fRecoEvent->getY()<<" +- "<< fRecoEvent->getYError()
	    <<" chargeX "<< fRecoEvent->getChargeX()<<" chargeY "<< fRecoEvent->getChargeY()
	    <<" evtime: "<<std::setprecision(4)<<  fRecoEvent->getTime()<< std::endl;

}


void TargetReconstruction::EndProcessing()
{;}
