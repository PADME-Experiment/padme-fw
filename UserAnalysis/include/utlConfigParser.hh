/**
 * \file PadmeReco/RecoBase/include/utlConfigParser.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-08
 */
#ifndef _utlConfigParser_hh_
#define _utlConfigParser_hh_ 1

#include<map>
#include<string>
#include<vector>

#include"fwkException.hh"

/**
 * \class utl::ConfigParser
 * \brief Parses the configuration files
 *
 * This class parses configuration files.
 * Configuration file structure:
 *
 * [Group]\n
 * <Command> <Arg1> <Arg2> ... <ArgN>
 *
 * The name of default group is [Default].\n
 * Comment sign is #. It cannot be escaped!
 */
namespace utl{
  class ConfigParser_except:public fwk::Exception{
    public:
      ConfigParser_except(){fType="ConfigParser_except";}
      ConfigParser_except& SyntaxErrorArgumentExpected(const std::string&fn,int ln){
        fWhat=fn+":"+std::to_string(ln)+" Syntax error. At least one argument expected.";
        return *this;
      }
      ConfigParser_except& SyntaxErrorConfigFieldRedefinition(const std::string&fn,int ln,const std::string&fd){
        fWhat=fn+":"+std::to_string(ln)+" Syntax error. Redefinition of config field '"+fd+"'.";
        return *this;
      }
      ConfigParser_except& SyntaxErrorUnknownParameter(const std::string&fn,int ln,const std::string&par){
        fWhat=fn+":"+std::to_string(ln)+" Syntax error. Parameter '"+par+"' is unknown.";
        return *this;
      }
      ConfigParser_except& SyntaxErrorParameterNotFound(const std::string&fn,const std::string&grp,const std::string&par){
        fWhat=fn+" Syntax error. In ["+grp+"] parameter '"+par+"' not found.";
        return *this;
      }
      ConfigParser_except& SyntaxErrorMoreParametersExpected(const std::string&fn,int ln){
        fWhat=fn+":"+std::to_string(ln)+" Syntax error. More parameters expected.";
        return *this;
      }
      ConfigParser_except& SyntaxErrorGroupNotFound(const std::string&fn,const std::string&gr){
        fWhat=fn+" Syntax error. Group ["+gr+"] was not found.";
        return *this;
      }
      ConfigParser_except& SyntaxErrorUnknownGroup(const std::string&fn,const std::string&gr){
        fWhat=fn+" Syntax error. Unknown group ["+gr+"].";
        return *this;
      }
      ConfigParser_except& SyntaxErrorSingleArgExpected(const std::string&fn,const std::string&gr,const std::string&cfg){
        fWhat=fn+" Syntax error. In ["+gr+"] for '"+cfg+"' exactly one argument expected.";
        return *this;
      }
      ConfigParser_except& SyntaxErrorEmptyGroupName(const std::string&fn,int ln){
        fWhat=fn+":"+std::to_string(ln)+" Syntax error. Empty group name is not allowed.";
        return *this;
      }
      ConfigParser_except& FileNotFound(const std::string&fn){
        fWhat="File not found '"+fn+"'.";
        return *this;
      }
  };


  class ConfigParser{
    public:
      typedef std::map<std::string,std::vector<std::string>> conf_group_t;
    public:
      ConfigParser(const std::string& fn);
      std::string              GetSingleArg(const std::string&grp,const std::string&cfg)const;
      std::vector<std::string> GetConfig   (const std::string&grp,const std::string&cfg)const;
      bool                     HasConfig   (const std::string&grp,const std::string&cfg)const;
      bool                     HasOneArgConfig(const std::string&grp,const std::string&cfg)const;
      bool                     IsSingleArgConfig(const std::string&grp,const std::string& cfg)const;
      void                     Print       ()const;
      const conf_group_t&      GetGroup    (const std::string& str);
    protected:
      std::string fFileName;
      std::map<std::string,conf_group_t> fConfigLines;
  };
}
#endif
