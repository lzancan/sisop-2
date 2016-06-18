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
    struct t2fs_record a;
    strcpy(m,"/gvdambros/");
    mkdir2("/gvdambros");
    for(i = 1; i <= 80; i ++){
        sprintf(n,"%d",i);
        concat(g,m, n);
        printf("%s\n",g);
        mkdir2(g);
    }
    return 0;
}
