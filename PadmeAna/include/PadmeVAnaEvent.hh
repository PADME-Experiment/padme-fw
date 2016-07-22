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

enum EventType{
  General,MCDigi,MCHisto,MCHits,Digi,Reco,MCEOB,MCSOB,DATA,AnaData
};


class PadmeVAnaEvent {
public:
  PadmeVAnaEvent(){ evType=General; };
  PadmeVAnaEvent(EventType type){ evType=type; };
  virtual ~PadmeVAnaEvent(){;};
  virtual void FillEvent(PadmeVAnaEvent *evt);
  virtual void Init();
  EventType GetEventType(){return evType;};

  //private:
  EventType evType;
  int time;
  int size;
  
};

#endif
