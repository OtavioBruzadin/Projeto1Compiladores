#include <stdio.h>
#include <stdlib.h>

#include "lexico.h"
#include "sintatico.h"
/*
Versao compilador lexico e sintatico em arquivos separados
E ::= numero | identificador | +EE | *EE

Para compilar no vscode use:
gcc compilador.c lexico.c sintatico.c -Wall -Og -g -o compilador

Testar com valgrind com 
valgrind --leak-check=yes ./compilador 
*/
// declaradas como glogais ao projeto no sintatico.c
extern TAtomo lookahead;
extern TInfoAtomo info_atomo;

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