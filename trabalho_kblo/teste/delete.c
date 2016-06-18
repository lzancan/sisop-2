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
    int i, j, handle, handle2;
    int file_size = 3000;
    char str[file_size];
    struct t2fs_record a;
    memset(&str,"DOGE",file_size);

    printf("Crio diretorio gvdambros.\n");
    mkdir2("/gvdambros");
    chdir2("/gvdambros");

    printf("Crio Arquivo.\n");
    handle = create2("MontyPython");
    if(handle == ERROR_SIGNAL) return 3;

    printf("Escrevo nele.\n");    
	if( write2(handle, str , file_size) == ERROR_SIGNAL) return 4;
    close2(handle);

    printf("Deleto Arquivo.\n");
    if( delete2("MontyPython") != 0) return 5;

    printf("Executou com sucesso.\n");
    return 0;
}

