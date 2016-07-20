/**
 * \file TREVDigi.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include<vector>
#include"TREVRaw.hh"

#ifndef _TREVDigi_hh_
#define _TREVDigi_hh_ 0
/**
 * \class TREVDigi
 * \brief RE Digi prototype class
 *
 * The class contains packed DAQ-Channel infomation.
 */
class TREVDigi{
  public:
    void AddRaw(TREVRaw *raw);
  protected:
    typedef typename fw::TSimpleList<TREVRaw*> rawCont_t;
    rawCont_t  fAssocRaws;
    int fDAQChannelId;
    int fDAQBoardId;
};
#endif
