#ifndef PRODUTO_H
#define PRODUTO_H

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int codigo;
    char nome[100];
    float precoCompra;
    float precoVenda;
    int quantidade;
} Produto;

void limparBuffer(void);
int lerInteiro(int *valor, const char *mensagem);
int lerFloat(float *valor, const char *mensagem);
int lerTexto(char *destino, size_t tamanho, const char *mensagem);

int produtoExiste(int codigo);
void substituirEspacos(char *nome);
int cadastrarProduto(Produto *p);
void salvarProduto(Produto p);
void listarProdutos(void);
void pesquisarProduto(void);
void alterarProduto(void);
void excluirProduto(void);

#endif
