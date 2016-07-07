/**
 * \file TREVHit.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include<vector>
#include"TREVSemiHit.hh"

#ifndef _TREVHit_hh_
#define _TREVHit_hh_ 0
class TREVHit{
  public:
  /// Add SemiHits related to this Hit
    void AddSemiHit(const TREVSemiHit *const sHit){fAssocSemiHits.push_back(sHit);}
  private:
    std::vector<const TREVSemiHit*> fAssocSemiHits;
   // something to be done for the case of single
   // PMT for multiple scintillators. 
};
#endif
