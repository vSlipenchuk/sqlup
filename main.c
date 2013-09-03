#include <stdio.h>
#include <stdlib.h>
#include "common.h"

database DB,*db=&DB;

int main(int npar,char **par) {
    if (!db_connect(db,"local.db",0,0)) {
       printf("Error code=%d text=%s\n",db->err_code,db->error);
       return 1;
       }
    printf("Hello world, Connected to sqlite!\n");
    return 0;
}
