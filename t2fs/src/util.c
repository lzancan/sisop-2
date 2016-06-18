#include "../include/main.h"

int eh_alfanumerico (char letra){
    if(letra >= 48 && letra <= 57) // n�meros de 0 a 9
        return 1;
    else if (letra >=97 && letra <= 122) // letras de (a-z) (min�sculas)
        return 1;
        else if (letra>=65 && letra <=90) // letras de (A-Z) (mai�sculas)
            return 1;
            else return 0; // caso n�o alfanum�rico, retorna 0
}

int testa_nome(char* Filename){
    int i = 0;
    char char_atual = Filename[i];
    while (char_atual != '\0'){
        if(!eh_alfanumerico (char_atual))
            return 0;
        else {
        i++;
        char_atual = Filename[i];
        if(i>32)
            return 0;
        if(char_atual=='\0')
            return 1;
        }
    }
    return 0;
}

int procura_arquivo (char* Filename){
    testa_nome(Filename);
    return 0;

}
