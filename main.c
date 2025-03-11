/*
Implementacao do mini analisador lexico conforme os slides da aula Aula3(miniLexico).pdf
IDENTIFICADOR -> LETRA_MINUSCULA(LETRA_MINUSCULA|DIGITO)*
NUMERO -> DIGITO+.DIGITO+

Para compilar, por exemplo, no VSCode use:
gcc miniLexico.c -Wall -Og -g -o miniLexico

// Teste de memoria:
https://diveintosystems.org/book/C3-C_debug/valgrind.html

Rode o Valgrind com:
valgrind --leak-check=yes ./miniLexico

Caso não esteja instalado, use:
sudo apt update
sudo apt install valgrind
sudo apt upgrade
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definições dos tokens
typedef enum {
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
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    float atributo_numero;
    char atributo_ID[16];
} TInfoAtomo;

char *strAtomo[] = {
    "ERRO", "IDENTIFICADOR", "NUMERO", "EOS",
    "CHAR", "ELSE", "IF", "INT", "MAIN",
    "READINT", "VOID", "WHILE", "WRITEINT"
};

int contaLinha = 1;
char *entrada; // Ponteiro que será usado para percorrer a string lida do arquivo

// Declaração das funções do analisador léxico
TInfoAtomo obter_atomo();
TInfoAtomo reconhece_num();
TInfoAtomo reconhece_id();

// Função para ler o arquivo inteiro e retornar uma string
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
        // Tratamento de erro, como fechar o arquivo ou tomar outra ação
    }
    buffer[tamanho] = '\0';
    fclose(arq);
    return buffer;
}

int main(void) {
    // Lê o arquivo "entrada.txt" e armazena seu conteúdo
    char *entrada_inicial = le_arquivo("/home/otavio/CLionProjects/Projeto1Compiladores/arquivo.txt");
    entrada = entrada_inicial;  // 'entrada' será usada para o processamento

    TInfoAtomo info_atm;
    do {
        info_atm = obter_atomo();
        printf("%03d# %s | ", info_atm.linha, strAtomo[info_atm.atomo]);
        if (info_atm.atomo == NUMERO)
            printf("%.2f", info_atm.atributo_numero);
        if (info_atm.atomo == IDENTIFICADOR)
            printf("%s", info_atm.atributo_ID);
        printf("\n");
    } while (info_atm.atomo != ERRO && info_atm.atomo != EOS);

    printf("fim de analise lexica\n");

    // Libera a memória usando o ponteiro original retornado por le_arquivo()
    free(entrada_inicial);
    return 0;
}

// Implementação da função obter_atomo
TInfoAtomo obter_atomo() {
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;

    // Elimina delimitadores (espaço, quebras de linha, etc.)
    while (*entrada == ' '  ||
           *entrada == '\n' ||
           *entrada == '\r' ||
           *entrada == '\t') {
        if (*entrada == '\n')
            contaLinha++;
        entrada++;
    }
    if (*entrada == '\0') {
        info_atomo.atomo = EOS;
        info_atomo.linha = contaLinha;
        return info_atomo;
    }
    if (isdigit(*entrada)) {
        info_atomo = reconhece_num();
    }
    else if (isalpha(*entrada) || *entrada == '_') {
        info_atomo = reconhece_id();
    }
    info_atomo.linha = contaLinha;
    return info_atomo;
}

// Implementação da função reconhece_num (para números no formato DIGITO+.DIGITO+)
TInfoAtomo reconhece_num() {
    TInfoAtomo info_num;
    char str_num[10];
    char *ini_num;
    info_num.atomo = ERRO;

    ini_num = entrada;
    // q0:
    if (isdigit(*entrada)) {
        entrada++; // consome dígito
        goto q1;
    }
    return info_num; // erro léxico
q1:
    if (isdigit(*entrada)) {
        entrada++; // consome dígito
        goto q1;
    }
    if (*entrada == '.') {
        entrada++; // consome o ponto
        goto q2;
    }
    return info_num; // erro léxico
q2:
    if (isdigit(*entrada)) {
        entrada++; // consome dígito
        goto q3;
    }
    return info_num; // erro léxico
q3:
    if (isdigit(*entrada)) {
        entrada++; // consome dígito
        goto q3;
    }
    if (isalpha(*entrada)) {
        return info_num; // erro léxico se vier letra após o número
    }
    info_num.atomo = NUMERO;
    strncpy(str_num, ini_num, entrada - ini_num);
    str_num[entrada - ini_num] = '\0';
    info_num.atributo_numero = atof(str_num);
    return info_num;
}

// Implementação da função reconhece_id (para identificadores)
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