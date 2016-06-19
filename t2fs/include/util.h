#ifndef __util_h__
#define __util_h__


// Testa se um caractere é alfanumérico ou não. Retorna 1 caso verdadeiro e 0 caso falso
int eh_alfanumerico (char letra);
// Testa se o nome está de acordo com a especificação (máximo 32 caracteres alfanuméricos. Retorna 1 caso estiver de acordo e 0 caso contrário
int testa_nome(char* Filename);
// Procura o arquivo no disco. Retorna 1 caso encontrar ou caso o nome esteja com problema e 0 caso contrário
int procura_arquivo (char* Filename,char* diretorio_corrente);
// le o superbloco para obter as informações
void le_superbloco(struct t2fs_superbloco *superbloco);
// imprime as informações do superbloco
void imprime_superbloco (struct t2fs_superbloco superbloco);
// le record
void le_record(struct t2fs_record* record,unsigned int sector);
// imprime dados de um record (arquivo)
void imprime_record (struct t2fs_record record);







#endif
