#include"ADCCellCalibHistograms.hh"

#include<sstream>

void ADCCellCalibHistograms::FillHistograms(TADCBoard* raw_board, std::string det_name, bool is_calibrated){
  int board_sn = raw_board->GetBoardSN();
  if(fHists.count(board_sn)==0){
    CreateHisos(raw_board, det_name);
  }
  for(unsigned chan_i=0; chan_i<raw_board->GetNADCChannels(); chan_i++){
    TADCChannel *raw_chan = raw_board->ADCChannel(chan_i);
    hists_t& hist = fHists[board_sn][chan_i];
    int start_index_cell = raw_board->ADCTrigger(chan_i/8)->GetStartIndexCell();
    for(unsigned t=0; t < 1000; t++){ // ignore last 24 samples
      const int p  = (t+start_index_cell)%raw_chan->GetNSamples();
      const int Ct = raw_chan->GetSample(t);
      if(is_calibrated){
        hist.calib_cell->Fill(p, Ct);
        hist.calib_time->Fill(t, Ct);
      }else{
        hist.nocal_cell->Fill(p, Ct);
        hist.nocal_time->Fill(t, Ct);
      }
    }
  }
}

void ADCCellCalibHistograms::CreateHisos(TADCBoard* raw_board, const std::string &det_name){
  int board_sn = raw_board->GetBoardSN();
  for(int chan_i=0; chan_i<32; chan_i++){
    std::stringstream name;
    name<<det_name<<"_sn"<<board_sn<<"_b"<<int(raw_board->GetBoardId())<<"_ch"<<chan_i<<"_";
    fHists[board_sn][chan_i].nocal_cell = new TH2S((name.str()+"nocal_cell").c_str(),(name.str()+"nocal_cell;CellID; "       ).c_str(),1024,0-.5,1024-.5, fYHigh-fYLow, fYLow-.5,fYHigh-.5);
    fHists[board_sn][chan_i].calib_cell = new TH2S((name.str()+"calib_cell").c_str(),(name.str()+"calib_cell;CellID; "       ).c_str(),1024,0-.5,1024-.5, fYHigh-fYLow, fYLow-.5,fYHigh-.5);
    fHists[board_sn][chan_i].nocal_time = new TH2S((name.str()+"nocal_time").c_str(),(name.str()+"nocal_time;Time[samples]; ").c_str(),1024,0-.5,1024-.5, fYHigh-fYLow, fYLow-.5,fYHigh-.5);
    fHists[board_sn][chan_i].calib_time = new TH2S((name.str()+"calib_time").c_str(),(name.str()+"calib_time;Time[samples]; ").c_str(),1024,0-.5,1024-.5, fYHigh-fYLow, fYLow-.5,fYHigh-.5);
  }
}

void ADCCellCalibHistograms::WriteHistograms(TFile*rootfile){
  rootfile->cd("/");
  rootfile->mkdir(fDirName.c_str())->cd();
  for(auto hist_it=fHists.begin(); hist_it!=fHists.end(); hist_it++){
    for(int chan_i=0; chan_i<32; chan_i++){
      hist_it->second[chan_i].nocal_cell->Write();
      hist_it->second[chan_i].calib_cell->Write();
      hist_it->second[chan_i].nocal_time->Write();
      hist_it->second[chan_i].calib_time->Write();
    }
  }
}
