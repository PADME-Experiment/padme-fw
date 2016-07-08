/**
 * \file PadmeReco/RecoBase/include/TVConfigParser.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-08
 */

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<map>

#ifndef _TVConfigParser_hh_
#define _TVConfigParser_hh_ 0

/**
 * \class TVConfigParser
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
class TVConfigParser{
  public:
    TVConfigParser(std::string icfilename);
    std::string GetSingleArgConfig(std::string grp,std::string cfg);
    bool HasConfig(std::string grp, std::string cfg);
    bool IsSingleArgConfig(std::string grp, std::string cfg);
    void Verbose();
  protected:
    std::string fFileName;
    std::map<std::string,std::map<std::string,std::vector<std::string> > > fConfigLines;

};
#endif
