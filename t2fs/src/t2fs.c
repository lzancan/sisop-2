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
    //printf ("tamanho bloco: %d\n",TAMANHO_BLOCO);
    struct t2fs_record records[4]; // declara array de records, que formam um setor
    if(procura_arquivo(records,filename,dir_corrente)!=ERRO){ //se existe o arquivo no diret�rio corrente
        return ERRO;
    }
    else{ // n�o existe, cria!
        int bloco_livre=searchFreeBlock2();
        if(bloco_livre!=0){ // se for 0 indica que n�o tem blocos livres ou deu erro
            printf ("%d\n",bloco_livre);
            char content[SECTOR_SIZE];
            allocBlock2 (bloco_livre); // aloca o bloco de dados para o arquivo
            struct t2fs_record* novo_record = inicializaRecord(filename, bloco_livre); // inicializa o arquivo, colocando os dados necess�rios
            int record_do_setor_a_escrever=procura_TypeVal (records,dir_corrente,TYPEVAL_INVALIDO); // record da struct global setor em que vai escrever
            if(record_do_setor_a_escrever!=ERRO){ // encontrou; e o setor (struct global) estar� com os dados, ent�o � s� gravar
                records[record_do_setor_a_escrever]=*novo_record;
                //printf("pos atual: %d\n",posicao_atual);
                //printf("setor atual: %d\n",setor_atual);
                //imprime_setor (records);
                write_sector ((posicao_atual*TAMANHO_BLOCO)+setor_atual,(char*) records);
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
    struct t2fs_record records[4]; // array de records para formar um setor
    int record_do_setor_a_escrever=procura_arquivo(records,filename,dir_corrente); // se existe o arquivo no diret�rio corrente // procura arquivo retorna o record do setor (0,1,2,3)
    if(record_do_setor_a_escrever!=ERRO){ // posicao_atual estar� com o n�mero do bloco // setor atual estar� com o n�mero do setor... e records[] conter� o setor atual.
        int ponteiro_direto0= records[record_do_setor_a_escrever].dataPtr[0];
        int ponteiro_direto1= records[record_do_setor_a_escrever].dataPtr[1];
        int ponteiro_indireto_simples=records[record_do_setor_a_escrever].singleIndPtr;
        int ponteiro_indireto_duplo=records[record_do_setor_a_escrever].doubleIndPtr;
        libera_blocos_record(ponteiro_direto0,ponteiro_direto1,ponteiro_indireto_simples,ponteiro_indireto_duplo);
        records[record_do_setor_a_escrever].TypeVal=TYPEVAL_INVALIDO;
        strcpy(records[record_do_setor_a_escrever].name, "\0");
        write_sector ((posicao_atual*TAMANHO_BLOCO)+setor_atual,(char*) records);
        return 0;
    }
    else { // n�o existe no diret�rio corrente. N�o � poss�vel apagar, retorna erro
        return ERRO;
    }
}

FILE2 open2 (char *filename){
    inicializa (&superbloco_lido);
    struct t2fs_record records[4];
    int record_do_setor = procura_arquivo(records,filename,dir_corrente);
    if(record_do_setor==ERRO){ // n�o achou o arquivo
        return ERRO;
    }
    else{ // achou o arquivo, abre ele...
        abre_arquivo(records[record_do_setor]);
    }




    //int handle = open(record,diretorio_corrente);//N�O SEI SE � ESSA A CHAMADA DO PONTEIRO
   // return handle;

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
    /*
    inicializa (&superbloco_lido);
    OPENED_FILE* aux = findOpenedFile(handle);
    if(!aux) return -1;
    if(aux->record.bytesFileSize <= offset) aux->cursor = offset;
    else if (offset == -1) aux->cursor = aux->record.bytesFileSize;
    else return ERRO;
    */
    return 0;
}

int mkdir2 (char *pathname){
    int i = 0;//numero de dirs
    int k=0;
    char **temp;
    temp = matPath(pathname,&i);
    char* dirs[i];
    for(k=0;k<i;k++)
    {
        dirs[k] = temp[k];
    }


    //    inicializa (&superbloco_lido);
//    contador_diretorios=0;
//    struct t2fs_record records[4]; // declara array de records, que formam um setor
//    char* path="";
//    if(caminho_absoluto_relativo (pathname)==0){// relativo, come�a SEM '/' (0= relativo, 1= absoluto) // absoluto � a partir do raiz, relativo � a partir do diret�rio corrente
//        // relativo, a partir do diret�rio corrente
//        strcat(path,diretorio_corrente); // path recebe o diret�rio corrente ('/')
//        strcat(path,pathname);// path recebe o pathname ('/dir1')
//    }
//    else{
//        // absoluto, a partir do raiz;
//        strcpy(path,pathname); //path recebe o nome do pathname ('/dir1')
//    }
//        // exemplo: 'dir1/dir2/dir3' , exemplo2: 'dir1'
//    // procura o diret�rio, procurando de um em um...
//
//
//    // TODO
//
//    // *********************
//    // se n�o existir, cria:
//    struct t2fs_record *novos_records = (struct t2fs_record*) calloc(4, sizeof(struct t2fs_record));
//
//    novos_records[0].bytesFileSize = 0;
//    novos_records[0].TypeVal = TYPEVAL_DIRETORIO;
//    strcpy(novos_records[0].name, ".");
//    novos_records[0].dataPtr[0] = 20; // bloco onde ser� gravado
//
//    novos_records[1].bytesFileSize = 0;
//    novos_records[1].TypeVal = TYPEVAL_DIRETORIO;
//    strcpy(novos_records[1].name, "..");
//    novos_records[0].dataPtr[0] = 20; // lugar do pai
//

}

int rmdir2 (char *pathname){
    inicializa (&superbloco_lido);
    return 0;
}

DIR2 opendir2 (char *pathname){
    inicializa (&superbloco_lido);
    //ACHA O DIREtORIO
    int handle = 0;
    //handle = open(record,PAI DO DIRETORIO QUE TU ACHOU);//N�O SEI SE � ESSA A CHAMADA DO PONTEIRO
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
