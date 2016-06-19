#include "../include/main.h"

int main(){
    char* diretorio_corrente;
    struct t2fs_superbloco superbloco;
    struct t2fs_record record;
    le_superbloco(&superbloco);
    imprime_superbloco(superbloco);
    le_record(&record,8);
    imprime_record(record);


    return 0;
}
