#include "ECALAnal.hh"

#include "HistoManager.hh"

ECALAnal::ECALAnal() {

  fRawEvent = 0;

  //HistoManager* hMan = HistoManager::GetInstance();
  //ECALHisto* ecalH = (ECALHisto*)hMan->Histo("ECAL");
  fECALHisto = (ECALHisto*)HistoManager::GetInstance()->Histo("ECAL");

  // Initialize vector with sample indexes (used for TGraph).
  for(int ll=0;ll<TADCCHANNEL_NSAMPLES;ll++) fSampleIndex[ll]=(Float_t)ll;

}

ECALAnal::~ECALAnal()
{}

void ECALAnal::AnalyzeCharge()
{

  //printf("Called ECALAnal::AnalyzeCharge() for event %d\n",fRawEvent->GetEventNumber());

  // Loop over boards
  UChar_t nBoards = fRawEvent->GetNADCBoards();
  for(UChar_t b=0;b<nBoards;b++){

    // Check if we are looking at the ECAL ADC board (board id 0)
    TADCBoard* adcB = fRawEvent->ADCBoard(b);
    UChar_t bid = adcB->GetBoardId();
    if (bid!=0) continue;

    // Show board info
    UChar_t nTrg = adcB->GetNADCTriggers();
    UChar_t nChn = adcB->GetNADCChannels();
    printf("Ntrg Nchn %d %d\n",nTrg,nChn);

    // Loop over triggers
    //for(UChar_t t=0;t<nTrg;t++){
    //  TADCTrigger* trg = adcB->ADCTrigger(t);
    //  Float_t Sam[1024];
    //  for(Int_t tt=0;tt<trg->GetNSamples();tt++){
    //    Sam[tt] = (Float_t) trg->GetSample(tt);
    //  }
    //  if(b==0) his->FillGraph("CaloTrig",t,trg->GetNSamples(),SampInd,Sam);
    //}

    // Loop over in this event channels
    fQTotal[bid]=0.;
    for(UChar_t c=0;c<nChn;c++){

      TADCChannel* chn = adcB->ADCChannel(c);
      UChar_t cnr = chn->GetChannelNumber();

      // Compute channel average over first 80 samples
      Int_t NAvg=80;
      Float_t SumSam=0.;
      for(UShort_t s=0;s<chn->GetNSamples();s++){
	fSample[s] = (Float_t)chn->GetSample(s);
	if(s<NAvg) SumSam+=fSample[s];
      }
      Float_t Avg=SumSam/NAvg;

      // Subtract the average and compute the charge
      fQChannel[bid][cnr] = 0.;
      for(UShort_t s=0;s<chn->GetNSamples();s++){
	fSampleReco[s] = (fSample[s]-Avg)/4096.; // Counts to Volt
	fQChannel[bid][cnr] += -fSampleReco[s]/50*1E-9/1E-12; // dT(bin)=1ns, R=50 Ohm, Q in pC
      }

      fECALHisto->Fill1DHisto(Form("ECALPed%d",cnr),Avg);
      fECALHisto->Fill1DHisto(Form("ECALQCh%d",cnr),fQChannel[bid][cnr]);

      fQTotal[bid] += fQChannel[bid][cnr];
      printf("%d ch %d AVG %f Q0 %f QTOT %f\n",c,cnr,Avg,fQChannel[bid][cnr],fQTotal[bid]);

    } //end of loop over channels

    fECALHisto->Fill1DHisto("ECALQTot",fQTotal[bid]);
    printf("%d Bd %d Qtot %f\n",b,bid,fQTotal[bid]);

  } // end of loop over boards

}

void ECALAnal::AnalyzePosition()
{

  // Map of crystal positions
  Float_t Xcry[TADCBOARD_NCHANNELS];
  Float_t Ycry[TADCBOARD_NCHANNELS];
  Xcry[0]= 1.; Xcry[1]= 0.; Xcry[2]=-1.;
  Ycry[0]= 1.; Ycry[1]= 1.; Ycry[2]= 1.;
  Xcry[3]= 1.; Xcry[4]= 0.; Xcry[5]=-1.;
  Ycry[3]= 0.; Ycry[4]= 0.; Ycry[5]= 0.;
  Xcry[6]= 1.; Xcry[7]= 0.; Xcry[8]=-1.;
  Ycry[6]=-1.; Ycry[7]=-1.; Ycry[8]=-1.;

  // Loop over boards
  UChar_t nBoards = fRawEvent->GetNADCBoards();
  for(UChar_t b=0;b<nBoards;b++){

    // Check if we are looking at the ECAL ADC board (board id 0) and we have some signal
    TADCBoard* adcB = fRawEvent->ADCBoard(b);
    UChar_t bid = adcB->GetBoardId();
    if (bid != 0 || fQTotal[bid] == 0.) continue;

    // Get number of active channels in this board
    UChar_t nChn = adcB->GetNADCChannels();

    Float_t XcryTot = 0.;
    Float_t YcryTot = 0.;
    for(Int_t c=0;c<nChn;c++){
      UChar_t cnr = adcB->ADCChannel(c)->GetChannelNumber();
      XcryTot += Xcry[cnr]*fQChannel[bid][cnr];
      YcryTot += Ycry[cnr]*fQChannel[bid][cnr];
    }
    fECALHisto->Fill2DHisto("ECALPos",XcryTot/fQTotal[bid],YcryTot/fQTotal[bid]);

  }

}
