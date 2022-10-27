// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-10-25
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "LeadGlassReconstruction.hh"

#define LEADGLASS_BOARD 14
#define LEADGLASS_CHANNEL 31

LeadGlassReconstruction::LeadGlassReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "LeadGlass", ConfigFileName)
{

  printf("LeadGlassReconstruction::Initialize - Initializing\n");

  // Get pedestal and charge reconstruction parameters from config file
  fPedestalSamples = fConfigParser->HasConfig("RECO","PedestalSamples")?std::stoi(fConfigParser->GetSingleArg("RECO","PedestalSamples")):100;
  fSignalSamplesStart = fConfigParser->HasConfig("RECO","SignalSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesStart")):200;
  fSignalSamplesEnd = fConfigParser->HasConfig("RECO","SignalSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesEnd")):600;

  // Get charge-to-NPoTs conversion factor from config file. Complain if not found
  fChargeToNPoTs = 0.239; // Value for 402MeV and LeadGlass at 650V
  if (fConfigParser->HasConfig("RECO","ChargeToNPoTs")) {
    fChargeToNPoTs = std::stod(fConfigParser->GetSingleArg("RECO","ChargeToNPoTs"));
  } else {
    printf("LeadGlassReconstruction::Initialize - WARNING: ChargeToNPoTs not set in config file. Using %f\n",fChargeToNPoTs);
  }

  // Get charge-to-energy conversion factor from config file. Complain if not found
  fChargeToEnergy = 1.; // Need value
  if (fConfigParser->HasConfig("RECO","ChargeToEnergy")) {
    fChargeToEnergy = std::stod(fConfigParser->GetSingleArg("RECO","ChargeToEnergy"));
  } else {
    printf("LeadGlassReconstruction::Initialize - WARNING: ChargeToEnergy not set in config file. Using %f\n",fChargeToEnergy);
  }

  // Get threshold for bunch length evaluation.
  fBunchLengthThreshold = 50.;
  if (fConfigParser->HasConfig("RECO","BunchLengthThreshold")) {
    fBunchLengthThreshold = std::stod(fConfigParser->GetSingleArg("RECO","BunchLengthThreshold"));
  } else {
    printf("LeadGlassReconstruction::Initialize - WARNING: BunchLengthThreshold not set in config file. Using %f\n",fBunchLengthThreshold);
  }

}

LeadGlassReconstruction::~LeadGlassReconstruction()
{;}

void LeadGlassReconstruction::HistoInit()
{

  // Create histograms
  fHLGPedestal = new TH1D("LG_Pedestal","LG_Pedestal",120,3500.,4100.);
  fHLGPedRMS = new TH1D("LG_PedRMS","LG_PedRMS",100,0.,50.);
  fHLGTotCharge = new TH1D("LG_TotCharge","LG_TotCharge",1000,0.,5000.);
  fHLGNPoTs = new TH1D("LG_NPoTs","LG_NPoTs",1000,0.,20000.);
  fHLGBunchLength = new TH1D("LG_BunchLength","LG_BunchLength",1000,0.,1000.);

  // Store histograms for final output
  AddHisto("LG_Pedestal",fHLGPedestal);
  AddHisto("LG_PedRMS",fHLGPedRMS);
  AddHisto("LG_TotCharge",fHLGTotCharge);
  AddHisto("LG_NPoTs",fHLGNPoTs);
  AddHisto("LG_BunchLength",fHLGBunchLength);	   

}

void LeadGlassReconstruction::ProcessEvent(TRawEvent* rawEv)
{

  if(fTriggerProcessor) {
    BuildTriggerInfo(rawEv);
    if (TriggerToBeSkipped()) return;
  }

  // Find LeadGlass channel in RawEvent (Board 14 Channel 31)
  for(UChar_t b = 0; b < rawEv->GetNADCBoards(); b++) {
    if (rawEv->ADCBoard(b)->GetBoardId() == LEADGLASS_BOARD) {
      for(UChar_t c = 0; c < rawEv->ADCBoard(b)->GetNADCChannels(); c++) {
	if (rawEv->ADCBoard(b)->ADCChannel(c)->GetChannelNumber() == LEADGLASS_CHANNEL) {

	  // Compute pedestal, total charge, nPoTs, bunch length from ADC samples
	  AnalyzeChannel(rawEv->ADCBoard(b)->ADCChannel(c)->GetSamplesArray());
	  //printf("Pedestal %f PedestalRMS %f Charge %f NPoTs %f BunchLength %f\n",fLGPedestal,fLGPedRMS,fLGCharge,fLGNPoTs,fBunchLength);
	}
      }
    }
  }

  //Processing is over, let's analyze what's here, if requested
  if (fGlobalRecoConfigOptions->IsMonitorMode()) AnalyzeEvent(rawEv);

}

Bool_t LeadGlassReconstruction::TriggerToBeSkipped()
{
  // Only analyze BTF triggers
  if ( !(GetTriggerProcessor()->IsBTFTrigger()) ) return true;
  return false; 
}

void LeadGlassReconstruction::AnalyzeEvent(TRawEvent* rawEv)
{
  fHLGPedestal->Fill(fLGPedestal);
  fHLGPedRMS->Fill(fLGPedRMS);
  fHLGTotCharge->Fill(fLGCharge);
  fHLGNPoTs->Fill(fLGNPoTs);
  fHLGBunchLength->Fill(fBunchLength);
}

void LeadGlassReconstruction::AnalyzeChannel(Short_t* samples)
{

  // Compute pedestal and total charge in leadglass
  ComputeTotalCharge(samples);

  // Compute lenght of bunch (period above a given thershold)
  ComputeBunchLength(samples);

  // Evaluate total energy from total charge
  fLGEnergy = fLGCharge*fChargeToEnergy;

  // Compute number of positrons on target from total charge
  fLGNPoTs = fLGCharge/fChargeToNPoTs;

}

void LeadGlassReconstruction::ComputeTotalCharge(Short_t* samples)
{

  // Get total signal area using first fPedestalSamples samples as pedestal
  Int_t sum = 0;
  Int_t sum_ped = 0;
  ULong_t sum2_ped = 0;
  for(UInt_t s = 0; s<1024; s++) {
    if (s<fPedestalSamples) {
      sum_ped += samples[s];
      sum2_ped += samples[s]*samples[s];
    } else if (s >= fSignalSamplesStart) {
      if (s < fSignalSamplesEnd) {
        sum += samples[s];
      } else {
        break;
      }
    }
  }

  fLGPedestal = (Double_t)sum_ped/(Double_t)fPedestalSamples;
  fLGPedRMS = sqrt(((Double_t)sum2_ped - (Double_t)sum_ped*fLGPedestal)/((Double_t)fPedestalSamples-1.));
  fLGCharge = fLGPedestal*(Double_t)(fSignalSamplesEnd-fSignalSamplesStart)-(Double_t)sum;
  // Convert counts to charge in pC
  //charge = counts/(4096.*50.)*(1.E-9/1.E-12);
  fLGCharge *= 4.8828E-3;

}

void LeadGlassReconstruction::ComputeBunchLength(Short_t* samples)
{

  // Get length of bunch using a given threshold.
  // N.B. Assumes that pedestal was already computed
  Bool_t bunch = false;
  UInt_t bunchStart =0.;
  UInt_t bunchEnd = 0.;
  for(UInt_t s = fSignalSamplesStart; s<fSignalSamplesEnd; s++) {
    if (bunch) {
      if (fLGPedestal-(Double_t)samples[s] < fBunchLengthThreshold) {
        if (s-bunchStart > 1) { // Ignore noise ffluctuations
          bunchEnd = s;
          break;
        }
      }
    } else {
      if (fLGPedestal-(Double_t)samples[s] > fBunchLengthThreshold) {
        bunch = true;
        bunchStart = s;
      }
    }
  }
  if (bunch && bunchEnd == 0.) {
    printf("LeadGlassReconstruction::ComputeBunchLength - WARNING - Bunch length longer than signal interval\n");
    bunchEnd = fSignalSamplesEnd;
  }

  // Convert sample interval to ns. DAQ is assumed at 1GHz.
  fBunchLength = (Double_t)(bunchEnd-bunchStart)*1.;

}
