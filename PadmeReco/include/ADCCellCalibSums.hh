#ifndef __PADME_RECO__ADC_CELL_CALIB_SUMS_H__
#define __PADME_RECO__ADC_CELL_CALIB_SUMS_H__ 0

#include "TRawEvent.hh"

#include<stdint.h>
#include<string>
#include<map>
#include<sstream>

class ADCCellCalibSums{
  private:
    typedef unsigned __int128 uint128_t;
    struct capacitor_sum_t{
      capacitor_sum_t():
        n     (0),
        t     (0),
        t2    (0),
        t3    (0),
        t4    (0),
        t5    (0),
        t6    (0),
        t7    (0),
        t8    (0),
        t9    (0),
        t10   (0),
        Ct    (0),
        tCt   (0),
        t2Ct  (0),
        t3Ct  (0),
        t4Ct  (0),
        t5Ct  (0)
      {}
      // An overflow can occur when the number of
      // processed events is bigger than 65535
      uint16_t     n   ; // 1<<16 65535
      uint32_t     t   ; // 1<<32/1024**1 4194304
      uint64_t     t2  ;
      uint64_t     t3  ;
      uint64_t     t4  ; // 1<<64/1024**4 16777216
      uint128_t    t5  ;
      uint128_t    t6  ;
      uint128_t    t7  ;
      uint128_t    t8  ;
      uint128_t    t9  ;
      uint128_t    t10 ; // 1<<128/1024**10 268435456
      uint64_t     Ct  ;
      uint64_t     tCt ;
      uint64_t     t2Ct;
      uint64_t     t3Ct; // 1<<64/(4096*1024**3) 4194304
      uint128_t    t4Ct;
      uint128_t    t5Ct;
    };
  public:
    ADCCellCalibSums(const std::string& sums_out_file_name):
      fNEvtsProcessed(0) {
        fSumsOutFileName = sums_out_file_name;
      }
    void Process(TRawEvent* ev);
    void WriteFile();
  private:
    ADCCellCalibSums();
    std::string fSumsOutFileName;
    std::map<int, capacitor_sum_t[32][1024]> fAllBoardSums;
    unsigned fNEvtsProcessed;
};
#endif
