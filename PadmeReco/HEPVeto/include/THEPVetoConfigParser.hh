/**
 * \file THEPVetoConfigParser.hh
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

#ifndef _THEPVetoConfigParser_hh_
#define _THEPVetoConfigParser_hh_ 0

class THEPVetoConfigParser:TDetVConfigParser{
  public:
    THEPVetoConfigParser(std::string a):TDetVConfigParser(a){ }
};
#endif
