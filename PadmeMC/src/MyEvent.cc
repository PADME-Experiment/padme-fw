#include "MyEvent.hh"
#include "iostream"


MyEvent* MyEvent::instance = 0;

MyEvent* MyEvent::GetInstance(){

#ifdef DEBUG  
  //  std::cout << "MYEVENT::GetInstance memory address before: " << instance << std::endl;
#endif

  //static MyEvent TheEvent;
  if(!instance) {
    static MyEvent TheEvent;
    TheEvent.genEvt = new MyEventGenerator;
    TheEvent.simEvt = new MySimEvent;
    TheEvent.ClearEvent();

#ifdef DEBUG  
    //    std::cout << "MYEVENT::GetInstance created generator event substructure" << std::endl;
#endif
    instance = &TheEvent;
  }
#ifdef DEBUG  
  //  std::cout << "MYEVENT: Asked instance of the event " << std::endl;
  //  std::cout << "MYEVENT::GetInstance memory address: " << instance << std::endl;
#endif

  return instance;
}


MyEvent::MyEvent(){
  if(!instance) {
    //Ooook, the class instantiation is not done through new:
  }
  instance = this;    
  
#ifdef DEBUG  
  //  std::cout << "MYEVENT::Constructor Constructor called" << std::endl;
  //  std::cout << "MYEVENT::Constructor memory address: " << instance << std::endl;
#endif
}

void*  MyEvent::operator new (size_t size){
  return GetInstance();
}



void MyEvent::ClearEvent(){
  genEvt->ClearEvent();
  simEvt->ClearEvent();
}
