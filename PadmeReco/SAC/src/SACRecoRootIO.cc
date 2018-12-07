#include "SACRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TSACRecoEvent.hh"
#include "SACReconstruction.hh"
#include "TRecoVCluster.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


SACRecoRootIO::SACRecoRootIO() 
  : RecoVRootIO(TString("SAC"))
{
  iev = 0;
  fEvent = new TSACRecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "SACRecoRootIO: Initialized" << std::endl;

}
SACRecoRootIO::~SACRecoRootIO()
{;}

void SACRecoRootIO::SaveEvent()
{
  
  TSACRecoEvent* myEvt = (TSACRecoEvent*)fEvent;
  myEvt->Clear();
  
  RecoVRootIO::SaveEvent();

  PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  SACReconstruction* Reco      = (SACReconstruction*)MyReco;

  vector<TRecoVCluster*> clVector = Reco->getClusters();
  for (Int_t j=0; j<clVector.size(); ++j)
    {
      myEvt->getClusCollection()->AddElement(clVector[j]);
    }

  /*
  ++iev;
  Int_t iCl = 1;
  TRecoVCluster* cl = new TRecoVCluster();
  cl->SetChannelId(iev);
  cl->SetEnergy(10*iev);
  cl->SetTime(100*iev);
  myEvt->getClusCollection()->AddElement(cl);
  std::cout<<"Event "<<iev<<" Adding cluster "<<iCl<<" with ch, energy, time "<<myEvt->getClusCollection()->LastElement()->GetChannelId()<<" "<<myEvt->getClusCollection()->LastElement()->GetEnergy()<<" "<<myEvt->getClusCollection()->LastElement()->GetTime()<<std::endl;
  if ((iev%2)==0) {
    TRecoVCluster* cl1 = new TRecoVCluster();
    cl1->SetChannelId(iev+100);
    cl1->SetEnergy(100*iev);
    cl1->SetTime(1000*iev);
    myEvt->getClusCollection()->AddElement(cl1);
  std::cout<<"Event "<<iev<<" Adding cluster "<<iCl<<" with ch, energy, time "<<myEvt->getClusCollection()->LastElement()->GetChannelId()<<" "<<myEvt->getClusCollection()->LastElement()->GetEnergy()<<" "<<myEvt->getClusCollection()->LastElement()->GetTime()<<std::endl;
  
  }
  */
}


