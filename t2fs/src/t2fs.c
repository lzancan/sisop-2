#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/t2fs.h"
#include "../include/util.h"

int identify2 (char *name, int size){
    char* identificador="Gabriel Ammes Pinho <230277> - Luciano Zancan <230047>";
    if(size>=54){
        strcpy(name,identificador);
        return 0;
    }
    else return ERRO;
}

FILE2 create2 (char *filename){
    inicializa (&superbloco_lido);
    struct t2fs_record records[TAMANHO_BLOCO]; // declara array de records, que formam um bloco
    if(procura_arquivo(records,filename,dir_corrente)!=ERRO){ //se existe o arquivo no diretório corrente
        return ERRO;
    }
    else{ // não existe, cria!
        int bloco_livre=searchFreeBlock2();
        if(bloco_livre!=0){ // se for 0 indica que não tem blocos livres ou deu erro
            printf ("%d\n",bloco_livre);
            char content[SECTOR_SIZE];
            allocBlock2 (bloco_livre); // aloca o bloco de dados para o arquivo
            struct t2fs_record* novo_record = inicializaRecord(filename, bloco_livre); // inicializa o arquivo, colocando os dados necessários
            int record_do_setor_a_escrever=procura_TypeVal (records,dir_corrente,TYPEVAL_INVALIDO); // record da struct global setor em que vai escrever
            if(record_do_setor_a_escrever!=ERRO){ // encontrou; e o setor (struct global) estará com os dados, então é só gravar
                records[record_do_setor_a_escrever]=*novo_record;
                //imprime_setor(records);
                printf("pos atual: %d\n",posicao_atual);
                printf("setor atual: %d\n",setor_atual);
                write_sector ((posicao_atual*4)+setor_atual,(char*) records);
                printf ("criou\n");
                int handle;
                handle = open2(filename);
                return handle;
            }
        }
    }
    return ERRO;
}

int delete2 (char *filename){
    inicializa (&superbloco_lido);
    return 0;
}

FILE2 open2 (char *filename){
    /*
    inicializa (&superbloco_lido);
    //ACHA O ARQUIVO
    int handle = open(record,diretorio_corrente);//NÃO SEI SE É ESSA A CHAMADA DO PONTEIRO
    return handle;
    */
}

int close2 (FILE2 handle){
    inicializa (&superbloco_lido);
    close(0x01, handle);
    return 0;
}

int read2 (FILE2 handle, char *buffer, int size){
    inicializa (&superbloco_lido);
    return 0;
}

int write2 (FILE2 handle, char *buffer, int size){
    inicializa (&superbloco_lido);
    return 0;
}

int seek2 (FILE2 handle, DWORD offset){
    inicializa (&superbloco_lido);
    OPENED_FILE* aux = findOpenedFile(handle);
    if(!aux) return -1;
    if(aux->record.bytesFileSize <= offset) aux->cursor = offset;
    else if (offset == -1) aux->cursor = aux->record.bytesFileSize;
    else return ERRO;
    return 0;
}

int mkdir2 (char *pathname){
    inicializa (&superbloco_lido);
    return 0;
}

int rmdir2 (char *pathname){
    inicializa (&superbloco_lido);
    return 0;
}

DIR2 opendir2 (char *pathname){
    inicializa (&superbloco_lido);
    //ACHA O DIREtORIO
    int handle = 0;
    //handle = open(record,PAI DO DIRETORIO QUE TU ACHOU);//NÃO SEI SE É ESSA A CHAMADA DO PONTEIRO
    return handle;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
    inicializa (&superbloco_lido);
    return 0;
}

int closedir2 (DIR2 handle){
    inicializa (&superbloco_lido);
    close(0x02, handle);
    return 0;
}

int chdir2 (char *pathname){
    inicializa (&superbloco_lido);
    return 0;
}

int getcwd2 (char *pathname, int size){
    inicializa (&superbloco_lido);
    return 0;
}
