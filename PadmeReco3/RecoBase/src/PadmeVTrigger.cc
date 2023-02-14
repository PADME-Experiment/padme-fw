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
  
  /*
  if (bID==27 && gID==2) {
    Short_t * sample = tr->GetSamplesArray();
    for (unsigned int j=0; j<TADCTRIGGER_NSAMPLES; ++j)std::cout<<"dID 27; gID 2 sample @ "<<j<<" = "<<sample[j]<<std::endl;
  }
  if (bID==27 && gID==3) {
    Short_t * sample = tr->GetSamplesArray();
    for (unsigned int j=0; j<TADCTRIGGER_NSAMPLES; ++j)std::cout<<"dID 27; gID 3 sample @ "<<j<<" = "<<sample[j]<<std::endl;
  }
  */
  double Time = CalcTime(tr->GetSamplesArray());
  if (Time < -100.) std::cout << "Trigger time NOT COMPUTED: (set to " << Time << ") for Board: " << bID << " group gID:  "<< gID<<std::endl;
  fTriggerTime[std::make_pair(bID,gID)] = Time*GetTimeBin(bID,gID);
  //std::cout << "Processed trigger for Board: " << bID << " group gID:  "<< gID<<" trigger time = "<<Time*GetTimeBin(bID,gID)<<" time bin = "<<GetTimeBin(bID,gID)<<std::endl;;

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
  double time=-10000.;
  int imiddle=0;

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

  //std::cout<<"1)min, max, middle "<<min<<" "<<max<<" "<<middle<<" imin, imax, imiddle "<< imin <<" "<< imax <<" "<<imiddle<<std::endl;
  if (imin<imax) {
    //std::cout<<" since imin < imax look for another max"<<std::endl;
    //The logic assumes that the signal starts high and then goes low.... the max must be found before the min
    max=0;
    for(int ss=0;ss<imin;ss++){
      if(sample[ss] > max){
	max = sample[ss];
	imax = ss;
      }
    }
  }

  //std::cout<<"2)min, max, middle "<<min<<" "<<max<<" "<<middle<<" imin, imax, imiddle "<< imin <<" "<< imax <<" "<<imiddle<<std::endl;
  middle = (min*1. + max*1.)/2.;
  

  for(int ss=0;ss<ns-1;ss++){
    //std::cout<<ss<<" bohhhh "<<sample[ss]<<" "<<middle<<sample[ss+1]<<std::endl;
    if(sample[ss] > middle && sample[ss+1]<= middle  ){
      imiddle = ss;
      //std::cout<<" imiddle = "<<imiddle<<std::endl;
      break;
    }    
  }
  //std::cout<<"3)min, max, middle "<<min<<" "<<max<<" "<<middle<<" imin, imax, imiddle "<< imin <<" "<< imax <<" "<<imiddle<<std::endl;

  
  avgmax = 0.;
  int count = 0;
  for (int ss = imiddle-20; ss >  imiddle - 100; ss--) {
    if (ss < 0 || ss > ns-1) continue;
    count++;
    avgmax+= sample[ss];
  } 
  if (count > 0) avgmax/=(double)count;
  //std::cout<<" avgmax = "<<avgmax<<" over ncounts ="<<count<<std::endl;
 
  count=0;
  avgmin = 0.;
  count = 0;
  for (int ss = imiddle+20; ss <  imiddle + 100; ss++) {
    if (ss < 0 || ss > ns-1) continue;
    count++;
    avgmin+= sample[ss];
  } 
  if (count > 0) avgmin/=(double)count;
  //std::cout<<" avgmin = "<<avgmin<<" over ncounts ="<<count<<std::endl;
 
  avg = (avgmin + avgmax)/2.;
  //std::cout<<" avg = "<<avg<<std::endl;

  count = 0;
  for (int ss = imiddle-20;ss < imiddle + 20; ss++) {
    if (ss < 0 || ss > ns-1) continue;
    count++;
    //std::cout<<" search ... ss="<<ss<<" sample[ss]="<<sample[ss]<<" avg="<<avg<<std::endl;
    if (sample[ss] >= avg && sample[ss+1] < avg){
      time = ss + (sample[ss] - avg )/ (sample[ss] - sample[ss+1]);
    }
  }

  if (time < -100.) {
    std::cout << "Trigger time NOT COMPUTED"<<std::endl;
    std::cout << "Trigger time NOT COMPUTED: min, max, middle, imin, imax, imiddle, avg "<< min<<" "<<max<<" "<<middle<<" "
	      <<imin<<" "<<imax<<" "<<imiddle<<" "<<avg<<std::endl;
    std::cout << "unsuccessfull search for Trigger time done over "<<count<<" samples:"<<std::endl;
    for (int ss = imiddle-20;ss < imiddle + 20; ss++ ) std::cout<<" sample "<<ss<<" value "<<sample[ss]<<std::endl;
  }
  //else std::cout << "Trigger time:  " << time << std::endl;

  return time;
  
  
}


double PadmeVTrigger::GetTimeBin(int bID,int gID){
  return 1./ fSamplingFrequency[std::make_pair(bID,gID)];
}
