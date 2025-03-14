/*
Primeira versao do analisador sintatico.
Para gramática
E ::= numero | identificador | +EE | *EE

Para compilar no vscode use:
gcc ASDR3.c -Wall -Og -g -o ASDR3

Testar com valgrind com
valgrind --leak-check=yes ./ASDR3
*/

// LEXICO
#include <ctype.h>  //isupper
#include <string.h> // strncpy
#include <stdlib.h> // atof

// SINTATICO
#include <stdio.h>
#include <stdlib.h>

//######## DECLARACOES LEXICO
// definicoes dos atomos
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

// declaracao de variaveis globais
char *strAtomo[]={"ERRO","IDENTIFICADOR","NUMERO","+","*","EOS"};
int contaLinha = 1;
char *entrada;          // usado para análise (será modificado)
char *entradaOriginal;  // armazena o ponteiro original


// declaracao da funcao
TInfoAtomo obter_atomo();
TInfoAtomo reconhece_num();
TInfoAtomo reconhece_id();
char *le_arquivo();

//######## FIM DECLARACOES LEXICO

//######## DECLARACOES SINTATICO
// variavel global
TAtomo lookahead;
TInfoAtomo info_atomo;

// E ::= a | b | +EE | *EE

// SINTATICO - prototipacao de funcao
void E();
void consome( TAtomo atomo );

//FIM DECLARCOES SINTATICO

int main(){
    entradaOriginal = le_arquivo("/home/otavio/CLionProjects/Projeto1Compiladores/arquivo.txt");
    entrada = entradaOriginal;  // entrada aponta para o início do buffer

    // Inicializa o lookahead
    info_atomo = obter_atomo();
    lookahead = info_atomo.atomo;

    E(); // chama o símbolo inicial da gramática

    printf("\nExpressao sintaticamente correta.\n");
    free(entradaOriginal);

    return 0;
}

//######## IMPLEMENTACAO LEXICO
TInfoAtomo obter_atomo(){
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;

    // Eliminar delimitadores
    while(*entrada == ' ' || *entrada == '\n' || *entrada == '\r' || *entrada == '\t'){
        if(*entrada == '\n')
            contaLinha++;
        entrada++;
    }

    // Se chegou ao final da entrada, retorne EOS imediatamente
    if(*entrada == '\0'){
        info_atomo.atomo = EOS;
        info_atomo.linha = contaLinha;
        return info_atomo;
    }

    if(*entrada == '+'){
        entrada++;
        info_atomo.atomo = OP_SOMA;
    }
    else if(*entrada == '*'){
        entrada++;
        info_atomo.atomo = OP_MULT;
    }
    else if(isdigit(*entrada)){
        info_atomo = reconhece_num();
    }
    else if(isalpha(*entrada) || *entrada == '_'){
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

TInfoAtomo reconhece_id() {
    TInfoAtomo info_id;
    char str_id[16];
    char *ini_id = entrada;
    size_t len;

    info_id.atomo = ERRO;

    // Permitir que o identificador comece com letra ou underscore
    if (isalpha(*entrada) || *entrada == '_') {
        entrada++; // consome o caractere inicial
        goto q1;
    }
    return info_id;
    q1:
        // Consome enquanto for letra, dígito ou underscore
        if (isalnum(*entrada) || *entrada == '_') {
            entrada++;
            goto q1;
        }
    len = entrada - ini_id;
    if (len >= sizeof(str_id))
        len = sizeof(str_id) - 1;
    strncpy(str_id, ini_id, len);
    str_id[len] = '\0';
    strncpy(info_id.atributo_ID, str_id, len + 1);

    if (strcmp(info_id.atributo_ID, "if") == 0) {
        info_id.atomo = IF;
    } else if (strcmp(info_id.atributo_ID, "else") == 0) {
        info_id.atomo = ELSE;
    } else if (strcmp(info_id.atributo_ID, "int") == 0) {
        info_id.atomo = INT;
    } else if (strcmp(info_id.atributo_ID, "char") == 0) {
        info_id.atomo = CHAR;
    } else if (strcmp(info_id.atributo_ID, "main") == 0) {
        info_id.atomo = MAIN;
    } else if (strcmp(info_id.atributo_ID, "readint") == 0) {
        info_id.atomo = READINT;
    } else if (strcmp(info_id.atributo_ID, "void") == 0) {
        info_id.atomo = VOID;
    } else if (strcmp(info_id.atributo_ID, "while") == 0) {
        info_id.atomo = WHILE;
    } else if (strcmp(info_id.atributo_ID, "writeint") == 0) {
        info_id.atomo = WRITEINT;
    } else {
        info_id.atomo = IDENTIFICADOR;
    }

    return info_id;
}
//######## FIM IMPLEMENTACAO LEXICO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//######## IMPLEMENTACAO SINTATICO
// E ::= a | b | +EE | *EE
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

//######## FIM IMPLEMENTACAO SINTATICO

// FUNCOES AUXILIARES

char *le_arquivo(const char *nome_arquivo) {
    FILE *arq = fopen(nome_arquivo, "r");
    if (!arq) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    rewind(arq);
    char *buffer = malloc(tamanho + 1);
    if (!buffer) {
        perror("Erro ao alocar memoria");
        exit(EXIT_FAILURE);
    }
    size_t lidos = fread(buffer, 1, tamanho, arq);
    if (lidos != tamanho) {
        fprintf(stderr, "Erro ao ler o arquivo.\n");
    }
    buffer[tamanho] = '\0';
    fclose(arq);
    return buffer;
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