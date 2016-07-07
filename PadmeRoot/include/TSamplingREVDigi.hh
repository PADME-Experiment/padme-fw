/**
 * \file TSamplingREVDigi.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include"TREVDigi.hh"
#include"TSamplingREVRaw.hh"
#include<string>

#ifndef _TSamplingREVDigi_hh_
#define _TSamplingREVDigi_hh_ 0
/**
 * \class TSamplingREVDigi
 * \brief RE Digi prototype class for Sampling digitizers
 *
 * The class contains packed DAQ-Channel infomation for
 * Sampling digitizers. Such as ADCs.
 */
class TSamplingREVDigi:TREVDigi{
  public:
  /// Add Raw chuncks related to this Digi
    void AddRaw(const TSamplingREVRaw *const raw){
      fAssocDigis.push_back(raw);} //
  private:
    int fIndexBegin;
    int fIndexEnd;
    double fTimeStart;
    double fTimeStep;
    std::vector<const TSamplingREVRaw*> fAssocDigis;
};
#endif
