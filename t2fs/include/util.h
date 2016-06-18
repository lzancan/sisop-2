#ifndef __util_h__
#define __util_h__

// Testa se um caractere é alfanumérico ou não. Retorna 1 caso verdadeiro e 0 caso falso
int eh_alfanumerico (char letra);
// Testa se o nome está de acordo com a especificação (máximo 32 caracteres alfanuméricos. Retorna 1 caso estiver de acordo e 0 caso contrário
int testa_nome(char* Filename);
// Procura o arquivo no disco. Retorna 1 caso encontrar e 0 caso contrário
int procura_arquivo (char* Filename);







#endif
