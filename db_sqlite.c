#include "common.h"
#include "sqlite3.h"

int db_connect(database *db,char *name,char *user,char *pass) {
db->err_code = sqlite3_open(name,(void*)&db->db );
if (db->err_code) {
  db->error = sqlite3_errmsg( (void*)db->db);
  return 0;
  }
return 1; // ok
}

int db_exec_sql(database *db,char *sql) {

}
