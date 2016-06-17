#include "../include/main.h"

char* copy_string_size (char* from, int size){
    int resize =size+1;
    char* to = malloc(sizeof(char) * resize);
    while(*from!='\0' && resize>1)
    {
        *to = *from;
        from++;
        to++;
        resize--;
    }
    *to='\0';
    while(resize<=size)
    {
        to--;
        resize++;
    }
    return to;
}
