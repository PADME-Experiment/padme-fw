#ifndef _RecoBase_fwkUniverseBuilder_HH_
#define _RecoBase_fwkUniverseBuilder_HH_
#include"fwk.hh"
#include"utlConfigParser.hh"
#include"det.hh"
#include"utl.hh"
namespace fwk{
  class UniverseBuilder_except: public fwk::Exception{
  };
  class UniverseBuilder{
    private:
      utl::ConfigParser fConfig;
    public:
      UniverseBuilder(const std::string & fn):fConfig(fn){InitSeq();}
    private:
      void InitSeq();
      void SubSystems();
      void Layers();
      void Detectors();
      void RODevices();
      void Digitizers();
  };
}
#endif
