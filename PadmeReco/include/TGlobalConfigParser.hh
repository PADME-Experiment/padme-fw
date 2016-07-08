/**
 * \file TGlobalConfigParser.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
#include"TVConfigParser.hh"

#ifndef _TGlobalConfigParser_hh_
#define _TGlobalConfigParser_hh_ 0


class TGlobalConfigParser:TVConfigParser{
  public:
    TGlobalConfigParser(std::string a):TVConfigParser(a){}
    std::string GetConfig(std::string grp,std::string cfg){
      return TVConfigParser::GetConfig(grp,cfg);}
    bool HasConfig(std::string grp,std::string cfg){
      return TVConfigParser::HasConfig(grp,cfg);}
};

#endif
