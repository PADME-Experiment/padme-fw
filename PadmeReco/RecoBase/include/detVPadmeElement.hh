#ifndef _RECOBASE_DETVPADMEELEMENT_HH_
#define _RECOBASE_DETVPADMEELEMENT_HH_
#include<stdio.h>
#include<memory>
#include<iostream>
#include<vector>
#include<map>
#include<stdarg.h>
#include <typeinfo>   // operator typeid

#include"fwk.hh"
#include"utl.hh"


namespace det{
  class VPadmeElement{
    protected:
      virtual~VPadmeElement(){INFO("Destructor  '"+fName+"'");}
      std::string fName;
    public:
      VPadmeElement(){ERROR("");}
      VPadmeElement(const std::string&n):fName(n){}
      VPadmeElement& SetName(const std::string&n){fName=n;return *this;}
      const std::string& GetName()const{return fName;}
  };
}

#endif
