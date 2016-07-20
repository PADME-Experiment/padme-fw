/**
 * \file TREVHit.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include<vector>
#include"TREVSemiHit.hh"
#include"fwTSimpleList.hh"

#ifndef _TREVHit_hh_
#define _TREVHit_hh_ 0
class TREVHit{
  public:
  /// Add SemiHits related to this Hit
    void AddSemiHit(TREVSemiHit *sHit);
  protected:
    typedef typename fw::TSimpleList<TREVSemiHit*> semiHitCont_t;
    semiHitCont_t fAssocSemiHits;
  private:
};
#endif
