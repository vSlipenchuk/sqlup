#include "common.h"
#include "sqlite3.h"

int db_error(database *db) {
db->error = (void*) sqlite3_errmsg( (void*)db->db );
return 0;
}

int db_connect(database *db,char *name,char *user,char *pass) {
db->err_code = sqlite3_open(name,(void*)&db->db );
if (db->err_code) return db_error(db);
return 1; // ok
}

int db_disconnect(database *db) {
sqlite3_close((void*)db->db);
return 0;
}

int db_compile(database *db,char *sql) {
char *tail = sql;
if (db->stmt) {
  sqlite3_finalize(db->stmt); db->stmt = 0;

  }
db->err_code = sqlite3_prepare_v2( (void*)db->db, (void*) sql,  -1, (void*) &db->stmt, (void*)& tail );
if (db->err_code) {
  db->error = (void*)sqlite3_errmsg( (void*)db->db);
  return 0;
  }
return 1; // OK
}

int db_bind_text(database *db,int idx, char *val,int len) {
db->err_code = sqlite3_bind_text ( (void*)db->stmt, idx, val, len, SQLITE_TRANSIENT);
if (db->err_code) return db_error(db);
//db->bound++;
return 1;
}

int db_exec(database *db) {
db->err_code = sqlite3_step( db->stmt );
//printf("E:%d\n",db->err_code);
if (db->err_code == SQLITE_OK || db->err_code == SQLITE_DONE) {
    //printf("Here reset\n");
    sqlite3_reset( db->stmt);
    //sqlite3_clear_bindings( db->stmt );
    }
db->row_here = db->err_code == SQLITE_ROW;
db->col_count = 0;
if (db->err_code && (db->err_code!=SQLITE_DONE) && ( db->err_code != SQLITE_ROW)) {
  return db_error(db);
  }
if (db->err_code == SQLITE_ROW) db->col_count = sqlite3_column_count( db->stmt );
return 1;
}

char *db_text(database *db, int icol) {
db->col_text = (void*)sqlite3_column_text( (void*)db->stmt, icol);
return db->col_text;
}
