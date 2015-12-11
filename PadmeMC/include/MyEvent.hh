#ifndef _MY_EVENT_HH_
#define _MY_EVENT_HH_

//#define DEBUG

#include "MyEventGeneration.hh"
#include "MySimEvent.hh"


//Private event structure to be used inside GEANT/ROOT/RECO programs
//The class is a "singleton" - only one static object of it can exist

class MyEvent {
  
public:
  // defaults
  MyEvent();
  ~MyEvent(){;};

  void ClearEvent();


  //===================================================
  //Instantiation
private:
  static MyEvent *instance;
public:
  //Method to get/or create an instance of the event
  static MyEvent *GetInstance();
  static void* operator new (size_t size); 
  //===================================================

  
public:
  MyEventGenerator *GetGenEvent(){return genEvt;};
  MySimEvent *GetSimEvent(){return simEvt;};



private:
  MyEventGenerator *genEvt;
  MySimEvent *simEvt;
  // MyRecoEvent *recoEvt;
  
  
  

};


#endif
