#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/openlib.h"
#include "../include/filelib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static struct opened_entries2 opened_files[0x10] = {0};
static struct opened_entries2 opened_dirs[0x10] = {0};

void printEntry(OPENED_ENTRY p){
    printf("handle: %d cursos: %d father sector: %d record position: %d\n", p.handle, p.cursor, p.father_sector, p.record_position);
}

int handle_hash(int handle) {
    return handle & 0x0f;
}

int writeRecord(int sector, int recordPos, struct t2fs_record record)
{
    struct t2fs_record records[8];
    if(read_sector(sector,(char*) records)) {
        return ERROR_SIGNAL;
    }
    records[recordPos] = record;
    if(write_sector(sector,(char*) records)) {
        return ERROR_SIGNAL;
    }
}

int open(int TypeVal, char *pathname)
{
    static int last_handle = 0;
    char aux[strlen(pathname)];
    struct t2fs_record record = {0};
    struct opened_entries2 *entries;
    OPENED_ENTRY *new_entry;
    int father_sector = -1;
    int record_position = -1;
    int handle;
    int hash;

    withoutRedundancy(pathname, aux);





    if(strcmp(pathname,"/") || strcmp(aux, "")){
        findFileAndFather(pathname, &record, &father_sector, &record_position);
    } else {
        record.TypeVal = TYPEVAL_DIRETORIO;
    }



    if (record.TypeVal != TypeVal) {
        return ERROR_SIGNAL;
    }
    new_entry = calloc(1, sizeof(OPENED_ENTRY));
    if (new_entry == NULL) {
        // Allocation failure, out of memory.
        return ERROR_SIGNAL;
    }

    handle = ++last_handle;
    new_entry->handle = handle;
    new_entry->record = record;
    new_entry->cursor = 0; // se for diretorio o cursor vai guardar a entrada a ser lida no readdir2
    new_entry->father_sector = father_sector;
    new_entry->record_position = record_position;

    hash = handle_hash(handle);
    switch (TypeVal) {
        case TYPEVAL_REGULAR:
            entries = &opened_files[hash];
            break;
        case TYPEVAL_DIRETORIO:
            entries = &opened_dirs[hash];
            break;
        default:
            // Shouldn't ever happen.
            free(new_entry);
            return ERROR_SIGNAL;
    }

    if (entries->last == NULL) {
        entries->first = new_entry;
        entries->last = new_entry;
    } else {
        entries->last->next = new_entry;
        entries->last = new_entry;
    }

    return handle;
}

int close(int TypeVal, int handle) {
    OPENED_ENTRIES *entries;
    OPENED_ENTRY *entry = NULL;
    OPENED_ENTRY *previous = NULL;
    int hash;

    hash = handle_hash(handle);
    switch (TypeVal) {
        case TYPEVAL_REGULAR:
            entries = &opened_files[hash];
            break;
        case TYPEVAL_DIRETORIO:
            entries = &opened_dirs[hash];
            break;
        default:
            // Shouldn't ever happen.
            return ERROR_SIGNAL;
    }

    entry = entries->first;
    while (entry != NULL) {
        if (entry->handle == handle) {
            if (previous) {
                previous->next = entry->next;
            }
            if (entries->first == entry) {
                entries->first = entry->next;
            }
            if (entries->last == entry) {
                entries->last = previous;
            }
            free(entry);
            return 0;
        }
        previous = entry;
        entry = entry->next;
    }

    return ERROR_SIGNAL;
}

OPENED_ENTRY* find_opened_entry(int TypeVal, int handle)
{
    OPENED_ENTRIES *entries;
    OPENED_ENTRY *entry;
    int hash;

    hash = handle_hash(handle);
    switch (TypeVal) {
        case TYPEVAL_REGULAR:
            entries = &opened_files[hash];
            break;
        case TYPEVAL_DIRETORIO:
            entries = &opened_dirs[hash];
            break;
        default:
            // Shouldn't ever happen. // We hope so.
            return NULL;
    }
    if(!entries) return NULL;
    entry = entries->first;
    while (entry != NULL) {
        if (entry->handle == handle) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}


// FILES

