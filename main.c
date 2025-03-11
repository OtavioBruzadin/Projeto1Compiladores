/*
Implementacao do mini analisador lexico conforme slides da aula Aula3(miniLexico).pdf
 IDENTIFICADOR -> LETRA_MINUSCULA(LETRA_MINUSCULA|DIGITO)*
 NUMERO -> DIGITO+.DIGITO+

Para compilar no vscode use:
gcc miniLexico.c -Wall -Og -g -o miniLexico

// teste de memoria
https://diveintosystems.org/book/C3-C_debug/valgrind.html

Rode o Valgrind com
valgrind --leak-check=yes ./miniLexico

caso não esteja instalado use
sudo apt update
sudo apt install valgrind
sudo apt upgrade


*/
#include <stdio.h>
#include <ctype.h>
#include <string.h> // strncpy
#include <stdlib.h> // atof

// definicoes dos atamos
typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUMERO,
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

// declaracao de variaveis globais
char *strAtomo[] = {
    "ERRO", "IDENTIFICADOR", "NUMERO", "EOS",
    "CHAR", "ELSE", "IF", "INT", "MAIN",
    "READINT", "VOID", "WHILE", "WRITEINT"
};
int contaLinha = 1;
char *entrada = " main void   \nvar1   12.4\n\rvar1\n\n\n\n \n _ai9n9  While\n while \n Main if If";

// declaracao da funcao
TInfoAtomo obter_atomo();
TInfoAtomo reconhece_num();
TInfoAtomo reconhece_id();

int main(void){
    TInfoAtomo info_atm;
    do{
        info_atm = obter_atomo();

        printf("%03d# %s | ", info_atm.linha, strAtomo[info_atm.atomo]);

        if (info_atm.atomo == NUMERO)
            printf("%.2f", info_atm.atributo_numero);

        if(info_atm.atomo == IDENTIFICADOR)
            printf("%s",info_atm.atributo_ID);
        printf("\n");

        if ( info_atm.atomo == MAIN || info_atm.atomo == CHAR ||
            info_atm.atomo == ELSE || info_atm.atomo == IF || info_atm.atomo == INT ||
            info_atm.atomo == READINT || info_atm.atomo == VOID || info_atm.atomo == WHILE ||
            info_atm.atomo == WRITEINT);



    } while (info_atm.atomo != ERRO && info_atm.atomo != EOS);

    printf("fim de analise lexica\n");
    return 0;
}
// implementacao da funcao
TInfoAtomo obter_atomo(){
    TInfoAtomo info_atomo;

    info_atomo.atomo = ERRO;

    // eliminar delimitadores
    while(*entrada == ' '||
          *entrada == '\n'||
          *entrada == '\r'||
          *entrada == '\t'){
        if(*entrada == '\n')
            contaLinha++;
        entrada++;
    }
    if(*entrada == '\0'){
        info_atomo.atomo = EOS;
    }
    if( isdigit(*entrada)){
        info_atomo = reconhece_num();
    }
    else if( isalpha(*entrada)|| *entrada == '_'){
        info_atomo = reconhece_id();
    }

    info_atomo.linha = contaLinha;
    return info_atomo;
}

// NUMERO -> DIGITO+.DIGITO+
TInfoAtomo reconhece_num(){
    TInfoAtomo info_num;
    char str_num[10];
    char *ini_num;

    info_num.atomo = ERRO;

    ini_num = entrada;
//q0:
    if(isdigit(*entrada)){
        entrada++; // consome digito
        goto q1;
    }

    //se terminar a palavra retorna 0, pois
    //nao eh estado final
    return info_num; // erro lexico

q1:
    if(isdigit(*entrada)){
        entrada++; // consome digito
        goto q1;
    }
    if(*entrada == '.'){
        entrada++; // consome .
        goto q2;
    }
    //[outro]
    return info_num; // erro lexico
q2:
    if(isdigit(*entrada)){
        entrada++; // consome digito
        goto q3;
    }
    //[outro]
    return info_num; // erro lexico
q3:
    if(isdigit(*entrada)){
        entrada++; // consome digito
        goto q3;
    }
    // se vier letra
    if(isalpha(*entrada)){
        return info_num; // erro lexico
    }
    info_num.atomo = NUMERO;
    //man strncpy
    strncpy(str_num,ini_num,entrada - ini_num);
    //finalizar a string
    str_num[entrada - ini_num]='\0';
    info_num.atributo_numero = atof(str_num);
    return info_num;
}

// IDENTIFICADOR -> LETRA_MINUSCULA(LETRA_MINUSCULA|DIGITO)*

TInfoAtomo reconhece_id(){
    TInfoAtomo info_id;
    char str_id[16];
    char *ini_id = entrada;
    size_t len;

    // Inicializa o token com ERRO
    info_id.atomo = ERRO;

    // Permitir que o identificador comece com letra ou underscore
    if(isalpha(*entrada) || *entrada == '_'){
        entrada++; // consome o caractere inicial
        goto q1;
    }
    return info_id;

    q1:
        // Consome enquanto for letra, dígito ou underscore
        if(isalnum(*entrada) || *entrada == '_'){
            entrada++;
            goto q1;
        }

    len = entrada - ini_id;
    if(len >= sizeof(str_id))
        len = sizeof(str_id) - 1;
    strncpy(str_id, ini_id, len);
    str_id[len] = '\0';
    strncpy(info_id.atributo_ID, str_id, len + 1);

    if(strcmp(info_id.atributo_ID, "if") == 0){
        info_id.atomo = IF;
    } else if(strcmp(info_id.atributo_ID, "else") == 0){
        info_id.atomo = ELSE;
    } else if(strcmp(info_id.atributo_ID, "int") == 0){
        info_id.atomo = INT;
    } else if(strcmp(info_id.atributo_ID, "char") == 0){
        info_id.atomo = CHAR;
    } else if(strcmp(info_id.atributo_ID, "main") == 0){
        info_id.atomo = MAIN;
    } else if(strcmp(info_id.atributo_ID, "readint") == 0){
        info_id.atomo = READINT;
    } else if(strcmp(info_id.atributo_ID, "void") == 0){
        info_id.atomo = VOID;
    } else if(strcmp(info_id.atributo_ID, "while") == 0){
        info_id.atomo = WHILE;
    } else if(strcmp(info_id.atributo_ID, "writeint") == 0){
        info_id.atomo = WRITEINT;
    } else {

        info_id.atomo = IDENTIFICADOR;
    }

    return info_id;
}