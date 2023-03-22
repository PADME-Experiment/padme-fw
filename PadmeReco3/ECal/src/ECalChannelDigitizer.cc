#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream

#include "Riostream.h"

#include "TRawEvent.hh"

#include "TMath.h"
#include "TH1D.h"

#include "PadmeVRecoConfig.hh"
#include "TrigTimeSvc.hh"
#include "HistoSvc.hh"

#include "ECalHit.hh"

#include "ECalChannelDigitizer.hh"

ECalChannelDigitizer::ECalChannelDigitizer(PadmeVRecoConfig* cfg)
  : fECalConfig(cfg)
{

  // Define verbose level
  fVerbose = fECalConfig->GetParOrDefault("Output","Verbose",0);

  // Attach to Trigger Time service
  fTrigTimeSvc = TrigTimeSvc::GetInstance();

  // Attach to Histogrammin service
  fHistoSvc = HistoSvc::GetInstance();

  // Define parameters to use for hit building process
  fGoodSamples = fECalConfig->GetParOrDefault("RECO","GoodSamples",994);
  fPedestalSamples = fECalConfig->GetParOrDefault("RECO","PedestalSamples",100);
  fPedestalRMSThreshold = fECalConfig->GetParOrDefault("RECO","PedestalRMSThreshold",5.);
  fZeroSupRMSThreshold = fECalConfig->GetParOrDefault("RECO","ZeroSupRMSThreshold",5.);

  // Get name of template file to use
  fTemplateFileName = fECalConfig->GetParOrDefault("RECO","TemplateFileName","config/ECalTemplate.dat");

  // Define constant fraction parameters
  fCFRatio = fECalConfig->GetParOrDefault("RECO","ConstantFractionRatio",0.6);;
  fCFShift = fECalConfig->GetParOrDefault("RECO","ConstantFractionShift",5);;
  fCFThreshold = fECalConfig->GetParOrDefault("RECO","ConstantFractionThreshold",0.);;

  if (fVerbose>1) {
    printf("ECalChannelDigitizer::ECalChannelDigitizer - digitization parameters set to:\n");
    printf("\tGoodSamples\t\t\t%d\n",fGoodSamples);
    printf("\tPedestalSamples\t\t\t%d\n",fPedestalSamples);
    printf("\tPedestalRMSThreshold\t\t%.1f\n",fPedestalRMSThreshold);
    printf("\tZeroSupRMSThreshold\t\t%.1f\n",fZeroSupRMSThreshold);
    printf("\tTemplateFileName\t\t%s\n",fTemplateFileName.Data());
    printf("\tConstantFractionRatio\t\t%.2f\n",fCFRatio);
    printf("\tConstantFractionShift\t\t%d\n",fCFShift);
    printf("\tConstantFractionThreshold\t%.2f\n",fCFThreshold);
  }

  if (fVerbose) printf("ECalChannelDigitizer::ECalChannelDigitizer - ECal channel digitization system created\n");
}

ECalChannelDigitizer::~ECalChannelDigitizer()
{
  if (fVerbose) printf("ECalChannelDigitizer::~ECalChannelDigitizer - Deleting ECal channel digitization system\n");
}

Bool_t ECalChannelDigitizer::Init()
{

  if (fVerbose) printf("ECalChannelDigitizer::Init - Initializing ECal channel digitizer\n");

  if (fVerbose) printf("ECalChannelDigitizer::Init - Reading Waveform Template file %s\n",fTemplateFileName.Data());
  std::ifstream infile;
  std::string Line = "";
  infile.open(fTemplateFileName.Data());
  if ( ! infile ) {
    std::cout << "ECalChannelDigitizer::Init - ERROR - Waveform Template file " << fTemplateFileName << " not found" << std::endl;
    return false;
  }
  Double_t value;
  Double_t totVal = 0.;
  fTemplateNBins = 0;
  fTemplateMaxBin = 0;
  fTemplateMaxVal = 0.;
  TH1D* hTemplate = fHistoSvc->BookHisto("ECal","TemplateWF","BGO Template waveform",ECALCHANNELDIGITIZER_TEMPLATE_MAXBINS,0.,(Double_t)ECALCHANNELDIGITIZER_TEMPLATE_MAXBINS);
  TH1D* hTemplateCF = fHistoSvc->BookHisto("ECal","TemplateCF","BGO Template constant fraction",ECALCHANNELDIGITIZER_TEMPLATE_MAXBINS,0.,(Double_t)ECALCHANNELDIGITIZER_TEMPLATE_MAXBINS);
  while ( (! infile.eof()) && (fTemplateNBins<ECALCHANNELDIGITIZER_TEMPLATE_MAXBINS) ) {
    getline(infile,Line);
    std::istringstream iss(Line);
    if ( iss >> value ) {
      if (fVerbose>2) printf("\tBin %d Value %f\n",fTemplateNBins,value);
      if (value>fTemplateMaxVal) {
	fTemplateMaxVal = value;
	fTemplateMaxBin = fTemplateNBins;
      }
      fTemplate[fTemplateNBins++] = value;
      totVal += value;
      hTemplate->SetBinContent(fTemplateNBins,value);
    }
  }
  infile.close();

  // Compute constant fraction time for template wavefunction
  fTemplateCFTime = 0.;
  for(Int_t i=0;i<fCFShift;i++) hTemplateCF->SetBinContent(i+1,0.); // For completeness
  Double_t val;
  Double_t oldVal = 0.;
  for(Int_t i=fCFShift;i<1024;i++) {
    val = fTemplate[i-fCFShift]-fCFRatio*fTemplate[i];
    hTemplateCF->SetBinContent(i+1,val);
    // If we cross the threshold, linearly interpolate the two bins to find precise time
    if ( (oldVal<fCFThreshold) && (val>=fCFThreshold) ) {
      fTemplateCFTime = (Double_t)i-(val-fCFThreshold)/(val-oldVal);
      //break;
    }
    oldVal = val;
  }
  //for(Int_t i=0;i<1024;i++) {
  //  if (i<fCFShift) {
  //    val = -fCFRatio*fTemplate[i];
  //  } else {
  //    val = -fCFRatio*fTemplate[i]+fTemplate[i-fCFShift];
  //  }
  //  //hTemplateCF->SetBinContent(i+1,val);
  //  // If we cross the threshold, linearly interpolate the two bins to find precise time
  //  if ( (oldVal<fCFThreshold) && (val>=fCFThreshold) ) {
  //    fTemplateCFTime = (Double_t)i-(val-fCFThreshold)/(val-oldVal);
  //    break;
  //  }
  //  oldVal = val;
  //}

  if (fTemplateCFTime == 0.) {
    std::cout << "ECalChannelDigitizer::Init - ERROR - Could not compute constant fraction time for Waveform Template" << std::endl;
    return false;
  }
  
  if (fVerbose) {
    printf("ECalChannelDigitizer::Init - Read %d values from Waveform Template file %s\n",fTemplateNBins,fTemplateFileName.Data());
    printf("ECalChannelDigitizer::Init - Template integral is %f - Max value is %f at bin %d\n",totVal,fTemplateMaxVal,fTemplateMaxBin);
    printf("ECalChannelDigitizer::Init - Template constant fraction time is %f\n",fTemplateCFTime);
  }

  return true;

}

Bool_t ECalChannelDigitizer::DigitizeChannel(UChar_t adcBoard,UChar_t adcChannel,Int_t channel,UShort_t sic,Short_t* samples,vector<ECalHit*>& hits)
{

  // Get name of directory to store histograms (created by ECalReconstruction)
  TString hDir = TString::Format("ECal/Evt%d",fEventNumber);

  if (fVerbose>3) printf("ECalChannelDigitizer::DigitizeChannel - Digitizing channel %d\n",channel);
  if (fVerbose>4) {
    printf("Samples");
    for(UInt_t i=0; i<1024; i++) printf(" %5d",samples[i]);
    printf("\n");
  }

  // Convert samples to double
  Double_t samplesD[1024];
  for(UInt_t i=0; i<1024; i++) samplesD[i] = (Double_t)samples[i];

  // Get RMS of "good" samples and use it to reject empty waveforms (poor man's zero suppression)
  Double_t rms = TMath::RMS(fGoodSamples,samplesD);
  //printf("RMS %f\n",rms);
  if (rms<fZeroSupRMSThreshold) return true;

  // Compute pedestal
  Double_t pedestal = TMath::Mean(fPedestalSamples,samplesD);
  //printf("Pedestal %f\n",pedestal);

  // Check if pedestal samples are flat
  Double_t pedrms = TMath::RMS(fPedestalSamples,samplesD);
  if (pedrms > fPedestalRMSThreshold) {
    printf("ECalChannelDigitizer::DigitizeChannel - Channel %d has anomalous pedestal RMS of %f\n",channel,pedrms);
  }

  // Subtract pedestal and convert samples to positive signal in mV
  Double_t ybinD[1024];
  Double_t maxValue = 0.;
  for(Int_t i=0; i<1024; i++) {
    ybinD[i] = -(samplesD[i]-pedestal)/4096.*1000.;
    if ( (i<fGoodSamples) && (ybinD[i]>maxValue) ) maxValue = ybinD[i];
  }

  TString hId = TString::Format("WF%4.4d",channel);
  TString hName = TString::Format("Channel %4.4d Waveform",channel);
  TH1D* hWF = fHistoSvc->BookHisto(hDir.Data(),hId.Data(),hName.Data(),1024,0.,1024.);
  for(Int_t i=0; i<1024; i++) hWF->SetBinContent(i,ybinD[i]);

  // Smooth the waveform by averaging over N consecutive points
  Double_t yavgD[1024];
  Double_t maxAvgValue = 0.;
  Int_t navg = 3; // Number of samples before and after each bin to use in average
  for(Int_t i=0;i<1024;i++) {
    yavgD[i] = 0.;
    Int_t iavg = 0;
    for(Int_t j=std::max(0,i-navg); j<std::min(1024,i+navg+1); j++) {
      yavgD[i] += ybinD[j];
      iavg++;
    }
    yavgD[i] = yavgD[i]/(Double_t)iavg;
    if ( (i<fGoodSamples) && (yavgD[i]>maxAvgValue) ) maxAvgValue = yavgD[i];
  }

  hId = TString::Format("WFs%4.4d",channel);
  hName = TString::Format("Channel %4.4d Smoothed Waveform",channel);
  hWF = fHistoSvc->BookHisto(hDir.Data(),hId.Data(),hName.Data(),1024,0.,1024.);
  for(Int_t i=0; i<1024; i++) hWF->SetBinContent(i,yavgD[i]);

  // Compute derivative over N points
  Double_t yderD[1024];
  Int_t nder = 3; // Samples interval to use for derivative
  for(Int_t i=0;i<1024-nder;i++) yderD[i] = ybinD[i+nder]-ybinD[i];
  for(Int_t i=1024-nder;i<1024;i++) yderD[i] = 0.; // For completeness  

  hId = TString::Format("WFd%4.4d",channel);
  hName = TString::Format("Channel %4.4d Derived Waveform",channel);
  hWF = fHistoSvc->BookHisto(hDir.Data(),hId.Data(),hName.Data(),1024,0.,1024.);
  for(Int_t i=0; i<1024; i++) hWF->SetBinContent(i,yderD[i]);

  // Compute constant fraction
  Double_t yconD[1024];
  for(Int_t i=0;i<fCFShift;i++) yconD[i] = 0.; // For completeness  
  for(Int_t i=fCFShift;i<1024;i++) yconD[i] = ybinD[i-fCFShift]-fCFRatio*ybinD[i];

  hId = TString::Format("WFc%4.4d",channel);
  hName = TString::Format("Channel %4.4d Constant Fraction %.2f Waveform",channel,fCFRatio);
  hWF = fHistoSvc->BookHisto(hDir.Data(),hId.Data(),hName.Data(),1024,0.,1024.);
  for(Int_t i=0; i<1024; i++) hWF->SetBinContent(i,yconD[i]);

  // Compute constant fraction from smoothed waveform
  Double_t ycfmD[1024];
  for(Int_t i=0;i<fCFShift;i++) ycfmD[i] = 0.; // For completeness  
  for(Int_t i=fCFShift;i<1024;i++) ycfmD[i] = yavgD[i-fCFShift]-fCFRatio*yavgD[i];

  hId = TString::Format("WFcs%4.4d",channel);
  hName = TString::Format("Channel %4.4d Constant Fraction %.2f Smoothed Waveform",channel,fCFRatio);
  hWF = fHistoSvc->BookHisto(hDir.Data(),hId.Data(),hName.Data(),1024,0.,1024.);
  for(Int_t i=0; i<1024; i++) hWF->SetBinContent(i,ycfmD[i]);

  // Apply constant fraction to waveform to look for precise hit time
  Double_t hitCFTime = 0.;
  Double_t val;
  Double_t oldVal = 0.;
  Bool_t signalZone = false;
  for(Int_t i=fCFShift;i<1024;i++) {

    //val = ybinD[i-fCFShift]-fCFRatio*ybinD[i];
    val = yavgD[i-fCFShift]-fCFRatio*yavgD[i];
    if (signalZone) {
      // Check if we crossed the constant fraction threshold
      if ( (oldVal < fCFThreshold) && (val >= fCFThreshold) ) {
	// Linearly interpolate the two bins to find precise time
	hitCFTime = (Double_t)i-(val-fCFThreshold)/(val-oldVal);
	break;
      }
    } else {
      // Check if we entered the signal rise zone
      if (val < -3.*pedrms) signalZone = true;
    }
    oldVal = val;

  }
  if (fVerbose>3) printf("ECalChannelDigitizer::DigitizeChannel - Channel %d has hitCFTime = %f\n",channel,hitCFTime);

  // Scale and translate template to match current waveform
  if (hitCFTime > 0.) {

    Double_t ysubD[1024];

    Int_t tempShift = (Int_t)(hitCFTime-fTemplateCFTime); // Will use precise values
    //Double_t tempScale = maxValue/fTemplateMaxVal;
    Double_t tempScale = maxAvgValue/fTemplateMaxVal;

    hId = TString::Format("TempST%4.4d",channel);
    hName = TString::Format("Channel %4.4d Template scale %.3f translate %d",channel,tempScale,tempShift);
    hWF = fHistoSvc->BookHisto(hDir.Data(),hId.Data(),hName.Data(),1024,0.,1024.);
    for(Int_t i=0; i<1024; i++) {
      Int_t iTemp = i-tempShift;
      if (iTemp<0) {
	//ysubD[i] = ybinD[i];
	ysubD[i] = yavgD[i];
	hWF->SetBinContent(i,0.);
      } else {
	hWF->SetBinContent(i,fTemplate[iTemp]*tempScale);
	//ysubD[i] = ybinD[i]-fTemplate[iTemp]*tempScale;
	ysubD[i] = yavgD[i]-fTemplate[iTemp]*tempScale;
      }
    }

    hId = TString::Format("WFsub%4.4d",channel);
    hName = TString::Format("Channel %4.4d Subtracted Waveform",channel);
    hWF = fHistoSvc->BookHisto(hDir.Data(),hId.Data(),hName.Data(),1024,0.,1024.);
    for(Int_t i=0; i<1024; i++) hWF->SetBinContent(i,ysubD[i]);

  }

  return true;

}
