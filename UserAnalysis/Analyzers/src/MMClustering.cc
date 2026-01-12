#include "MMClustering.hh"

MMClustering* MMClustering::fInstance = 0;

MMClustering* MMClustering::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new MMClustering(); }
  return fInstance;
}

void MMClustering::Init(TMMRecoEvent* mmevent, Bool_t fHistoMode){
}
