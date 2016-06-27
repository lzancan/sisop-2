#include "../include/main.h"

int main(){
    create2("oi");
    //imprime_superbloco(superbloco);

    //le_record(&record,superbloco.blockSize*2,0); // setor 8 a 11
    //imprime_record(record);
    //printf ("%d\n",posicao_atual);
    //printf ("%d\n",inicio_bloco_dados*4);
    //printf("%d\n",superbloco.blockSize*2);
    //printf("%d\n",inicio_bloco_dados*4);

    //le_record(&record,inicio_bloco_dados*4);
    printf("%d",procura_arquivo("teste.txt",dir_corrente));
    //le_setor(&setor,8);
    //imprime_setor(setor);

    return 0;
}
