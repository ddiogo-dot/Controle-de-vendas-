#ifndef ARMAZENAMENTO_H
#define ARMAZENAMENTO_H
#include "produto.h"
/* 1 = registro; 0 = fim; -1 = erro. Uso sequencial, processo unico. */
void caminhoArquivo(const char *nome, char *destino, size_t tamanho);
int lerProdutoArquivo(FILE *arquivo, Produto *produto);
int erroLeituraProdutos(void);
int validarArquivoProdutos(FILE *arquivo);
int concluirProdutos(FILE *origem, FILE *temporario);
int fecharGravacao(FILE *arquivo);
int lerHistorico(FILE *arquivo, int campos, int *id, int *codigo, int *quantidade,
                 float *receita, float *custo, float *lucro);
#endif
