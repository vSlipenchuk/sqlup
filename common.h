#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

typedef struct {
  void *db; // handle to database
  void *stmt; // prepared SQL stmt (cursor)?
  int err_code;
  int row_here; // fetchable
  char *error;
  int col_count; // 0 or more...
  char *col_text;
  } database;

int  db_connect(database *db,char *name,char *user,char *pass);
int  db_disconnect(database *db);
int  db_compile(database *db,char *sql);
int  db_exec(database *db);
char *db_text(database *db, int col); // gets a colums as text


#endif // COMMON_H_INCLUDED
