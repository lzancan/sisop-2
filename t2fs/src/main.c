#include "../include/main.h"

int main(){
//    printf("%d\n",create2("oi"));
    //inicializa(&superbloco_lido);
    //le_superbloco(&superbloco);
    //imprime_superbloco(superbloco);
    //struct t2fs_record records[4];
    //le_setor(records,TAMANHO_BLOCO*2); // diretório raiz
    //imprime_setor(records);
    //le_setor(records,TAMANHO_BLOCO*2+1); // diretório raiz
    //imprime_setor(records);
    //open2("oi1");
    //open2("oi2");
    //open2("oi1");
    //open2("oi2");
    char oi[MAX_FILE_NAME_SIZE];
    getcwd2 (oi, 15);
    printf("%s",oi);
    //imprime_arquivos_abertos();
    //imprime_arquivos_abertos();
    //close2 (2);close2 (3);close2 (1);close2 (0);close2 (2);close2 (3);
    //imprime_arquivos_abertos();

    //close2(1);close2(0);
    //imprime_arquivos_abertos();
    //imprime_arquivos_abertos();
    //printf ("num arq_abertos: %d , num dirs_abertos: %d \n",numero_arquivos_abertos,numero_diretorios_abertos);

    //printf("%d",delete2("oi"));
/*
    int i=0;
    for(i=4;i<30;i++)
    freeBlock2(i);
*/
    //printf ("%s\n",content);
    //printf ("%s",(char*) setor);
    //le_setor(records,8);
    //imprime_setor(records);
    char *path= "/teste/hue/kkk/lol/hehe/birl";
    mkdir2(path);

    return 0;
}
