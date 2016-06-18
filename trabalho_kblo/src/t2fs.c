#include <stdio.h>
#include <stdlib.h>
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/fatlib.h"
#include "../include/filelib.h"
#include "../include/openlib.h"
#include <string.h>

#define init() if (initSuperbloco == 0) { if (readSuperbloco() == 0) initSuperbloco = -1; else return -1; }

struct t2fs_superbloco currentDisk;
int isAbsolute(char *pathname);
void turnToAbsolute(char *relative, char *absolute);
int readSuperbloco();
void printSuperbloco();
void printRecord(struct t2fs_record record);
void setCurrentDirRecord(struct t2fs_record record);
struct t2fs_record* initializeFileRecord(char *name, int firstCluster);
struct t2fs_record* initializeDirRecord(char *name, int firstCluster);
int selectRecordOnDirectory(struct t2fs_record *records);
int verifyRecordOnDirectory(struct t2fs_record *records);
int freeRecord(struct t2fs_record record);
int isDotOrDotDot(struct t2fs_record record);
int findRecordByName(struct t2fs_record *records, int len, char *name);
int isValidRecord(BYTE validByte);
int findFile(char *pathname, struct t2fs_record *recordRequired);
int findFileAndFather(char *pathname, struct t2fs_record *recordRequired, int* sectorFather, int* recordPosition);
void equal(struct t2fs_record *a, struct t2fs_record b);
int alocCluster(int cluster);  // last cluster of file
int findLastCluster(int cluster);
int readCluster(int firstSector, char *buffer);
int writeCluster(int firstSector, char *buffer);
int findNextSector(int sector);
int isValidCluster(int cluster);
int isNextSectorRoot(int sector);
int isLastSectorOfRoot(int sector);
int isLastSectorOfCluster(int sector);
int clusterOfDataSector(int sector);
unsigned short int findNextCluster(int cluster);
int firstSectorOfCluster(int cluster);
void printDirEnt(DIRENT2 dirEnt);

static int initSuperbloco = 0;

int identify2 (char *name, int size)
{
    if(size < 81) return -1;
    else
    {
        strcpy(name, "Alexandre Leuck - 220493, Gustavo Dambros - 240490 e Mariane Giambastini - 230166");
        return 0;
    }
}

int isAbsolute(char *pathname)
{
    if(pathname && pathname[0] == '/') return 1;
    else return 0;
}

void turnToAbsolute(char *relative, char *absolute)
{
    char *aux = malloc(strlen(relative) + strlen(currentDirectory));
    if(!isAbsolute(relative))
    {
        concat(aux, currentDirectory, relative);
    }
    else
    {
        strcpy(aux, relative);
    }

    withoutRedundancy(aux, absolute);

    return ;
}

int readSuperbloco()
{
    char data[SECTOR_SIZE];
    read_sector (0, data);
    memcpy(&currentDisk, data, sizeof(struct t2fs_superbloco));


    NUM_REC_PER_CLUSTER = NUM_REC_PER_SECTOR*currentDisk.SectorPerCluster;

    CLUSTER_SIZE = SECTOR_SIZE*currentDisk.SectorPerCluster;

    OFFSET_DATA = (currentDisk.DataSectorStart + 1) % currentDisk.SectorPerCluster;

    currentDirectory = (char*) calloc(500,sizeof(char));
    if (currentDirectory == NULL) {
        return ERROR_SIGNAL;
    }
    strcpy(currentDirectory,"/");
    currentDirRecord = NULL;
    return 0;
}

void printSuperbloco()
{
    printf("ID: %s",currentDisk.Id);
    printf("\nVersion: %x",currentDisk.Version);
    printf("\nSuper Cluster Size: %x", currentDisk.SuperBlockSize);
    printf("\nDisk Size: %d", currentDisk.DiskSize);
    printf("\nNumber of Sectors: %d", currentDisk.NofSectors);
    printf("\nSectors per Cluster: %d\n", currentDisk.SectorPerCluster);
}

void printRecord(struct t2fs_record record)
{
    printf("Type: %u\n",(unsigned int)record.TypeVal);
    printf("Name: %s\n",record.name);
    printf("Size: %u\n",record.bytesFileSize);
    printf("First: %x\n\n",record.firstCluster);
}

void printListOfRecords(struct t2fs_record *record, int len)
{
    int i;
    printf("..................................................\n");
    for(i = 0; i < len; i++)
    {
        printRecord(record[i]);
    }
}

void setCurrentDirRecord(struct t2fs_record record)
{

    if(record.TypeVal == TYPEVAL_INVALIDO)
    {
        free(currentDirRecord);
        currentDirRecord = NULL;
        return ;
    }

    else if(!currentDirRecord) currentDirRecord = malloc(sizeof(struct t2fs_record));

    currentDirRecord->bytesFileSize = record.bytesFileSize;
    currentDirRecord->firstCluster = record.firstCluster;
    strcpy(currentDirRecord->name, record.name);
    currentDirRecord->TypeVal = record.TypeVal;


    return ;
}

struct t2fs_record* initializeFileRecord(char *name, int firstCluster)
{
    struct t2fs_record *record = (struct t2fs_record*) calloc(1, sizeof(struct t2fs_record));
    record->TypeVal = TYPEVAL_REGULAR;
    memset (record->name,'\0',25);
    strncpy(record->name, name,25);
    record->bytesFileSize = 0;
    record->firstCluster = firstCluster;
    return record;
}

struct t2fs_record* initializeDirRecord(char *name, int firstCluster)
{
    struct t2fs_record *record = (struct t2fs_record*) calloc(1, sizeof(struct t2fs_record));
    record->TypeVal = TYPEVAL_DIRETORIO;
    memset (record->name,'\0',25);
    strncpy(record->name, name,25);
    record->bytesFileSize = 0;
    record->firstCluster = firstCluster;
    return record;
}

int selectRecordOnDirectory(struct t2fs_record *records)
{
    int i = 0;
    while(i < NUM_REC_PER_SECTOR && !freeRecord( records[i] ) )
    {
        i++;
    }
    if(i == NUM_REC_PER_SECTOR) return ERROR_SIGNAL;
    else return i;
}

int verifyRecordOnDirectory(struct t2fs_record *records)
{
    int i = 0;
    while(i < NUM_REC_PER_SECTOR)
    {
        if(freeRecord( records[i]) || isDotOrDotDot(records[i]))
            i++;
        else
            return ERROR_SIGNAL;
    }
    if(i == NUM_REC_PER_SECTOR) return 0;
    else return ERROR_SIGNAL;
}

int freeRecord(struct t2fs_record record)
{
    if((unsigned int)record.TypeVal != 1 && (unsigned int)record.TypeVal != 2) return 1;
    else return 0;
}

int isDotOrDotDot(struct t2fs_record record)
{
    if(!strcmp(record.name,"." )|| !strcmp(record.name, "..")) return 1;
    else return 0;
}

int findRecordByName(struct t2fs_record *records, int len, char *name)
{
    int i;
    for(i = 0; i < len; i++)
    {
        if(isValidRecord(records[i].TypeVal) && !strcmp(records[i].name,name))
        {
            return i;
        }
    }
    return -1;
}

int isValidRecord(BYTE validByte)
{
    if(validByte == TYPEVAL_REGULAR || validByte == TYPEVAL_DIRETORIO)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int findFile(char *pathname, struct t2fs_record *recordRequired)
{
    int sectorFather, recordPosition;
    return findFileAndFather(pathname,recordRequired, &sectorFather,&recordPosition);
}

int findFileAndFather(char *pathname, struct t2fs_record *recordRequired, int* sectorFather, int* recordPosition)
{

    int i, currentSector;

    struct t2fs_record records[NUM_REC_PER_SECTOR];

    if(isAbsolute(pathname) || !currentDirRecord)
    {
        currentSector = currentDisk.RootSectorStart;
    }
    else
    {
        currentSector = firstSectorOfCluster(currentDirRecord->firstCluster);
    }


    if(!pathname || !strcmp(pathname, ""))
    {
        if(currentDirRecord)
        {
            equal(recordRequired, *currentDirRecord);
        }
        return 0;
    }

    else if(!strcmp(pathname,"/"))
    {

        return 0;
    }

    char fileName[50];
    findNameOfLastFile(pathname, fileName);

    char innerDir[50];

    char remnant[ strlen(pathname) ];

    strcpy(remnant, pathname);

    do
    {



        findNameOfFirstFile(remnant, innerDir);
        findRemnant(remnant, remnant);


        do
        {
            if(read_sector(currentSector,(char*) records))
            {
                return ERROR_SIGNAL;
            }


            i = findRecordByName(records, NUM_REC_PER_CLUSTER, innerDir);

            if(i == ERROR_SIGNAL)
            {
                currentSector = findNextSector(currentSector);


                if(currentSector == ERROR_SIGNAL)
                {
                    return ERROR_SIGNAL;
                }
            }
            else
            {
                *sectorFather = currentSector;
                *recordPosition = i;
                break;
            }
        }
        while(1);

        if(records[i].TypeVal == 0x02 && records[i].firstCluster == 0)  // quero voltar para o root
        {
            currentSector = currentDisk.RootSectorStart;
        }
        else
        {
            currentSector = records[i].firstCluster * currentDisk.SectorPerCluster - 1;
        }


    }
    while(remnant && strcmp(remnant,"") && records[i].TypeVal == 0x02);

    if(remnant && strcmp(remnant,"") && records[i].TypeVal != 0x02)
    {
        return ERROR_SIGNAL;
    }

    equal(recordRequired, records[i]);


    return 0;
}

void equal(struct t2fs_record *a, struct t2fs_record b)
{
    a->TypeVal = b.TypeVal;
    strcpy(a->name,b.name);
    a->bytesFileSize = b.bytesFileSize;
    a->firstCluster = b.firstCluster;

}

int alocCluster(int cluster)  // last cluster of file
{
    int next = selectClusterOnFAT();
    if(next == ERROR_SIGNAL){
        return ERROR_SIGNAL;
    }
    if( writeOnFAT(cluster, next) == ERROR_SIGNAL){
        return ERROR_SIGNAL;
    }
    if( writeOnFAT(next, 0x0FFFF) == ERROR_SIGNAL){
        return ERROR_SIGNAL;
    }
    return next;
}

int findLastCluster(int cluster)
{
    int aux = readOnFAT(cluster);

    if(aux == 0x0FFFF)
    {
        return cluster;
    }
    else if(isValidCluster(aux))
    {
        int aux2 = cluster;
        while(aux != 0x0FFFF)
        {
            aux2 = aux;
            aux = readOnFAT(aux);
        }
    return aux2;
    }
}

int readCluster(int firstSector, char *buffer)
{
    int i;
    for(i = 0; i < currentDisk.SectorPerCluster; i++)
    {
        if(read_sector(firstSector + i, (buffer + i*SECTOR_SIZE)))
        {
            return ERROR_SIGNAL;
        }
    }
    return 0;
}

int writeCluster(int firstSector, char *buffer)
{
    int i;
    for(i = 0; i < currentDisk.SectorPerCluster; i++)
    {
        if(write_sector(firstSector + i, (buffer + i*SECTOR_SIZE)))
        {
            return ERROR_SIGNAL;
        }
    }
    return 0;
}

int findNextSector(int sector)
{
    if( isLastSectorOfRoot(sector) )  // ultimo do diretorio raiz, logo n\E3o tem o que fazer
    {
        return ERROR_SIGNAL;
    }
    else if( isNextSectorRoot(sector) )  // ta no diretorio raiz mas n\E3o \E9 o ultimo
    {
        return sector+1;
    }
    else
    {
        if( !isLastSectorOfCluster(sector) )   // n\E3o estou no ultimo setor do cluster
        {
            return sector+1;
        }
        else
        {
            int cluster = findNextCluster( clusterOfDataSector(sector) );
            if( !isValidCluster(cluster) )
            {
                return ERROR_SIGNAL;
            }
            else
            {
                return firstSectorOfCluster(cluster);
            }
        }
    }
}

int isValidCluster(int cluster)
{
    return cluster >= 0x0002 && cluster <= 0x0FFFD ;
}

int isNextSectorRoot(int sector)
{
    return sector >= currentDisk.RootSectorStart && sector < currentDisk.DataSectorStart - 1;
}

int isLastSectorOfRoot(int sector)
{
    return sector == currentDisk.DataSectorStart - 1;
}

int isLastSectorOfCluster(int sector)
{
    return sector - firstSectorOfCluster( clusterOfDataSector(sector) ) == currentDisk.SectorPerCluster - 1;
}

int clusterOfDataSector(int sector)
{
    if(sector > currentDisk.NofSectors) return ERROR_SIGNAL;
    return 2 + (sector - currentDisk.DataSectorStart)/currentDisk.SectorPerCluster;
}

unsigned short int findNextCluster(int cluster)
{
    return readOnFAT(cluster);
}

int firstSectorOfCluster(int cluster)
{
    if(cluster < 2) return ERROR_SIGNAL;
    return currentDisk.DataSectorStart + (cluster - 2)*currentDisk.SectorPerCluster;
}

int getcwd2 (char *pathname, int size)
{
    init();
    if(strlen(currentDirectory) < size)
    {
        strcpy(pathname, currentDirectory);
        return 0;
    }
    else return ERROR_SIGNAL;
}

int chdir2 (char *pathname)
{
    init();
    struct t2fs_record recordOfDir = {0};

    if (findFile(pathname, &recordOfDir) == ERROR_SIGNAL) {
        return ERROR_SIGNAL;
    } else {
        turnToAbsolute(pathname, currentDirectory);

        if (currentDirectory[ strlen(currentDirectory) -1 ] != '/') {
            strcat(currentDirectory, "/");
        }
        setCurrentDirRecord(recordOfDir);

        return 0;
    }
}

int mkdir2 (char *pathname)
{
    init();
    int readSector;
    int recordPosition;
    int destinyIsNotRoot = 0;
    struct t2fs_record records[NUM_REC_PER_CLUSTER];
    struct t2fs_record recordOfDir = {0};

    if(checkPathname(pathname) == ERROR_SIGNAL)
    {
        return ERROR_SIGNAL;
    }

    // checa se o diretorio j\E1 existe
    if(findFile(pathname, &recordOfDir) != ERROR_SIGNAL)
    {
        return ERROR_SIGNAL;
    }

    char aux[26];

    withoutLastName(pathname,aux);

    if(findFile(aux, &recordOfDir) && strcmp(aux, ""))  //acha o pai
    {
        return ERROR_SIGNAL;
    }

    if(recordOfDir.TypeVal == TYPEVAL_DIRETORIO)  // quero adicionar em um diretorio n\E3o raiz
    {
        readSector = firstSectorOfCluster(recordOfDir.firstCluster);
        destinyIsNotRoot = 1;
    } else if(recordOfDir.TypeVal == TYPEVAL_INVALIDO) {
        // quero adicionar na raiz
        readSector = currentDisk.RootSectorStart;
    } else {
        return ERROR_SIGNAL;
    }

    do
    {
        if(read_sector(readSector,(char*) records))
        {
            return ERROR_SIGNAL;
        }

        recordPosition = selectRecordOnDirectory(records);

        if(recordPosition == ERROR_SIGNAL)
        {

            readSector = findNextSector(readSector);

            if(readSector == ERROR_SIGNAL)
            {

                if(destinyIsNotRoot)
                {
                    readSector = firstSectorOfCluster( alocCluster(findLastCluster(recordOfDir.firstCluster)) );
                }
                else return ERROR_SIGNAL;
            }
        }
        else break;
    }
    while(1);

    WORD firstCluster = selectClusterOnFAT();

    if(firstCluster == -1)
    {
        return ERROR_SIGNAL;
    }

    char name[26];
    if( findNameOfLastFile(pathname, name) == ERROR_SIGNAL)
    {
        return ERROR_SIGNAL;
    }

    struct t2fs_record *new_record = initializeDirRecord(name, firstCluster);

    records[recordPosition] = *new_record;

    write_sector(readSector, (char*)records);

    struct t2fs_record *new_records = (struct t2fs_record*) calloc(NUM_REC_PER_CLUSTER, sizeof(struct t2fs_record));

    new_records[0].bytesFileSize = 0;
    new_records[0].TypeVal = 0x02;
    strcpy(new_records[0].name, ".");
    new_records[0].firstCluster = firstCluster;

    new_records[1].bytesFileSize = 0;
    new_records[1].TypeVal = 0x02;
    strcpy(new_records[1].name, "..");
    if(recordOfDir.TypeVal == TYPEVAL_INVALIDO) // quero adicionar o diretorio no diretorio raiz
        new_records[1].firstCluster = 0x00;
    else
        new_records[1].firstCluster = recordOfDir.firstCluster;

    writeCluster( firstCluster*currentDisk.SectorPerCluster - 1, (char* )new_records);

    writeOnFAT(firstCluster, 0x0FFFF);

    return 0;
}

FILE2 create2 (char *filename)
{
    init();
    int readSector;
    int recordPosition;
    int destinyIsNotRoot = 0;
    struct t2fs_record records[NUM_REC_PER_CLUSTER];
    struct t2fs_record recordOfDir = {0};



    if(!checkFilename(filename) || strlen(filename) >= 26)
    {
        return ERROR_SIGNAL;
    }

    //verificar se o nome \E9 valido

    if(findFile(filename, &recordOfDir) != ERROR_SIGNAL)// checa se o arquivo j\E1 existe
    {
        return ERROR_SIGNAL;
    }

    //O diret\F3rio pai, do novo arquivo, \E9 o diret\F3rio corrente.

    if(findFile(currentDirectory, &recordOfDir) && strcmp(currentDirectory, ""))  //acha o pai
    {
        return ERROR_SIGNAL;
    }



    //Vai ser adicionado no diretorio corrente um arquivo.

    if(recordOfDir.TypeVal == TYPEVAL_DIRETORIO)  // quero adicionar em um diretorio n\E3o raiz
    {
        readSector = firstSectorOfCluster(recordOfDir.firstCluster);
        destinyIsNotRoot = 1;
    }
    else if(recordOfDir.TypeVal == TYPEVAL_INVALIDO) // quero adicionar na raiz
    {
        readSector = currentDisk.RootSectorStart;
    }
    else
    {
        return ERROR_SIGNAL;
    }

    do
    {
        if(read_sector(readSector,(char*) records))
        {
            return ERROR_SIGNAL;
        }

        recordPosition = selectRecordOnDirectory(records);

        if(recordPosition == ERROR_SIGNAL)
        {

            readSector = findNextSector(readSector);

            if(readSector == ERROR_SIGNAL)
            {

                if(destinyIsNotRoot)
                {
                    readSector = firstSectorOfCluster( alocCluster(findLastCluster(recordOfDir.firstCluster)) );
                }
                else return ERROR_SIGNAL;
            }
        }
        else break;
    }
    while(1);


    WORD firstCluster = selectClusterOnFAT();

    if(firstCluster == -1)
    {
        return ERROR_SIGNAL;
    }

    struct t2fs_record *new_record = initializeFileRecord(filename, firstCluster);

    records[recordPosition] = *new_record;

    write_sector(readSector, (char*)records); //Grava o arquivo no record do diret\F3rio.

    writeOnFAT(firstCluster, 0x0FFFF);

    int handle_returned;

    handle_returned = open2(filename);

    return handle_returned;
}

int rmdir2 (char *pathname)
{


    init();
    int readSector;
    int recordPosition;
    int fatherSector, recordPositionInFather;
    int dirIsEmpty = 0;
    struct t2fs_record records[NUM_REC_PER_CLUSTER];
    struct t2fs_record recordOfDir = {0};


    //turnToAbsolute(pathname, pathname);

    if(findFileAndFather(pathname, &recordOfDir, &fatherSector, &recordPositionInFather) == 0)
    {

        if(recordOfDir.TypeVal == TYPEVAL_DIRETORIO)  // quero remover um diretorio n\E3o raiz
        {
            readSector = firstSectorOfCluster(recordOfDir.firstCluster);
        }
        else  // quero remover diretorio raiz
        {
            return ERROR_SIGNAL;
        }

        do
        {
            if(read_sector(readSector,(char*) records))
            {
                return ERROR_SIGNAL;
            }

            recordPosition = verifyRecordOnDirectory(records);

            if(recordPosition == 0)
            {

                readSector = findNextSector(readSector);

                if(readSector == ERROR_SIGNAL)
                {
                    dirIsEmpty = 1;
                    break;
                }
            }
            else break;
        }
        while(1);

        if(dirIsEmpty)// diretorio vai ser removido
        {
            // remover record do diretorio no pai

            struct t2fs_record clear_record;
            clear_record.TypeVal = TYPEVAL_INVALIDO;
            strcpy(clear_record.name, "\0");
            clear_record.bytesFileSize = 0;
            clear_record.firstCluster = 0;

            if(read_sector(fatherSector,(char*) records))
            {
                return ERROR_SIGNAL;
            }
            records[recordPositionInFather] = clear_record;

            write_sector(fatherSector, (char*)records);

            // remover do FAT

            struct t2fs_record recordAux;
            do
            {
                recordAux.firstCluster = readOnFAT(recordOfDir.firstCluster);
                writeOnFAT(recordOfDir.firstCluster, 0x00);
                recordOfDir.firstCluster = recordAux.firstCluster;

            }
            while(recordAux.firstCluster != 0xFFFF);

            return 0;
        }
        else
        {
            return ERROR_SIGNAL;
        }
    }
    else
    {
        return ERROR_SIGNAL; //n\E3o pode remover um diretorio que n\E3o existe
    }
}

int delete2 (char *filename)
{
    init();


    int readSector;
    int fatherSector, recordPositionInFather;
    struct t2fs_record records[NUM_REC_PER_CLUSTER];
    struct t2fs_record recordOfDir = {0};


    char *pathfile = malloc(strlen(filename)+strlen(currentDirectory) + 1);

    char *realpathfile = malloc(strlen(filename)+strlen(currentDirectory) + 1);

    concat(pathfile,"",filename);


    concat(realpathfile, currentDirectory, pathfile);

    if(findFileAndFather(realpathfile, &recordOfDir, &fatherSector, &recordPositionInFather) == 0)
    {
        if(recordOfDir.TypeVal == TYPEVAL_REGULAR)  // quero remover um arquivo n\E3o da raiz
        {
            readSector = firstSectorOfCluster(recordOfDir.firstCluster);
        }
        else  // quero remover um arquivo raiz
        {
           readSector = currentDisk.RootSectorStart;
        }

        // remover record(arquivo) do diretorio no pai

        struct t2fs_record clear_record;
        clear_record.TypeVal = TYPEVAL_INVALIDO;
        strcpy(clear_record.name, "\0");
        clear_record.bytesFileSize = 0;
        clear_record.firstCluster = 0;

        if(read_sector(fatherSector,(char*) records))
        {
            return ERROR_SIGNAL;
        }
        records[recordPositionInFather] = clear_record;


        write_sector(fatherSector, (char*)records);

        // remover do FAT
        struct t2fs_record recordAux;
        do
        {
            recordAux.firstCluster = readOnFAT(recordOfDir.firstCluster);
            writeOnFAT(recordOfDir.firstCluster, 0x00);
            recordOfDir.firstCluster = recordAux.firstCluster;

        }
        while(recordAux.firstCluster != 0xFFFF);

        return 0;
    }
    else
    {
        return ERROR_SIGNAL; //n\E3o pode remover um arquivo que n\E3o existe
    }
}

FILE2 open2 (char *filename)
{
    init();
    return open(TYPEVAL_REGULAR, filename);
}

int close2 (FILE2 handle)
{
    return close(TYPEVAL_REGULAR, handle);
}

int read2 (FILE2 handle, char *buffer, int size)
{
    // TODO
    OPENED_ENTRY *entry = find_opened_entry(TYPEVAL_REGULAR, handle);
    if (entry == NULL) {
        return ERROR_SIGNAL;
    }

    int file_relative_sector = entry->cursor / SECTOR_SIZE;
    int first_byte = entry->cursor % SECTOR_SIZE;
    int file_size = entry->record.bytesFileSize;
    char sector[SECTOR_SIZE];
    int readSector = firstSectorOfCluster( entry->record.firstCluster );
    int i;
    int size_read;

    if (entry->cursor + size > file_size) {
        size = file_size - entry->cursor;
    }

    for (i = 0; i < file_relative_sector; i++) {
        readSector = findNextSector(readSector);
    }

    i = 0; // i is now number of bytes read so far.

    while (i < size && readSector != ERROR_SIGNAL) {
        read_sector(readSector, (char*) sector);

        if (size + first_byte - i > SECTOR_SIZE) {
            size_read = SECTOR_SIZE - first_byte;
        } else {
            size_read = size - i;
        }

        strncpy(&buffer[i], &sector[first_byte], size_read);
        i += size_read;
        first_byte = 0;
        readSector = findNextSector(readSector);
    }
    return i;
}

int write2 (FILE2 handle, char *buffer, int size)
{
    // TODO
    OPENED_ENTRY *entry = find_opened_entry(TYPEVAL_REGULAR, handle);
    if (entry == NULL) {
        return ERROR_SIGNAL;
    }
    int file_relative_sector = entry->cursor / SECTOR_SIZE;
    int first_byte = entry->cursor % SECTOR_SIZE;
    int file_size = entry->record.bytesFileSize;
    char sector[SECTOR_SIZE];
    int current_cluster = entry->record.firstCluster;
    int writeSector = firstSectorOfCluster(current_cluster);
    int i;
    int size_write;
    for (i = 0; i < file_relative_sector; i++) {
        writeSector = findNextSector(writeSector);
        current_cluster = clusterOfDataSector(writeSector);
    }

    i = 0; // i is now number of bytes written so far.
    while (i < size) {
        if (writeSector == ERROR_SIGNAL) {
            // try to allocate a new cluster
            current_cluster = alocCluster(current_cluster);
            if (current_cluster == ERROR_SIGNAL) {
                // couldn't allocate: abort.
                return i;
            }
            // allocation sucessfull, get the fine sector.
            writeSector = firstSectorOfCluster(current_cluster);
        }
        if (size + first_byte - i > SECTOR_SIZE) {
            // read all from current sector
            size_write = SECTOR_SIZE - first_byte;
        } else {
            // read only remaining data
            size_write = size - i;
        }
        memset(&sector, 0x00, SECTOR_SIZE);
        strncpy(&sector[first_byte], &buffer[i], size_write);
        write_sector(writeSector, &buffer[i]);

        i += size_write;
        current_cluster = clusterOfDataSector(writeSector);
        writeSector = findNextSector(writeSector);
        first_byte = 0;
    }
    // update cursor
    if (entry->cursor + i > file_size) {
        file_size = entry->cursor + i;
        entry->record.bytesFileSize = entry->cursor + i;
    }
    writeRecord(entry->father_sector, entry->record_position, entry->record);
    entry->cursor = entry->cursor + i;
    return i;
}


int seek2 (FILE2 handle, unsigned int offset)
{
    OPENED_ENTRY* aux = find_opened_entry(TYPEVAL_REGULAR, handle);
    if(!aux) return -1;
    if(aux->record.bytesFileSize <= offset) aux->cursor = offset;
    else if (offset == -1) aux->cursor = aux->record.bytesFileSize;
    else return ERROR_SIGNAL;
    return 0;
}


// DIRS

DIR2 opendir2 (char *pathname)
{
    init();
    return open(TYPEVAL_DIRETORIO, pathname);
}

int closedir2 (DIR2 handle)
{
    return close(TYPEVAL_DIRETORIO, handle);
}

int readdir2 (DIR2 handle, DIRENT2 *dentry)
{
    OPENED_ENTRY* aux = find_opened_entry(TYPEVAL_DIRETORIO, handle);
    if(!aux || aux->handle == 0) return -1;
    struct t2fs_record records[NUM_REC_PER_SECTOR];
    int i, count = aux->cursor;
    int readSector;
    if(aux->father_sector == -1){
        readSector = currentDisk.RootSectorStart;
    }
    else{
        readSector = firstSectorOfCluster( aux->record.firstCluster );
    }
    do
    {
        if(read_sector(readSector,(char*) records))
        {
            return ERROR_SIGNAL;
        }
        for(i = 0; i < NUM_REC_PER_SECTOR; i++)
        {
            if(isValidRecord(records[i].TypeVal))
            {
                if( count > 0 )
                {
                    count--;
                }
                else
                {
                    strcpy(dentry->name, records[i].name);
                    dentry->fileSize = records[i].bytesFileSize;
                    if(records[i].TypeVal == TYPEVAL_REGULAR)
                    {
                        dentry->fileType = 0;
                    }
                    else
                    {
                        dentry->fileType = 1;
                    }
                    aux->cursor++;
                    return 0;
                }
            }
        }
        readSector = findNextSector(readSector);
        if(readSector == ERROR_SIGNAL)
        {
            return ERROR_SIGNAL;
        }
    }
    while(1);
    return ERROR_SIGNAL;
}

void printDirEnt(DIRENT2 dirEnt)
{
    printf("Name: %s\nTipo: %d\nTamanho: %d\n", dirEnt.name, dirEnt.fileType, dirEnt.fileSize);
    return ;
}
