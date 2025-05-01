#ifndef PadmeVRecoConfig_H
#define PadmeVRecoConfig_H


#include "PadmeVNamedModule.hh"
#include "utlConfigParser.hh"

#include <vector>
#include <string>


class PadmeVRecoConfig: public PadmeVNamedModule
{
public:
  PadmeVRecoConfig(utl::ConfigParser *,TString Name);
  virtual ~PadmeVRecoConfig();
  virtual Bool_t BoardIsMine(Int_t);
  Int_t GetNBoards(){return fNBoards;};
  utl::ConfigParser * GetConfigParser(){return fConfigParser;};
  std::string GetReadOutType(){return fReadOutType;}
  std::vector<Short_t>  *GetBoards() {return &fBoards;};
  //Int_t GetChanneID(Int_t, Int_t);
  //  Int_t 
  
  int GetParOrDefault(std::string,std::string,int);
  double GetParOrDefault(std::string,std::string,double);
  std::string GetParOrDefault(std::string,std::string,std::string);

  void ConfigADCReadout();

private:
  std::vector<Short_t> fBoards;
  std::string fReadOutType;
  Int_t fNBoards;

  utl::ConfigParser *fConfigParser;

};



#endif
