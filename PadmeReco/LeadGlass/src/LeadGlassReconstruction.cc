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
#define SECOND_LEADGLASS_BOARD 21
#define SECOND_LEADGLASS_CHANNEL 31

LeadGlassReconstruction::LeadGlassReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "LeadGlass", ConfigFileName)
{

  printf("LeadGlassReconstruction::Initialize - Initializing\n");

  // Get pedestal and charge reconstruction parameters from config file
  fPedestalSamples = fConfigParser->HasConfig("RECO","PedestalSamples")?std::stoi(fConfigParser->GetSingleArg("RECO","PedestalSamples")):100;
  fSignalSamplesStart = fConfigParser->HasConfig("RECO","SignalSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesStart")):100;
  fSignalSamplesEnd = fConfigParser->HasConfig("RECO","SignalSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesEnd")):450;

  fLEDSamplesStart = fConfigParser->HasConfig("RECO","LEDSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","LEDSamplesStart")):500;
  fLEDSamplesEnd = fConfigParser->HasConfig("RECO","LEDSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","LEDSamplesEnd")):850;

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
  fHLGBunchBBQ = new TH1D("LG_BunchBBQ","LG_BunchBBQ",1000,0.,1000.);
  //fHLGBunchBBQWF = new TH1D("LG_BunchBBQWF","LG_BunchBBQWF",1024,0.,1024.);
  //fHLGBunchBBQWF2 = new TH1D("LG_BunchBBQWF2","LG_BunchBBQWF2",1024,0.,1024.);

  // Store histograms for final output
  AddHisto("LG_Pedestal",fHLGPedestal);
  AddHisto("LG_PedRMS",fHLGPedRMS);
  AddHisto("LG_TotCharge",fHLGTotCharge);
  AddHisto("LG_NPoTs",fHLGNPoTs);
  AddHisto("LG_BunchLength",fHLGBunchLength);	   
  AddHisto("LG_BunchBBQ",fHLGBunchBBQ);	   
  //AddHisto("LG_BunchBBQWF",fHLGBunchBBQWF);	   
  //AddHisto("LG_BunchBBQWF2",fHLGBunchBBQWF2);	   

}

void LeadGlassReconstruction::ProcessEvent(TRawEvent* rawEv)
{

  // Reset list of LeadGlass blocks found in this event
  for(UChar_t il = 0; il<N_LEADGLASS; il++) fLeadGlassFound[il] = false;

  if(fTriggerProcessor) {
    BuildTriggerInfo(rawEv);
    //printf("\nTriggerMask %d\n",rawEv->GetEventTrigMask());
    if (TriggerToBeSkipped()) {
      //printf("LeadGlassReconstruction::ProcessEvent - Skip event because of trigger type\n");
      return;
    }
  }

  // Find LeadGlass channel in RawEvent (Board 14 Channel 31 || Board 21 Channel 31)
  //UChar_t lg_b,lg_c;
  // Loop through the boards
  for(UChar_t b = 0; b < rawEv->GetNADCBoards(); b++) {
    TADCBoard* adcB = rawEv->ADCBoard(b);

    // Leadglass-on-beam
    if (adcB->GetBoardId() == LEADGLASS_BOARD) {
      leadglassID = 0;
      //printf("Channels = %d\n",adcB->GetNADCChannels());
      //printf("BoardID = %d Leadglass = %d Trigger = %d\n",adcB->GetBoardId(),leadglassID,rawEv->GetEventTrigMask());
      // Get StartIndexCell
      UChar_t nTrg = adcB->GetNADCTriggers();
      //printf("Ntrig = %u\n",nTrg);
      for(UChar_t trig=0; trig<nTrg; trig++){
        TADCTrigger *adcTrig = adcB->ADCTrigger(trig);
        UChar_t nGroup = adcTrig->GetGroupNumber();
        if (nGroup == 3) {
          fLGStartIndexCell[leadglassID] = adcB->ADCTrigger(trig)->GetStartIndexCell();
          //printf("trig = %u -- group = %u -- startIndex = %u\n", trig, nGroup, startIndexCell);
        }
      }

      // Loop through the channels
      for(UChar_t c = 0; c < adcB->GetNADCChannels(); c++) {
        //printf("channel = %d ",c);
	if (adcB->ADCChannel(c)->GetChannelNumber() == LEADGLASS_CHANNEL) {
	  //printf(" chanNum = %d", adcB->ADCChannel(c)->GetChannelNumber());
	  fLeadGlassFound[0] = true;
	  //lg_c = c;
	  // Compute pedestal, total charge, nPoTs, bunch length from ADC samples
	  AnalyzeChannel(leadglassID,adcB->ADCChannel(c)->GetSamplesArray());
	  //printf("Pedestal %f PedestalRMS %f Charge %f NPoTs %f BunchLength %f\n",fLGPedestal,fLGPedRMS,fLGCharge,fLGNPoTs,fBunchLength);
	}
      }
    }

    // reference Leadglass
    if (adcB->GetBoardId() == SECOND_LEADGLASS_BOARD) {  
      //lg_b = b;
      leadglassID = 1;

      // Get StartIndexCell
      UChar_t nTrg = adcB->GetNADCTriggers();
      //printf("Ntrig = %u\n",nTrg);
      for(UChar_t trig=0; trig<nTrg; trig++){
        TADCTrigger *adcTrig = adcB->ADCTrigger(trig);
        UChar_t nGroup = adcTrig->GetGroupNumber();
        if (nGroup == 3) {
          fLGStartIndexCell[leadglassID] = adcB->ADCTrigger(trig)->GetStartIndexCell();
          //printf("trig = %u -- group = %u -- startIndex = %u\n", trig, nGroup, startIndexCell);
        }
      }
      
      for(UChar_t c = 0; c < adcB->GetNADCChannels(); c++) {
        if (adcB->ADCChannel(c)->GetChannelNumber() == SECOND_LEADGLASS_CHANNEL) {
        fLeadGlassFound[1] = true;
        //lg_c = c;
        // Compute pedestal, total charge, nPoTs, bunch length from ADC samples
        AnalyzeChannel(leadglassID,adcB->ADCChannel(c)->GetSamplesArray());
        //printf("Pedestal %f PedestalRMS %f Charge %f NPoTs %f BunchLength %f\n",fLGPedestal,fLGPedRMS,fLGCharge,fLGNPoTs,fBunchLength);
        }
      }
    }
  }

  //Processing is over, let's analyze what's here, if requested
  if (fLeadGlassFound && fGlobalRecoConfigOptions->IsMonitorMode()) AnalyzeEvent(rawEv);

  /*
  if (rawEv->GetEventNumber() == 110) {
    printf("Event %d Bunch length %f BBQ %f\n",rawEv->GetEventNumber(),fBunchLength,fBunchBBQ);
    Short_t* samples = rawEv->ADCBoard(lg_b)->ADCChannel(lg_c)->GetSamplesArray();
    for(Int_t i=0; i<1024; i++) {
      fHLGBunchBBQWF2->SetBinContent(i,samples[i]);
      //printf("%4d ",samples[i]);
    }
    printf("\n");
  }

  if (fBunchBBQ>300.) {
    printf("Event %d Bunch length %f BBQ %f\n",rawEv->GetEventNumber(),fBunchLength,fBunchBBQ);
    Short_t* samples = rawEv->ADCBoard(lg_b)->ADCChannel(lg_c)->GetSamplesArray();
    for(Int_t i=0; i<1024; i++) {
      fHLGBunchBBQWF->SetBinContent(i,samples[i]);
      //printf("%4d ",samples[i]);
    }
    printf("\n");
  }
  */

}

Bool_t LeadGlassReconstruction::TriggerToBeSkipped()
{
  // Only analyze BTF triggers and LED triggers 
  if (GetTriggerProcessor()->IsBTFTrigger()) {
    //printf("Found BTF trigger!\n");
    return false; 
  }
  else if(GetTriggerProcessor()->IsLEDTrigger()) {
    //printf("Found LED trigger!\n");
    return false;
  }
  else {
    //printf("Skip trigger!\n");
    return true; 
  }
}

void LeadGlassReconstruction::AnalyzeEvent(TRawEvent* rawEv)
{
  fHLGPedestal->Fill(fLGPedestal[0]);
  fHLGPedRMS->Fill(fLGPedRMS[0]);
  fHLGTotCharge->Fill(fLGCharge[0]);
  fHLGNPoTs->Fill(fLGNPoTs);
  fHLGBunchLength->Fill(fBunchLength);
  fHLGBunchBBQ->Fill(fBunchBBQ);
}

void LeadGlassReconstruction::AnalyzeChannel(UChar_t lgID, Short_t* samples)
{

  // Compute pedestal and total charge in leadglass -- for BTF triggers
  ComputeTotalCharge(lgID,samples);

  // Compute lenght of bunch (period above a given thershold) -- only for the LG-on-beam
  if(lgID == 0) ComputeBunchLength(samples);

  // Evaluate total energy from total charge
  fLGEnergy[lgID] = fLGCharge[lgID]*fChargeToEnergy;

  // Compute number of positrons on target from total charge -- only for the LG-on-beam
  if(lgID == 0) fLGNPoTs = fLGCharge[0]/fChargeToNPoTs;

}

void LeadGlassReconstruction::ComputeTotalCharge(UChar_t lgID, Short_t* samples)
{
  Int_t sum = 0;
  Int_t sum_ped = 0;
  ULong_t sum2_ped = 0;

  // if BTF triggers:
  if (GetTriggerProcessor()->IsBTFTrigger()) {
    // Get total signal area using first fPedestalSamples samples as pedestal
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
  }

  // if LED triggers:
  if (GetTriggerProcessor()->IsLEDTrigger()) { 
    for(UInt_t s = 0; s<1024; s++) {
      if (s<fPedestalSamples) {
        sum_ped += samples[s];
        sum2_ped += samples[s]*samples[s];
      } else if (s >= fLEDSamplesStart) {
        if (s < fLEDSamplesEnd) {
          sum += samples[s];
        } else {
          break;
        }
      }
    }
  }

  fLGPedestal[lgID] = (Double_t)sum_ped/(Double_t)fPedestalSamples;
  fLGPedRMS[lgID] = sqrt(((Double_t)sum2_ped - (Double_t)sum_ped*fLGPedestal[lgID])/((Double_t)fPedestalSamples-1.));
  
  fLGCharge[lgID] = fLGPedestal[lgID]*(Double_t)(fSignalSamplesEnd-fSignalSamplesStart)-(Double_t)sum;
  // Convert counts to charge in pC
  //charge = counts/(4096.*50.)*(1.E-9/1.E-12);
  fLGCharge[lgID] *= 4.8828E-3;
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
  if (GetTriggerProcessor()->IsBTFTrigger()) {
    for(UInt_t s = fSignalSamplesStart; s<fSignalSamplesEnd; s++) {
      if (bunch) {
        if (fLGPedestal[0]-(Double_t)samples[s] < fBunchLengthThreshold) {
          if (s-bunchStart > 1) { // Ignore noise fluctuations
            bunchEnd = s;
            break;
          }
          sum += samples[s];
          sum2 += samples[s]*samples[s];
        }
      } else {
        if (fLGPedestal[0]-(Double_t)samples[s] > fBunchLengthThreshold) {
          bunch = true;
          bunchStart = s;
          sum = samples[s];
          sum2 = samples[s]*samples[s];
        }
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
