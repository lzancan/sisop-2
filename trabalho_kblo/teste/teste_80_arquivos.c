#include <stdio.h>
#include <stdlib.h>
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/fatlib.h"
#include "../include/filelib.h"
#include "../include/openlib.h"
#include <string.h>

int main ()
{
    int i, j;
    char n[50];
    char m[50];
    char g[50];

    int handles[80];
    struct t2fs_record a;
    mkdir2("/mtgiambastiani");
    chdir2("/mtgiambastiani");
    for(i = 1; i <= 80; i ++){
        sprintf(n,"%d",i);
        create2(n);
    	printf("Crio Arquivo %d.\n",i);
    }
    return 0;
}

