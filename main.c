/*
 * Primeira versão do analisador sintático.
 * Gramática:
 *   E ::= numero | identificador | +EE | *EE
 *
 * Para compilar no vscode use:
 *   gcc ASDR3.c -Wall -Og -g -o ASDR3
 *
 * Testar com valgrind com:
 *   valgrind --leak-check=yes ./ASDR3
 */

#include <ctype.h>   // isdigit, isalpha, etc.
#include <string.h>  // strncpy, strcmp
#include <stdlib.h>  // atof, malloc, free, exit
#include <stdio.h>

//######## DECLARAÇÕES LÉXICO

// Definições dos átomos (tokens)
typedef enum {
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    OP_SOMA,
    OP_MULT,
    EOS,       // fim de buffer
    CHAR,
    ELSE,
    IF,
    INT,
    MAIN,      // main como palavra reservada
    READINT,
    VOID,
    WHILE,
    WRITEINT,
    COMENTARIO
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    float atributo_numero;
    char atributo_ID[16];
    char atributo_comentario[256]; // para armazenar o texto do comentário
} TInfoAtomo;

// Array de strings para impressão dos tokens
char *strAtomo[] = {
    "ERRO", "IDENTIFICADOR", "NUMERO", "+", "*", "EOS",
    "CHAR", "ELSE", "IF", "INT", "MAIN", "READINT", "VOID", "WHILE", "WRITEINT", "COMENTARIO"
};

int contaLinha = 1;
char *entrada;          // usado para análise (será modificado)
char *entradaOriginal;  // armazena o ponteiro original do buffer

// Declaração de funções do analisador léxico
TInfoAtomo obter_atomo();
TInfoAtomo reconhece_num();
TInfoAtomo reconhece_id();
TInfoAtomo processa_comentario();
char *le_arquivo(const char *nome_arquivo);

//######## FIM DECLARAÇÕES LÉXICO

//######## DECLARAÇÕES SINTÁTICO

// Variáveis globais para o analisador sintático
TAtomo lookahead;
TInfoAtomo info_atomo;

// Protótipos das funções do analisador sintático
void E();
void consome(TAtomo atomo);
void nextToken();

//######## FIM DECLARAÇÕES SINTÁTICO

void nextToken(){
    do {
        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
        if(lookahead == COMENTARIO){
            printf("%03d# %s \n", info_atomo.linha, strAtomo[info_atomo.atomo]);
        }
    } while(lookahead == COMENTARIO);
}

int main(){
    // Carrega o arquivo de entrada (ajuste o caminho conforme necessário)
    entradaOriginal = le_arquivo("arquivo.txt");
    entrada = entradaOriginal;  // entrada aponta para o início do buffer

    // Inicializa o lookahead chamando nextToken() para garantir que ele não seja um comentário.
    nextToken();

    // Inicia a análise sintática
    E();

    printf("\nExpressao sintaticamente correta.\n");
    free(entradaOriginal);
    return 0;
}

//######## IMPLEMENTAÇÃO LÉXICO

TInfoAtomo obter_atomo(){
    TInfoAtomo info;
    info.atomo = ERRO;

    // Eliminar delimitadores (espaços, quebras de linha, tabs, etc.)
    while(*entrada == ' ' || *entrada == '\n' || *entrada == '\r' || *entrada == '\t'){
        if(*entrada == '\n')
            contaLinha++;
        entrada++;
    }

    // Se chegou ao final da entrada, retorne EOS imediatamente
    if(*entrada == '\0'){
        info.atomo = EOS;
        info.linha = contaLinha;
        return info;
    }

    // Se inicia um comentário, processa-o e retorna o token de comentário.
    if(*entrada == '/' && (*(entrada+1) == '/' || *(entrada+1) == '*')){
        info = processa_comentario();
        info.linha = contaLinha;
        return info;
    }

    // Processa demais tokens
    if(*entrada == '+'){
        entrada++;
        info.atomo = OP_SOMA;
    }
    else if(*entrada == '*'){
        entrada++;
        info.atomo = OP_MULT;
    }
    else if(isdigit(*entrada)){
        info = reconhece_num();
    }
    else if(isalpha(*entrada) || *entrada == '_'){
        info = reconhece_id();
    }
    else{
        // Se o caractere não é reconhecido, marca erro e consome-o.
        info.atomo = ERRO;
        entrada++;
    }

    info.linha = contaLinha;
    return info;
}

// Função para processar comentários (tanto de linha quanto de bloco)
TInfoAtomo processa_comentario(){
    TInfoAtomo info_atomo;
    int i = 0;
    info_atomo.linha = contaLinha;

    // Comentário de uma linha: //
    if(*entrada == '/' && *(entrada+1) == '/'){
        entrada += 2; // consome os dois caracteres iniciais
        while(*entrada != '\n' && *entrada != '\0'){
            if(i < (int)sizeof(info_atomo.atributo_comentario)-1)
                info_atomo.atributo_comentario[i++] = *entrada;
            entrada++;
        }
        info_atomo.atributo_comentario[i] = '\0';
        info_atomo.atomo = COMENTARIO;
        return info_atomo;
    }
    // Comentário de várias linhas: /* ... */
    else if(*entrada == '/' && *(entrada+1) == '*'){
        entrada += 2; // consome "/*"
        while(!(*entrada == '*' && *(entrada+1) == '/')){
            if(*entrada == '\n'){
                contaLinha++;
            }
            if(*entrada == '\0'){
                fprintf(stderr, "Erro: comentario nao terminado na linha %d\n", contaLinha);
                exit(EXIT_FAILURE);
            }
            if(i < (int)sizeof(info_atomo.atributo_comentario)-1)
                info_atomo.atributo_comentario[i++] = *entrada;
            entrada++;
        }
        info_atomo.atributo_comentario[i] = '\0';
        info_atomo.atomo = COMENTARIO;
        entrada += 2; // consome "*/"
        return info_atomo;
    }
    info_atomo.atomo = ERRO;
    return info_atomo;
}

// Reconhecimento de números: NUMERO -> DIGITO+.DIGITO+
TInfoAtomo reconhece_num(){
    TInfoAtomo info_num;
    char str_num[10];
    char *ini_num;

    info_num.atomo = ERRO;
    ini_num = entrada;

    if(isdigit(*entrada)){
        entrada++; // consome dígito
        goto q1;
    }
    return info_num; // erro léxico

q1:
    if(isdigit(*entrada)){
        entrada++; // consome dígito
        goto q1;
    }
    if(*entrada == '.'){
        entrada++; // consome '.'
        goto q2;
    }
    return info_num; // erro léxico

q2:
    if(isdigit(*entrada)){
        entrada++; // consome dígito
        goto q3;
    }
    return info_num; // erro léxico

q3:
    if(isdigit(*entrada)){
        entrada++; // consome dígito
        goto q3;
    }
    if(isalpha(*entrada)){
        return info_num; // erro léxico se vier letra após o número
    }
    info_num.atomo = NUMERO;
    strncpy(str_num, ini_num, entrada - ini_num);
    str_num[entrada - ini_num] = '\0';
    info_num.atributo_numero = atof(str_num);
    return info_num;
}

// Reconhecimento de identificadores: IDENTIFICADOR -> LETRA (LETRA | DIGITO)*
// Aceita também o caractere '_' no início.
TInfoAtomo reconhece_id(){
    TInfoAtomo info_id;
    char str_id[16];
    char *ini_id = entrada;
    size_t len;

    info_id.atomo = ERRO;
    if(isalpha(*entrada) || *entrada == '_'){
        entrada++; // consome o caractere inicial
        goto q1;
    }
    return info_id;

q1:
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

    // Verifica se o identificador é uma palavra reservada
    if(strcmp(info_id.atributo_ID, "if") == 0){
        info_id.atomo = IF;
    }
    else if(strcmp(info_id.atributo_ID, "else") == 0){
        info_id.atomo = ELSE;
    }
    else if(strcmp(info_id.atributo_ID, "int") == 0){
        info_id.atomo = INT;
    }
    else if(strcmp(info_id.atributo_ID, "char") == 0){
        info_id.atomo = CHAR;
    }
    else if(strcmp(info_id.atributo_ID, "main") == 0){
        info_id.atomo = MAIN;
    }
    else if(strcmp(info_id.atributo_ID, "readint") == 0){
        info_id.atomo = READINT;
    }
    else if(strcmp(info_id.atributo_ID, "void") == 0){
        info_id.atomo = VOID;
    }
    else if(strcmp(info_id.atributo_ID, "while") == 0){
        info_id.atomo = WHILE;
    }
    else if(strcmp(info_id.atributo_ID, "writeint") == 0){
        info_id.atomo = WRITEINT;
    }
    else{
        info_id.atomo = IDENTIFICADOR;
    }
    return info_id;
}

//######## FIM IMPLEMENTAÇÃO LÉXICO

//######## IMPLEMENTAÇÃO SINTÁTICO

void E(){
    switch(lookahead){
        case OP_SOMA:
            consome(OP_SOMA);
            E();
            E();
            break;
        case OP_MULT:
            consome(OP_MULT);
            E();
            E();
            break;
        case IDENTIFICADOR:
        case NUMERO:
            consome(lookahead);
            break;
        default:
            printf("%03d# Erro sintatico: esperado [IDENTIFICADOR ou NUMERO] encontrado [%s]\n",
                   contaLinha, strAtomo[lookahead]);
            free(entradaOriginal);
            exit(1);
    }
}

void consome(TAtomo atomo){
    if(lookahead == atomo){
        printf("%03d# %s | ", info_atomo.linha, strAtomo[info_atomo.atomo]);
        if(info_atomo.atomo == NUMERO)
            printf("%.2f", info_atomo.atributo_numero);
        if(info_atomo.atomo == IDENTIFICADOR)
            printf("%s", info_atomo.atributo_ID);
        printf("\n");
        nextToken();
    }
    else{
        printf("%03d# Erro sintatico: esperado [%s] encontrado [%s]\n",
               contaLinha, strAtomo[atomo], strAtomo[lookahead]);
        free(entradaOriginal);
        exit(1);
    }
}
//######## FIM IMPLEMENTAÇÃO SINTÁTICO

// FUNÇÕES AUXILIARES

char *le_arquivo(const char *nome_arquivo){
    FILE *arq = fopen(nome_arquivo, "r");
    if(!arq){
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    rewind(arq);
    char *buffer = malloc(tamanho + 1);
    if(!buffer){
        perror("Erro ao alocar memoria");
        exit(EXIT_FAILURE);
    }
    size_t lidos = fread(buffer, 1, tamanho, arq);
    if(lidos != (size_t)tamanho){
        fprintf(stderr, "Erro ao ler o arquivo.\n");
    }
    buffer[tamanho] = '\0';
    fclose(arq);
    return buffer;
}
