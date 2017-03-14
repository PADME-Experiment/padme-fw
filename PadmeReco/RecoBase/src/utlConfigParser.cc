/**
 * \file PadmeReco/RecoBase/src/utlConfigParser.cc
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-08
 */

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<map>

#include"utlConfigParser.hh"




  utl::ConfigParser::ConfigParser
(const std::string&fn):fFileName(fn)
{
  std::ifstream icfile(fFileName.c_str());
  int lineNum=0;
  if(!icfile.is_open()) throw ConfigParser_except().FileNotFound(fFileName);
  std::string cfggroupname="Default";
  while(!icfile.eof()){
    std::string tmpstr;
    std::getline(icfile,tmpstr);
    ++lineNum;
    if(!(tmpstr.length()>0))continue;
    if(tmpstr.find_first_of('#')!=std::string::npos)
      tmpstr.erase(tmpstr.find_first_of('#'),tmpstr.length()); // # comment char
    if(!(tmpstr.length()>0))continue;
    tmpstr=tmpstr.substr(
        tmpstr.find_first_not_of(" \t\f\v\n\r"),
        tmpstr.find_last_not_of(" \t\f\v\n\r")+1
        );
    if(!(tmpstr.length()>0))continue;
    if(tmpstr[0]=='[') {
      if(tmpstr[tmpstr.length()-1]==']'){
        if(!(tmpstr.length()>2))
          throw ConfigParser_except().SyntaxErrorEmptyGroupName(fFileName,lineNum);
        tmpstr=tmpstr.substr(1,tmpstr.length()-2);
        cfggroupname=tmpstr;
      }
      continue;
    }
    std::stringstream ss(tmpstr);
    std::string configfield;
    ss>>configfield;
    if(fConfigLines[cfggroupname].count(configfield)){
      throw ConfigParser_except().SyntaxErrorConfigFieldRedefinition(fFileName,lineNum,configfield);
    }
    while(ss>>tmpstr) {
      fConfigLines[cfggroupname][configfield].push_back(tmpstr);
    }
    if(!(fConfigLines[cfggroupname][configfield].size()>0)){
      fConfigLines[cfggroupname][configfield];
      //throw ConfigParser_except().SyntaxErrorArgumentExpected(fFileName,lineNum);
    }
  }
}


  void
utl::ConfigParser::Print()
  const
{
  std::cout<<std::endl;
  std::cout<<"Process config file "<<fFileName<<std::endl;
  for(auto groupmap_it=fConfigLines.begin(); groupmap_it!=fConfigLines.end();++groupmap_it){
    for(auto map_it=groupmap_it->second.begin(); map_it!=groupmap_it->second.end();++map_it){
      std::cout<<"["<<groupmap_it->first<<"]  '"<<map_it->first<<"' = {";
      for(auto vec_it=map_it->second.begin();vec_it!=map_it->second.end();++vec_it){
        std::cout<<" '"<<*vec_it<<"'";
      }
      std::cout<<" }"<<std::endl;
    }
  }
  std::cout<<"Finish processing"<<std::endl;
}

  std::string
utl::ConfigParser::GetSingleArg(const std::string&grp,const std::string&par)
  const
{
  if(!(fConfigLines.count(grp)))
    throw utl::ConfigParser_except().SyntaxErrorGroupNotFound(fFileName,grp);
  if(!fConfigLines.at(grp).count(par))
    throw utl::ConfigParser_except().SyntaxErrorParameterNotFound(fFileName,grp,par);
  if(!(fConfigLines.at(grp).at(par).size()==1))
    throw utl::ConfigParser_except().SyntaxErrorSingleArgExpected(fFileName,grp,par);
  return fConfigLines.at(grp).at(par).at(0);
}
std::vector<std::string> utl::ConfigParser::GetConfig(const std::string&grp,const std::string&par)const{
  if(!fConfigLines.count(grp))
    throw utl::ConfigParser_except().SyntaxErrorGroupNotFound(fFileName,grp);
  if(!fConfigLines.at(grp).count(par))
    throw utl::ConfigParser_except().SyntaxErrorParameterNotFound(fFileName,grp,par);
  if(!(fConfigLines.at(grp).at(par).size()>=0))
    throw utl::ConfigParser_except().SyntaxErrorSingleArgExpected(fFileName,grp,par);
  return fConfigLines.at(grp).at(par);
}


  bool
utl::ConfigParser::HasConfig(const std::string&grp,const std::string&par)
  const
{
  //if(!fConfigLines.count(grp))
  //throw utl::ConfigParser_except().SyntaxErrorGroupNotFound(fFileName,grp);
  return fConfigLines.count(grp)&&fConfigLines.at(grp).count(par);
}
bool utl::ConfigParser::HasOneArgConfig(const std::string&grp,const std::string&par)const{
  return fConfigLines.count(grp)&&fConfigLines.at(grp).count(par)&&(fConfigLines.at(grp).at(par).size()==1);
}


  bool
utl::ConfigParser::IsSingleArgConfig(const std::string&grp,const std::string&par)
  const
{
  if(!fConfigLines.count(grp))
    throw utl::ConfigParser_except().SyntaxErrorGroupNotFound(fFileName,grp);
  if(!fConfigLines.at(grp).count(par))
    throw utl::ConfigParser_except().SyntaxErrorParameterNotFound(fFileName,grp,par);
  return fConfigLines.at(grp).at(par).size()==1;
}


  const utl::ConfigParser::conf_group_t&
utl::ConfigParser::GetGroup    (const std::string& str)
{
  if(!fConfigLines.count(str)){
    throw ConfigParser_except().SyntaxErrorGroupNotFound(fFileName,str);
  }
  return fConfigLines.at(str);
}
