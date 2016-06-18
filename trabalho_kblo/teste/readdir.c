#include <stdio.h>
#include <stdlib.h>
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/fatlib.h"
#include "../include/filelib.h"
#include "../include/openlib.h"
#include <string.h>

void printDirEntry(DIRENT2 dirEnt)
{
    printf("Name: %s\nTipo: %d\nTamanho: %d\n", dirEnt.name, dirEnt.fileType, dirEnt.fileSize);
    return ;
}

int main ()
{
    int i, j, handle, handle2;
    int file_size = 3000;
    DIRENT2 a;
    char str[file_size];
    mkdir2("gvdambros");
    chdir2("/gvdambros");
    create2("sisopI");
    create2("sisopII");
    mkdir2("/gvdambros/alexandre");
    printf("%d\n",handle);
    handle = opendir2("/");

    printf("Entradas do diretorio raiz: \n\n");

    while( 0 == readdir2(handle,&a) )
    {
        printDirEnt(a);
    }

    printf("Entradas do diretorio gvdambros: \n\n");

    handle2 = opendir2("/gvdambros");

    while( 0 == readdir2(handle2,&a) )
    {
        printDirEntry(a);
    }

    closedir2(handle);
	
	rmdir("/gvdambros");

   return 0;
}


