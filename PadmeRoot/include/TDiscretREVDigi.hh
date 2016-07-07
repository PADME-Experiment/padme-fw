/**
 * \file TDiscretREVDigi.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include"TREVDigi.hh"
#include"TDiscretREVRaw.hh"

#ifndef _TDiscretREVDigi_hh_
#define _TDiscretREVDigi_hh_ 0
/**
 * \class TDiscretREVDigi
 * \brief RE Digi prototype class for Discret/Non sampling
 * digitizers
 *
 * The class contains packed DAQ-Channel infomation for
 * Discret/Non sampling digitizers. Such as discriminators
 * and time stamplers.
 */
class TDiscretREVDigi:TREVDigi{
  public:
  /// Add Raw chuncks related to this Digi
    void AddRaw(const TDiscretREVRaw *const raw){
      fAssocDigis.push_back(raw);} //
  private:
    std::vector<const TDiscretREVRaw*> fAssocDigis;
};
#endif
