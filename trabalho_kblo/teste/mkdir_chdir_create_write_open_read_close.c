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
    char n[2000] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut eget placerat odio. Vestibulum blandit dolor eros. Morbi pharetra pellentesque eros, sit amet porttitor neque maximus eu. Nulla vestibulum orci turpis, nec vestibulum sapien faucibus eu. Mauris molestie sit amet ligula eget dictum. Curabitur sollicitudin sem ut accumsan malesuada. Proin consequat dolor vel dolor placerat semper.\nNullam dapibus justo quis quam tempor faucibus. Quisque quis suscipit enim. Sed fermentum viverra efficitur. Cras ut imperdiet ligula, at consequat leo. Morbi augue est, luctus et facilisis quis, bibendum ut odio. Quisque porta egestas massa, in molestie massa dapibus at. Donec nec velit eget velit vehicula pulvinar. Nullam lectus mauris, aliquam eget dolor sed, facilisis sagittis quam. Ut vitae tellus a est bibendum ultricies non viverra massa. Donec in elit est. Donec accumsan turpis quam, gravida sodales sem placerat a. Nunc fringilla molestie nunc, non gravida eros luctus ac. Nunc ipsum justo metus.\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Ut eget placerat odio. Vestibulum blandit dolor eros. Morbi pharetra pellentesque eros, sit amet porttitor neque maximus eu. Nulla vestibulum orci turpis, nec vestibulum sapien faucibus eu. Mauris molestie sit amet ligula eget dictum. Curabitur sollicitudin sem ut accumsan malesuada. Proin consequat dolor vel dolor placerat semper. ";
    char m[2000];
    int nil = 0;
    int len = strlen(n);
    struct t2fs_record a;
    if( mkdir2("/mtgiambastiani") !=0) return 10;
    if( chdir2("/mtgiambastiani") !=0) return 2;
    handle = create2("MontyPython");
    if(handle == ERROR_SIGNAL) return 3;
    if( write2(handle,n,len) == ERROR_SIGNAL) return 4;
    handle2 = open2("MontyPython");
    if(handle2 == ERROR_SIGNAL) return 5;
    if( read2(handle2, m, len) == ERROR_SIGNAL ) return 6;
    printf("%s\n",m);
    close2(handle);
    close2(handle2);
    return 0;
}

