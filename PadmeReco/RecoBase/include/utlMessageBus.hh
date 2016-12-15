#ifndef _RecoBase_utlMessageBus_hh_
#define _RecoBase_utlMessageBus_hh_ 1

#define WARNING_N(     level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eWarning,   level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define SUCCESS_N(     level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eSuccess,   level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define INFO_N(        level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eInfo,      level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define ERROR_N(       level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eError,     level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define FATAL_ERROR_N( level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eFatalError,level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))

#define WARNING(      err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eWarning,   1, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define SUCCESS(      err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eSuccess,   3, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define INFO(         err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eInfo,      2, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define ERROR(        err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eError,     0, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define FATAL_ERROR(  err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eFatalError,0, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))


#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include"fwkException.hh"

namespace utl{
  /// MessageBus help to log information from the program execution
  /** This is a singleton class which makes log messages
   * easier to read.
   * There are preprocessor functions which make the usage
   * of the class easier.
   *
   * WARNING_N     (LevelOfVerbosity, message )\n
   * SUCCESS_N     (LevelOfVerbosity, message )\n
   * INFO_N        (LevelOfVerbosity, message )\n
   * ERROR_N       (LevelOfVerbosity, message )\n
   * FATAL_ERRORN_ (LevelOfVerbosity, message )\n
   * WARNING       (message ) = WARNING_     (1, message )\n
   * SUCCESS       (message ) = SUCCESS_     (3, message )\n
   * INFO          (message ) = INFO_        (2, message )\n
   * ERROR         (message ) = ERROR_       (0, message )\n
   * FATAL_ERROR   (message ) = FATAL_ERROR_ (0, message )\n
   *
   * Example1:\n
   * DEFAULT_ERROR("Some error message")\n
   *
   * Example2:\n
   * ERROR(0,"Some error message")\n
   *
   * Example3:\n
   * std::string m="Some error message";\n
   * std::string m1="Some other message";\n
   * ERROR(0,m+m1);\n
   *
   * Example4:\n
   * std::stringstream s;\n
   * s<<"Some"<<" "<<"error message";\n
   * ERROR(0,s.str());\n
   */

  class MessageBus{
    public:
      enum class eMessageType{
        eSuccess,
        eInfo,
        eWarning,
        eError,
        eFatalError
      };

      static MessageBus& GetInstance();
      void TypeOfString();
      void SetLogFile(const std::string& fn);
      void NewMessage(eMessageType t, unsigned int l, const std::string& mes);
      void SetDebugLevel(unsigned int l){fDebugLevel=l;}
      void SetVerbosity(unsigned int l){fDebugLevel=l;}
      void IncDebugLevel(){++fDebugLevel;}
      void DecDebugLevel(){--fDebugLevel;}
      //      void NewMessage(eMessageType t, unsigned int l, const std::stringstream& message){
      //        NewMessage(t, l, message.str());
      //      }
    private:
      MessageBus():fDebugLevel(5){};
      ~MessageBus(){if(fFile.is_open()) fFile.close();}
      unsigned int fDebugLevel;
      std::ofstream fFile;
  };
  class MessageBus_except:public fwk::Exception{
    public:
      MessageBus_except(){fType="MessageBus_except";}
      MessageBus_except& CannotCreateFile(const std::string&fn);
      MessageBus_except& UnknownMessageType(MessageBus::eMessageType&);
  };
}
#endif
