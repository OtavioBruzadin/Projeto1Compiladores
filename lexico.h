#ifndef	_LEXICO_H
#define _LEXICO_H

typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    OP_SOMA,
    OP_MULT,
    EOS, // fim de buffer
    CHAR,
    ELSE,
    IF,
    INT,
    MAIN,  // main como palavra reservada
    READINT,
    VOID,
    WHILE,
    WRITEINT

}TAtomo;

typedef struct{
    TAtomo atomo;
    int linha;
    float atributo_numero;
    char atributo_ID[16];
}TInfoAtomo;

TInfoAtomo obter_atomo();
TInfoAtomo reconhece_num();
TInfoAtomo reconhece_id();
char *le_arquivo();
extern int contaLinha;
char *entrada;
char *entradaOriginal;  // armazena o ponteiro original
#endif