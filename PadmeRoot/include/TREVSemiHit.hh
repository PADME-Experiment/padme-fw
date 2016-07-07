/**
 * \file TREVSemiHit.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include<vector>

#include"TREVDigi.hh"
#include"TSamplingREVDigi.hh"
#include"TDiscretREVDigi.hh"

#ifndef _TREVSemiHit_hh_
#define _TREVSemiHit_hh_ 0
class TREVSemiHit{
  public:
  /// Add Digis related to this SemiHit
    void AddDigi(const TREVDigi *const digi){
      fAssocDigis.push_back(digi);} //
  private:
    std::vector<const TREVDigi*> fAssocDigis;
};
#endif 
