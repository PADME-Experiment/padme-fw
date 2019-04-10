#include "PadmeVTrigger.hh"
#include <iostream>

PadmeVTrigger::PadmeVTrigger(){
  ;
};

PadmeVTrigger::~PadmeVTrigger(){
  ;
}



void PadmeVTrigger::Init(PadmeVRecoConfig *cfg){
  ;
}

double PadmeVTrigger::GetTriggerTime( int bID,int gID){
  //find returns both keys+variable
  return fTriggerTime[std::make_pair(bID,gID)];
}
double PadmeVTrigger::GetChannelTriggerTime( int bID,int ich){
  //find returns both keys+variable
  if(ich > 31 || ich < 0) {
    std::cout << "Error: Wrong channel ID" << std::endl;

  }
    
  int gID = ich/8;
  return fTriggerTime[std::make_pair(bID,gID)];
}

double PadmeVTrigger::GetFrequency( int bID,int gID){
  return fSamplingFrequency[std::make_pair(bID,gID)];
  //fSamplingFrequency.find(std::make_pair(Board,igr))->second;
}


void PadmeVTrigger::ProcessTrigger(int bID, TADCTrigger* tr){
  
  int freqflag = tr->GetFrequency();
  int gID  = tr->GetGroupNumber();
  if(gID < 0 || gID > 3) {
    std::cout << "ERROR: Invalid group ID" << std::endl;
  }

  //  fSamplingFrequency.insert(std::pair<  std::pair<int,int>, UChar_t > (std::make_pair(bID,gID),freq) );

  double freq;
  if(freqflag == 0) freq = 5.; // IN GSPS!
  if(freqflag == 1) freq = 2.5;
  if(freqflag == 2) freq = 1.;
  if(freqflag > 2) 
    std::cout << "Unknown frequency..... CHECK!" << std::endl;

  fSamplingFrequency[std::make_pair(bID,gID)] = freq;

  //  std::cout << "Board: " << bID << " gID:  " <<  gID  <<  "  frequency:  " <<  fSamplingFrequency.find(std::make_pair(bID,gID))->second << std::endl;
  //  std::cout << "Board: " << bID << " gID:  " <<  gID  <<  "  frequency:  " <<  fSamplingFrequency[std::make_pair(bID,gID)] << std::endl;

  double Time = CalcTime(tr->GetSamplesArray());
  fTriggerTime[std::make_pair(bID,gID)] = Time*GetTimeBin(bID,gID);
}

double PadmeVTrigger::CalcTime(Short_t * sample){
  int ns = TADCTRIGGER_NSAMPLES;

  //  std::cout << " === Trigger === "  << std::endl;

  float avgmax;
  float avgmin;
  float middle;

  float avg;

  int min;
  int imin;
  int max;
  int imax;
  double time;

  min = 4096;
  max = 0;
  for(int ss=0;ss<ns;ss++){
    if(sample[ss] > max){
      max = sample[ss];
      imax = ss;
    }

    if(sample[ss] < min){
      min = sample[ss];
      imin = ss;
    }
    //    std::cout <<"sample[" << ss<< "]="  <<  sample[ss] << std::endl;    
  }

  middle = (min*1. + max*1.)/2.;
  
  int imiddle=0;

  for(int ss=0;ss<ns-1;ss++){
    if(sample[ss] > middle && sample[ss+1]< middle  ){
      imiddle = ss;
      break;
    }    
  }

  avgmax = 0.;
  for (int ss = imiddle-20; ss >  imiddle - 100; ss--) {
    avgmax+= sample[ss];
  } 
  avgmax/=80.;

  
  avgmin = 0.;
  for (int ss = imiddle+20; ss <  imiddle + 100; ss++) {
    avgmin+= sample[ss];
  } 
  avgmin/=80.;

  avg = (avgmin + avgmax)/2.;


  for (int ss = imiddle-20;ss < imiddle + 20; ss++) {
    if (sample[ss] > avg && sample[ss+1] < avg){
      time = ss + (sample[ss] - avg )/ (sample[ss] - sample[ss+1]);
    }
  }

  //  std::cout << "Trigger time:  " << time << std::endl;

  return time;
  
  
}


double PadmeVTrigger::GetTimeBin(int bID,int gID){
  return 1./ fSamplingFrequency[std::make_pair(bID,gID)];
}
