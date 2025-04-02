## Como compilar o programa:

Para compilar seguimos a orientação do slide
```bash
gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador
```

## Como executar o programa:
```bash
./compilador arquivo.txt
```

## O que foi implementado no código:
A implementação do compilador foi dividida em duas partes:

   **Analisador Léxico**: Responsável por identificar os átomos da linguagem (tokens). Ele identifica identificadores, palavras reservadas, operadores, e constantes.
  
   **Analisador Sintático**: Responsável por validar a estrutura do programa de acordo com a gramática especificada. Ele utiliza os tokens fornecidos pelo analisador léxico para construir a árvore sintática 
e verificar se o programa segue a gramatica.


## Aprofundando mais em cada analisador:

**Analisador Léxico:**

* Ignora espaços em branco, quebras de linha, tabulações e retorno de carro, mantendo a contagem de linhas.

* Reconhecimento de comentários de uma linha com // e de múltiplas linhas /*...*/, mantendo a contagem de linhas também.

* Identificação de identificadores, com limite de 15 caracteres(16 teoricamente pois tem o \0), e palavras reservadas, como int, char, if, while, entre outras.

* Reconhecimento de constantes inteiras em hexadecimal com prefixo 0x e constantes de caractere.


**Analisador Sintático:**

* Verificação da estrutura do programa conforme a gramática EBNF.

* O compilador valida expressões, comandos condicionais (if), de repetição (while), e a atribuição de variáveis.

## Saída do Compilador:

* Imprime a descrição dos átomos, incluindo identificadores e palavras reservadas, no formato especificado.

* Caso ocorra um erro sintático ou léxico, a mensagem de erro é exibida com a linha e o tipo do erro.
	
* Caso não ocorra erros é feita a impressão dos átomos linha a linha, indicando o tipo e a linha em que foi encontrado.


## Imagens comprovando a execução e as saídas:

  ![image](https://github.com/user-attachments/assets/265c30b8-7c1e-4b3a-9da1-ed0c67ae7fcc)  
  ![image](https://github.com/user-attachments/assets/b90149af-2b77-4ccb-b36f-25da0c73b6b7)  
  ![image](https://github.com/user-attachments/assets/fd5e632a-3c50-4e1d-8a0c-3baa043f96e2)

## Exemplo utilizado para o teste acima:

  ![image](https://github.com/user-attachments/assets/3388e99a-4a8a-401a-b9f9-b95ca8409183)









