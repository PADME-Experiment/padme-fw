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
  
  Int_t GetChanneID(Int_t, Int_t);


private:
  std::vector<Short_t> fBoards;
  Int_t fNBoards;
  typedef std::vector<Int_t> ADCMap;
  std::vector<ADCMap> fChannelIDs;
  
};



#endif
