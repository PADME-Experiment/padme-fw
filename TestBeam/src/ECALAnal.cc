#include "ECALAnal.hh"

#include "HistoManager.hh"

ECALAnal::ECALAnal() {

  fRawEvent = 0;
  fPlotEvent = 1;

  //HistoManager* hMan = HistoManager::GetInstance();
  //ECALHisto* ecalH = (ECALHisto*)hMan->Histo("ECAL");
  fECALHisto = (ECALHisto*)HistoManager::GetInstance()->Histo("ECAL");

  // Initialize vector with sample indexes (used for TGraph).
  //for(int ll=0;ll<TADCCHANNEL_NSAMPLES;ll++) fSampleIndex[ll]=(Float_t)ll;

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

    // See if this event should be shown
    int showEvent = 0;
    if ( fPlotEvent && (nChn>0) ) {
      showEvent = 1;
      fPlotEvent = 0;
    }

    // Loop over channels in this event
    fQTotal1[bid]=0.;
    //fQTotal2[bid]=0.;
    for(UChar_t c=0;c<nChn;c++){

      TADCChannel* chn = adcB->ADCChannel(c);
      UChar_t cnr = chn->GetChannelNumber();

      // Verify that we are monitoring the correct set of channels
      //if ( cnr>15 ) {
      if ( cnr>1 ) {
        printf("WARNING Found channel %d in RAW file!\n",cnr);
        continue;
      }

      // Compute channel average over first 60 samples
      Int_t NAvg=60;
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
	fQChannel[bid][cnr] += fSampleReco[s]/50*1E-9/1E-12; // dT(bin)=1ns, R=50 Ohm, Q in pC
      }
      fQChannel[bid][cnr] = -fQChannel[bid][cnr]; // PMT have negative signal

      // PMT have signal V<0
      //if (cnr<9) {
      //	fQChannel[bid][cnr] = -fQChannel[bid][cnr];
      //}

      fECALHisto->Fill1DHisto(Form("ECALPedCh%d",cnr),Avg);
      fECALHisto->Fill1DHisto(Form("ECALQCh%d",cnr),fQChannel[bid][cnr]);

      /*
      if (cnr<9) {
	fQTotal1[bid] += fQChannel[bid][cnr];
	printf("%d ch %d AVG %f Q0 %f QTOT %f\n",c,cnr,Avg,fQChannel[bid][cnr],fQTotal1[bid]);
      } else {
	fQTotal2[bid] += fQChannel[bid][cnr];
	printf("%d ch %d AVG %f Q0 %f QTOT %f\n",c,cnr,Avg,fQChannel[bid][cnr],fQTotal2[bid]);
      }
      */
      //fQTotal1[bid] += fQChannel[bid][cnr];
      //printf("%d ch %d AVG %f Q0 %f QTOT %f\n",c,cnr,Avg,fQChannel[bid][cnr],fQTotal1[bid]);
      printf("%d ch %d AVG %f Q0 %f\n",c,cnr,Avg,fQChannel[bid][cnr]);

      if ( showEvent ) {
	TH1D* sigH = fECALHisto->Get1DHisto(Form("ECALSigCh%d",cnr));
	TH1D* rawH = fECALHisto->Get1DHisto(Form("ECALRawCh%d",cnr));
	for(UShort_t s=0;s<chn->GetNSamples();s++){
	  sigH->SetBinContent(s+1,fSampleReco[s]);
	  rawH->SetBinContent(s+1,fSample[s]);
	}
      }

    } //end of loop over channels

    // Show trigger signals for sample event
    if ( showEvent ) {
      for(UChar_t t=0;t<nTrg;t++){
	TADCTrigger* trg = adcB->ADCTrigger(t);
	UChar_t tnr = trg->GetGroupNumber();
	TH1D* tH = NULL;
	if (tnr == 0) {
	  tH = fECALHisto->Get1DHisto("ECALTR00");
	//} else if (tnr == 1) {
	//  tH = fECALHisto->Get1DHisto("ECALTR01");
	//} else if (tnr == 2) {
	//  tH = fECALHisto->Get1DHisto("ECALTR10");
	//} else if (tnr == 3) {
	//  tH = fECALHisto->Get1DHisto("ECALTR11");
	} else {
	  printf("ERROR: found trigger id %d in file (?)\n",tnr);
	}
	if (tH) {
	  for(UShort_t s=0;s<trg->GetNSamples();s++){
	    tH->SetBinContent(s+1,(Float_t)trg->GetSample(s));
	  }
	}
      }
    }

    //fECALHisto->Fill1DHisto("ECALQTot",fQTotal1[bid]);
    //printf("%d Bd %d Qtot %f\n",b,bid,fQTotal1[bid]);
    //fECALHisto->Fill1DHisto("ECALQTot2",fQTotal2[bid]);
    //printf("%d Bd %d Qtot %f\n",b,bid,fQTotal2[bid]);

  } // end of loop over boards

}

void ECALAnal::AnalyzePosition()
{

  // Map of crystal positions
  Float_t Xcry[TADCBOARD_NCHANNELS];
  Float_t Ycry[TADCBOARD_NCHANNELS];
  Xcry[ 0]= 1.; Xcry[ 1]= 3.; Xcry[ 2]= 5.; Xcry[ 3]= 7.;
  Ycry[ 0]= 1.; Ycry[ 1]= 1.; Ycry[ 2]= 1.; Ycry[ 3]= 1.;
  Xcry[ 4]= 1.; Xcry[ 5]= 3.; Xcry[ 6]= 5.; Xcry[ 7]= 7.;
  Ycry[ 4]= 3.; Ycry[ 5]= 3.; Ycry[ 6]= 3.; Ycry[ 7]= 3.;
  Xcry[ 8]= 1.; Xcry[ 9]= 3.; Xcry[10]= 5.; Xcry[11]= 7.;
  Ycry[ 8]= 5.; Ycry[ 9]= 5.; Ycry[10]= 5.; Ycry[11]= 5.;
  Xcry[12]= 1.; Xcry[13]= 3.; Xcry[14]= 5.; Xcry[15]= 7.;
  Ycry[12]= 7.; Ycry[13]= 7.; Ycry[14]= 7.; Ycry[15]= 7.;

  // Loop over boards
  UChar_t nBoards = fRawEvent->GetNADCBoards();
  for(UChar_t b=0;b<nBoards;b++){

    // Check if we are looking at the ECAL ADC board (board id 0) and we have some signal
    TADCBoard* adcB = fRawEvent->ADCBoard(b);
    UChar_t bid = adcB->GetBoardId();
    if (bid != 0 || fQTotal1[bid] == 0.) continue;

    printf("AnalPos - Board %d Charge %f\n",bid,fQTotal1[bid]);

    // Get number of active channels in this board
    UChar_t nChn = adcB->GetNADCChannels();

    Float_t XcryTot = 0.;
    Float_t YcryTot = 0.;
    for(Int_t c=0;c<nChn;c++){
      UChar_t cnr = adcB->ADCChannel(c)->GetChannelNumber();
      if (cnr<16) {
	XcryTot += Xcry[cnr]*fQChannel[bid][cnr];
	YcryTot += Ycry[cnr]*fQChannel[bid][cnr];
      }
    }
    fECALHisto->Fill2DHisto("ECALPos",XcryTot/fQTotal1[bid],YcryTot/fQTotal1[bid]);

  }

}
