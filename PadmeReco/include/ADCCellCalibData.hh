#ifndef __PADME_RECO__ADC_CELL_CALIB_DATA_H__
#define __PADME_RECO__ADC_CELL_CALIB_DATA_H__ 0

#include"TADCBoard.hh"
#include<map>
#include<string>

class ADCCellCalibData{
  public:
    ADCCellCalibData(const std::string &caldatjson);
    void CalibrateADC(TADCBoard* raw_board);

  private:
    ADCCellCalibData();
    std::map<unsigned int, double[32][1024][6]> fData;
};

#endif
