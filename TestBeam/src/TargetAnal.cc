#include "TargetAnal.hh"

#include "HistoManager.hh"

TargetAnal::TargetAnal() {

  fRawEvent = 0;
  fPlotEvent = 1;

  fTargetHisto = (TargetHisto*)HistoManager::GetInstance()->Histo("Target");

  // Initialize vector with sample indexes (used for TGraph).
  //for(int ll=0;ll<TADCCHANNEL_NSAMPLES;ll++) fSampleIndex[ll]=(Float_t)ll;

}

TargetAnal::~TargetAnal()
{}

void TargetAnal::AnalyzeCharge()
{

  //printf("Called TargetAnal::AnalyzeCharge() for event %d\n",fRawEvent->GetEventNumber());

  // Loop over boards
  UChar_t nBoards = fRawEvent->GetNADCBoards();
  for(UChar_t b=0;b<nBoards;b++){

    // Check if we are looking at the Target ADC board (board id 1)
    TADCBoard* adcB = fRawEvent->ADCBoard(b);
    UChar_t bid = adcB->GetBoardId();
    if (bid!=1) continue;

    // Show board info
    UChar_t nTrg = adcB->GetNADCTriggers();
    UChar_t nChn = adcB->GetNADCChannels();
    printf("Ntrg Nchn %d %d\n",nTrg,nChn);

    // See if this event should be shown
    int showEvent = 0;
    if ( fPlotEvent && (nChn>3) ) {
      showEvent = 1;
      fPlotEvent = 0;
    }

    // Loop over channels in this event
    fQTotal[bid]=0.;
    for(UChar_t c=0;c<nChn;c++){

      TADCChannel* chn = adcB->ADCChannel(c);
      UChar_t cnr = chn->GetChannelNumber();

      // These channels are not used
      if (cnr==0 || cnr==4 || cnr==6 || cnr==8 || cnr==12 || cnr==13 || cnr==18 || cnr==19) {
	printf("WARNING Found channel %d in RAW file!\n",cnr);
	continue;
      }

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
	if (s>100 && s<200) { // Ad hoc interval to compute peak charge
	  fQChannel[bid][cnr] += fSampleReco[s]/50*1E-9/1E-12; // dT(bin)=1ns, R=50 Ohm, Q in pC
	}
      }

      fTargetHisto->Fill1DHisto(Form("TargPedCh%d",cnr),Avg);
      fTargetHisto->Fill1DHisto(Form("TargQCh%d",cnr),fQChannel[bid][cnr]);

      if (cnr>=16 && cnr<21) {
	fQTotal[bid] += fQChannel[bid][cnr];
      }
      printf("%d ch %d AVG %f Q0 %f QTOT %f\n",c,cnr,Avg,fQChannel[bid][cnr],fQTotal[bid]);

      if ( showEvent ) {

	TH1D* sigH = fTargetHisto->Get1DHisto(Form("TargSigCh%d",cnr));
	TH1D* rawH = fTargetHisto->Get1DHisto(Form("TargRawCh%d",cnr));
	for(UShort_t s=0;s<chn->GetNSamples();s++){
	  sigH->SetBinContent(s+1,fSampleReco[s]);
	  rawH->SetBinContent(s+1,fSample[s]);
	}

	TH1D* trgH = fTargetHisto->Get1DHisto("TargTR2");
	for (int it=0;it<adcB->GetNADCTriggers();it++){
	  TADCTrigger* trg = adcB->ADCTrigger(it);
	  if (trg->GetGroupNumber()==2) {
	    for(UShort_t s=0;s<trg->GetNSamples();s++){
	      trgH->SetBinContent(s+1,(trg->GetSample(s))/4096.);
	    }
	  }
	}

      }

    } //end of loop over channels

    fTargetHisto->Fill1DHisto("TargQTot",fQTotal[bid]);
    printf("%d Bd %d Qtot %f\n",b,bid,fQTotal[bid]);

  } // end of loop over boards

}

void TargetAnal::AnalyzePosition()
{
  /*
  // Map of crystal positions (fix it for target)
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

    // Check if we are looking at the Target ADC board (board id 1) and we have some signal
    TADCBoard* adcB = fRawEvent->ADCBoard(b);
    UChar_t bid = adcB->GetBoardId();
    if (bid != 1 || fQTotal[bid] == 0.) continue;

    // Get number of active channels in this board
    UChar_t nChn = adcB->GetNADCChannels();

    Float_t XcryTot = 0.;
    Float_t YcryTot = 0.;
    for(Int_t c=0;c<nChn;c++){
      UChar_t cnr = adcB->ADCChannel(c)->GetChannelNumber();
      XcryTot += Xcry[cnr]*fQChannel[bid][cnr];
      YcryTot += Ycry[cnr]*fQChannel[bid][cnr];
    }
    fTargetHisto->Fill2DHisto("TargPos",XcryTot/fQTotal[bid],YcryTot/fQTotal[bid]);

  }
  */
}
