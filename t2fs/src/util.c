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
        handle=0;
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

//int setor_cursor(int cursor, struct t2fs_record record)
//{
//    float k = cursor/TAMANHO_BLOCO;
//    char *buffer, *buffer2;
//    int d = floor(k);
//    if(k<1)
//        return record.dataPtr[0];
//    else if(k<2)
//        return record.dataPtr[1];
//    else if(cursor < 66){
//        read_sector(record.singleIndPtr, buffer);
//        return atoi(buffer[d-2]);}
//    else{
//        read_sector(record.doubleIndPtr, buffer);
//        int i = floor((d-66)/64);
//        int m = floor((d-66)%64);
//        read_sector(atoi(buffer[i]), buffer2);
//        return atoi(buffer2[m]);}
//}
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

void imprime_arquivo_aberto(OPENED_FILE arquivo){
    printf("handle: %d\n", arquivo.handle);
    printf("nome: %s\n",arquivo.record.name);
    printf ("current_pointer: %d\n", arquivo.current_pointer);
}

void imprime_arquivos_abertos (){
    OPENED_FILE* arquivo_atual=arquivos_abertos.first;
    OPENED_FILE* diretorio_atual=diretorios_abertos.first;
    if(numero_arquivos_abertos>0){ // printa os arquivos abertos
        while(arquivo_atual!=arquivos_abertos.last){
           imprime_arquivo_aberto(*arquivo_atual);
           arquivo_atual=arquivo_atual->next;
        }
        if(arquivo_atual==arquivos_abertos.last){
            imprime_arquivo_aberto(*arquivo_atual);
        }
    }
    if(numero_diretorios_abertos>0){ // printa os diretorios abertos
        while(diretorio_atual!=diretorios_abertos.last){
           imprime_arquivo_aberto(*diretorio_atual);
           diretorio_atual=diretorio_atual->next;
        }
        if(diretorio_atual==arquivos_abertos.last){
            imprime_arquivo_aberto(*diretorio_atual);
        }
    }
}

int abre_arquivo_diretorio(struct t2fs_record record){

    OPENED_FILE *novo_arquivo;
    if(numero_arquivos_abertos + numero_diretorios_abertos>20){ // número máximo de arquivos abertos
        return ERRO;
    }

    novo_arquivo = calloc(1, sizeof(OPENED_FILE));

    if (novo_arquivo != NULL) {
        novo_arquivo->handle = handle;
        handle++;
        //novo_arquivo->nome="";
        novo_arquivo->record = record;
        novo_arquivo->current_pointer = 0;
        if(record.TypeVal == TYPEVAL_REGULAR){
            if (arquivos_abertos.last == NULL) {//ta vazio
                arquivos_abertos.first = novo_arquivo;
                arquivos_abertos.last = novo_arquivo;
            } else {
                arquivos_abertos.last->next = novo_arquivo;
                arquivos_abertos.last = novo_arquivo;
            }
            numero_arquivos_abertos++;
        }
        else if(record.TypeVal == TYPEVAL_DIRETORIO){
                if (diretorios_abertos.last == NULL) {//ta vazio
                    diretorios_abertos.first = novo_arquivo;
                    diretorios_abertos.last =novo_arquivo;
                } else {
                    diretorios_abertos.last->next = novo_arquivo;
                    diretorios_abertos.last = novo_arquivo;
                }
                numero_diretorios_abertos++;
        }
    return novo_arquivo->handle;
    }
    return ERRO;
}

int fecha_arquivo_aberto(int handle) {
    OPENED_FILE* arquivo=procura_arquivo_aberto(handle);
    if(arquivo!=NULL && arquivo->record.TypeVal==TYPEVAL_REGULAR){
        OPENED_FILE* arquivo_temp=arquivos_abertos.first;
        if(arquivos_abertos.first==arquivos_abertos.last){ // só tem um arquivo
            arquivos_abertos.first=NULL;
            arquivos_abertos.last=NULL;
            numero_arquivos_abertos=0;
            return 0;
        }
        if(arquivo==arquivo_temp){ // é o primeiro
                arquivos_abertos.first=arquivo->next;
            }
        else{
                while (arquivo_temp->next!=arquivo) {
                    if(arquivo_temp==arquivos_abertos.last){
                        arquivo_temp=arquivos_abertos.first;
                    }
                    else{
                        arquivo_temp=arquivo_temp->next;
                    }
                }
                if(arquivo_temp->next==arquivo){
                    if(arquivo->next!=NULL){
                        arquivo_temp->next=arquivo->next;
                        arquivo=NULL;
                    }
                    else {
                        arquivo=NULL;
                        arquivos_abertos.last=arquivo_temp;
                    }
                }
        }

                free(arquivo);
                numero_arquivos_abertos--;
                return 0;
    }
    else if(arquivo!=NULL&&arquivo->record.TypeVal == TYPEVAL_DIRETORIO){
        OPENED_FILE* arquivo_temp=diretorios_abertos.first;
        if(diretorios_abertos.first==diretorios_abertos.last){ // só tem um arquivo
            diretorios_abertos.first=NULL;
            diretorios_abertos.last=NULL;
            numero_diretorios_abertos=0;
            return 0;
        }
        if(arquivo==arquivo_temp){ // é o primeiro
                    arquivos_abertos.first=arquivo->next;
        }
        else{
            while (arquivo_temp->next!=arquivo) {
                 if(arquivo_temp==diretorios_abertos.last){
                        arquivo_temp=diretorios_abertos.first;
                    }
                    else{
                        arquivo_temp=arquivo_temp->next;
                    }
            }
            if(arquivo_temp->next==arquivo){
                    if(arquivo->next!=NULL){
                        arquivo_temp->next=arquivo->next;
                        arquivo=NULL;
                    }
                    else {
                        arquivo=NULL;
                        arquivos_abertos.last=arquivo_temp;
                    }
            }
        }
            free(arquivo);
            numero_arquivos_abertos--;
            return 0;
}
    return ERRO;
}

