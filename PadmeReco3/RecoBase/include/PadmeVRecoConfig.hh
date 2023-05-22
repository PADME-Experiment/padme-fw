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

  utl::ConfigParser * GetConfigParser(){return fConfigParser;};

  int GetParOrDefault(std::string,std::string,int);
  double GetParOrDefault(std::string,std::string,double);
  std::string GetParOrDefault(std::string,std::string,std::string);

  std::string GetReadOutType(){return fReadOutType;}

  Int_t GetNBoards() {return fNBoards;}
  std::vector<Short_t>* GetBoards() {return &fBoards;}
  virtual Bool_t BoardIsMine(Int_t);
  Int_t GetDetectorChannel(UChar_t b, UChar_t c) { return fChannelMap[b][c]; }

private:

  void ConfigADCReadout();

  std::string fReadOutType;

  Int_t fNBoards;
  std::vector<Short_t> fBoards;
  Int_t fChannelMap[32][32];

  utl::ConfigParser* fConfigParser;

};



#endif
