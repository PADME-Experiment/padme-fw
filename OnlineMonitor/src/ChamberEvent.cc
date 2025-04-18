#include "ChamberEvent.hh"

ChamberEvent::ChamberEvent()
{
  fChain = 0;
}

ChamberEvent::~ChamberEvent()
{;}

void ChamberEvent::Init(TTree *tree)
{
  
  // Set object pointer
  srsFec = 0;
  srsChip = 0;
  srsChan = 0;
  mmChamber = 0;
  mmLayer = 0;
  mmReadout = 0;
  mmStrip = 0;
  raw_q = 0;
  max_q = 0;
  t_max_q = 0;

  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("evt", &evt, &b_evt);
  fChain->SetBranchAddress("error", &error, &b_error);
  fChain->SetBranchAddress("daqTimeSec", &daqTimeSec, &b_daqTimeSec);
  fChain->SetBranchAddress("daqTimeMicroSec", &daqTimeMicroSec, &b_daqTimeMicroSec);
  fChain->SetBranchAddress("srsTimeStamp", &srsTimeStamp, &b_srsTimeStamp);
  fChain->SetBranchAddress("srsTrigger", &srsTrigger, &b_srsTrigger);
  fChain->SetBranchAddress("srsFec", &srsFec, &b_srsFec);
  fChain->SetBranchAddress("srsChip", &srsChip, &b_srsChip);
  fChain->SetBranchAddress("srsChan", &srsChan, &b_srsChan);
  fChain->SetBranchAddress("mmChamber", &mmChamber, &b_mmChamber);
  fChain->SetBranchAddress("mmLayer", &mmLayer, &b_mmLayer);
  fChain->SetBranchAddress("mmReadout", &mmReadout, &b_mmReadout);
  fChain->SetBranchAddress("mmStrip", &mmStrip, &b_mmStrip);
  fChain->SetBranchAddress("raw_q", &raw_q, &b_raw_q);
  fChain->SetBranchAddress("max_q", &max_q, &b_max_q);
  fChain->SetBranchAddress("t_max_q", &t_max_q, &b_t_max_q);

}
