#include"fwTSimpleList.hh"
#include"TREVHit.hh"

void TREVHit::AddSemiHit(TREVSemiHit *sHit){
  fAssocSemiHits.AddEntry(sHit);
}
