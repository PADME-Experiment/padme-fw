#include"ADCCellCalibData.hh"
#include<json/json.h>
#include<fstream>

ADCCellCalibData::ADCCellCalibData(const std::string &caldatjson){
  if(caldatjson.size()==0){
    return;
  }
  Json::Value caldat;
  std::ifstream caldat_file(caldatjson.c_str(), std::ifstream::binary);
  caldat_file >> caldat;
  for(auto it=caldat.begin();it!=caldat.end();it++){
    for(int ch=0;ch<32;++ch){
      for(int cell=0;cell<1024;++cell){
        fData[std::atoi(it.key().asString().c_str())][ch][cell][0]=
          caldat[it.key().asString()][ch][cell][0].asDouble();
        fData[std::atoi(it.key().asString().c_str())][ch][cell][1]=
          caldat[it.key().asString()][ch][cell][1].asDouble();
        fData[std::atoi(it.key().asString().c_str())][ch][cell][2]=
          caldat[it.key().asString()][ch][cell][2].asDouble();
        fData[std::atoi(it.key().asString().c_str())][ch][cell][3]=
          caldat[it.key().asString()][ch][cell][3].asDouble();
        fData[std::atoi(it.key().asString().c_str())][ch][cell][4]=
          caldat[it.key().asString()][ch][cell][4].asDouble();
        fData[std::atoi(it.key().asString().c_str())][ch][cell][5]=
          caldat[it.key().asString()][ch][cell][5].asDouble();
      }
    }
  }
}

void ADCCellCalibData::CalibrateADC(TADCBoard* raw_board){
  int n_chans = raw_board->GetNADCChannels();
  for(int chan_i=0; chan_i<n_chans; chan_i++){
    TADCChannel* raw_chan = raw_board->ADCChannel(chan_i);
    int start_cell = raw_board->ADCTrigger(chan_i/8)->GetStartIndexCell();
    double (*chan_caldat)[6] = fData[raw_board->GetBoardSN()][chan_i];
    auto raw_samp = raw_chan->GetSamplesArray();
    int n_samp = raw_chan->GetNSamples();
    for(int t=0; t<n_samp; t++){
      long long int tPower = 1;
      for(int power=0; power<6; power++){
        raw_samp[t] -= chan_caldat[(t+start_cell)%1024][power]*tPower;
        tPower*=t;
      }
    }
  }
}
