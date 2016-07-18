#include "PadmeAnaTrack.hh"



PadmeAnaTrack::PadmeAnaTrack(){
  //q = 0 means no track
  q=0;
}

PadmeAnaTrack::PadmeAnaTrack(double pin, double qin, double tin){
  p = pin;
  q = qin;
  time = tin;
}
