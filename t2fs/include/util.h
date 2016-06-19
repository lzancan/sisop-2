#ifndef __util_h__
#define __util_h__

static int superbloco_lido =0;
char* diretorio_corrente;
struct t2fs_superbloco superbloco;
struct t2fs_record record;
int inicio_bloco_dados;
int fim_bloco_dados; // e fim do disco também
static int contador_posicao=0;
int posicao_atual;

// inicializa (le o disco caso ainda não tiver sido lido) e seta diretório corrente para o raiz
void inicializa (int* superbloco_lido);
// Testa se um caractere é alfanumérico ou não. Retorna 1 caso verdadeiro e 0 caso falso
int eh_alfanumerico (char letra);
// Testa se o nome está de acordo com a especificação (máximo 32 caracteres alfanuméricos. Retorna 1 caso estiver de acordo e 0 caso contrário
int testa_nome(char* Filename);
// Procura o arquivo no disco. Retorna 1 caso encontrar ou caso o nome esteja com problema e 0 caso contrário
int procura_arquivo (char* Filename,char* diretorio_corrente);
// le o superbloco para obter as informações. Retorna 0 caso conseguiu e -1 caso negativo
int le_superbloco(struct t2fs_superbloco *superbloco);
// imprime as informações do superbloco
void imprime_superbloco (struct t2fs_superbloco superbloco);
// le record. Retorna 0 caso conseguiu e -1 caso negativo
int le_record(struct t2fs_record* record,unsigned int sector);
// imprime dados de um record (arquivo)
void imprime_record (struct t2fs_record record);
// incrementa posição da cabeça (passa para o próximo bloco)
void incrementa_posicao();
// decrementa posição da cabeça (passa para o bloco anterior)
void decrementa_posicao();






#endif
