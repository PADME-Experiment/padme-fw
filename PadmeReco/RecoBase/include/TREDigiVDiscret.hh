/**
 * \file TREDigiVDiscret.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include"TREVDigi.hh"
#include"TRERawVDiscret.hh"

#ifndef _TREDigiVDiscret_hh_
#define _TREDigiVDiscret_hh_ 0
/**
 * \class TREDigiVDiscret
 * \brief RE Digi prototype class for Discret/Non sampling
 * digitizers
 *
 * The class contains packed DAQ-Channel infomation for
 * Discret/Non sampling digitizers. Such as discriminators
 * and time stamplers.
 */
class TREDigiVDiscret:TREVDigi{
  public:
  /// Add Raw chuncks related to this Digi
    void AddRaw(const TRERawVDiscret *const raw){
      fAssocDigis.push_back(raw);} //
  private:
    std::vector<const TRERawVDiscret*> fAssocDigis;
};
#endif
