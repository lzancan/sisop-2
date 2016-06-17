#include "../include/main.h"

int main(){
    int size =54;
    char name[MAX_FILE_NAME_SIZE];
    printf ("retorno: %d\n",identify2(name,size));
    printf ("nome main: %s\n",name);
    return 0;
}
