#include "PadmeAnaEvent.hh"
#include "PadmeAnaInMCHitsEvent.hh"
#include "PadmeAnaInMCHistoEvent.hh"
#include "PadmeAnaInMCDigiEvent.hh"

#include <iostream>


PadmeAnaEvent::PadmeAnaEvent(){
  evType = AnaData;

}

PadmeAnaEvent::~PadmeAnaEvent(){
  ;
}

void PadmeAnaEvent::FillEvent(PadmeVAnaEvent *evt  ){
  //  std::cout << "Filling the event" << std::endl;
  
  switch (evt->GetEventType()) {
  case MCHits:
    FillEvent((PadmeAnaInMCHitsEvent *) evt);
    break;
  case MCDigi:
    FillEvent((PadmeAnaInMCDigiEvent *) evt);
    break;
  case MCHisto:
    FillEvent((PadmeAnaInMCHistoEvent *) evt);
    break;
       
    
  default:
    std::cout << "Unknown input event type.... " << std::endl;
    break;
  }  

}

void PadmeAnaEvent::FillEvent(PadmeAnaInMCHitsEvent *evt  ){
  std::cout << "Filling the event from MCHits" << std::endl;
  
}

void PadmeAnaEvent::FillEvent(PadmeAnaInMCHistoEvent *evt  ){
  std::cout << "Filling the event from MCHisto" << std::endl;
  
  ;
}

void PadmeAnaEvent::FillEvent(PadmeAnaInMCDigiEvent *evt  ){
  std::cout << "Filling the event from the Digi" << std::endl;
  
  ;
}
