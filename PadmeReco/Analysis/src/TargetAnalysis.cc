#include "TargetAnalysis.hh"

#include "TTargetRecoEvent.hh"
#include "TRecoVHit.hh"
#include "HistoSvc.hh"
#include <iostream>

TargetAnalysis::TargetAnalysis()
{
  fNhits=0;
  fhitEvent=NULL;
}
TargetAnalysis::~TargetAnalysis()
{
  fNhits=0;
  fhitEvent=NULL;
}
Bool_t TargetAnalysis::Init(Int_t nh, TTargetRecoEvent* ev, TTargetRecoBeam* b)
{
  Bool_t retCode = 0;
  fNhits    = nh;
  fhitEvent = ev;
  fRecoBeam = b;

  return retCode;
}
Bool_t TargetAnalysis::Process()
{
  Bool_t retCode = 0;
  
  HistoSvc* hSvc =  HistoSvc::GetInstance();
  
  Double_t eMaxX  =  0.;
  Double_t eMaxY  =  0.;
  Int_t    iLeadX = -1;
  Int_t    iLeadY = -1;
  Double_t eTotX  =  0.;
  Double_t eTotY  =  0.;
  Double_t maxDt  =  0.;
  TRecoVHit* hit=NULL;
  std::string hname;	
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    //Bool_t isX=true;
    Int_t istrip    = hit->GetChannelId();
    Double_t energy = hit->GetEnergy();
    Double_t faenergy = fabs(energy);
    Double_t time   = hit->GetTime();
	
    for (Int_t j=0; j<i; ++j)
      {
	if ( fabs(time - fhitEvent->Hit(j)->GetTime() ) > fabs(maxDt))
	  {
	    maxDt = time - fhitEvent->Hit(j)->GetTime();
	  }
      }
    
    if (istrip > 16) {
      //isX = false;
      istrip = istrip-16;
      // Y strip       
      if (faenergy > fabs(eMaxY)) {iLeadY = i; eMaxY=energy;}
      eTotY += energy;
      hname = "Target_Yprofile";
      hSvc->FillHisto(hname,(Double_t)istrip,energy);
      hname = "Target_YstripTime";
      hSvc->FillHisto(hname,time,  1.);
      hname = "Target_YstripEne";
      hSvc->FillHisto(hname,energy,1.);
    }
    else {
      // X strip 
      if (faenergy > fabs(eMaxX)) {iLeadX = i; eMaxX=energy;}
      eTotX += energy;
      hname = "Target_Xprofile";
      hSvc->FillHisto(hname,(Double_t)istrip,energy);
      hname = "Target_XstripTime";
      hSvc->FillHisto(hname,time,  1.);
      hname = "Target_XstripEne";
      hSvc->FillHisto(hname,energy,1.);
    }
   }

   hname = "Target_XtotEne";
   hSvc->FillHisto(hname,eTotX,1.);
   hname = "Target_YtotEne";
   hSvc->FillHisto(hname,eTotY,1.);

   TRecoVHit* lHitX = fhitEvent->Hit(iLeadX);
   TRecoVHit* lHitY = fhitEvent->Hit(iLeadY);
   if (!lHitX) std::cout<<" a problem here with an X strip "<<iLeadX<<std::endl;
   if (!lHitY) std::cout<<" a problem here with an Y strip "<<iLeadY<<std::endl;
   
   Int_t lix = lHitX->GetChannelId();
   Int_t liy = lHitY->GetChannelId()-16;
   Double_t lenergyX = lHitX->GetEnergy();
   Double_t lenergyY = lHitY->GetEnergy();

   hname = "Target_LeadingHitMap";
   hSvc->FillHisto2(hname, lix, liy, 1.);
   hname = "Target_LeadingEneMap";
   hSvc->FillHisto2(hname, lix, liy, 0.5*(lenergyX+lenergyY));

   hname = "Target_maxDt";
   hSvc->FillHisto(hname, maxDt, 1.);
  

   //TTargetRecoBeam* pRecoBeam = fhitEvent->getTargetRecoBeam();
   std::cout<<" from the TargetRecoEvent Beam X, Y "<<fRecoBeam->getX()<<" "<<fRecoBeam->getY()<<std::endl;
   
   
   return retCode;
}
