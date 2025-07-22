// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-10-25
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "MMReconstruction.hh"
#include "MMGeometry.hh"
#include "MMClusterization.hh"
#include "TH2F.h"

MMReconstruction::MMReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "MM", ConfigFileName)
{
  fMMCharge = 0;
  printf("MMReconstruction::Initialize - Initializing\n");
  fChannelReco = new DigitizerChannelMM();
  fGeometry = new MMGeometry();
  fClusterization = new MMClusterization();
  // Get pedestal and charge reconstruction parameters from config file
//  fPedestalSamples = fConfigParser->HasConfig("RECO","PedestalSamples")?std::stoi(fConfigParser->GetSingleArg("RECO","PedestalSamples")):100;
//  fSignalSamplesStart = fConfigParser->HasConfig("RECO","SignalSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesStart")):200;
//  fSignalSamplesEnd = fConfigParser->HasConfig("RECO","SignalSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesEnd")):600;

  // Get charge-to-NPoTs conversion factor from config file. Complain if not found
//  if (fConfigParser->HasConfig("RECO","ChargeToNPoTs")) {
//    fChargeToNPoTs = std::stod(fConfigParser->GetSingleArg("RECO","ChargeToNPoTs"));
//  } else {
//    printf("MMReconstruction::Initialize - WARNING: ChargeToNPoTs not set in config file. Using %f\n",fChargeToNPoTs);
//  }

  // Get charge-to-energy conversion factor from config file. Complain if not found
//  if (fConfigParser->HasConfig("RECO","ChargeToEnergy")) {
//    fChargeToEnergy = std::stod(fConfigParser->GetSingleArg("RECO","ChargeToEnergy"));
//  } else {
//    printf("MMReconstruction::Initialize - WARNING: ChargeToEnergy not set in config file. Using %f\n",fChargeToEnergy);
//  }

  // Get threshold for bunch length evaluation.
//  fBunchLengthThreshold = 50.;
//  if (fConfigParser->HasConfig("RECO","BunchLengthThreshold")) {
//    fBunchLengthThreshold = std::stod(fConfigParser->GetSingleArg("RECO","BunchLengthThreshold"));
//  } else {
//    printf("MMReconstruction::Initialize - WARNING: BunchLengthThreshold not set in config file. Using %f\n",fBunchLengthThreshold);
//  }

}

MMReconstruction::~MMReconstruction()
{;}

void MMReconstruction::HistoInit()
{
  // Create histograms
  fHMMTotCharge = new TH1D("MM_TotCharge","MM_TotCharge",1000,0.,5000.);

  // Store histograms for final output
  AddHisto("MM_TotCharge",fHMMTotCharge);

  for (int iev = 0; iev < 30; iev++){
    TString histoname = Form("MM_NoiseFinder_ev%d",iev);
    AddHisto(histoname.Data(),new TH2F(histoname.Data(),"BoardSN vs sample",27,0,27,128,0,128));
  }
  fEventCounter = 0;
}

void MMReconstruction::ProcessEvent(TRawEvent* rawEv, TMMRawEvent* MMRawEv){

  ClearHits();
  vector<TRecoVHit *> &Hits  = GetRecoHits();

  fMMCharge = 0;
  fMMFound = false;

  PadmeVReconstruction::ProcessEvent(rawEv,MMRawEv);
  
  std::cout << "Second This event has " << (int) MMRawEv->GetNMMBoards() << " Boards " << std::endl;

  NoiseFinder(MMRawEv);
  
  //UChar_t lg_b,lg_c;
  for(Int_t b = 0; b < MMRawEv->GetNMMBoards(); b++) {
    TMMBoard* board = MMRawEv->MMBoard(b);
    for(Int_t c = 0; c < board->GetNMMChannels(); c++) {
      TMMChannel* channel = board->MMChannel(c);
      fChannelReco->SetDigis(
			     channel->GetNSamples(),
			     channel->GetSamplesArray());
      
      ((DigitizerChannelMM*)fChannelReco)->Reconstruct(Hits,board,channel);
      if (Hits.size()) fMMCharge += Hits.at(Hits.size()-1)->GetEnergy();
      //      std::cout << "Board " << b << " Channel " << MMRawEv->MMBoard(b)->MMChannel(c)->GetChannelNumber() << " fired, channel " << c << " / " << MMRawEv->MMBoard(b)->GetNMMChannels() << std::endl;
      fMMFound = true;
    }
  }

//  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetRecoHits());

// evaluate positions from the channelID 
  if(fGeometry)  fGeometry->ComputePositions(GetRecoHits());
//  
//  // from Hits to Clusters
//  ClearClusters();
  if (fClusterization) BuildClusters();

  //  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetClusters());

//Processing is over, let's analyze what's here, if requested
//  if (fMMFound && fGlobalRecoConfigOptions->IsMonitorMode()) AnalyzeEvent(rawEv);


}

void MMReconstruction::NoiseFinder(TMMRawEvent* MMRawEv){
  if (fEventCounter == 30) return;

  int summedSampling[TMMCHANNEL_NSAMPLES][8];
  
  for(Int_t b = 0; b < MMRawEv->GetNMMBoards(); b++) {
    TMMBoard* board = MMRawEv->MMBoard(b);
    for (int i=0; i<TMMCHANNEL_NSAMPLES; i++) {
      for (int q = 0; q<8; q++) summedSampling[i][q] = 0;
    }
    for(Int_t c = 0; c < board->GetNMMChannels(); c++) {
      TMMChannel* channel = board->MMChannel(c);
      int iblock = channel->GetChannelNumber()/32;
      for (int i=0; i<TMMCHANNEL_NSAMPLES; i++) summedSampling[i][iblock] += channel->GetSamplesArray()[i];
    }
    if (fEventCounter < 30){
      TString histoname = Form("MM_NoiseFinder_ev%d",fEventCounter);
      for (int i=0; i<TMMCHANNEL_NSAMPLES; i++) {
	for (int q = 0; q<8; q++) ((TH2F*)GetHisto(histoname.Data()))->Fill(i,8*b+q,summedSampling[i][q]);
      }
    }
  }

  fEventCounter++;
}

Bool_t MMReconstruction::TriggerToBeSkipped()
{
  // Only analyze BTF triggers
  if ( !(GetTriggerProcessor()->IsBTFTrigger()) ) return true;
  return false; 
}

void MMReconstruction::AnalyzeEvent(TRawEvent* rawEv)
{
  fHMMTotCharge->Fill(fMMCharge);
}

void MMReconstruction::AnalyzeChannel(Short_t* samples)
{

//  // Compute pedestal and total charge in leadglass
//  ComputeTotalCharge(samples);
//
//  // Compute lenght of bunch (period above a given thershold)
//  ComputeBunchLength(samples);
//
//  // Evaluate total energy from total charge
//  fLGEnergy = fLGCharge*fChargeToEnergy;
//
//  // Compute number of positrons on target from total charge
//  fLGNPoTs = fLGCharge/fChargeToNPoTs;
//
}

void MMReconstruction::ComputeTotalCharge(Short_t* samples)
{

//  // Get total signal area using first fPedestalSamples samples as pedestal
//  Int_t sum = 0;
//  Int_t sum_ped = 0;
//  ULong_t sum2_ped = 0;
//  for(UInt_t s = 0; s<1024; s++) {
//    if (s<fPedestalSamples) {
//      sum_ped += samples[s];
//      sum2_ped += samples[s]*samples[s];
//    } else if (s >= fSignalSamplesStart) {
//      if (s < fSignalSamplesEnd) {
//        sum += samples[s];
//      } else {
//        break;
//      }
//    }
//  }
//
//  fLGPedestal = (Double_t)sum_ped/(Double_t)fPedestalSamples;
//  fLGPedRMS = sqrt(((Double_t)sum2_ped - (Double_t)sum_ped*fLGPedestal)/((Double_t)fPedestalSamples-1.)); //CONTROLLARE
//  fLGCharge = fLGPedestal*(Double_t)(fSignalSamplesEnd-fSignalSamplesStart)-(Double_t)sum;
//  // Convert counts to charge in pC
//  //charge = counts/(4096.*50.)*(1.E-9/1.E-12);
//  fLGCharge *= 4.8828E-3;
}

void MMReconstruction::ComputeBunchLength(Short_t* samples)
{

//  // Get length of bunch using a given threshold.
//  // N.B. Assumes that pedestal was already computed
//  Bool_t bunch = false;
//  UInt_t bunchStart =0.;
//  UInt_t bunchEnd = 0.;
//  Int_t sum = 0;
//  ULong_t sum2 = 0;
//  for(UInt_t s = fSignalSamplesStart; s<fSignalSamplesEnd; s++) {
//    if (bunch) {
//      if (fLGPedestal-(Double_t)samples[s] < fBunchLengthThreshold) {
//        if (s-bunchStart > 1) { // Ignore noise fluctuations
//          bunchEnd = s;
//          break;
//        }
//	sum += samples[s];
//	sum2 += samples[s]*samples[s];
//      }
//    } else {
//      if (fLGPedestal-(Double_t)samples[s] > fBunchLengthThreshold) {
//        bunch = true;
//        bunchStart = s;
//	sum = samples[s];
//	sum2 = samples[s]*samples[s];
//      }
//    }
//  }
//  if (bunch && bunchEnd == 0.) {
//    printf("MMReconstruction::ComputeBunchLength - WARNING - Bunch length longer than signal interval\n");
//    bunchEnd = fSignalSamplesEnd;
//  }
//
//  // Convert sample interval to ns. DAQ is assumed at 1GHz.
//  fBunchLength = (Double_t)(bunchEnd-bunchStart)*1.;
//
//  // Compute bunch quality parameter BBQ
//  fBunchBBQ = sqrt(((Double_t)sum2 - (Double_t)sum*(Double_t)sum/(Double_t)(bunchEnd-bunchStart))/(Double_t)(bunchEnd-bunchStart-1));

}
