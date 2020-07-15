#ifndef PadmeVODB_H
#define PadmeVODB_h

#include <mysql/mysql.h>
#include <stdlib.h>
#include <string>

class PadmeVODB {
 public:
  PadmeVODB();

  virtual ~PadmeVODB(){;};

  bool Connected(){return fConStatus;};
  bool Connect();
  virtual void Init();
  
  
 private:
  static MYSQL *fConnection;
  static std::string fHost;
  static std::string fDataBase;
  static std::string fPasswd;
  static std::string fUser;
  
  bool fConStatus;
  

};


#endif
