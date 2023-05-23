// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-10-25
//
// --------------------------------------------------------------
#include "Riostream.h"

#include <TH1D.h>
#include <TString.h>

#include "PadmeVRecoConfig.hh"

#include "TRawEvent.hh"

#include "HistoSvc.hh"
#include "TrigTimeSvc.hh"
#include "RunConditionSvc.hh"
#include "RunConfigurationSvc.hh"

#include "LeadGlassReconstruction.hh"

LeadGlassReconstruction::LeadGlassReconstruction(TString ConfigFileName)
{

  // Attach global services
  fHistoSvc = HistoSvc::GetInstance();
  fTrigTimeSvc = TrigTimeSvc::GetInstance();
  fRunConditionSvc = RunConditionSvc::GetInstance();
  fRunConfigurationSvc = RunConfigurationSvc::GetInstance();

  // Create LeadGlass histogram directory
  fHistoSvc->CreateDir("LeadGlass");

  // Enable LeadGlass specific configuration service
  fLeadGlassConfig = new PadmeVRecoConfig(new utl::ConfigParser(ConfigFileName.Data()),"LeadGlass");

  // Define verbose level
  fVerbose = fLeadGlassConfig->GetParOrDefault("Output", "Verbose", 0);

  // Get pedestal and charge reconstruction parameters from config file
  fPedestalSamples = fLeadGlassConfig->GetParOrDefault("RECO","PedestalSamples",100);
  fSignalSamplesStart = fLeadGlassConfig->GetParOrDefault("RECO","SignalSamplesStart",200);
  fSignalSamplesEnd = fLeadGlassConfig->GetParOrDefault("RECO","SignalSamplesEnd",600);
  
  //// Get charge-to-NPoTs conversion factor from config file. Complain if not found
  //fChargeToNPoTs = 0.239; // Value for 402MeV and LeadGlass at 650V
  //if (fConfigParser->HasConfig("RECO","ChargeToNPoTs")) {
  //  fChargeToNPoTs = std::stod(fConfigParser->GetSingleArg("RECO","ChargeToNPoTs"));
  //} else {
  //  printf("LeadGlassReconstruction::Initialize - WARNING: ChargeToNPoTs not set in config file. Using %f\n",fChargeToNPoTs);
  //}

  // Get charge-to-energy conversion factor from config file
  fChargeToEnergy = fLeadGlassConfig->GetParOrDefault("RECO","ChargeToEnergy",1./0.79256);

  // Get threshold for bunch length evaluation.
  fBunchLengthThreshold = fLeadGlassConfig->GetParOrDefault("RECO","BunchLengthThreshold",50.);

  if (fVerbose>1) {
    printf("LeadGlassReconstruction::LeadGlassReconstruction - digitization parameters set to:\n");
    printf("\tPedestalSamples\t\t%d\n",fPedestalSamples);
    printf("\tSignalSamplesStart\t%d\n",fSignalSamplesStart);
    printf("\tSignalSamplesEnd\t%d\n",fSignalSamplesEnd);
    printf("\tChargeToEnergy\t\t%.5f MeV/pC\n",fChargeToEnergy);
    printf("\tBunchLengthThreshold\t%.1f\n",fBunchLengthThreshold);
  }

  // Check if LeadGlass data must be written to the output file
  fWriteHits = (Bool_t)fLeadGlassConfig->GetParOrDefault("Output","Hits",1);

  if (fVerbose) printf("LeadGlassReconstruction::LeadGlassReconstruction - LeadGlass reconstruction system created\n");

}

LeadGlassReconstruction::~LeadGlassReconstruction()
{;}

void LeadGlassReconstruction::Init()
{

  if (fRunConfigurationSvc->IsMonitorMode()) {
    // Create histograms
    fHLGPedestal = fHistoSvc->BookHisto("LeadGlass","LG_Pedestal","LeadGlass Pedestal",120,3500.,4100.);
    fHLGPedRMS = fHistoSvc->BookHisto("LeadGlass","LG_PedRMS","LG_PedRMS",100,0.,50.);
    fHLGTotCharge = fHistoSvc->BookHisto("LeadGlass","LG_TotCharge","LG_TotCharge",1000,0.,5000.);
    fHLGNPoTs = fHistoSvc->BookHisto("LeadGlass","LG_NPoTs","LG_NPoTs",1000,0.,20000.);
    fHLGBunchLength = fHistoSvc->BookHisto("LeadGlass","LG_BunchLength","LG_BunchLength",1000,0.,1000.);
    fHLGBunchBBQ = fHistoSvc->BookHisto("LeadGlass","LG_BunchBBQ","LG_BunchBBQ",1000,0.,1000.);
    //fHLGBunchBBQWF = fHistoSvc->BookHisto("LG_BunchBBQWF","LG_BunchBBQWF",1024,0.,1024.);
    //fHLGBunchBBQWF2 = fHistoSvc->BookHisto("LG_BunchBBQWF2","LG_BunchBBQWF2",1024,0.,1024.);
  }

}

void LeadGlassReconstruction::EndProcessing()
{
  if (fVerbose) printf("LeadGlassReconstruction::EndProcessing - Finalizing LeadGlass reconstruction\n");
}

void LeadGlassReconstruction::ProcessEvent(TRawEvent* rawEv)
{

  fLeadGlassFound = false;

  if (fVerbose>1) printf("LeadGlassReconstruction::ProcessEvent - Run %d Event %d TrigMask 0x%2.2x\n",rawEv->GetRunNumber(),rawEv->GetEventNumber(),rawEv->GetEventTrigMask());

  // Only analyze BTF, random and off-beam events (trigger mask 1100 0001 = 0xc1)
  //if (! rawEv->EventTrigMaskGetBit(0)) return;
  if (! (rawEv->GetEventTrigMask() & 0xc1)) return;

  // Show run energy (only when run changes)
  static Int_t run = 0;
  if (fRunConditionSvc->GetCurrentRun() != run) {
    run = fRunConditionSvc->GetCurrentRun();
    fRunEnergy = fRunConditionSvc->GetRunEnergy();
    fChargeToNPoTs = fChargeToEnergy/fRunEnergy;
    if (fVerbose) printf("LeadGlassReconstruction::ProcessEvent - Run %d has energy %.3f MeV - Charge to NPots conversion factor is %.5f\n",run,fRunEnergy,fChargeToNPoTs);
  }

  // Find LeadGlass channel in RawEvent (Board 14 Channel 31)
  for(UChar_t b = 0; b < rawEv->GetNADCBoards(); b++) {
    if (rawEv->ADCBoard(b)->GetBoardId() == LEADGLASS_BOARD) {
      for(UChar_t c = 0; c < rawEv->ADCBoard(b)->GetNADCChannels(); c++) {
	if (rawEv->ADCBoard(b)->ADCChannel(c)->GetChannelNumber() == LEADGLASS_CHANNEL) {
	  fLeadGlassFound = true;
	  // Compute pedestal, total charge, nPoTs, bunch length from ADC samples
	  AnalyzeChannel(rawEv->ADCBoard(b)->ADCChannel(c)->GetSamplesArray());
	}
      }
    }
  }

  if (fVerbose>2) {
    printf("LeadGlassReconstruction::ProcessEvent - Run %d Event %d Pedestal %.1f PedRMS %.1f TotCharge %.2f TotEnergy %.0f MeV NPots %.0f BunchLen %.1f BunchBBQ %.1f\n",rawEv->GetRunNumber(),rawEv->GetEventNumber(),fLGPedestal,fLGPedRMS,fLGCharge,fLGEnergy,fLGNPoTs,fBunchLength,fBunchBBQ);
  }

  //Processing is over, let's analyze what's here, if requested
  if (fLeadGlassFound && fRunConfigurationSvc->IsMonitorMode()) AnalyzeEvent(rawEv);

}

void LeadGlassReconstruction::AnalyzeEvent(TRawEvent* rawEv)
{
  fHLGPedestal->Fill(fLGPedestal);
  fHLGPedRMS->Fill(fLGPedRMS);
  fHLGTotCharge->Fill(fLGCharge);
  fHLGNPoTs->Fill(fLGNPoTs);
  fHLGBunchLength->Fill(fBunchLength);
  fHLGBunchBBQ->Fill(fBunchBBQ);
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
  fLGNPoTs = fLGCharge*fChargeToNPoTs;
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
      if (s >= fSignalSamplesEnd) break;
      sum += samples[s];
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
  Int_t sum = 0;
  ULong_t sum2 = 0;
  for(UInt_t s = fSignalSamplesStart; s<fSignalSamplesEnd; s++) {
    if (bunch) {
      if (fLGPedestal-(Double_t)samples[s] < fBunchLengthThreshold) {
        if (s-bunchStart > 1) { // Ignore noise fluctuations
          bunchEnd = s;
          break;
        }
	sum += samples[s];
	sum2 += samples[s]*samples[s];
      }
    } else {
      if (fLGPedestal-(Double_t)samples[s] > fBunchLengthThreshold) {
        bunch = true;
        bunchStart = s;
	sum = samples[s];
	sum2 = samples[s]*samples[s];
      }
    }
  }
  if (bunch && bunchEnd == 0.) {
    printf("LeadGlassReconstruction::ComputeBunchLength - WARNING - Bunch length longer than signal interval\n");
    bunchEnd = fSignalSamplesEnd;
  }

  // Convert sample interval to ns. DAQ is assumed at 1GHz.
  fBunchLength = (Double_t)(bunchEnd-bunchStart)*1.;

  // Compute bunch quality parameter BBQ
  fBunchBBQ = sqrt(((Double_t)sum2 - (Double_t)sum*(Double_t)sum/(Double_t)(bunchEnd-bunchStart))/(Double_t)(bunchEnd-bunchStart-1));

}
