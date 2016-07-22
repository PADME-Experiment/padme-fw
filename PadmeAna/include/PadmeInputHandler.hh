#ifndef PadmeInputHandler_H
#define PadmeInputHandler_H


//Define all possible inputs
#include "PadmeVAnaInEvent.hh"


#include <string>
#include <iostream>


class PadmeInputHandler{


public:
  PadmeInputHandler();
  PadmeInputHandler(std::string fname);
  ~PadmeInputHandler(); 


  void PadmeGeneralInputHandler(std::string fname);
  void PadmeROOTInputHandler(std::string fname);
  
  PadmeVAnaInEvent *GetEventHandler(){return fInEvent;};

  int isROOTFile(std::string fname);
  //Event handlers

  void InputDigiEvent();
  void InputMCHitsEvent();
  void InputRecoEvent();


protected:
  PadmeVAnaInEvent *fInEvent;

};
#endif
