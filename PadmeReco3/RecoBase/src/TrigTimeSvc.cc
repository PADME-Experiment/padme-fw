#include <iostream>

#include "TrigTimeSvc.hh"

#include "TMath.h"

#include "TRawEvent.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrigTimeSvc* TrigTimeSvc::fInstance = 0;
TrigTimeSvc* TrigTimeSvc::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TrigTimeSvc(); }
  return fInstance;
}

TrigTimeSvc::TrigTimeSvc()
  :fVerbose(0)
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrigTimeSvc::~TrigTimeSvc()
{;}

Bool_t TrigTimeSvc::Initialize()
{
  if (fVerbose) printf("TrigTimeSvc::Initialize - Initializing Trigger Time Service\n");

  // Define default method to compute trigger time reference point

  fMethod = 0; // Standard level-based method

  // Initialize sample intervals to use for computing max and min averages according to sampling frequency

  // 5GHz
  fMaxAvgStart[0] = -100;
  fMaxAvgEnd[0] = -20;
  fMinAvgStart[0] = 100;
  fMinAvgEnd[0] = 180;

  // 2.5GHz
  fMaxAvgStart[1] = -100;
  fMaxAvgEnd[1] = -20;
  fMinAvgStart[1] = 50;
  fMinAvgEnd[1] = 130;

  // 1GHz
  fMaxAvgStart[2] = -100;
  fMaxAvgEnd[2] = -20;
  fMinAvgStart[2] = 20;
  fMinAvgEnd[2] = 100;

  // Initialize constant fraction default parameters
  fCFShift         = 5;   // Number of bins to shift
  fCFRatio         = 0.6; // Multiplier for shifted waveform
  fCFThreshold     = 0.;  // Threshold value to define trigger time
  fPedestalSamples = 100; // Number of samples to use to compute channel pedestal

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t TrigTimeSvc::Finalize()
{
  if (fVerbose) printf("TrigTimeSvc::Finalize - Finalizing Trigger Time Service\n");
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t TrigTimeSvc::ProcessEvent(TRawEvent* rawEv)
{

  if (fVerbose>1) printf("TrigTimeSvc::ProcessEvent - Processing Run %d Event %d\n",rawEv->GetRunNumber(),rawEv->GetEventNumber());

  // Reset map of trigger times for this event
  fTrigTimeMap.clear();

  // Loop over all boards of this event
  for (UChar_t b=0; b<rawEv->GetNADCBoards(); b++) {

    TADCBoard* board = rawEv->ADCBoard(b);
    UChar_t boardId = board->GetBoardId();

    // Loop over all trigger groups of this board
    for(UChar_t t=0;t<board->GetNADCTriggers();t++){
      TADCTrigger* trig = board->ADCTrigger(t);
      UChar_t trigId = trig->GetGroupNumber();
      UChar_t trigFreq = trig->GetFrequency();
      UShort_t trigSIC = trig->GetStartIndexCell();
      // Process trigger samples extracting the reference sample point
      if (fVerbose>2) printf("TrigTimeSvc::ProcessEvent - Processing samples from board %d group %d\n",boardId,trigId);
      Double_t trigRefSamp = -1.;
      if (fMethod == 0) {
	trigRefSamp = ComputeReferencePoint(trigFreq,trig->GetSamplesArray());
      } else if (fMethod == 1) {
	trigRefSamp = ComputeReferencePointCF(trigFreq,trig->GetSamplesArray());
      } else {
	printf("TrigTimeSvc::ProcessEvent - ERROR - Reference point method %d not available\n",fMethod);
      }
      //fTrigTimeMap[std::make_pair(boardId,trigId)] = std::make_pair(trigFreq,trigRefSamp);
      fTrigTimeMap[std::make_pair(boardId,trigId)] = std::make_tuple(trigFreq,trigSIC,trigRefSamp);
    }

  }

  // Debug printout of final map
  if (fVerbose>3) {
    //std::map<std::pair<UChar_t,UChar_t>,std::pair<UChar_t,Double_t>>::iterator it;
    std::map<std::pair<UChar_t,UChar_t>,std::tuple<UChar_t,UShort_t,Double_t>>::iterator it;
    for (it = fTrigTimeMap.begin(); it != fTrigTimeMap.end(); it++) {
      //printf("TrigTimeSvc::ProcessEvent - Board %2d Group %1d Freq %1d Reference %f\n",it->first.first,it->first.second,it->second.first,it->second.second);
      printf("TrigTimeSvc::ProcessEvent - Board %2d Group %1d Freq %1d SIC %4d Reference %f\n",it->first.first,it->first.second,std::get<0>(it->second),std::get<1>(it->second),std::get<2>(it->second));
    }
  }

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

UChar_t TrigTimeSvc::GetGroupFrequency(UChar_t board, UChar_t group)
{
  UChar_t freq = 2; // By default report 1GHz
  std::pair<UChar_t,UChar_t> addr = std::make_pair(board,group);
  if ( fTrigTimeMap.find(addr) != fTrigTimeMap.end() ) {
    //freq = fTrigTimeMap[addr].first;
    freq = std::get<0>(fTrigTimeMap[addr]);
  } else {
    printf("TrigTimeSvc::GetGroupFrequency - WARNING - Frequency for board %d group %d is not defined\n",board,group);
  }
  if (fVerbose>3) printf("TrigTimeSvc::GetGroupFrequency - Frequency for board %d group %d is %d\n",board,group,freq);
  return freq;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

UShort_t TrigTimeSvc::GetStartIndexCell(UChar_t board, UChar_t group)
{
  UShort_t sic = 0;
  std::pair<UChar_t,UChar_t> addr = std::make_pair(board,group);
  if ( fTrigTimeMap.find(addr) != fTrigTimeMap.end() ) {
    sic = std::get<1>(fTrigTimeMap[addr]);
  } else {
    printf("TrigTimeSvc::GetStartIndexCell - WARNING - SIC for board %d group %d is not defined\n",board,group);
  }
  if (fVerbose>3) printf("TrigTimeSvc::GetStartIndexCell - SIC for board %d group %d is %d\n",board,group,sic);
  return sic;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Double_t TrigTimeSvc::GetReferencePoint(UChar_t board, UChar_t group)
{
  Double_t refPoint = -100.;
  std::pair<UChar_t,UChar_t> addr = std::make_pair(board,group);
  if ( fTrigTimeMap.find(addr) != fTrigTimeMap.end() ) {
    //refPoint = fTrigTimeMap[addr].second;
    refPoint = std::get<2>(fTrigTimeMap[addr]);
  } else {
    printf("TrigTimeSvc::GetReferencePoint - WARNING - Reference point for board %d group %d is not defined\n",board,group);
  }
  if (fVerbose>3) printf("TrigTimeSvc::GetReferencePoint - Reference point for board %d group %d is %f\n",board,group,refPoint);
  return refPoint;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Double_t TrigTimeSvc::GetTimeDifference(UChar_t board, UChar_t group, Double_t point)
{
  // Returns time difference (in ns) from given point to reference point for given board/trigger group
  std::pair<UChar_t,UChar_t> addr = std::make_pair(board,group);
  if ( fTrigTimeMap.find(addr) == fTrigTimeMap.end() ) {
    printf("TrigTimeSvc::GetTimeDifference - WARNING - Reference point for board %d group %d is not defined\n",board,group);
    return 0.;
  }
  //std::pair<UChar_t,Double_t> value = fTrigTimeMap[addr];
  //UChar_t freq =  value.first;
  //Double_t refPoint = value.second;
  std::tuple<UChar_t,UShort_t,Double_t> value = fTrigTimeMap[addr];
  UChar_t freq =  std::get<0>(value);
  Double_t refPoint = std::get<2>(value);
  Double_t diff = point-refPoint;
  if (freq == 0) { // 5GHz
    return diff*0.2;
  } else if (freq == 1) { // 2.5GHz
    return diff*0.4;
  } else if (freq == 2) { // 1GHz
    return diff*1.;
  } else {
    printf("TrigTimeSvc::GetTimeDifference - WARNING - Frequency for board %d group %d is %d\n",board,group,freq);
    return 0.;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Double_t TrigTimeSvc::ComputeReferencePoint(UChar_t freq, Short_t* sample)
{

  const UShort_t nsmp = TADCTRIGGER_NSAMPLES;

  // Trigger is in NIM format: start by finding the minimum value of the waveform to define the -800mV level
  Short_t minVal = 4096;
  UShort_t minPos = 0;
  for (UShort_t s=0; s<nsmp; s++) {
    if (sample[s]<minVal) {
      minPos = s;
      minVal = sample[s];
    }
  }

  // Look for the 0mV level by finding the maximum value in the samples before the minimum
  Short_t maxVal = 0;
  //UShort_t maxPos = 0;
  for (UShort_t s=0; s<minPos; s++) {
    if (sample[s]>maxVal) {
      //maxPos = s;
      maxVal = sample[s];
    }
  }

  // Now find the sample where the midpoint between max and min is crossed
  UShort_t midPos = 0;
  Double_t midVal = 0.5*(Double_t)(maxVal+minVal);
  for (UShort_t s=0; s<minPos-1; s++) {
    if ( (sample[s]>midVal) && (sample[s+1]<=midVal) ) {
      midPos = s;
      break;
    }
  }

  // If midpoint is too early or too late in the waveform, reference time cannot be computed
  if ( (midPos<-fMaxAvgStart[freq]) || (midPos>nsmp-30-fMinAvgEnd[freq]) ) {
    printf("TrigTimeSvc::ComputeReferencePoint - WARNING - Raw midpoint is at %d. Cannot compute precise reference point\n",midPos);
    return (Double_t)midPos;
  }

  // Compute a better 0mV level by taking the average of samples before the midpoint
  Double_t maxAvg = 0.;
  for (UShort_t s=midPos+fMaxAvgStart[freq]; s<midPos+fMaxAvgEnd[freq]; s++) maxAvg += (Double_t)sample[s];
  maxAvg /= (Double_t)(fMaxAvgEnd[freq]-fMaxAvgStart[freq]);

  // Compute a better -800mV level by taking the average of samples after the midpoint
  Double_t minAvg = 0.;
  for (UShort_t s=midPos+fMinAvgStart[freq]; s<midPos+fMinAvgEnd[freq]; s++) minAvg += (Double_t)sample[s];
  minAvg /= (Double_t)(fMinAvgEnd[freq]-fMinAvgStart[freq]);

  // Position of precise midpoint is our reference value
  Double_t refPos = 0.;
  Double_t avgVal = 0.5*(maxAvg+minAvg);
  for (UShort_t s=midPos-20; s<midPos+20; s++) {
    if ( (sample[s]>avgVal) && (sample[s+1]<=avgVal) ) {
      refPos = (Double_t)s + ((Double_t)sample[s]-avgVal)/(Double_t)(sample[s]-sample[s+1]);
      break;
    }
  }
  if (refPos == 0.) {
    printf("TrigTimeSvc::ComputeReferencePoint - WARNING - Unable to compute precise reference point. Using raw midpoint %f\n",(Double_t)midPos);
    return (Double_t)midPos;
  }

  if (fVerbose>3) printf("TrigTimeSvc::ComputeReferencePoint - Reference point for freqency %d is %f\n",freq,refPos);

  return refPos;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Double_t TrigTimeSvc::ComputeReferencePointCF(UChar_t freq, Short_t* sample)
{

  const UShort_t nsmp = TADCTRIGGER_NSAMPLES;

  // Convert samples to double, subtract pedestal and overturn waveform
  Double_t samplesD[nsmp];
  for(UInt_t i=0; i<nsmp; i++) samplesD[i] = (Double_t)sample[i];
  Double_t pedestal = TMath::Mean(fPedestalSamples,samplesD);
  for(UInt_t i=0; i<nsmp; i++) samplesD[i] = pedestal-samplesD[i];

  // Apply constant fraction to waveform to look for precise hit time
  Double_t refPos = 0.;
  Double_t val;
  Double_t oldVal = 0.;
  Bool_t signalZone = false;
  for(Int_t i=fCFShift;i<nsmp;i++) {
    val = samplesD[i-fCFShift]-fCFRatio*samplesD[i];
    if (signalZone) {
      // Check if we crossed the constant fraction threshold
      if ( (oldVal < fCFThreshold) && (val >= fCFThreshold) ) {
	// Linearly interpolate the two bins to find precise time
	refPos = (Double_t)i-(val-fCFThreshold)/(val-oldVal);
	break;
      }
    } else if (val < -100.) signalZone = true;
    oldVal = val;
  }

  if (refPos == 0.) printf("TrigTimeSvc::ComputeReferencePointCF - WARNING - Unable to compute precise reference point\n");

  if (fVerbose>3) printf("TrigTimeSvc::ComputeReferencePointCF - Reference point for freqency %d is %f\n",freq,refPos);

  return refPos;

}
