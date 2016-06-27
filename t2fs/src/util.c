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
        if(le_superbloco(&superbloco)==0){
            *superbloco_lido=1;
            diretorio_corrente="/"; // raiz
            dir_corrente=2; // raiz
            inicio_bloco_dados = superbloco.superBlockSize+superbloco.bitmapSize+superbloco.rootSize;
            fim_bloco_dados=(superbloco.nOfSectors/superbloco.blockSize)-superbloco.blockSize; // �ltimo bloco do disco
            posicao_atual=inicio_bloco_dados*4+contador_posicao*4;
            TAM_MAX_ARQ=superbloco.blockSize*(pow(superbloco.blockSize/4,2)+(superbloco.blockSize/4)+2);
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

int le_setor (struct setor_corrente* setor,unsigned int sector){
    char content[SECTOR_SIZE];
    if(read_sector(sector,content)==0){
        memcpy(&setor->record0, content, sizeof(struct t2fs_record));
        memcpy(&setor->record1, content+64, sizeof(struct t2fs_record));
        memcpy(&setor->record2, content+128, sizeof(struct t2fs_record));
        memcpy(&setor->record3, content+192, sizeof(struct t2fs_record));
        return 0;
    }
    else return ERRO;
}

void imprime_superbloco (struct t2fs_superbloco superbloco){
    printf("id: %s\n",superbloco.id);
    printf("versao: %x\n",superbloco.version);
    printf("tamanho super bloco: %x\n", superbloco.superBlockSize); // 1 bloco
    printf("tamanho bloco: %x\n", superbloco.blockSize); //Quantidade de setores l�gicos que formam um bloco l�gico
    printf("tamanho disco: %d\n", superbloco.diskSize); // Quantidade de bytes da parti��o T2FS. Inclui o superbloco, a �rea de bitmap, o diret�rio raiz e a �rea de dados.
    printf("numero setores: %d\n", superbloco.nOfSectors); //Quantidade total de setores l�gicos na parti��o T2FS. Inclui o superbloco, a �rea de bitmap, o diret�rio raiz e a �rea de dados.
    printf("tamanho bitmap: %d\n", superbloco.bitmapSize); //Quantidade de blocos l�gicos usados para armazenar o bitmap de blocos livres e ocupados.
    printf("tamanho dir raiz : %d\n", superbloco.rootSize); //Quantidade de blocos l�gicos usados para armazenar o diret�rio raiz.
    printf("entradas no dir raiz: %d\n", superbloco.nOfDirEntries); //Quantidade de entradas no diret�rio raiz.
    printf("tamanho registro : %d\n", superbloco.fileEntrySize); //Quantidade de bytes de um registro na �rea de diret�rio.
}

void imprime_record (struct t2fs_record record){
    printf("tipo (valor): %u\n",(unsigned int)record.TypeVal); // Tipo da entrada. Indica se o registro � inv�lido (0x00), arquivo (0x01) ou diret�rio (0x02)
    printf("nome arquivo: %s\n",record.name);
    //printf("reservado: %u\n",(unsigned int)record.Reserved);
    printf("tamanho arquivo (blocos): %d\n",record.blocksFileSize); //Tamanho do arquivo, expresso em n�mero de blocos de dados
    printf("tamanho arquivo (bytes): %d\n",record.bytesFileSize); //Tamanho do arquivo. Expresso em n�mero de bytes.
    printf("ponteiros diretos: %d e %d\n",record.dataPtr[0], record.dataPtr[1]); //Dois ponteiros diretos (little endian).
    printf("ponteiro ind simples: %d\n",record.singleIndPtr); //Ponteiro de indire��o simples (little endian).
    printf("ponteiro ind dupla: %d\n",record.doubleIndPtr); //Ponteiro de indire��o dupla (little endian).
}

void imprime_setor (struct setor_corrente setor){
    imprime_record (setor.record0);
    imprime_record (setor.record1);
    imprime_record (setor.record2);
    imprime_record (setor.record3);
}

int eh_alfanumerico (char letra){
    if(letra=='.') // extens�o
        return 1;
    if(letra >= '0' && letra <= '9') // n�meros de 0 a 9
        return 1;
    else if (letra >='a' && letra <= 'z') // letras de (a-z) (min�sculas)
        return 1;
        else if (letra>='A' && letra <='Z') // letras de (A-Z) (mai�sculas)
            return 1;
            else return 0; // caso n�o alfanum�rico, retorna 0
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

void set_posicao(int bloco){
    posicao_atual=bloco*4;
}

int caminho_absoluto_relativo (char* caminho){
    if(caminho[0]=='/')
        return 1;
    else return 0;
}

int search_sector (struct setor_corrente* sector,char* Filename,unsigned int setor){
    le_setor(sector,setor);
    if(!strcmp((sector->record0.name),Filename)){
        return 0;
    }
    else if(!strcmp((sector->record1.name),Filename)){
        return 1;
    }
    else if(!strcmp((sector->record2.name),Filename)){
        return 2;
    }
    else if(!strcmp((sector->record3.name),Filename)){
        return 3;
    }
    else return ERRO;
}

int search_bloco (struct setor_corrente* sector,char* Filename,int bloco){
    int retorno;
    retorno = search_sector(sector,Filename,(bloco*4)+0);
    if(retorno!=ERRO){
        return retorno;
    }
    retorno=search_sector(sector,Filename,(bloco*4)+4);
    if(retorno!=ERRO){
        return retorno;
    }
    retorno=search_sector(sector,Filename,(bloco*4)+8);
    if(retorno!=ERRO){
        return retorno;
    }
    retorno=search_sector(sector,Filename,(bloco*4)+12);
    if(retorno!=ERRO){
        return retorno;
    }
    return ERRO;
}

int procura_continuidade_dir (struct t2fs_record record,char* Filename){
    int ponteiro_direto0=record.dataPtr[0];
    int ponteiro_direto1=record.dataPtr[1];
    int singleIndPtr=record.singleIndPtr;
    int doubleIndPtr=record.doubleIndPtr;
    int retorno;
    if((ponteiro_direto1==dir_corrente||ponteiro_direto1==0)&&(singleIndPtr==dir_corrente||singleIndPtr==0)
        &&(doubleIndPtr==dir_corrente||doubleIndPtr==0)){
                    return 0;
        }
    else{
        retorno=search_bloco(&setor,Filename,ponteiro_direto1); // procura no ponteiro direto 1
        if(retorno!=ERRO){ // achou
            return retorno;
        }
        else { // procura no single ind ptr
            retorno = procura_single_ind_ptr(Filename,singleIndPtr);
            if(retorno!=ERRO){ // achou
                return retorno;
            }
            else{ // procura no double ind ptr
                retorno = procura_double_ind_ptr(Filename,doubleIndPtr);
                if(retorno != ERRO){ // achou
                    return retorno;
                }
            }
        }
    }
    return 0; // n�o achou
}

int procura_single_ind_ptr(char* Filename,unsigned int singleIndPtr_setor){
    int retorno;
    char content[SECTOR_SIZE];
    if(read_sector(singleIndPtr_setor,content)==0){
        memcpy(&ponteiro_indireto_simples, content, sizeof(struct IndPtr));
        int i=0;
        for(i=0;i<64;i++){
            if(ponteiro_indireto_simples.bloco[i]!=0){
                retorno=search_bloco(&setor,Filename,ponteiro_indireto_simples.bloco[i]);
                if(retorno != ERRO)
                    return retorno;
            }
        }
    }
    return ERRO;

}

int procura_double_ind_ptr(char* Filename,unsigned int doubleIndPtr_setor){
    int retorno;
    char content[SECTOR_SIZE];
    if(read_sector(doubleIndPtr_setor,content)==0){
        memcpy(&ponteiro_indireto_duplo, content, sizeof(struct IndPtr));
        int i=0;
        for(i=0;i<64;i++){
            if(ponteiro_indireto_duplo.bloco[i]!=0){
                retorno=procura_single_ind_ptr(Filename,ponteiro_indireto_duplo.bloco[i]);
                if(retorno != ERRO)
                    return retorno;
            }
        }
    }
    return ERRO;
}

int procura_arquivo (char* Filename,int dir_corrente){
    int retorno;
    if(testa_nome(Filename)==0){ // se n�o "bate" o nome, retorna 0
        return ERRO;
    }
    set_posicao(dir_corrente);
    retorno=search_bloco(&setor,Filename,dir_corrente);
    if(retorno!=ERRO){
        return retorno;
    }
    else{ // se n�o encontrou no diret�rio, procura se o diret�rio � continuo (testa os ponteiros e navega, procurando pelo arquivo
        le_setor(&setor,dir_corrente*4);
        retorno=procura_continuidade_dir (setor.record0,Filename);
        if(retorno==0){
            return ERRO;
        }
        else {
            return retorno;
        }
    }
    return ERRO;
}
struct t2fs_record* inicializaRecord(char *name, int primeiroBloco)
{
    /*
    struct t2fs_record *record = (struct t2fs_record*) calloc(1, sizeof(struct t2fs_record));
    record->TypeVal = TYPEVAL_REGULAR;
    memset (record->name,'\0',25);
    strncpy(record->name, name,25);
    record->bytesFileSize = 0;
    record->dataPtr[0] = primeiroBloco;
    return record;
    */
    return 0;
}


//OPEN FILES

void printFile(OPENED_FILE p){
    //printf("handle: %d cursos: %d father sector: %d record position: %d\n", p.handle, p.cursor, p.father_sector, p.record_position);
}

OPENED_FILE* findOpenedFile(int handle)
{
    /*
    OPENED_FILE *file;

    if(opened_files->first == NULL) return NULL;
    file = opened_files->fidrst;
    while (file != NULL) {
        if (file>handle == handle) {
            return file;
        }
        file = file->next;
    }
*/
    return NULL;

}

int open(struct t2fs_record record, char* fatherName)//N�O SEI SE � ASSIM O PARAMETRO
{
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
    if(record.TypeVal == 0x01)//N�O SEI SE TA CERTO ESSE CMP
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
