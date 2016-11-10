#ifndef _RECOBASE_FWK_HH_
#define _RECOBASE_FWK_HH_ 1
#include<stdio.h>
#include<memory>
#include<iostream>
#include<ostream>
#include<vector>
#include<map>
#include<stdarg.h>
#include <typeinfo>   // operator typeid

namespace fwk{
  typedef unsigned long det_system_ID_t;
  typedef unsigned long detector_ID_t;
  typedef unsigned long RODevice_ID_t;
  typedef unsigned long digitizer_ID_t;
  typedef unsigned long digitizer_channel_ID_t;
  typedef std::string det_system_name_t;
  typedef std::string detector_name_t;
  typedef std::string RODevice_name_t;
  typedef std::string digitizer_name_t;
  typedef std::string digitizer_channel_name_t;
}
typedef unsigned int UShort_t ;

namespace fwk{
  class Exception:public std::exception{
    public:
      const char* what() const noexcept {return fWhat.c_str();}
      const std::string& GetType()const{return fType;}
    protected:
      virtual ~Exception(){}
      std::string fWhat;
      std::string fType;
  };
}

#endif
