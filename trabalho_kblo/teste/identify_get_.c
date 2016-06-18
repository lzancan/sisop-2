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
    char names[100];
    identify2(names, 100);
    printf("%s\n",names);

    getcwd2(names, 100);
    printf("%s\n", names);

    chdir2("teste");
    getcwd2(names, 100);
    printf("%s\n", names);
return 0;
}


