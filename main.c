#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <string.h>
//#include <utf8.h>

//#include "coders.c"

// dirty code to check sqlite&utf8 SQL

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

void db_prn_line(database *db) {
    int i;
for(i=0;i<db->col_count;i++) {
   printf("%s%c",db_text(db,i),(i+1<db->col_count)?'\t':'\n');
   }
}

int db_print(database *db,char *buf,int show_head) {
       if (!db_exec_once(db,buf)) {
          fprintf(stderr,"exec failed: %s\n",db->error);
          } else {
          //printf("exec ok, col_count=%d\n",db->col_count);
          if (db->col_count>0) { // do fetch

              while (db->row_here) {
                   db_prn_line(db);
                   if (!db_exec(db)) break;
                   }

              }
          }
return 1;
}

int db_console(database *db) {
 while(1) {
       char buf[1024];
       memset(buf,0,sizeof(buf));
       gets(buf);
       //printf("strlen=%d utf8len=%d buf='%s'\n",strlen(buf),utf8len(buf),buf);
       db_print(db,buf,1);
       //utf8_prn_words(buf);
       }
}

#define MAX_WORD 10 /* Maximum Columns in Table*/
#define COLLEN   30 /* MaxColumnNameLen*/

int skip_head = 1;
int had_col_names = 1; // extract it as colnames
int table_ready  = 0;

typedef char colNAME[COLLEN];

colNAME colName[MAX_WORD]; // columnNames
char tbl[100]="tbl"; // default table


#define strNcpy(A,B) { strncpy(A,B,sizeof(A)-1); A[sizeof(A)-1]=0;}
#include "blob.h"

int db_bind_text(database *db,int idx, char *val,int len);


char buf[1024],SQL[1024];

void load_file(FILE *f) {
//char buf[1024];
char *w[MAX_WORD];
int i;
table_ready = 0;
char *str = 0; // my blob here
int max_col = MAX_WORD;
for(i=0;i<MAX_WORD;i++) sprintf(colName[i],"W%d",i); // W0,W1,... - default column name
while ( fgets( buf,sizeof(buf), f) ) {
   int len = strlen(buf);
   char *p=buf;
   if (skip_head) {  skip_head--; continue;  }
   while(len>0 && strchr(" \t\r\n",buf[len-1])) {buf[len-1]=0; len--;}
   for(i=0;i<MAX_WORD;i++) w[i]=utf8_get_word(&p,0);
   if (had_col_names) { // use the line to rename
       had_col_names = 0;
       for(i=0;i<MAX_WORD;i++) {
              if (strlen(w[i])==0) { max_col = i; break; } // empty col means all
              strNcpy(colName[i],w[i]); // JustCopy
              }
       continue; // again
       }
    if (!table_ready) {
       db_exec_once(db,"drop table tbl");
       db_exec_once(db,"commit");


       //blob_setLength(&str,0);       blob_cat(&str,"

                                                 table tbl(",-1);       printf("STR<%s> len=%d\n",str,blob_getLength(str));
       sprintf(SQL,"create table tbl(");
       for(i=0;i<max_col;i++) {
           //printf("HERE COL: <%s>\n",colName[i]);
           //blob_cat(&str,colName[i],-1);           blob_cat(&str," varchar2 ",-1);
           //if (i==max_col-1) blob_cat(&str,")",-1); else blob_cat(&str,",",-1);
           strcat(SQL,colName[i]); strcat(SQL," varchar2 ");
           if (i==max_col-1) strcat(SQL,")"); else strcat(SQL,",");
           }
       //printf("CreateTableSQL: '%s'\n",SQL);
       //char *s = "create table tbl(Destination varchar(80) ,Gateway varchar(80) ,Genmask varchar(80) ,Flags varchar(80) ,MSS varchar(80) ,Window varchar(80) ,irtt varchar(80) ,Iface varchar(80))";

       int ok = db_exec_once(db,SQL);
        //printf("tbl created=%d sql:%s\n",ok,SQL);
       if (!ok) {
            fprintf(stderr,"sqlup: failed_create_table:%s, sql:%s\n",db->error,SQL);
            exit(4);
            //return 0;
            }
       table_ready = 1; // ok
 //      }
       blob_setLength(&str,0);       blob_cat(&str,"insert into tbl(",-1);
       //sprintf(SQL,"insert into tbl(");
       for(i=0;i<max_col;i++) {
           ///printf("HERE COL: <%s>\n",colName[i]);
           blob_cat(&str,colName[i],-1);
           //blob_cat(&str," varchar(80) ",-1);
           if (i==max_col-1) blob_cat(&str,")",-1); else blob_cat(&str,",",-1);
           //strcat(SQL,colName[i]); //strcat(SQL,",");
           //if (i==max_col-1) strcat(SQL,")"); else strcat(SQL,",");
           }
        blob_cat(&str," values(",-1);
        //strcat(SQL," values(");
        for(i=0;i<max_col;i++) {
           ///printf("HERE COL: <%s>\n",colName[i]);
           blob_cat(&str,"?",-1);
           //blob_cat(&str," varchar(80) ",-1);
           if (i==max_col-1) blob_cat(&str,")",-1); else blob_cat(&str,",",-1);
           //strcat(SQL,"?");
           //blob_cat(&str," varchar(80) ",-1);
           //if (i==max_col-1) strcat(SQL,")"); else strcat(SQL,",");
           }
       //printf("SQL:%s\n",str);
       if (!db_compile(db,str)) {
           fprintf(stderr,"SQL compile error %s\n",db->error);
           exit(3);
           //return 0;
           }
       //printf("OK, compiled\n");
       // - тут должна быть работа со строками. И без нее - вообще никуда.
}
   //while(len>0 && strchr(" \t\r\n",buf[len-1])) {buf[len-1]=0; len--;}
   //printf("LINE:<%s>\n",buf);
   //int i;
   //for(i=0;i<MAX_WORD;i++) w[i]=utf8_get_word(&p,0);
   //blob_setLength(&str,0);
   //blob_cat(&str,"insert into tbl( ",-1);
   for(i=0;i<max_col;i++) {
        char *p = w[i]; int l = strlen(p);
        //p="new_one_and";
        //printf("{{%s}}[%d]",p,strlen(p));
        //int k; for(k=0;k<l;k++) printf("-%x-",p[k]);
        //printf("\n");
        db_bind_text(db,i+1,p,l);
        }
    //printf("\n");
   if (!db_exec(db)) {
       fprintf(stderr,"sqlup: dbexec err:%s\n",db->error);
       exit(3);
}
   }
blob_release(&str);
}

int prn_help() {
printf("sqlup usage: [-h<head_lines_skip>] [-c<col_has_names>] [-S<sql_to_show>] [-H<sql_heads>] [-d<debug_level]\n");
return 0;
}

char *show_sql = "select * from tbl";
int   show_heads = 1;


int main(int npar,char **par) {
int i;
for(i=1;i<npar;i++) {
    char *cmd = par[i];
    if (*cmd == '-') {
       cmd++;
       switch(*cmd) {
       case 'h':
          sscanf(cmd+1,"%d",&skip_head);
         break;
       case 'c':
          sscanf(cmd+1,"%d",&had_col_names);
          break;
       case 'S':
           show_sql = cmd+1;
           break;
       case 'H':
           sscanf(cmd+1,"%d",&show_heads);
       default:
          prn_help(); exit(1);
       }

       } else {
          prn_help(); exit(1);
       }
    }
if (!db_connect(db,"local.db",0,0)) { // or - to memory ?
       fprintf(stderr,"sqlup: open db fail, err:%s",db->error);
       return 1;
       }
    db_exec_once(db,"drop table tbl"); // if exists

    //printf("Hello world, Connected to sqlite!\n");
    //db_console(db);
    load_file(stdin);
    //printf("Done\n");
    if (show_sql[0]) db_print(db,show_sql,show_heads);

    return 0;
}


 //;//create table tbl(Destination varchar(80) ,Gateway varchar(80) ,Genmask varchar(80) ,Flags varchar(80) ,MSS varchar(80) ,Window varchar(80) ,irtt varchar(80) ,Iface varchar(80))

