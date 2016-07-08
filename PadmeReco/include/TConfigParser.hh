/**
 * \file TConfigParser.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<map>

#ifndef _TConfigParser_hh_
#define _TConfigParser_hh_ 0

// to be included in makefile

class TVConfigParser{
  public:
    void Verbose(){
      std::cout<<std::endl;
      std::cout<<"Process config file "<<fFileName<<std::endl;
      for(std::map<std::string,std::vector<std::string> > ::iterator
          map_it=fConfigLines.begin(); map_it!=fConfigLines.end();++map_it){
        for(std::vector<std::string>::iterator vec_it=map_it->second.begin();vec_it!=map_it->second.end();++vec_it){
          std::cout<<map_it->first<<"  <-  "<<*vec_it<<std::endl;
        }
      }
      std::cout<<std::endl;
    }

    TVConfigParser(std::string icfilename){
      fFileName=icfilename;
      std::ifstream icfile(icfilename,std::ifstream::in);
      if(icfile.is_open()){
        while(!icfile.eof()){
          std::string tmpstr;
          std::getline(icfile,tmpstr);
          //tmpstr.erase(std::remove(tmpstring.begin(),tmpstring.end(),' '),tmpstring.end());
          if(tmpstr.find_first_of('#')!=std::string::npos)
            tmpstr.erase(tmpstr.find_first_of('#'),tmpstr.length()); // # comment char
          if(tmpstr.size()>0){
            std::stringstream ss(tmpstr);
            std::string configfield;
            if(ss>>configfield)
              fConfigLines[configfield].clear();
            while(ss>>tmpstr) {
              fConfigLines[configfield].push_back(tmpstr);
            }
          }
        }
      }else{
        std::cerr<<"File not found: \""<<icfilename<<"\""<<std::endl;
      }
      Verbose();
    }
    std::string GetConfig(std::string cfg){
      if(fConfigLines.find(cfg)!=fConfigLines.end() && fConfigLines[cfg].size()==1)
        return fConfigLines[cfg][0];
      return "";
    }
    bool HasConfig(std::string cfg){
      return fConfigLines.find(cfg)!=fConfigLines.end() && fConfigLines[cfg].size()==1;
    }

  protected:
    std::string fFileName;
    std::map<std::string,std::vector<std::string> > fConfigLines;

};

class TDetVConfigParser:TVConfigParser{
  public:
    TDetVConfigParser(std::string a):TVConfigParser(a){ }
};


class TTargetConfigParser:TDetVConfigParser{
  public:
    TTargetConfigParser(std::string a):TDetVConfigParser(a){ }
};
class TEVetoConfigParser:TDetVConfigParser{
  public:
    TEVetoConfigParser(std::string a):TDetVConfigParser(a){ }
};
class TPVetoConfigParser:TDetVConfigParser{
  public:
    TPVetoConfigParser(std::string a):TDetVConfigParser(a){ }
};
class THEPVetoConfigParser:TDetVConfigParser{
  public:
    THEPVetoConfigParser(std::string a):TDetVConfigParser(a){ }
};
class TECalConfigParser:TDetVConfigParser{
  public:
    TECalConfigParser(std::string a):TDetVConfigParser(a){ }
};
class TSACConfigParser:TDetVConfigParser{
  public:
    TSACConfigParser(std::string a):TDetVConfigParser(a){ }
};


class TGlobalConfigParser:TVConfigParser{
  public:
    TGlobalConfigParser(std::string a):TVConfigParser(a){}
    std::string GetConfig(std::string cfg){return TVConfigParser::GetConfig(cfg);}
    bool HasConfig(std::string cfg){return TVConfigParser::HasConfig(cfg);}


};



#endif
