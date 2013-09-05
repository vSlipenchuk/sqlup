#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <string.h>
//#include <utf8.h>

//#include "coders.c"

int utf8len(char *str) {
int len = strlen(str);
int cnt = 0;
char d[4];
while (len>0) {
    int l = utf8_peek(d,str,len);
    //printf("L=%d\n",l);
    if (l<0) break; // error
    str+=l; len-=l; cnt++;
    }
return cnt;
}

char *utf8_get_word(char **str,char *delims) { // returns word and shift a pointer to the next word
char *r=*str; // return value
if (!delims) delims=" \t";
while ( *r && strchr(delims,*r) ) r++; // ltrim
char *p = r, d[4]; // now move it
int len = strlen(p);
while (len>0) {
  int l = utf8_peek(d,p,len);
  if (l<0) break; // error
  if (l==1 && strchr(delims,*p)) break; // done
  len-=l; p+=l;
  }
if (len>0) { *p=0; p++;  len--; };
while( len>0 && strchr(delims,*p)) { p++; len--;}
*str=p; // corrects a len
return r; // returns begin of word
}

void utf8_prn_words(char *w) {
 while (*w) {
    char *a = utf8_get_word(&w,0);
    printf("<%s>\n",a);
    }

}

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
       memset(buf,0,sizeof(buf));
       gets(buf);
       printf("strlen=%d utf8len=%d buf='%s'\n",strlen(buf),utf8len(buf),buf);
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
       utf8_prn_words(buf);
       }
}

#define MAX_WORD 10

int skip_head = 1;
int had_col_names = 1; // extract it as colnames



void load_file(FILE *f) {
char buf[1024];
char *w[MAX_WORD];
while ( fgets( buf,sizeof(buf), f) ) {
   int len = strlen(buf);
   char *p=buf;
   if (skip_head) {  skip_head--; continue;  }
   if (had_col_names) {
       had_col_names = 0;

       }

   while(len>0 && strchr(" \t\r\n",buf[len-1])) {buf[len-1]=0; len--;}
   printf("LINE:<%s>\n",buf);
   int i;
   for(i=0;i<MAX_WORD;i++) w[i]=utf8_get_word(&p,0);

   for(i=0;i<MAX_WORD;i++) printf("W%d=%s\n",i,w[i]);

   }
}

int main(int npar,char **par) {
    if (!db_connect(db,"local.db",0,0)) { // or - to memory ?
       printf("Error code=%d text=%s\n",db->err_code,db->error);
       return 1;
       }
    db_exec_once(db,"drop table tbl"); // if exists

    printf("Hello world, Connected to sqlite!\n");
    //db_console(db);
    load_file(stdin);
    printf("Done\n");
    return 0;
}
