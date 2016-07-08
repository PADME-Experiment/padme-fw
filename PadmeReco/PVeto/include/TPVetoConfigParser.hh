/**
 * \file TPVetoConfigParser.hh
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

#ifndef _TPVetoConfigParser_hh_
#define _TPVetoConfigParser_hh_ 0

class TPVetoConfigParser:TDetVConfigParser{
  public:
    TPVetoConfigParser(std::string a):TDetVConfigParser(a){ }
};
#endif
