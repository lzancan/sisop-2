#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/util.h"

void le_superbloco(struct t2fs_superbloco* superbloco){
    char content[SECTOR_SIZE];
    if(read_sector(0,content)==0){
        memcpy(superbloco, content, sizeof(struct t2fs_superbloco));
    }
    //imprime_superbloco(superbloco);
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

int procura_arquivo (char* Filename){
    testa_nome(Filename);
    return 0;

}
