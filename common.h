#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

typedef struct {
  void *db; // handle to database
  int err_code;
  char *error;
  } database;

int db_connect(database *db,char *name,char *user,char *pass);
int db_disconnect(database *db);


#endif // COMMON_H_INCLUDED
