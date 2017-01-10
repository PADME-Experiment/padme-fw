#ifndef _RecoBase_fwkException_hh_
#define _RecoBase_fwkException_hh_ 1
#include<typeinfo>

namespace fwk{
  class Exception:public std::exception{
    public:
      const char* what() const noexcept {return fWhat.c_str();}
      const std::string& GetWhat()const noexcept{return fWhat;}
      const std::string& GetType()const noexcept{return fType;}
    protected:
      virtual ~Exception(){}
      void SetType(Exception& a){fType=typeid(a).name();}
      std::string fWhat;
      std::string fType;
  };
}

#endif
