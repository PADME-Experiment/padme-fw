#ifndef __PADME_RECO__ADC_CELL_CALIB_HISTOGRAMS_H__
#define __PADME_RECO__ADC_CELL_CALIB_HISTOGRAMS_H__ 0

#include"TADCBoard.hh"

#include<TH2S.h>
#include<TFile.h>

#include<string>

class ADCCellCalibHistograms{
  public:
    struct hists_t{
      TH2S* nocal_cell;
      TH2S* calib_cell;
      TH2S* nocal_time;
      TH2S* calib_time;
    };
    void FillHistograms(TADCBoard* raw_board, std::string det_name, bool is_calibrated);
    void WriteHistograms(TFile*);
    ADCCellCalibHistograms(const std::string& dirname, unsigned ylow=3300, unsigned yhigh=4100):
      fYLow(ylow),
      fYHigh(yhigh),
      fDirName(dirname)
  {}
  private:
    ADCCellCalibHistograms();
    unsigned fYLow, fYHigh;
    std::string fDirName;
    std::map<int, hists_t[32]> fHists;
    void CreateHisos(TADCBoard* raw_board, const std::string &det_name);
};
#endif
