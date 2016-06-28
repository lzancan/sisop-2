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
int contador_diretorios;// conta o número de diretórios do raiz ao pathname
int dir_corrente;
struct t2fs_superbloco superbloco;
struct t2fs_record record_corrente;
struct IndPtr ponteiro_indireto_simples;
struct IndPtr ponteiro_indireto_duplo;
int inicio_bloco_dados;
int fim_bloco_dados; // e fim do disco também
static int contador_posicao=0;
int posicao_atual; // equivale a bloco atual
int setor_atual; // setor no bloco atual (posicao atual)
int TAMANHO_BLOCO;
int TAM_MAX_ARQ;

static struct opened_files2 opened_files;
static struct opened_files2 opened_dirs;

// inicializa (le o disco caso ainda não tiver sido lido) e seta diretório corrente para o raiz
void inicializa (int* superbloco_lido);
// Testa se um caractere é alfanumérico ou não. Retorna 1 caso verdadeiro e 0 caso falso
int eh_alfanumerico (char letra);
// Testa se o nome está de acordo com a especificação (máximo 32 caracteres alfanuméricos. Retorna 1 caso estiver de acordo e 0 caso contrário
int testa_nome(char* Filename);
// Procura o arquivo no disco. Retorna o número do record (0,1,2 ou 3) se encontrar e o nome estiver ok! e erro caso não encontre
int procura_arquivo (struct t2fs_record* records,char* Filename,int dir_corrente);
// le o superbloco para obter as informações. Retorna 0 caso conseguiu e -1 caso negativo
int le_superbloco(struct t2fs_superbloco *superbloco);
// imprime as informações do superbloco
void imprime_superbloco (struct t2fs_superbloco superbloco);
// le record. Retorna 0 caso conseguiu e -1 caso negativo. posicao_no_setor recebe a posicao no setor a ser lido (0 a 3)
int le_record(struct t2fs_record* record,unsigned int sector,int posicao_no_setor);
// imprime dados de um record (arquivo)
void imprime_record (struct t2fs_record record);
// incrementa posição da cabeça (passa para o próximo bloco)
void incrementa_posicao(); // não usada mais *******
// decrementa posição da cabeça (passa para o bloco anterior)
void decrementa_posicao(); // não usada mais ********
// checa se o caminho é absoluto (começa pelo raiz) ou relativo (a partir do diretório corrente).
//Retorna 1 caso seja absoluto e 0 caso seja relativo
int caminho_absoluto_relativo (char* caminho);
// imprime o setor inteiro (4 records)
void imprime_setor (struct t2fs_record* records);
// le o setor todo, colocando os registros em uma struct com 4 registros. Retorna 1 se conseguiu, senão ERRO
int le_setor (struct t2fs_record* records,unsigned int sector);
//muda a posicao corrente do bloco/arquivo
void set_posicao(int bloco);
//procura pelo arquivo no setor. Se achar retorna em qual record está (0,1,2,3). Retorna ERRO (-1) se não encontrar
int search_sector (struct t2fs_record* records,char* Filename,unsigned int setor);
//procura pelo arquivo no bloco (diretório). Retorna o record se achou e -1 em caso de erro.
int search_bloco (struct t2fs_record* records,char* Filename,int bloco);
//procura continuidade no diretório através dos ponteiros que ele aponta. Retorna 0 se não tiver continuidade e 1 se houver
int procura_continuidade_dir (struct t2fs_record* record, char* Filename);
// procura o arquivo no single ind ptr. Se achou retorna o número do record (0,1,2,3), senão retorna ERRO (-1)
int procura_single_ind_ptr(struct t2fs_record* records,char* Filename,unsigned int singleIndPtr_setor);
//procura o arquivo no double ind ptr. Se achou retorna o número do record (0,1,2,3), senão retorna ERRO (-1)
int procura_double_ind_ptr(struct t2fs_record* records,char* Filename,unsigned int doubleIndPtr_setor);
// inicializa um record para posterior gravação no disco
struct t2fs_record* inicializaRecord(char *name, int Bloco);
// procura um typeval no diretório (bloco) referente ao inteiro 'bloco'. Caso encontre, retorna o setor do bloco, senão retorna ERRO
int procura_TypeVal (struct t2fs_record* records,int bloco,unsigned int TypeVal);
// semelhante ao search_bloco, mas procura pelo typeval (usada para procurar locais onde seja possivel escrever um arquivo, procura pelo typeval 0 no caso)
int search_bloco_TypeVal (struct t2fs_record* records,unsigned int TypeVal, int dir_corrente);
// identico ao search sector mas com as mesmas características do search bloco typeval
int search_sector_TypeVal (struct t2fs_record* records,unsigned int TypeVal,unsigned int setor);
// idem para todos os com typeval
int procura_continuidade_dir_TypeVal (struct t2fs_record* record,unsigned int TypeVal);
int procura_single_ind_ptr_TypeVal (struct t2fs_record* records,unsigned int TypeVal,unsigned int singleIndPtr_setor);
int procura_double_ind_ptr_TypeVal(struct t2fs_record* records,unsigned int TypeVal,unsigned int doubleIndPtr_setor);
// libera os blocos relacionados aos ponteiros. É usada quando se exclui um arquivo do disco.
void libera_blocos_record(int ponteiro_dir0,int ponteiro_dir1,int ponteiro_ind_simples,int ponteiro_ind_duplo);


#endif
