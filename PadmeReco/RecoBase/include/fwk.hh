#ifndef _RecoBase_fwk_hh_
#define _RecoBase_fwk_hh_ 1
#include<stdio.h>
#include<memory>
#include<iostream>
#include<ostream>
#include<vector>
#include<map>
#include<stdarg.h>
#include<typeinfo>   // operator typeid

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


#endif
