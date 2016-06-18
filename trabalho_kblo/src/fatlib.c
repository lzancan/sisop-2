#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int writeOnFAT(unsigned int number, unsigned short int content)
{
    unsigned short int FAT[SECTOR_SIZE / sizeof(unsigned short int)];
    int pFATSector = currentDisk.pFATSectorStart + (number-2)/NUM_FAT_PER_SECTOR;
    int sFATSector = currentDisk.sFATSectorStart + (number-2)/NUM_FAT_PER_SECTOR;

    if(pFATSector >= currentDisk.sFATSectorStart)
    {
        return -1;
    }

    if(read_sector(pFATSector, (char*) FAT))
    {
        return -1;
    }

    FAT[(number-2) % NUM_FAT_PER_SECTOR] = content;

    if(write_sector(pFATSector, (char *) FAT))
    {
        return -1;
    }

    if(write_sector(sFATSector, (char *) FAT))
    {
        // Duas opções, tentar de novo ou desfazer alteração FAT1
        return -1;
    }

    return 0;
}

int selectClusterOnFAT()
{
    int i = 0, j = 0;
    char data[SECTOR_SIZE];
    unsigned short int pFAT[ SECTOR_SIZE / sizeof(unsigned short int)];
    j = currentDisk.pFATSectorStart ;

    while(j < currentDisk.sFATSectorStart)
    {

        if(read_sector(j++, data))
        {
            return -1;
        }
        memcpy(pFAT, data, SECTOR_SIZE);



        i = 0;

        while(i < NUM_FAT_PER_SECTOR)
        {
            if(pFAT[i] == 0)
            {
                return i + 2; // + 2 pq, como os blocos 0 e 1 são inválidos, o primeiro de pFAT é referente ao bloco 2
            }
            else i++;
        }
    }
    return -1;
}

int readOnFAT(unsigned int number)
{
    int  j = 0;
    unsigned short int pFAT[SECTOR_SIZE / sizeof(unsigned short int)];
    j = currentDisk.pFATSectorStart + (number-2)/NUM_FAT_PER_SECTOR;

    if(j >= currentDisk.sFATSectorStart)
    {
        return ERROR_SIGNAL;
    }

    if(read_sector(j, (char*) pFAT))
    {
        return ERROR_SIGNAL;
    }

    return pFAT[(number-2) % NUM_FAT_PER_SECTOR];
}

void printFATS()
{
    unsigned short int  buffer[ SECTOR_SIZE / sizeof(unsigned short int) ];
    int i, j;
    printf("---------------------- Primary FAT ----------------------\n\n") ;

    for(i = currentDisk.pFATSectorStart; i < currentDisk.sFATSectorStart; i++)
    {
        read_sector(i , (char*) buffer);
        for(j = 0; j < SECTOR_SIZE / sizeof(unsigned short int); j++)
        {
            printf("%x ", buffer[j]);
        }
    }
    printf("\n\n---------------------- Secundary FAT ----------------------\n\n") ;

    for(i = currentDisk.sFATSectorStart; i < currentDisk.RootSectorStart; i++)
    {
        read_sector(i , (char*) buffer);
        for(j = 0; j < SECTOR_SIZE / sizeof(unsigned short int) ; j++)
        {
            printf("%x ", buffer[j]);
        }
    }
}
