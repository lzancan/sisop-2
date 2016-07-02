#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/t2fs.h"
#include "../include/util.h"

int main(){
    inicializa(&superbloco_lido);
    int a,b;
    create2("oi1");
    create2("oi2");
    a = open2("oi1");
    b = open2("oi2");
    imprime_arquivos_abertos();
    close2(a);
    imprime_arquivos_abertos();
    close2(b);
    imprime_arquivos_abertos();
    printf("%d\n",delete2("oi1"));
    printf("%d",open2("oi1"));


    return 0;
}
