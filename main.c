#include <stdio.h>
#include <stdlib.h>
#include "common.h"

database DB,*db=&DB;

int db_exec_once(database *db, char *sql) {
return db_compile(db,sql) && db_exec(db);
}

int db_prn_line(database *db) {
    int i;
for(i=0;i<db->col_count;i++) {
   printf("%s ",db_text(db,i));
   }
printf("\n");
}

int db_console(database *db) {
 while(1) {
       char buf[1024];
       gets(buf);
       if (!db_exec_once(db,buf)) {
          printf("exec failed: %s\n",db->error);
          } else {
          printf("exec ok, col_count=%d\n",db->col_count);
          if (db->col_count>0) { // do fetch
              while (db->row_here) {
                   db_prn_line(db);
                   if (!db_exec(db)) break;
                   }

              }
          }
       }
}

int main(int npar,char **par) {
    if (!db_connect(db,"local.db",0,0)) {
       printf("Error code=%d text=%s\n",db->err_code,db->error);
       return 1;
       }
    printf("Hello world, Connected to sqlite!\n");
    db_console(db);
    return 0;
}
