#include <TTree.h>

#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

#include "StdNtuple.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StdNtuple::Book()
{

  fNtuple = HistoSvc::GetInstance()->BookNtupleList("TOP","PADME_Event");

  fNtuple->Branch("NTEvent_Run_Number"  , &(fMyEvt.NTEvent_Run_Number)  , "NTEvent_Run_Number/I"  );
  fNtuple->Branch("NTEvent_Event_Number", &(fMyEvt.NTEvent_Event_Number), "NTEvent_Event_Number/I");
  fNtuple->Branch("NTEvent_Reco_Time"   , &(fMyEvt.NTEvent_Reco_Time)   , "NTEvent_Reco_Time/D"   );
  fNtuple->Branch("NTEvent_Event_Time"  , &(fMyEvt.NTEvent_Event_Time)  , "NTEvent_Event_Time/D"  ); 
  fNtuple->Branch("NTEvent_Run_Clock"   , &(fMyEvt.NTEvent_Run_Clock)   , "NTEvent_Run_Clock/l"   );
  fNtuple->Branch("NTEvent_Event_Status", &(fMyEvt.NTEvent_Event_Status), "NTEvent_Event_Status/i");
  fNtuple->Branch("NTEvent_Trigger_Mask", &(fMyEvt.NTEvent_Trigger_Mask), "NTEvent_Trigger_Mask/i");

  fNtuple->Branch("NTNTarget_Hits",         &(fMyEvt.NTNTarget_Hits),         "NTNTarget_Hits/I"   );
  fNtuple->Branch("NTTarget_Hits_ChannelId",&(fMyEvt.NTTarget_Hits_ChannelId),"NTTarget_Hits_ChannelId[NTNTarget_Hits]/I");
  fNtuple->Branch("NTTarget_Hits_Energy",   &(fMyEvt.NTTarget_Hits_Energy),   "NTTarget_Hits_Energy[NTNTarget_Hits]/D");
  fNtuple->Branch("NTTarget_Hits_Time",     &(fMyEvt.NTTarget_Hits_Time),     "NTTarget_Hits_Time[NTNTarget_Hits]/D");
  fNtuple->Branch("NTTarget_Hits_Xpos",     &(fMyEvt.NTTarget_Hits_Xpos),     "NTTarget_Hits_Xpos[NTNTarget_Hits]/D");
  fNtuple->Branch("NTTarget_Hits_Ypos",     &(fMyEvt.NTTarget_Hits_Ypos),     "NTTarget_Hits_Ypos[NTNTarget_Hits]/D");
  fNtuple->Branch("NTTarget_Hits_Zpos",     &(fMyEvt.NTTarget_Hits_Zpos),     "NTTarget_Hits_Zpos[NTNTarget_Hits]/D");

  fNtuple->Branch("NTTargetBeam_X"    ,     &(fMyEvt.NTTargetBeam_X),         "NTTargetBeam_X/D");
  fNtuple->Branch("NTTargetBeam_XErr" ,     &(fMyEvt.NTTargetBeam_XErr),      "NTTargetBeam_XErr/D"); 
  fNtuple->Branch("NTTargetBeam_XW"   ,     &(fMyEvt.NTTargetBeam_XW),        "NTTargetBeam_XW/D");
  fNtuple->Branch("NTTargetBeam_XWErr",     &(fMyEvt.NTTargetBeam_XWErr),     "NTTargetBeam_XWErr/D");
  fNtuple->Branch("NTTargetBeam_Xfit" ,     &(fMyEvt.NTTargetBeam_Xfit),      "NTTargetBeam_Xfit/D");
  fNtuple->Branch("NTTargetBeam_XfitErr",   &(fMyEvt.NTTargetBeam_XfitErr),   "NTTargetBeam_XfitErr/D");
  fNtuple->Branch("NTTargetBeam_XWfit",     &(fMyEvt.NTTargetBeam_XWfit),     "NTTargetBeam_XWfit/D");
  fNtuple->Branch("NTTargetBeam_XWfitErr",  &(fMyEvt.NTTargetBeam_XWfitErr),  "NTTargetBeam_XWfitErr/D");
  fNtuple->Branch("NTTargetBeam_XChi2"   ,  &(fMyEvt.NTTargetBeam_XChi2),     "NTTargetBeam_XChi2/D");
  fNtuple->Branch("NTTargetBeam_XNdof"   ,  &(fMyEvt.NTTargetBeam_XNdof),     "NTTargetBeam_XNdof/D");
  fNtuple->Branch("NTTargetBeam_XCharge" ,  &(fMyEvt.NTTargetBeam_XCharge),   "NTTargetBeam_XCharge/D");
  fNtuple->Branch("NTTargetBeam_XChargeErr",&(fMyEvt.NTTargetBeam_XChargeErr),"NTTargetBeam_XChargeErr/D");
  fNtuple->Branch("NTTargetBeam_XTime"   ,  &(fMyEvt.NTTargetBeam_XTime),     "NTTargetBeam_XTime/D");
  fNtuple->Branch("NTTargetBeam_XTimeErr",  &(fMyEvt.NTTargetBeam_XTimeErr),  "NTTargetBeam_XTimeErr/D");
  					 	                          			   
  fNtuple->Branch("NTTargetBeam_Y" ,        &(fMyEvt.NTTargetBeam_Y),         "NTTargetBeam_Y/D");
  fNtuple->Branch("NTTargetBeam_YErr" ,     &(fMyEvt.NTTargetBeam_YErr),      "NTTargetBeam_YErr/D"); 
  fNtuple->Branch("NTTargetBeam_YW" ,       &(fMyEvt.NTTargetBeam_YW),        "NTTargetBeam_YW/D");
  fNtuple->Branch("NTTargetBeam_YWErr" ,    &(fMyEvt.NTTargetBeam_YWErr),     "NTTargetBeam_YWErr/D");
  fNtuple->Branch("NTTargetBeam_Yfit" ,     &(fMyEvt.NTTargetBeam_Yfit),      "NTTargetBeam_Yfit/D");
  fNtuple->Branch("NTTargetBeam_YfitErr" ,  &(fMyEvt.NTTargetBeam_YfitErr),   "NTTargetBeam_YfitErr/D");
  fNtuple->Branch("NTTargetBeam_YWfit" ,    &(fMyEvt.NTTargetBeam_YWfit),     "NTTargetBeam_YWfit/D");
  fNtuple->Branch("NTTargetBeam_YWfitErr" , &(fMyEvt.NTTargetBeam_YWfitErr),  "NTTargetBeam_YWfitErr/D");
  fNtuple->Branch("NTTargetBeam_YChi2" ,    &(fMyEvt.NTTargetBeam_YChi2),     "NTTargetBeam_YChi2/D");
  fNtuple->Branch("NTTargetBeam_YNdof" ,    &(fMyEvt.NTTargetBeam_YNdof),     "NTTargetBeam_YNdof/D");
  fNtuple->Branch("NTTargetBeam_YCharge" ,  &(fMyEvt.NTTargetBeam_YCharge),   "NTTargetBeam_YCharge/D");
  fNtuple->Branch("NTTargetBeam_YChargeErr",&(fMyEvt.NTTargetBeam_YChargeErr),"NTTargetBeam_YChargeErr/D");
  fNtuple->Branch("NTTargetBeam_YTime" ,    &(fMyEvt.NTTargetBeam_YTime),     "NTTargetBeam_YTime/D");
  fNtuple->Branch("NTTargetBeam_YTimeErr" , &(fMyEvt.NTTargetBeam_YTimeErr),  "NTTargetBeam_YTimeErr/D");

  fNtuple->Branch("NTTargetBeam_NPOT" ,   &(fMyEvt.NTTargetBeam_NPOT),   "NTTargetBeam_NPOT/D");
  fNtuple->Branch("NTTargetBeam_NPOTErr" ,&(fMyEvt.NTTargetBeam_NPOTErr),"NTTargetBeam_NPOTErr/D"); 

  fNtuple->Branch("NTNPVeto_Hits",         &(fMyEvt.NTNPVeto_Hits),         "NTNPVeto_Hits/I");
  fNtuple->Branch("NTPVeto_Hits_ChannelId",&(fMyEvt.NTPVeto_Hits_ChannelId),"NTPVeto_Hits_ChannelId[NTNPVeto_Hits]/I");
  fNtuple->Branch("NTPVeto_Hits_Energy",   &(fMyEvt.NTPVeto_Hits_Energy),   "NTPVeto_Hits_Energy[NTNPVeto_Hits]/D");
  fNtuple->Branch("NTPVeto_Hits_Time",     &(fMyEvt.NTPVeto_Hits_Time),     "NTPVeto_Hits_Time[NTNPVeto_Hits]/D");
  fNtuple->Branch("NTPVeto_Hits_Xpos",     &(fMyEvt.NTPVeto_Hits_Xpos),     "NTPVeto_Hits_Xpos[NTNPVeto_Hits]/D");
  fNtuple->Branch("NTPVeto_Hits_Ypos",     &(fMyEvt.NTPVeto_Hits_Ypos),     "NTPVeto_Hits_Ypos[NTNPVeto_Hits]/D");
  fNtuple->Branch("NTPVeto_Hits_Zpos",     &(fMyEvt.NTPVeto_Hits_Zpos),     "NTPVeto_Hits_Zpos[NTNPVeto_Hits]/D");

  fNtuple->Branch("NTNPVeto_Clusters",         &(fMyEvt.NTNPVeto_Clusters),         "NTNPVeto_Clusters/I");
  fNtuple->Branch("NTPVeto_Clusters_ChannelId",&(fMyEvt.NTPVeto_Clusters_ChannelId),"NTPVeto_Clusters_ChannelId[NTNPVeto_Clusters]/I");
  fNtuple->Branch("NTPVeto_Clusters_Energy",   &(fMyEvt.NTPVeto_Clusters_Energy),   "NTPVeto_Clusters_Energy[NTNPVeto_Clusters]/D");
  fNtuple->Branch("NTPVeto_Clusters_Time",     &(fMyEvt.NTPVeto_Clusters_Time),     "NTPVeto_Clusters_Time[NTNPVeto_Clusters]/D");
  fNtuple->Branch("NTPVeto_Clusters_Xpos",     &(fMyEvt.NTPVeto_Clusters_Xpos),     "NTPVeto_Clusters_Xpos[NTNPVeto_Clusters]/D");
  fNtuple->Branch("NTPVeto_Clusters_Ypos",     &(fMyEvt.NTPVeto_Clusters_Ypos),     "NTPVeto_Clusters_Ypos[NTNPVeto_Clusters]/D");
  fNtuple->Branch("NTPVeto_Clusters_Zpos",     &(fMyEvt.NTPVeto_Clusters_Zpos),     "NTPVeto_Clusters_Zpos[NTNPVeto_Clusters]/D");

  fNtuple->Branch("NTNEVeto_Hits",         &(fMyEvt.NTNEVeto_Hits),         "NTNEVeto_Hits/I");
  fNtuple->Branch("NTEVeto_Hits_ChannelId",&(fMyEvt.NTEVeto_Hits_ChannelId),"NTEVeto_Hits_ChannelId[NTNEVeto_Hits]/I");
  fNtuple->Branch("NTEVeto_Hits_Energy",   &(fMyEvt.NTEVeto_Hits_Energy),   "NTEVeto_Hits_Energy[NTNEVeto_Hits]/D");
  fNtuple->Branch("NTEVeto_Hits_Time",     &(fMyEvt.NTEVeto_Hits_Time),     "NTEVeto_Hits_Time[NTNEVeto_Hits]/D");
  fNtuple->Branch("NTEVeto_Hits_Xpos",     &(fMyEvt.NTEVeto_Hits_Xpos),     "NTEVeto_Hits_Xpos[NTNEVeto_Hits]/D");
  fNtuple->Branch("NTEVeto_Hits_Ypos",     &(fMyEvt.NTEVeto_Hits_Ypos),     "NTEVeto_Hits_Ypos[NTNEVeto_Hits]/D");
  fNtuple->Branch("NTEVeto_Hits_Zpos",     &(fMyEvt.NTEVeto_Hits_Zpos),     "NTEVeto_Hits_Zpos[NTNEVeto_Hits]/D");

  fNtuple->Branch("NTNEVeto_Clusters",         &(fMyEvt.NTNEVeto_Clusters),         "NTNEVeto_Clusters/I");
  fNtuple->Branch("NTEVeto_Clusters_ChannelId",&(fMyEvt.NTEVeto_Clusters_ChannelId),"NTEVeto_Clusters_ChannelId[NTNEVeto_Clusters]/I");
  fNtuple->Branch("NTEVeto_Clusters_Energy",   &(fMyEvt.NTEVeto_Clusters_Energy),   "NTEVeto_Clusters_Energy[NTNEVeto_Clusters]/D");
  fNtuple->Branch("NTEVeto_Clusters_Time",     &(fMyEvt.NTEVeto_Clusters_Time),     "NTEVeto_Clusters_Time[NTNEVeto_Clusters]/D");
  fNtuple->Branch("NTEVeto_Clusters_Xpos",     &(fMyEvt.NTEVeto_Clusters_Xpos),     "NTEVeto_Clusters_Xpos[NTNEVeto_Clusters]/D");
  fNtuple->Branch("NTEVeto_Clusters_Ypos",     &(fMyEvt.NTEVeto_Clusters_Ypos),     "NTEVeto_Clusters_Ypos[NTNEVeto_Clusters]/D");
  fNtuple->Branch("NTEVeto_Clusters_Zpos",     &(fMyEvt.NTEVeto_Clusters_Zpos),     "NTEVeto_Clusters_Zpos[NTNEVeto_Clusters]/D");

  fNtuple->Branch("NTNHEPVeto_Hits",         &(fMyEvt.NTNHEPVeto_Hits),         "NTNHEPVeto_Hits/I" );
  fNtuple->Branch("NTHEPVeto_Hits_ChannelId",&(fMyEvt.NTHEPVeto_Hits_ChannelId),"NTHEPVeto_Hits_ChannelId[NTNHEPVeto_Hits]/I");
  fNtuple->Branch("NTHEPVeto_Hits_Energy",   &(fMyEvt.NTHEPVeto_Hits_Energy),   "NTHEPVeto_Hits_Energy[NTNHEPVeto_Hits]/D");
  fNtuple->Branch("NTHEPVeto_Hits_Time",     &(fMyEvt.NTHEPVeto_Hits_Time),     "NTHEPVeto_Hits_Time[NTNHEPVeto_Hits]/D");
  fNtuple->Branch("NTHEPVeto_Hits_Xpos",     &(fMyEvt.NTHEPVeto_Hits_Xpos),     "NTHEPVeto_Hits_Xpos[NTNHEPVeto_Hits]/D");
  fNtuple->Branch("NTHEPVeto_Hits_Ypos",     &(fMyEvt.NTHEPVeto_Hits_Ypos),     "NTHEPVeto_Hits_Ypos[NTNHEPVeto_Hits]/D");
  fNtuple->Branch("NTHEPVeto_Hits_Zpos",     &(fMyEvt.NTHEPVeto_Hits_Zpos),     "NTHEPVeto_Hits_Zpos[NTNHEPVeto_Hits]/D");

  fNtuple->Branch("NTNHEPVeto_Clusters",         &(fMyEvt.NTNHEPVeto_Clusters),         "NTNHEPVeto_Clusters/I");
  fNtuple->Branch("NTHEPVeto_Clusters_ChannelId",&(fMyEvt.NTHEPVeto_Clusters_ChannelId),"NTHEPVeto_Clusters_ChannelId[NTNHEPVeto_Clusters]/I");
  fNtuple->Branch("NTHEPVeto_Clusters_Energy",   &(fMyEvt.NTHEPVeto_Clusters_Energy),   "NTHEPVeto_Clusters_Energy[NTNHEPVeto_Clusters]/D");
  fNtuple->Branch("NTHEPVeto_Clusters_Time",     &(fMyEvt.NTHEPVeto_Clusters_Time),     "NTHEPVeto_Clusters_Time[NTNHEPVeto_Clusters]/D");
  fNtuple->Branch("NTHEPVeto_Clusters_Xpos",     &(fMyEvt.NTHEPVeto_Clusters_Xpos),     "NTHEPVeto_Clusters_Xpos[NTNHEPVeto_Clusters]/D");
  fNtuple->Branch("NTHEPVeto_Clusters_Ypos",     &(fMyEvt.NTHEPVeto_Clusters_Ypos),     "NTHEPVeto_Clusters_Ypos[NTNHEPVeto_Clusters]/D");
  fNtuple->Branch("NTHEPVeto_Clusters_Zpos",     &(fMyEvt.NTHEPVeto_Clusters_Zpos),     "NTHEPVeto_Clusters_Zpos[NTNHEPVeto_Clusters]/D");
 
  fNtuple->Branch("NTNECal_Hits",             &(fMyEvt.NTNECal_Hits),         "NTNECal_Hits/I");
  fNtuple->Branch("NTECal_Hits_ChannelId",    &(fMyEvt.NTECal_Hits_ChannelId),"NTECal_Hits_ChannelId[NTNECal_Hits]/I");
  fNtuple->Branch("NTECal_Hits_Energy",       &(fMyEvt.NTECal_Hits_Energy),   "NTECal_Hits_Energy[NTNECal_Hits]/D");
  fNtuple->Branch("NTECal_Hits_Time",         &(fMyEvt.NTECal_Hits_Time),     "NTECal_Hits_Time[NTNECal_Hits]/D");
  fNtuple->Branch("NTECal_Hits_Xpos",         &(fMyEvt.NTECal_Hits_Xpos),     "NTECal_Hits_Xpos[NTNECal_Hits]/D");
  fNtuple->Branch("NTECal_Hits_Ypos",         &(fMyEvt.NTECal_Hits_Ypos),     "NTECal_Hits_Ypos[NTNECal_Hits]/D");
  fNtuple->Branch("NTECal_Hits_Zpos",         &(fMyEvt.NTECal_Hits_Zpos),     "NTECal_Hits_Zpos[NTNECal_Hits]/D");

  fNtuple->Branch("NTNECal_Clusters",         &(fMyEvt.NTNECal_Clusters),         "NTNECal_Clusters/I"); 
  fNtuple->Branch("NTECal_Clusters_ChannelId",&(fMyEvt.NTECal_Clusters_ChannelId),"NTECal_Clusters_ChannelId[NTNECal_Clusters]/I");
  fNtuple->Branch("NTECal_Clusters_Energy",   &(fMyEvt.NTECal_Clusters_Energy),   "NTECal_Clusters_Energy[NTNECal_Clusters]/D");
  fNtuple->Branch("NTECal_Clusters_Time",     &(fMyEvt.NTECal_Clusters_Time),     "NTECal_Clusters_Time[NTNECal_Clusters]/D");
  fNtuple->Branch("NTECal_Clusters_Xpos",     &(fMyEvt.NTECal_Clusters_Xpos),     "NTECal_Clusters_Xpos[NTNECal_Clusters]/D");
  fNtuple->Branch("NTECal_Clusters_Ypos",     &(fMyEvt.NTECal_Clusters_Ypos),     "NTECal_Clusters_Ypos[NTNECal_Clusters]/D");
  fNtuple->Branch("NTECal_Clusters_Zpos",     &(fMyEvt.NTECal_Clusters_Zpos),     "NTECal_Clusters_Zpos[NTNECal_Clusters]/D");


  fNtuple->Branch("NTNSAC_Hits",         &(fMyEvt.NTNSAC_Hits),         "NTNSAC_Hits/I");
  fNtuple->Branch("NTSAC_Hits_ChannelId",&(fMyEvt.NTSAC_Hits_ChannelId),"NTSAC_Hits_ChannelId[NTNSAC_Hits]/I");
  fNtuple->Branch("NTSAC_Hits_Energy",   &(fMyEvt.NTSAC_Hits_Energy),   "NTSAC_Hits_Energy[NTNSAC_Hits]/D");
  fNtuple->Branch("NTSAC_Hits_Time",     &(fMyEvt.NTSAC_Hits_Time),     "NTSAC_Hits_Time[NTNSAC_Hits]/D");
  fNtuple->Branch("NTSAC_Hits_Xpos",     &(fMyEvt.NTSAC_Hits_Xpos),     "NTSAC_Hits_Xpos[NTNSAC_Hits]/D");
  fNtuple->Branch("NTSAC_Hits_Ypos",     &(fMyEvt.NTSAC_Hits_Ypos),     "NTSAC_Hits_Ypos[NTNSAC_Hits]/D");
  fNtuple->Branch("NTSAC_Hits_Zpos",     &(fMyEvt.NTSAC_Hits_Zpos),     "NTSAC_Hits_Zpos[NTNSAC_Hits]/D");

  fNtuple->Branch("NTNSAC_Clusters",         &(fMyEvt.NTNSAC_Clusters),         "NTNSAC_Clusters/I" );
  fNtuple->Branch("NTSAC_Clusters_ChannelId",&(fMyEvt.NTSAC_Clusters_ChannelId),"NTSAC_Clusters_ChannelId[NTNSAC_Clusters]/I");
  fNtuple->Branch("NTSAC_Clusters_Energy",   &(fMyEvt.NTSAC_Clusters_Energy),   "NTSAC_Clusters_Energy[NTNSAC_Clusters]/D");
  fNtuple->Branch("NTSAC_Clusters_Time",     &(fMyEvt.NTSAC_Clusters_Time),     "NTSAC_Clusters_Time[NTNSAC_Clusters]/D");
  fNtuple->Branch("NTSAC_Clusters_Xpos",     &(fMyEvt.NTSAC_Clusters_Xpos),     "NTSAC_Clusters_Xpos[NTNSAC_Clusters]/D");
  fNtuple->Branch("NTSAC_Clusters_Ypos",     &(fMyEvt.NTSAC_Clusters_Ypos),     "NTSAC_Clusters_Ypos[NTNSAC_Clusters]/D");
  fNtuple->Branch("NTSAC_Clusters_Zpos",     &(fMyEvt.NTSAC_Clusters_Zpos),     "NTSAC_Clusters_Zpos[NTNSAC_Clusters]/D");
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StdNtuple::Fill(PadmeAnalysisEvent* evt)
{

  TRecoEvent* rev;
  Int_t nHits;
  TRecoVHit* hit;
  Int_t nClus;
  TRecoVCluster* clu;
  TTargetRecoBeam* beam;

  // Event information
  rev = evt->RecoEvent;
  fMyEvt.NTEvent_Run_Number = rev->GetRunNumber();
  fMyEvt.NTEvent_Event_Number = rev->GetEventNumber();
  fMyEvt.NTEvent_Reco_Time = Double_t(rev->GetRecoTime().GetSec())+Double_t(rev->GetRecoTime().GetNanoSec())/1.E9;
  fMyEvt.NTEvent_Event_Time = Double_t(rev->GetEventTime().GetSec())+Double_t(rev->GetEventTime().GetNanoSec())/1.E9;
  fMyEvt.NTEvent_Run_Clock = rev->GetRunClock();
  fMyEvt.NTEvent_Event_Status = rev->GetEventStatus();
  fMyEvt.NTEvent_Trigger_Mask = rev->GetTriggerMask();

  // Target hits
  fMyEvt.NTNTarget_Hits=(evt->TargetRecoEvent)->GetNHits();
  nHits = std::min(fMyEvt.NTNTarget_Hits,NTargetMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->TargetRecoEvent)->Hit(i);
    fMyEvt.NTTarget_Hits_ChannelId[i]=hit->GetChannelId();
    fMyEvt.NTTarget_Hits_Energy[i]=hit->GetEnergy();
    fMyEvt.NTTarget_Hits_Time[i]=hit->GetTime();
    fMyEvt.NTTarget_Hits_Xpos[i]=hit->GetPosition().X();
    fMyEvt.NTTarget_Hits_Ypos[i]=hit->GetPosition().Y();
    fMyEvt.NTTarget_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // Target information
  beam = evt->TargetRecoBeam;
  fMyEvt.NTTargetBeam_X=beam->getX();
  fMyEvt.NTTargetBeam_Y=beam->getY();
  fMyEvt.NTTargetBeam_XErr=beam->getXError();
  fMyEvt.NTTargetBeam_YErr=beam->getYError();
  fMyEvt.NTTargetBeam_XW=beam->getXWidth();
  fMyEvt.NTTargetBeam_YW=beam->getYWidth();
  fMyEvt.NTTargetBeam_XWErr=beam->getXWidthError();
  fMyEvt.NTTargetBeam_YWErr=beam->getYWidthError();
  fMyEvt.NTTargetBeam_XChi2=beam->getXChi2();
  fMyEvt.NTTargetBeam_YChi2=beam->getYChi2();
  fMyEvt.NTTargetBeam_XNdof=beam->getXNdof();
  fMyEvt.NTTargetBeam_YNdof=beam->getYNdof();
  fMyEvt.NTTargetBeam_XCharge=beam->getXCharge();
  fMyEvt.NTTargetBeam_XChargeErr=beam->getXChargeErr();
  fMyEvt.NTTargetBeam_YCharge=beam->getYCharge();
  fMyEvt.NTTargetBeam_YChargeErr=beam->getYChargeErr();
  fMyEvt.NTTargetBeam_XTime=beam->getXTime();
  fMyEvt.NTTargetBeam_XTimeErr=beam->getXTimeErr();
  fMyEvt.NTTargetBeam_YTime=beam->getYTime();
  fMyEvt.NTTargetBeam_YTimeErr=beam->getYTimeErr();
  fMyEvt.NTTargetBeam_NPOT=beam->getnPOT();
  fMyEvt.NTTargetBeam_NPOTErr=beam->getnPOTError();

  // PVeto hits
  fMyEvt.NTNPVeto_Hits = (evt->PVetoRecoEvent)->GetNHits();
  nHits = std::min(fMyEvt.NTNPVeto_Hits,NPVetoMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->PVetoRecoEvent)->Hit(i);
    fMyEvt.NTPVeto_Hits_ChannelId[i]=hit->GetChannelId();
    fMyEvt.NTPVeto_Hits_Energy[i]=hit->GetEnergy();
    fMyEvt.NTPVeto_Hits_Time[i]=hit->GetTime();
    fMyEvt.NTPVeto_Hits_Xpos[i]=hit->GetPosition().X();
    fMyEvt.NTPVeto_Hits_Ypos[i]=hit->GetPosition().Y();
    fMyEvt.NTPVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // PVeto clusters
  fMyEvt.NTNPVeto_Clusters = (evt->PVetoRecoCl)->GetNElements();
  nClus = std::min(fMyEvt.NTNPVeto_Clusters,NPVetoMaxClusters);
  for (Int_t j=0; j<nClus; ++j){
    clu = (evt->PVetoRecoCl)->Element(j);
    fMyEvt.NTPVeto_Clusters_ChannelId[j]=clu->GetChannelId();
    fMyEvt.NTPVeto_Clusters_Energy[j]=clu->GetEnergy();
    fMyEvt.NTPVeto_Clusters_Time[j]=clu->GetTime();
    fMyEvt.NTPVeto_Clusters_Xpos[j]=clu->GetPosition().X();
    fMyEvt.NTPVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
    fMyEvt.NTPVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // EVeto hits
  fMyEvt.NTNEVeto_Hits = (evt->EVetoRecoEvent)->GetNHits();
  nHits = std::min(fMyEvt.NTNEVeto_Hits,NEVetoMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->EVetoRecoEvent)->Hit(i);
    fMyEvt.NTEVeto_Hits_ChannelId[i]=hit->GetChannelId();
    fMyEvt.NTEVeto_Hits_Energy[i]=hit->GetEnergy();
    fMyEvt.NTEVeto_Hits_Time[i]=hit->GetTime();
    fMyEvt.NTEVeto_Hits_Xpos[i]=hit->GetPosition().X();
    fMyEvt.NTEVeto_Hits_Ypos[i]=hit->GetPosition().Y();
    fMyEvt.NTEVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // EVeto clusters
  fMyEvt.NTNEVeto_Clusters = (evt->EVetoRecoCl)->GetNElements();
  nClus = std::min(fMyEvt.NTNEVeto_Clusters,NEVetoMaxClusters);
  for (Int_t j=0; j<nClus; ++j){
    clu = (evt->EVetoRecoCl)->Element(j);
    fMyEvt.NTEVeto_Clusters_ChannelId[j]=clu->GetChannelId();
    fMyEvt.NTEVeto_Clusters_Energy[j]=clu->GetEnergy();
    fMyEvt.NTEVeto_Clusters_Time[j]=clu->GetTime();
    fMyEvt.NTEVeto_Clusters_Xpos[j]=clu->GetPosition().X();
    fMyEvt.NTEVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
    fMyEvt.NTEVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // HEPVeto hits
  fMyEvt.NTNHEPVeto_Hits = (evt->HEPVetoRecoEvent)->GetNHits();
  nHits = std::min(fMyEvt.NTNHEPVeto_Hits,NHEPVetoMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->HEPVetoRecoEvent)->Hit(i);
    fMyEvt.NTHEPVeto_Hits_ChannelId[i]=hit->GetChannelId();
    fMyEvt.NTHEPVeto_Hits_Energy[i]=hit->GetEnergy();
    fMyEvt.NTHEPVeto_Hits_Time[i]=hit->GetTime();
    fMyEvt.NTHEPVeto_Hits_Xpos[i]=hit->GetPosition().X();
    fMyEvt.NTHEPVeto_Hits_Ypos[i]=hit->GetPosition().Y();
    fMyEvt.NTHEPVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // HEPVeto clusters
  fMyEvt.NTNHEPVeto_Clusters = (evt->HEPVetoRecoCl)->GetNElements();
  nClus = std::min(fMyEvt.NTNHEPVeto_Clusters,NHEPVetoMaxClusters);
  for (Int_t j=0; j<nClus; ++j){
    clu = (evt->HEPVetoRecoCl)->Element(j);
    fMyEvt.NTHEPVeto_Clusters_ChannelId[j]=clu->GetChannelId();
    fMyEvt.NTHEPVeto_Clusters_Energy[j]=clu->GetEnergy();
    fMyEvt.NTHEPVeto_Clusters_Time[j]=clu->GetTime();
    fMyEvt.NTHEPVeto_Clusters_Xpos[j]=clu->GetPosition().X();
    fMyEvt.NTHEPVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
    fMyEvt.NTHEPVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // ECal hits
  fMyEvt.NTNECal_Hits = (evt->ECalRecoEvent)->GetNHits();
  nHits = std::min(fMyEvt.NTNECal_Hits,NECalMaxHits);
  for (Int_t i=0; i<nHits; ++i) {
    hit = (evt->ECalRecoEvent)->Hit(i);
    fMyEvt.NTECal_Hits_ChannelId[i]=hit->GetChannelId();
    fMyEvt.NTECal_Hits_Energy[i]=hit->GetEnergy();
    fMyEvt.NTECal_Hits_Time[i]=hit->GetTime();
    fMyEvt.NTECal_Hits_Xpos[i]=hit->GetPosition().X();
    fMyEvt.NTECal_Hits_Ypos[i]=hit->GetPosition().Y();
    fMyEvt.NTECal_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // ECal clusters
  fMyEvt.NTNECal_Clusters = (evt->ECalRecoCl)->GetNElements();
  nClus = std::min(fMyEvt.NTNECal_Clusters,NECalMaxClusters);
  for (Int_t j=0; j<nClus; ++j) {
    clu = (evt->ECalRecoCl)->Element(j);
    fMyEvt.NTECal_Clusters_ChannelId[j]=clu->GetChannelId();
    fMyEvt.NTECal_Clusters_Energy[j]=clu->GetEnergy();
    fMyEvt.NTECal_Clusters_Time[j]=clu->GetTime();
    fMyEvt.NTECal_Clusters_Xpos[j]=clu->GetPosition().X();
    fMyEvt.NTECal_Clusters_Ypos[j]=clu->GetPosition().Y();
    fMyEvt.NTECal_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // SAC hits
  fMyEvt.NTNSAC_Hits = (evt->SACRecoEvent)->GetNHits();
  nHits = std::min(fMyEvt.NTNSAC_Hits,NSACMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->SACRecoEvent)->Hit(i);
    fMyEvt.NTSAC_Hits_ChannelId[i]=hit->GetChannelId();
    fMyEvt.NTSAC_Hits_Energy[i]=hit->GetEnergy();
    fMyEvt.NTSAC_Hits_Time[i]=hit->GetTime();
    fMyEvt.NTSAC_Hits_Xpos[i]=hit->GetPosition().X();
    fMyEvt.NTSAC_Hits_Ypos[i]=hit->GetPosition().Y();
    fMyEvt.NTSAC_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // SAC clusters
  fMyEvt.NTNSAC_Clusters = (evt->SACRecoCl)->GetNElements();
  nClus = std::min(fMyEvt.NTNSAC_Clusters,NSACMaxClusters);
  for (Int_t j=0; j<nClus; ++j){
    clu = (evt->SACRecoCl)->Element(j);
    fMyEvt.NTSAC_Clusters_ChannelId[j]=clu->GetChannelId();
    fMyEvt.NTSAC_Clusters_Energy[j]=clu->GetEnergy();
    fMyEvt.NTSAC_Clusters_Time[j]=clu->GetTime();
    fMyEvt.NTSAC_Clusters_Xpos[j]=clu->GetPosition().X();
    fMyEvt.NTSAC_Clusters_Ypos[j]=clu->GetPosition().Y();
    fMyEvt.NTSAC_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // Copy event ntuple to output file
  fNtuple->Fill();

}
