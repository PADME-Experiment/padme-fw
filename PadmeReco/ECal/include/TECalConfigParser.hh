/**
 * \file TECalConfigParser.hh
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

#ifndef _TECalConfigParser_hh_
#define _TECalConfigParser_hh_ 0

class TECalConfigParser:public TDetVConfigParser{
  public:
    TECalConfigParser(std::string a):TDetVConfigParser(a){ }
};
#endif
