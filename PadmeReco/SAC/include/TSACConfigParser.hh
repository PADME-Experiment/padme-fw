/**
 * \file TSACConfigParser.hh
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

#ifndef _TSACConfigParser_hh_
#define _TSACConfigParser_hh_ 0

class TSACConfigParser:TDetVConfigParser{
  public:
    TSACConfigParser(std::string a):TDetVConfigParser(a){ }
};
#endif
