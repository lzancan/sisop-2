#ifndef __util_h__
#define __util_h__

#define ERRO -1

typedef struct opened_file2 {
    int handle;
    char* nome;
    struct t2fs_record record;
    int current_pointer;
    struct opened_file2 *next;
} OPENED_FILE;

typedef struct opened_files2 {
    OPENED_FILE *first;
    OPENED_FILE *last;
} OPENED_FILES;


struct IndPtr{
    int bloco[64];
};


//int nOpenedFiles = 0;
//int nOpenedDirs = 0;
static int superbloco_lido =0;
char* diretorio_corrente;
char* path;
int contador_diretorios;// conta o n�mero de diret�rios do raiz ao pathname
int dir_corrente;
struct t2fs_superbloco superbloco;
struct t2fs_record record_corrente;
struct IndPtr ponteiro_indireto_simples;
struct IndPtr ponteiro_indireto_duplo;
int inicio_bloco_dados;
int fim_bloco_dados; // e fim do disco tamb�m
static int contador_posicao=0;
int posicao_atual; // equivale a bloco atual
int setor_atual; // setor no bloco atual (posicao atual)
int TAMANHO_BLOCO;
int TAM_MAX_ARQ;

static struct opened_files2 opened_files;
static struct opened_files2 opened_dirs;

// inicializa (le o disco caso ainda n�o tiver sido lido) e seta diret�rio corrente para o raiz
void inicializa (int* superbloco_lido);
// Testa se um caractere � alfanum�rico ou n�o. Retorna 1 caso verdadeiro e 0 caso falso
int eh_alfanumerico (char letra);
// Testa se o nome est� de acordo com a especifica��o (m�ximo 32 caracteres alfanum�ricos. Retorna 1 caso estiver de acordo e 0 caso contr�rio
int testa_nome(char* Filename);
// Procura o arquivo no disco. Retorna o n�mero do record (0,1,2 ou 3) se encontrar e o nome estiver ok! e erro caso n�o encontre
int procura_arquivo (struct t2fs_record* records,char* Filename,int dir_corrente);
// le o superbloco para obter as informa��es. Retorna 0 caso conseguiu e -1 caso negativo
int le_superbloco(struct t2fs_superbloco *superbloco);
// imprime as informa��es do superbloco
void imprime_superbloco (struct t2fs_superbloco superbloco);
// le record. Retorna 0 caso conseguiu e -1 caso negativo. posicao_no_setor recebe a posicao no setor a ser lido (0 a 3)
int le_record(struct t2fs_record* record,unsigned int sector,int posicao_no_setor);
// imprime dados de um record (arquivo)
void imprime_record (struct t2fs_record record);
// incrementa posi��o da cabe�a (passa para o pr�ximo bloco)
void incrementa_posicao(); // n�o usada mais *******
// decrementa posi��o da cabe�a (passa para o bloco anterior)
void decrementa_posicao(); // n�o usada mais ********
// checa se o caminho � absoluto (come�a pelo raiz) ou relativo (a partir do diret�rio corrente).
//Retorna 1 caso seja absoluto e 0 caso seja relativo
int caminho_absoluto_relativo (char* caminho);
// imprime o setor inteiro (4 records)
void imprime_setor (struct t2fs_record* records);
// le o setor todo, colocando os registros em uma struct com 4 registros. Retorna 1 se conseguiu, sen�o ERRO
int le_setor (struct t2fs_record* records,unsigned int sector);
//muda a posicao corrente do bloco/arquivo
void set_posicao(int bloco);
//procura pelo arquivo no setor. Se achar retorna em qual record est� (0,1,2,3). Retorna ERRO (-1) se n�o encontrar
int search_sector (struct t2fs_record* records,char* Filename,unsigned int setor);
//procura pelo arquivo no bloco (diret�rio). Retorna o record se achou e -1 em caso de erro.
int search_bloco (struct t2fs_record* records,char* Filename,int bloco);
//procura continuidade no diret�rio atrav�s dos ponteiros que ele aponta. Retorna 0 se n�o tiver continuidade e 1 se houver
int procura_continuidade_dir (struct t2fs_record* record, char* Filename);
// procura o arquivo no single ind ptr. Se achou retorna o n�mero do record (0,1,2,3), sen�o retorna ERRO (-1)
int procura_single_ind_ptr(struct t2fs_record* records,char* Filename,unsigned int singleIndPtr_setor);
//procura o arquivo no double ind ptr. Se achou retorna o n�mero do record (0,1,2,3), sen�o retorna ERRO (-1)
int procura_double_ind_ptr(struct t2fs_record* records,char* Filename,unsigned int doubleIndPtr_setor);
// inicializa um record para posterior grava��o no disco
struct t2fs_record* inicializaRecord(char *name, int Bloco);
// procura um typeval no diret�rio (bloco) referente ao inteiro 'bloco'. Caso encontre, retorna o setor do bloco, sen�o retorna ERRO
int procura_TypeVal (struct t2fs_record* records,int bloco,unsigned int TypeVal);
// semelhante ao search_bloco, mas procura pelo typeval (usada para procurar locais onde seja possivel escrever um arquivo, procura pelo typeval 0 no caso)
int search_bloco_TypeVal (struct t2fs_record* records,unsigned int TypeVal, int dir_corrente);
// identico ao search sector mas com as mesmas caracter�sticas do search bloco typeval
int search_sector_TypeVal (struct t2fs_record* records,unsigned int TypeVal,unsigned int setor);
// idem para todos os com typeval
int procura_continuidade_dir_TypeVal (struct t2fs_record* record,unsigned int TypeVal);
int procura_single_ind_ptr_TypeVal (struct t2fs_record* records,unsigned int TypeVal,unsigned int singleIndPtr_setor);
int procura_double_ind_ptr_TypeVal(struct t2fs_record* records,unsigned int TypeVal,unsigned int doubleIndPtr_setor);
// libera os blocos relacionados aos ponteiros. � usada quando se exclui um arquivo do disco.
void libera_blocos_record(int ponteiro_dir0,int ponteiro_dir1,int ponteiro_ind_simples,int ponteiro_ind_duplo);


#endif
