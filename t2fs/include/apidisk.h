#ifndef __apidisk_h__
#define __apidisk_h__
#define SECTOR_SIZE 256

/**
Realiza a leitura do setor "sector" lógico do disco e coloca os bytes lidos no espaço de memória indicado pelo
ponteiro"buffer".
Retorna "0", se a leitura foi realizada corretamente e um valor diferente de zero, caso tenha ocorrido algum erro.
*/
int read_sector (unsigned int sector, char *buffer);

/**
Realiza a escrita do conteúdo da memória indicada pelo ponteiro "buffer" no setor "sector" lógico do disco.
Retorna "0", se a escrita foi bem sucedida; retorna um valor diferente de zero, caso tenha ocorrido algum erro.
*/
int write_sector (unsigned int sector, char *buffer);
#endif
