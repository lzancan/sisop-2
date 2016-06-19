#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/util.h"

void inicializa (int* superbloco_lido){
    if(*superbloco_lido==0){
        if(le_superbloco(&superbloco)==0){
            *superbloco_lido=1;
            diretorio_corrente=".";
            inicio_bloco_dados = superbloco.superBlockSize+superbloco.bitmapSize+superbloco.rootSize;
            fim_bloco_dados=(superbloco.nOfSectors/superbloco.blockSize)-superbloco.blockSize; // último bloco do disco
            //printf ("%d\n",fim_bloco_dados);
            posicao_atual=inicio_bloco_dados*4+contador_posicao*4;
        }
    }
}

int le_superbloco(struct t2fs_superbloco* superbloco){
    char content[SECTOR_SIZE];
    if(read_sector(0,content)==0){
        memcpy(superbloco, content, sizeof(struct t2fs_superbloco));
        return 0;
    }
    else return -1;
}

int le_record(struct t2fs_record* record,unsigned int sector){
    char content[SECTOR_SIZE];
    if(read_sector(sector,content)==0){
        memcpy(record, content, sizeof(struct t2fs_record));
        return 0;
    }
    else return -1;
}

void imprime_superbloco (struct t2fs_superbloco superbloco){
    printf("id: %s\n",superbloco.id);
    printf("versao: %x\n",superbloco.version);
    printf("tamanho super bloco: %x\n", superbloco.superBlockSize); // 1 bloco
    printf("tamanho bloco: %x\n", superbloco.blockSize); //Quantidade de setores lógicos que formam um bloco lógico
    printf("tamanho disco: %d\n", superbloco.diskSize); // Quantidade de bytes da partição T2FS. Inclui o superbloco, a área de bitmap, o diretório raiz e a área de dados.
    printf("numero setores: %d\n", superbloco.nOfSectors); //Quantidade total de setores lógicos na partição T2FS. Inclui o superbloco, a área de bitmap, o diretório raiz e a área de dados.
    printf("tamanho bitmap: %d\n", superbloco.bitmapSize); //Quantidade de blocos lógicos usados para armazenar o bitmap de blocos livres e ocupados.
    printf("tamanho dir raiz : %d\n", superbloco.rootSize); //Quantidade de blocos lógicos usados para armazenar o diretório raiz.
    printf("entradas no dir raiz: %d\n", superbloco.nOfDirEntries); //Quantidade de entradas no diretório raiz.
    printf("tamanho registro : %d\n", superbloco.fileEntrySize); //Quantidade de bytes de um registro na área de diretório.
}

void imprime_record (struct t2fs_record record){
    printf("tipo (valor): %u\n",(unsigned int)record.TypeVal); // Tipo da entrada. Indica se o registro é inválido (0x00), arquivo (0x01) ou diretório (0x02)
    printf("nome arquivo: %s\n",record.name);
    printf("reservado: %u\n",(unsigned int)record.Reserved);
    printf("tamanho arquivo (blocos): %d\n",record.blocksFileSize); //Tamanho do arquivo, expresso em número de blocos de dados
    printf("tamanho arquivo (bytes): %d\n",record.bytesFileSize); //Tamanho do arquivo. Expresso em número de bytes.
    printf("ponteiros diretos: %d e %d\n",record.dataPtr[0], record.dataPtr[1]); //Dois ponteiros diretos (little endian).
    printf("ponteiro ind simples: %d\n",record.singleIndPtr); //Ponteiro de indireção simples (little endian).
    printf("ponteiro ind dupla: %d\n",record.doubleIndPtr); //Ponteiro de indireção supla (little endian).
}


int eh_alfanumerico (char letra){
    if(letra >= 48 && letra <= 57) // números de 0 a 9
        return 1;
    else if (letra >=97 && letra <= 122) // letras de (a-z) (minúsculas)
        return 1;
        else if (letra>=65 && letra <=90) // letras de (A-Z) (maiúsculas)
            return 1;
            else return 0; // caso não alfanumérico, retorna 0
}

int testa_nome(char* Filename){
    int i = 0;
    char char_atual = Filename[i];
    while (char_atual != '\0'){
        if(!eh_alfanumerico (char_atual))
            return 0;
        else {
        i++;
        char_atual = Filename[i];
        if(i>32)
            return 0;
        if(char_atual=='\0')
            return 1;
        }
    }
    return 0;
}

void incrementa_posicao(){
    if(contador_posicao<((fim_bloco_dados/4)-inicio_bloco_dados)){
        contador_posicao++;
        posicao_atual=inicio_bloco_dados*4+contador_posicao*4;
    }
}

void decrementa_posicao(){
    if(contador_posicao>0){
        contador_posicao--;
        posicao_atual=inicio_bloco_dados*4+contador_posicao*4;
    }
}


int procura_arquivo (char* Filename,char* diretorio_corrente){
    if(testa_nome(Filename)==0) // se não "bate" o nome, retorna 0
        return 1;
    incrementa_posicao();incrementa_posicao();incrementa_posicao();
    decrementa_posicao();

    return 0;

}
