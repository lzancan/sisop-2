typedef struct opened_entry2 {
    int handle;
    struct t2fs_record record;
    int cursor;
    int father_sector;
    int record_position;
    struct opened_entry2 *next;
} OPENED_ENTRY;

typedef struct opened_entries2 {
    OPENED_ENTRY *first;
    OPENED_ENTRY *last;
} OPENED_ENTRIES;

int handle_hash(int handle);
int open(int TypeVal, char *pathname);
int close(int TypeVal, int handle);
OPENED_ENTRY* find_opened_entry(int TypeVal, int handle);
FILE2 open2 (char *filename);
int close2 (FILE2 handle);

int writeRecord(int sector, int recordPos, struct t2fs_record record);
void printEntry(OPENED_ENTRY p);
