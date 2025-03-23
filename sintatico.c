#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strncpy
#include "sintatico.h"

// declaracao de variaveis globais
char *strAtomo[]={"ERRO","IDENTIFICADOR","NUMERO","+","*","EOS"};
TAtomo lookahead;
TInfoAtomo info_atomo;

void E(){
    switch( lookahead ){
        case OP_SOMA:
            //consome +
                consome(OP_SOMA);

        E();E();
        break;
        case OP_MULT:
            //consome *
                consome(OP_MULT);
        E();E();
        break;
        case IDENTIFICADOR:
        case NUMERO:
            //consome a | b
            consome(lookahead);
        break;
        default:
            consome(IDENTIFICADOR);
    }


}

void program(){
  consome(VOID);
  consome(MAIN);
  consome('(');
  consome(VOID);
  consome(')');
}

void consome( TAtomo atomo ){
    if( lookahead == atomo ){
        printf("%03d# %s | ", info_atomo.linha, strAtomo[info_atomo.atomo]);
        if (info_atomo.atomo == NUMERO)
            printf("%.2f", info_atomo.atributo_numero);
        if (info_atomo.atomo == IDENTIFICADOR)
            printf("%s", info_atomo.atributo_ID);
        printf("\n");

        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
    }
    else{
        printf("%03d# Erro sintatico: esperado [%s] encontrado [%s]\n",
               contaLinha, strAtomo[atomo], strAtomo[lookahead]);
        free(entradaOriginal);
        exit(1);
    }
}

int encontrarIndice(const char *palavra) {
    int tamanho = sizeof(strAtomo) / sizeof(strAtomo[0]);
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(strAtomo[i], palavra) == 0) {
            return i;
        }
    }
    return -1;
}