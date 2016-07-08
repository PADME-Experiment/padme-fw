/**
 * \file TREDigiVSampling.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include"TREVDigi.hh"
#include"TRERawVSampling.hh"
#include<string>

#ifndef _TREDigiVSampling_hh_
#define _TREDigiVSampling_hh_ 0
/**
 * \class TREDigiVSampling
 * \brief RE Digi prototype class for Sampling digitizers
 *
 * The class contains packed DAQ-Channel infomation for
 * Sampling digitizers. Such as ADCs.
 */
class TREDigiVSampling:TREVDigi{
  public:
  /// Add Raw chuncks related to this Digi
    void AddRaw(const TRERawVSampling *const raw){
      fAssocDigis.push_back(raw);} //
  private:
    int fIndexBegin;
    int fIndexEnd;
    double fTimeStart;
    double fTimeStep;
    std::vector<const TRERawVSampling*> fAssocDigis;
};
#endif
