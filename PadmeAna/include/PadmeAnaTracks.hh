#ifndef PadmeAnaTrack_H
#define PadmeAnaTrack_H


class PadmeAnaTrack{
public:
  PadmeAnaTrack();
  PadmeAnaTrack(double pin, double qin, double tin);
  ~PadmeAnaTrack();

public:
  double p;
  int q;
  double theta;
  double phi;
  double time;
  int n;
  
};


#endif
