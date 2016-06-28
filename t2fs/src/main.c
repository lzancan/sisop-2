#include "../include/main.h"

int main(){
    printf("%d\n",create2("oi"));
    printf("%d\n",create2("oi"));
    //imprime_superbloco(superbloco);
    inicializa(&superbloco_lido);
    struct t2fs_record records[TAMANHO_BLOCO];
    le_setor(records,8);
    imprime_setor(records);
    //printf ("%d\n",procura_arquivo(records,"oi",dir_corrente));
    //le_record(&record,superbloco.blockSize*2,0); // setor 8 a 11
    //imprime_record(record);
    //printf ("%d\n",posicao_atual);
    //printf ("%d\n",inicio_bloco_dados*4);
    //printf("%d\n",superbloco.blockSize*2);
    //printf("%d\n",inicio_bloco_dados*4);
    //le_record(&record,inicio_bloco_dados*4);
    //char content[SECTOR_SIZE];
    //read_sector(12,content);
/*
    int i=0;
    for(i=4;i<30;i++)
    freeBlock2(i);
*/
    //printf ("%s\n",content);
    //printf ("%s",(char*) setor);
    //le_setor(records,8);
    //imprime_setor(records);
    return 0;
}
