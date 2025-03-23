#include <stdio.h>
#include <ctype.h>  //isupper
#include <string.h> // strncpy
#include <stdlib.h> // atof

#include "lexico.h"

int contaLinha = 1;


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
