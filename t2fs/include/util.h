#ifndef __util_h__
#define __util_h__

static int superbloco_lido =0;
char* diretorio_corrente;
struct t2fs_superbloco superbloco;
struct t2fs_record record;
int inicio_bloco_dados;
int fim_bloco_dados; // e fim do disco tamb�m
static int contador_posicao=0;
int posicao_atual;

// inicializa (le o disco caso ainda n�o tiver sido lido) e seta diret�rio corrente para o raiz
void inicializa (int* superbloco_lido);
// Testa se um caractere � alfanum�rico ou n�o. Retorna 1 caso verdadeiro e 0 caso falso
int eh_alfanumerico (char letra);
// Testa se o nome est� de acordo com a especifica��o (m�ximo 32 caracteres alfanum�ricos. Retorna 1 caso estiver de acordo e 0 caso contr�rio
int testa_nome(char* Filename);
// Procura o arquivo no disco. Retorna 1 caso encontrar ou caso o nome esteja com problema e 0 caso contr�rio
int procura_arquivo (char* Filename,char* diretorio_corrente);
// le o superbloco para obter as informa��es. Retorna 0 caso conseguiu e -1 caso negativo
int le_superbloco(struct t2fs_superbloco *superbloco);
// imprime as informa��es do superbloco
void imprime_superbloco (struct t2fs_superbloco superbloco);
// le record. Retorna 0 caso conseguiu e -1 caso negativo
int le_record(struct t2fs_record* record,unsigned int sector);
// imprime dados de um record (arquivo)
void imprime_record (struct t2fs_record record);
// incrementa posi��o da cabe�a (passa para o pr�ximo bloco)
void incrementa_posicao();
// decrementa posi��o da cabe�a (passa para o bloco anterior)
void decrementa_posicao();






#endif
