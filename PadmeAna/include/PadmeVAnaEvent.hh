#ifndef PadmeVAnaEvent_H
#define PadmeVAnaEvent_H

//Event is everything that could happen during data taking
//All separate happening should inherit from this basic class
//Typical set of possible events:
//Start of RUN event
//Start of Bunch event
//End of RUN event
//End of Bunch event
//Event with data from the detectors
//Etc...


class PadmeVAnaEvent {
public:
  PadmeVAnaEvent(){ evType=0; };
  PadmeVAnaEvent(int type){ evType=type; };
  ~PadmeVAnaEvent(){;};
  void FillEvent();
  void Init();
  int EventType(){return evType;};

  //private:
  int evType;
  int time;
  int size;
  
};

#endif
