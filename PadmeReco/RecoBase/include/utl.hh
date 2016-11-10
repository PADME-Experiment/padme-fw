#ifndef _RecoBase_utl_hh_
#define _RecoBase_utl_hh_
#define WARNING(     level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eWarning,   level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define SUCCESS(     level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eSuccess,   level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define INFO(        level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eInfo,      level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define ERROR(       level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eError,     level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define FATAL_ERROR( level__ , err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eFatalError,level__, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))

#define DEFAULT_WARNING(      err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eWarning,   1, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define DEFAULT_SUCCESS(      err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eSuccess,   3, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define DEFAULT_INFO(         err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eInfo,      2, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define DEFAULT_ERROR(        err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eError,     0, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))
#define DEFAULT_FATAL_ERROR(  err___ ) (utl::MessageBus::GetInstance().NewMessage(utl::MessageBus::eMessageType::eFatalError,0, std::string(__FILE__)+":"+std::to_string(__LINE__) +"  ("+__FUNCTION__ +")  "+ err___))

#include"fwk.hh"
namespace utl{
  class MessageBus_except:public fwk::Exception{
    public:
      MessageBus_except(){fType="MessageBus_except";}
      MessageBus_except& CannotCreateFile(const std::string&fn){
        fWhat="File '"+fn+"' cannot be created.";
        return *this;
      }
  };
  /// MessageBus help to log information from the program execution
  /** This is a singleton class which makes log messages
   * easier to read.
   * There are preprocessor functions which make the usage
   * of the class easier.
   *
   * WARNING     (LevelOfVerbosity, message )\n
   * SUCCESS     (LevelOfVerbosity, message )\n
   * INFO        (LevelOfVerbosity, message )\n
   * ERROR       (LevelOfVerbosity, message )\n
   * FATAL_ERROR (LevelOfVerbosity, message )\n
   * DEFAULT_WARNING     (message ) = WARNING     (1, message )\n
   * DEFAULT_SUCCESS     (message ) = SUCCESS     (3, message )\n
   * DEFAULT_INFO        (message ) = INFO        (2, message )\n
   * DEFAULT_ERROR       (message ) = ERROR       (0, message )\n
   * DEFAULT_FATAL_ERROR (message ) = FATAL_ERROR (0, message )\n
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
      void SetDebugLevel(unsigned int l){fDebugLevel=l;}
      void IncDebugLevel(){++fDebugLevel;}
      void DecDebugLevel(){--fDebugLevel;}
      void SetOutFile(const std::string& fn){
        fFile.open(fn);
        if(!fFile.is_open()) throw MessageBus_except().CannotCreateFile(fn);
      }
      static MessageBus& GetInstance(){
        static MessageBus a;
        return a;
      }
      void NewMessage(eMessageType t, unsigned int l, const std::stringstream& message){
        NewMessage(t, l, message.str());
      }
      void NewMessage(eMessageType t, unsigned int l, const std::string& message){
        std::string pref;
        switch (t){
          case eMessageType::eSuccess   :pref="\e[1;32m[SUCC]\e[0m  ";break;
          case eMessageType::eInfo      :pref="\e[1;34m[INFO]\e[0m  ";break;
          case eMessageType::eWarning   :pref="\e[1;33m[WARN]\e[0m  ";break;
          case eMessageType::eError     :pref="\e[1;31m[ERROR]\e[0m ";break;
          case eMessageType::eFatalError:pref="\e[1;31m[FATAL]\e[0m ";break;
        }
        if(l<=fDebugLevel) std::cerr<<pref<<message<<std::endl<<std::flush;
        if(fFile.is_open()) fFile<<pref<<message<<std::endl;
      }
    private:
      MessageBus():fDebugLevel(1){};
      ~MessageBus(){if(fFile.is_open()) fFile.close();}
      unsigned int fDebugLevel;
      std::ofstream fFile;
  };
}
#endif
