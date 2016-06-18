#include "../include/main.h"


int identify2 (char *name, int size){
    char* identificador="Gabriel Ammes Pinho <230277> - Luciano Zancan <230047>";
    if(size>=54){
        strcpy(name,identificador);
        return 0;
    }
    else return -1;
}

FILE2 create2 (char *filename){
    procura_arquivo(filename);
    return 0;
}

int delete2 (char *filename){
    return 0;
}

FILE2 open2 (char *filename){
    return 0;
}

int close2 (FILE2 handle){
    return 0;
}

int read2 (FILE2 handle, char *buffer, int size){
    return 0;
}

int write2 (FILE2 handle, char *buffer, int size){
    return 0;
}

int seek2 (FILE2 handle, DWORD offset){
    return 0;
}

int mkdir2 (char *pathname){
    return 0;
}

int rmdir2 (char *pathname){
    return 0;
}

DIR2 opendir2 (char *pathname){
    return 0;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
    return 0;
}

int closedir2 (DIR2 handle){
    return 0;
}

int chdir2 (char *pathname){
    return 0;
}

int getcwd2 (char *pathname, int size){
    return 0;
}
