#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/util.h"

static struct opened_files2 opened_files;
static struct opened_files2 opened_dirs;

void inicializa (int* superbloco_lido){
    if(*superbloco_lido==0){
        if(le_superbloco(&superbloco)==0){
            *superbloco_lido=1;
            diretorio_corrente="."; // raiz
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
    if(letra >= 0 && letra <= 9) // números de 0 a 9
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

    if(!strcmp(diretorio_corrente,".")){
        // todo...
    }

    return 0;

}
struct t2fs_record* inicializaRecord(char *name, int primeiroBloco)
{
    struct t2fs_record *record = (struct t2fs_record*) calloc(1, sizeof(struct t2fs_record));
    record->TypeVal = TYPEVAL_REGULAR;
    memset (record->name,'\0',25);
    strncpy(record->name, name,25);
    record->bytesFileSize = 0;
    record->dataPtr[0] = primeiroBloco;
    return record;
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

int open(struct t2fs_record record, char* fatherName)//NÂO SEI SE É ASSIM O PARAMETRO
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
}
