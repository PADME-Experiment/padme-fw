/**
 * \file TTargetConfigParser.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-08
 */

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
#include"TDetVConfigParser.hh"

#ifndef _TTargetConfigParser_hh_
#define _TTargetConfigParser_hh_ 0

class TTargetConfigParser:public TDetVConfigParser{
  public:
    TTargetConfigParser(std::string a):TDetVConfigParser(a){ }
};
#endif
