#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/filelib.h"

#define ERROR_SIGNAL -1

int findNameOfLastFile(char *in, char *out)
{
    int i,len = strlen(in);

    if(in[len - 1] == '/')
    {
        len--;
    }

    for(i = len - 1; i >= 0; i--)
    {
        if(in[i] == '/') break;
    }

    if( strlen(in + i + 1) > 25 ) return -ERROR_SIGNAL;

    strcpy(out,in + i + 1);

    if(out[ strlen(out) -1 ] == '/') out[ strlen(out) -1 ] = '\0';

    return 0;
}

void findNameOfFirstFile(char *in, char *out)
{
    int i, bar = 0, len = strlen(in);

    if(in[0] == '/') bar = 1;

    for(i = 0 + bar; i < len; i++)
    {
        if(in[i] == '/') break;
    }

    strcpy(out,in + bar);

    out[i - bar] = '\0';

    return ;
}

void findRemnant(char *in, char *out)
{
    int i, barI = 0, len = strlen(in);

    if(in[0] == '/') barI++;

    for(i = 0 + barI; i < len; i++)
    {
        if(in[i] == '/') break;
    }

    if(len == i) i--;

    strcpy(out, in + i + 1);

    return ;
}


void withoutLastName(char *in, char *out)
{
    int i, barF = 0, barI = 0, len = strlen(in);

    if(in[len- 1] == '/') barF = 1;

    if(in[0] == '/') barI = 1;

    for(i = len - barF - 1; i >= 0; i--)
    {
        if(in[i] == '/') break;
    }

    if(i < 0) i = 0;

    strcpy(out, in);

    out[i + barI] = '\0';

    return ;
}

void withoutRedundancy(char * in, char *out)
{

    char *in_2 = malloc(strlen(in));
    strcpy(in_2,in);

    out[0] = '\0';

    char *aux = malloc(strlen(in));

    aux = strtok ( in_2 ,"/");
    while (aux)
    {
        if(!strcmp(aux,"..")){
            int i = strlen(out) - 1;
            while( out[i] != '/' ) i--;
            out[i] = '\0';
        }
        else if(strcmp(aux,".")){
            strcat(out, "/");
            strcat(out, aux);
        }
        aux = strtok (NULL, "/");
    }

}

void concat(char *out, char* ini, char* end)
{
    strcpy(out, ini);
    strcat(out, end);
    return ;
}

int isAlfaNumerico(char a){
    return (a >= 48 && a <= 57) || (a >= 65 && a <= 90) || (a >= 97 && a <= 122);
}

int checkFilename(char *filename){
    int i = 0;
    while(filename[i] != '\0' && isAlfaNumerico( filename[i] )  && i++ < 25);
    if(filename[i] == '\0') return 1;
    else return 0;
}

int checkPathname(char *filename){
    int i = 0;
    while(filename[i] != '\0' && (isAlfaNumerico( filename[i] ) || filename[i] == '/')   && i++ < 25);
    if(filename[i] == '\0') return 1;
    else return 0;
}






