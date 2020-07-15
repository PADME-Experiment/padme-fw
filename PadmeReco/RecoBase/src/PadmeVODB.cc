#include "PadmeVODB.hh"


PadmeVODB::PadmeVODB(){
  
  ;
}

void PadmeVODB::Init(){
  fConnection = mysql_init(NULL);
  fHost = "l0padme1";
  fDataBase =  "PadmeDCS";
  fPasswd = "padmeXXdcs";
  fUser = "padmedcs";
}



bool PadmeVODB::Connect(){
  printf("MySQL client version: %s\n", mysql_get_client_info());
  if (mysql_real_connect(fConnection, "l0padme1", "padmedcs", "padmeXXdcs", 
			 "PadmeDCS", 0, NULL, 0) == NULL) {
      mysql_close(fConnection);    
  }
  
  
  return false;
}
