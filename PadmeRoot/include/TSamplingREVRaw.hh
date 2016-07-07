/**
 * \file TADCREVRaw.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include"TREVRaw.hh"
#include<string>

#ifndef _TSamplingREVRaw_hh_
#define _TSamplingREVRaw_hh_ 0
class TSamplingREVRaw:TREVRaw{
  std::vector<int>* fSamples;
  int fSampleFirst; int fSampleLast;
};
#endif
