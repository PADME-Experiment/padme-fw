#include"ADCCellCalibSums.hh"

#include<stdint.h>
#include<fstream>

typedef union{
  struct{
    uint8_t btf      :1; //0: BTF trigger
    uint8_t cosmic   :1; //1: cosmics trigger
    uint8_t unused1  :1; //2: unused
    uint8_t local    :1; //3: local trigger (from dual-timer module)
    uint8_t unused2  :1; //4: unused
    uint8_t reserved :1; //5: reserved
    uint8_t random   :1; //6: random trigger
    uint8_t delayed  :1; //7: delayed trigger
  };
  uint32_t all;
}trig_type_t;

void ADCCellCalibSums::Process(TRawEvent* ev){
  if(fNEvtsProcessed > 65535) return; // overflow protection
  trig_type_t trig_mask;
  trig_mask.all = ev->GetEventTrigMask();
  if(trig_mask.btf)return;
  if(!(trig_mask.local || trig_mask.random || trig_mask.cosmic || trig_mask.delayed))return;
  fNEvtsProcessed++;
  for(unsigned board_i=0; board_i<ev->GetNADCBoards(); board_i++){
    TADCBoard* board = ev->ADCBoard(board_i);
    for(unsigned chan_i=0; chan_i<board->GetNADCChannels(); chan_i++){
      TADCChannel *chan = board->ADCChannel(chan_i);
      int start_index_cell = board->ADCTrigger(chan_i/8)->GetStartIndexCell();
      for(unsigned t=0; t < 1000; t++){ // ignore last 24 samples
        const int p  = (t+start_index_cell)%chan->GetNSamples();
        const int Ct = chan->GetSample(t);
        capacitor_sum_t &sums = fAllBoardSums[board->GetBoardSN()][chan_i][p];
        uint128_t tx;
        tx  = 1; sums.n   += tx; sums.Ct  += tx * Ct; // tx = t **  0
        tx *= t; sums.t   += tx; sums.tCt += tx * Ct; // tx = t **  1
        tx *= t; sums.t2  += tx; sums.t2Ct+= tx * Ct; // tx = t **  2
        tx *= t; sums.t3  += tx; sums.t3Ct+= tx * Ct; // tx = t **  3
        tx *= t; sums.t4  += tx; sums.t4Ct+= tx * Ct; // tx = t **  4
        tx *= t; sums.t5  += tx; sums.t5Ct+= tx * Ct; // tx = t **  5
        tx *= t; sums.t6  += tx;                      // tx = t **  6
        tx *= t; sums.t7  += tx;                      // tx = t **  7
        tx *= t; sums.t8  += tx;                      // tx = t **  8
        tx *= t; sums.t9  += tx;                      // tx = t **  9
        tx *= t; sums.t10 += tx;                      // tx = t ** 10
      }
    }
  }
}
void ADCCellCalibSums::WriteFile(){
  std::ofstream outF_linreg(fSumsOutFileName.c_str(), std::ios_base::trunc);
  outF_linreg.precision(40);
  outF_linreg<<"{"<<std::endl;
  for(auto b_it=fAllBoardSums.begin();b_it!=fAllBoardSums.end();b_it++){
    outF_linreg<<'"'<<b_it->first<<"\": ["<<std::endl;
    for(int ch=0; ch<32; ch++){
      outF_linreg<<"["<<std::endl;
      for(int cell=0; cell<1024;cell++){
        auto &summ = fAllBoardSums[b_it->first][ch][cell];
        outF_linreg
          <<"{"
          <<"\"n\": "      <<summ.n    <<", "
          <<"\"t\": "      <<summ.t    <<", "
          <<"\"t2\": "     <<summ.t2   <<", "
          <<"\"t3\": "     <<summ.t3   <<", "
          <<"\"t4\": "     <<(long double)summ.t4   <<", "
          <<"\"t5\": "     <<(long double)summ.t5   <<", "
          <<"\"t6\": "     <<(long double)summ.t6   <<", "
          <<"\"t7\": "     <<(long double)summ.t7   <<", "
          <<"\"t8\": "     <<(long double)summ.t8   <<", "
          <<"\"t9\": "     <<(long double)summ.t9   <<", "
          <<"\"t10\": "    <<(long double)summ.t10  <<", "
          <<"\"Ct\": "     <<summ.Ct   <<", "
          <<"\"tCt\": "    <<summ.tCt  <<", "
          <<"\"t2Ct\": "   <<summ.t2Ct <<", "
          <<"\"t3Ct\": "   <<(long double)summ.t3Ct <<", "
          <<"\"t4Ct\": "   <<(long double)summ.t4Ct <<", "
          <<"\"t5Ct\": "   <<(long double)summ.t5Ct
          <<"}";
        if(cell<1023){
          outF_linreg<<","<<std::endl;
        }else{
          outF_linreg<<std::endl;
        }
      }
      if(ch<31){ //channels
        outF_linreg<<"],"<<std::endl;
      }else{
        outF_linreg<<"]"<<std::endl;
      }
    }
    if(b_it->first != fAllBoardSums.rbegin()->first){
      outF_linreg<<"],"<<std::endl;
    }else{
      outF_linreg<<"]"<<std::endl;
    }
  }
  outF_linreg<<"}"<<std::endl;
  outF_linreg<<std::flush;
}
