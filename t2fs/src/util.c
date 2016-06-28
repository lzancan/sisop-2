#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/t2fs.h"
#include "../include/util.h"

void inicializa (int* superbloco_lido){
    if(*superbloco_lido==0){
        numero_arquivos_abertos = 0;
        numero_diretorios_abertos = 0;
        //printf("inicializando...\n");
        if(le_superbloco(&superbloco)==0){
            *superbloco_lido=1;
            diretorio_corrente="/"; // raiz
            dir_corrente=2; // raiz
            inicio_bloco_dados = superbloco.superBlockSize+superbloco.bitmapSize+superbloco.rootSize;
            fim_bloco_dados=(superbloco.nOfSectors/superbloco.blockSize)-superbloco.blockSize; // último bloco do disco
            posicao_atual=inicio_bloco_dados*superbloco.blockSize+contador_posicao*superbloco.blockSize;
            TAM_MAX_ARQ=superbloco.blockSize*(pow(superbloco.blockSize/4,2)+(superbloco.blockSize/4)+2);
            TAMANHO_BLOCO=superbloco.blockSize;
            posicao_atual=dir_corrente;
            setor_atual=0;
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

int le_record(struct t2fs_record* record,unsigned int sector,int posicao_no_setor){
    char content[SECTOR_SIZE];
    if(read_sector(sector,content)==0){
        memcpy(record, content+posicao_no_setor*64, sizeof(struct t2fs_record));
        return 0;
    }
    else return -1;
}

int le_setor (struct t2fs_record* records,unsigned int sector){
    char content[SECTOR_SIZE];
    if(read_sector(sector,content)==0){
        int i=0;
        for(i=0;i<4;i++){
            memcpy(&records[i], content+(64*i), sizeof(struct t2fs_record));
        }
        return 0;
    }
    else return ERRO;
}

void imprime_superbloco (struct t2fs_superbloco superbloco){
    printf("id: %s\n",superbloco.id);
    printf("versao: %x\n",superbloco.version);
    printf("tamanho super bloco: %d\n", superbloco.superBlockSize); // 1 bloco
    printf("tamanho bloco: %d\n", superbloco.blockSize); //Quantidade de setores lógicos que formam um bloco lógico
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
    //printf("reservado: %u\n",(unsigned int)record.Reserved);
    printf("tamanho arquivo (blocos): %d\n",record.blocksFileSize); //Tamanho do arquivo, expresso em número de blocos de dados
    printf("tamanho arquivo (bytes): %d\n",record.bytesFileSize); //Tamanho do arquivo. Expresso em número de bytes.
    printf("ponteiros diretos: %d e %d\n",record.dataPtr[0], record.dataPtr[1]); //Dois ponteiros diretos (little endian).
    printf("ponteiro ind simples: %d\n",record.singleIndPtr); //Ponteiro de indireção simples (little endian).
    printf("ponteiro ind dupla: %d\n",record.doubleIndPtr); //Ponteiro de indireção dupla (little endian).
}

void imprime_setor (struct t2fs_record* records){
    int i=0;
    for(i=0;i<4;i++){
        imprime_record (records[i]);
    }
}

int eh_alfanumerico (char letra){
    if(letra=='.') // extensão
        return 1;
    if(letra >= '0' && letra <= '9') // números de 0 a 9
        return 1;
    else if (letra >='a' && letra <= 'z') // letras de (a-z) (minúsculas)
        return 1;
        else if (letra>='A' && letra <='Z') // letras de (A-Z) (maiúsculas)
            return 1;
            else return 0; // caso não alfanumérico, retorna 0
}

int testa_nome(char* Filename){
    int i = 0;
    char char_atual = Filename[i];
    while (char_atual != '\0'){
        if(!eh_alfanumerico (char_atual)){
                return 0;
        }
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

void incrementa_posicao(){ // não mais usada
    if(contador_posicao<((fim_bloco_dados/4)-inicio_bloco_dados)){
        contador_posicao++;
        posicao_atual=inicio_bloco_dados*4+contador_posicao*4;
    }
}

void decrementa_posicao(){ // não mais usada
    if(contador_posicao>0){
        contador_posicao--;
        posicao_atual=inicio_bloco_dados*4+contador_posicao*4;
    }
}

void set_posicao(int bloco){ // não mais usada
    posicao_atual=bloco*TAMANHO_BLOCO;
}

int caminho_absoluto_relativo (char* caminho){
    if(caminho[0]=='/')
        return 1;
    else return 0;
}

int search_sector (struct t2fs_record* records,char* Filename,unsigned int setor){
    le_setor(records,setor);
    int i=0;
    for(i=0;i<4;i++){
        if(!strcmp((records[i].name),Filename)){
            return i;
        }
    }
    return ERRO;
}

int search_bloco (struct t2fs_record* records,char* Filename,int bloco){
    int retorno;
    posicao_atual=bloco;
    setor_atual=0;
    int i=0;
    for(i=0;i<TAMANHO_BLOCO;i++){
        retorno = search_sector(records,Filename,(bloco*TAMANHO_BLOCO)+i);
        if(retorno!=ERRO){
            setor_atual=i;
            return retorno;
        }
    }
    return ERRO;
}

int procura_continuidade_dir (struct t2fs_record* records,char* Filename){
    //int ponteiro_direto0=record.dataPtr[0];
    int ponteiro_direto1=records[0].dataPtr[1];
    int singleIndPtr=records[0].singleIndPtr;
    int doubleIndPtr=records[0].doubleIndPtr;
    int retorno;
    if((ponteiro_direto1==dir_corrente||ponteiro_direto1==0)&&(singleIndPtr==dir_corrente||singleIndPtr==0)
        &&(doubleIndPtr==dir_corrente||doubleIndPtr==0)){
                    return 0;
        }
    else{
        retorno=search_bloco(records,Filename,ponteiro_direto1); // procura no ponteiro direto 1
        if(retorno!=ERRO){ // achou
            posicao_atual=ponteiro_direto1;
            return retorno;
        }
        else { // procura no single ind ptr
            retorno = procura_single_ind_ptr(records,Filename,singleIndPtr);
            if(retorno!=ERRO){ // achou
                return retorno;
            }
            else{ // procura no double ind ptr
                retorno = procura_double_ind_ptr(records,Filename,doubleIndPtr);
                if(retorno != ERRO){ // achou
                    return retorno;
                }
            }
        }
    }
    return 0; // não achou
}

int procura_single_ind_ptr(struct t2fs_record* records,char* Filename,unsigned int singleIndPtr_setor){
    int retorno;
    char content[SECTOR_SIZE];
    if(read_sector(singleIndPtr_setor,content)==0){
        int j=0;
        for(j=0;j<4;j++){
            memcpy(&ponteiro_indireto_simples, content+(64*j), sizeof(struct IndPtr));
            int i=0;
            for(i=0;i<64;i++){
                if(ponteiro_indireto_simples.bloco[i]!=0){
                    retorno=search_bloco(records,Filename,ponteiro_indireto_simples.bloco[i]);
                    if(retorno != ERRO){
                        posicao_atual=ponteiro_indireto_simples.bloco[i];
                        return retorno;
                    }
                }
            }
        }
    }
    return ERRO;

}

int procura_double_ind_ptr(struct t2fs_record* records, char* Filename,unsigned int doubleIndPtr_setor){
    int retorno;
    char content[SECTOR_SIZE];
    if(read_sector(doubleIndPtr_setor,content)==0){
        int j=0;
        for(j=0;j<4;j++){
            memcpy(&ponteiro_indireto_duplo, content+(j*64), sizeof(struct IndPtr));
            int i=0;
            for(i=0;i<64;i++){
                if(ponteiro_indireto_duplo.bloco[i]!=0){
                    retorno=procura_single_ind_ptr(records,Filename,ponteiro_indireto_duplo.bloco[i]);
                    if(retorno != ERRO){
                        return retorno;
                    }
                }
            }
        }
    }
    return ERRO;
}

int procura_arquivo (struct t2fs_record* records,char* Filename,int dir_corrente){
    int retorno;
    if(testa_nome(Filename)==0){ // se não "bate" o nome, retorna 0
        return ERRO;
    }
    posicao_atual=dir_corrente;
    retorno=search_bloco(records,Filename,dir_corrente);
    if(retorno!=ERRO){
        return retorno;
    }
    else{ // se não encontrou no diretório, procura se o diretório é continuo (testa os ponteiros e navega, procurando pelo arquivo
        le_setor(records,dir_corrente*TAMANHO_BLOCO);
        retorno=procura_continuidade_dir (records,Filename);
        if(retorno==0){
            return ERRO;
        }
        else {
            return retorno;
        }
    }
    return ERRO;
}

struct t2fs_record* inicializaRecord(char *name, int Bloco){
    struct t2fs_record *record = (struct t2fs_record*) calloc(1, sizeof(struct t2fs_record));
    record->TypeVal = TYPEVAL_REGULAR;
    memset (record->name,'\0',32);
    strncpy(record->name, name,32);
    record->bytesFileSize = 0;
    record->dataPtr[0] = Bloco;
    return record;
}

int search_sector_TypeVal (struct t2fs_record* records,unsigned int TypeVal,unsigned int setor){
    le_setor(records,setor);
    int i=0;
    for(i=0;i<4;i++){
        if((unsigned int)records[i].TypeVal==TypeVal){
            return i;
        }
    }
    return ERRO;
}

int search_bloco_TypeVal (struct t2fs_record* records,unsigned int TypeVal, int bloco){
    int retorno;
    posicao_atual=bloco;
    setor_atual=0;
    int i=0;
    for(i=0;i<TAMANHO_BLOCO;i++){
        retorno = search_sector_TypeVal(records,TypeVal,(bloco*TAMANHO_BLOCO)+i);
        if(retorno!=ERRO){
            setor_atual=i;
            return retorno;
        }
    }
    return ERRO;
}

int procura_single_ind_ptr_TypeVal(struct t2fs_record* records,unsigned int TypeVal,unsigned int singleIndPtr_setor){
    int retorno;
    char content[SECTOR_SIZE];
    if(read_sector(singleIndPtr_setor,content)==0){
        int j=0;
        for(j=0;j<4;j++){
            memcpy(&ponteiro_indireto_simples, content+(64*j), sizeof(struct IndPtr));
            int i=0;
            for(i=0;i<64;i++){
                if(ponteiro_indireto_simples.bloco[i]!=0){
                    retorno=search_bloco_TypeVal(records,TypeVal,ponteiro_indireto_simples.bloco[i]);
                    if(retorno != ERRO){
                        posicao_atual=ponteiro_indireto_simples.bloco[i];
                        return retorno;
                    }
                }
            }
        }
    }
    return ERRO;

}

int procura_double_ind_ptr_TypeVal(struct t2fs_record* records,unsigned int TypeVal,unsigned int doubleIndPtr_setor){
    int retorno;
    char content[SECTOR_SIZE];
    if(read_sector(doubleIndPtr_setor,content)==0){
        int j=0;
        for(j=0;j<4;j++){
            memcpy(&ponteiro_indireto_duplo, content+(j*64), sizeof(struct IndPtr));
            int i=0;
            for(i=0;i<64;i++){
                if(ponteiro_indireto_duplo.bloco[i]!=0){
                    retorno=procura_single_ind_ptr_TypeVal(records,TypeVal,ponteiro_indireto_duplo.bloco[i]);
                    if(retorno != ERRO){
                        return retorno;
                    }
                }
            }
        }
    }
    return ERRO;
}

int procura_continuidade_dir_TypeVal (struct t2fs_record* records,unsigned int TypeVal){
    //int ponteiro_direto0=record.dataPtr[0];
    int ponteiro_direto1=records[0].dataPtr[1];
    int singleIndPtr=records[0].singleIndPtr;
    int doubleIndPtr=records[0].doubleIndPtr;
    int retorno;
    if((ponteiro_direto1==dir_corrente||ponteiro_direto1==0)&&(singleIndPtr==dir_corrente||singleIndPtr==0)
        &&(doubleIndPtr==dir_corrente||doubleIndPtr==0)){
                    return 0;
        }
    else{
        retorno=search_bloco_TypeVal(records,TypeVal,ponteiro_direto1); // procura no ponteiro direto 1
        if(retorno!=ERRO){ // achou
            posicao_atual=ponteiro_direto1;
            return retorno;
        }
        else { // procura no single ind ptr
            retorno = procura_single_ind_ptr_TypeVal(records,TypeVal,singleIndPtr);
            if(retorno!=ERRO){ // achou
                return retorno;
            }
            else{ // procura no double ind ptr
                retorno = procura_double_ind_ptr_TypeVal(records,TypeVal,doubleIndPtr);
                if(retorno != ERRO){ // achou
                    return retorno;
                }
            }
        }
    }
    return 0; // não achou
}


int procura_TypeVal (struct t2fs_record* records,int bloco,unsigned int TypeVal){
    int retorno;
    //set_posicao(dir_corrente);
    retorno=search_bloco_TypeVal(records,TypeVal,bloco); // procura no bloco um typeval igual a 0. Se encontrar retorna o número do setor (0,1,2,3)
    if(retorno!=ERRO){
        return retorno;
    }
    else{ // se não encontrou no diretório, procura se o diretório é continuo (testa os ponteiros e navega, procurando pelo arquivo
        le_setor(records,dir_corrente*(superbloco.blockSize));
        retorno=procura_continuidade_dir_TypeVal (records,TypeVal);
        if(retorno==0){
            return ERRO;
        }
        else {
            return retorno;
        }
    }
    return ERRO;
}

void libera_blocos_record(int ponteiro_dir0,int ponteiro_dir1,int ponteiro_ind_simples,int ponteiro_ind_duplo){
    if(ponteiro_dir0!=0){
    freeBlock2(ponteiro_dir0);
    }
    if(ponteiro_dir1!=0){
        freeBlock2(ponteiro_dir1);
    }
    char content[SECTOR_SIZE];
    if(ponteiro_ind_simples!=0){
        if(read_sector(ponteiro_ind_simples,content)==0){
        int j=0;
        for(j=0;j<4;j++){
            memcpy(&ponteiro_indireto_simples, content+(64*j), sizeof(struct IndPtr));
            int i=0;
            for(i=0;i<64;i++){
                if(ponteiro_indireto_simples.bloco[i]!=0){
                    freeBlock2(ponteiro_indireto_simples.bloco[i]);
                }
            }
        }
        }
        freeBlock2(ponteiro_ind_simples);
    }
    if(ponteiro_ind_duplo!=0){
       if(read_sector(ponteiro_ind_duplo,content)==0){
        int j=0;
        for(j=0;j<4;j++){
            memcpy(&ponteiro_indireto_duplo, content+(j*64), sizeof(struct IndPtr));
            int i=0;
            for(i=0;i<64;i++){
                if(ponteiro_indireto_duplo.bloco[i]!=0){
                    if(read_sector(ponteiro_indireto_simples.bloco[i],content)==0){
                        int j=0;
                        for(j=0;j<4;j++){
                            memcpy(&ponteiro_indireto_simples, content+(64*j), sizeof(struct IndPtr));
                            int i=0;
                            for(i=0;i<64;i++){
                                if(ponteiro_indireto_simples.bloco[i]!=0){
                                    freeBlock2(ponteiro_indireto_simples.bloco[i]);
                                }
                            }
                        }
                    }
                   freeBlock2(ponteiro_indireto_simples.bloco[i]);
                }
            }
        }
       }
       freeBlock2(ponteiro_ind_duplo);
    }
}

char** matPath(char *path, int *k){
    char* token;
    char* string;
    char* lim = "/";
    char* tofree;
    char* tst[32];
    string = strdup(path);
    int i=0;
    if (string != NULL) {

        tofree = string;
        token = strtok(string, lim);
        tst[i] = strdup(token);
//        printf("%s\n", tst[i]);
        i++;
        while ((token = strtok(NULL, lim)) != NULL)
        {
            tst[i] = strdup(token);
//            printf("%s\n", tst[i]);
            i++;
        }

    free(tofree);
    }
    *k=i;

    return tst;
}

//OPEN

OPENED_FILE* procura_arquivo_aberto(int handle)
{
    OPENED_FILE *arquivo;

    if(arquivos_abertos.first == NULL) {
        return NULL;
    }
    else{
        arquivo = arquivos_abertos.first;
        while (arquivo != NULL) {
            if (arquivo->handle == handle) {
                return arquivo;
            }
            arquivo = arquivo->next;
        }
    }
    return NULL;
}

//void printFile(OPENED_FILE p){
//}

int abre_arquivo(struct t2fs_record record, char* fatherName){
    /*
    static int last_handle = 0;
    OPENED_FILE *newFile;
    int handle;
    if((nOpenedFiles < 20) || (nOpenedDirs < 20))
    {
    newFile = calloc(1, sizeof(OPENED_FILE));
    if (newFile == NULL) {
        // Allocation failure, out of memory.
        return ERROR_SIGNAL;
    }
    handle = ++last_handle;
    newFile->handle = handle;
    newFile->record = record;
    newFile->cursor = 0;
    newFile->fatherName = fatherName;
    if(record.TypeVal == 0x01)//NÃO SEI SE TA CERTO ESSE CMP
    {
        if (opened_files->last == NULL) {//ta vazio
        opened_files->first = newFile;
        opened_files->last = newFile;
    } else {
        opened_files->last->next = newFile;
        opened_files->last = newFile;
    }
    nOpenedFiles++;
    }
    else if(record.TypeVal == 0x02){
        if (opened_dirs->last == NULL) {//ta vazio
        opened_dirs->first = newFile;
        opened_dirs->last = newFile;
    } else {
        opened_dirs->last->next = newFile;
        opened_dirss->last = newFile;
    }
    nOpenedDirs++;


    }

    return handle;
    }
    return -1; //MAX de 20 arquivos
    */
    return 0;
}

int close(char TypeVal, int handle) {
    /*
    if(TypeVal == 0x01){
        OPENED_FILE *file = NULL;
        OPENED_FILE *previous = NULL;
        file = opened_files->first;
            while (file != NULL) {
                if (file->handle == handle) {
                    if (previous) {
                        previous->next = file->next;
                    }
                if (opened_files->first == file) {
                    opened_files->first = file->next;
                }
                if (opened_files->last == file) {
                    opened_files->last = previous;
                }
                free(file);
                nOpenedFiles--;
            return 0;
            }
        previous = file;
        file = file->next;
        }
    }
    else if(TypeVal == 0x02)
    {
        OPENED_FILE *dir = NULL;
        OPENED_FILE *predir = NULL;
        dir = opened_dirs->first;
            while (dir!= NULL) {
                if (dir->handle == handle) {
                    if (predir) {
                        predir->next = dir->next;
                    }
                if (opened_dirs->first == dir) {
                    opened_dirs->first = dir->next;
                }
                if (opened_dirs->last == dir) {
                    opened_dirs->last = predir;
                }
                free(dir);
                nOpenedDirs--;
            return 0;
            }
        predir = dir;
        dir = dir->next;
        }

    }

    return ERROR_SIGNAL;
    */
    return 0;
}
