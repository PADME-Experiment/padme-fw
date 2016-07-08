/**
 * \file PadmeReco/RecoBase/include/TDetVConfigParser.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-08
 */

#include"TVConfigParser.hh"

#ifndef _TDetVConfigParser_hh_
#define _TDetVConfigParser_hh_ 0

class TDetVConfigParser:TVConfigParser{
  public:
    TDetVConfigParser(std::string a):TVConfigParser(a){ }
};

#endif
