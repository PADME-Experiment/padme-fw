/**
 * \file TEVetoConfigParser.hh
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

#ifndef _TEVetoConfigParser_hh_
#define _TEVetoConfigParser_hh_ 0

class TEVetoConfigParser:TDetVConfigParser{
  public:
    TEVetoConfigParser(std::string a):TDetVConfigParser(a){ }
};
#endif
