#ifndef REGRAS_H
#define REGRAS_H
#include "produto.h"
typedef enum {
    RESULTADO_OK, DADOS_INVALIDOS, ESTOQUE_INSUFICIENTE,
    PRODUTO_DUPLICADO, ERRO_ARQUIVO
} Resultado;
/* Sem perguntas ou mensagens de terminal: reutilizaveis por uma futura UI. */
Resultado validarProduto(const Produto *produto);
Resultado calcularVenda(const Produto *produto, int quantidade,
                        float *faturamento, float *custo, float *lucro);
Resultado calcularPerda(const Produto *produto, int quantidade, float *valor);
Resultado criarProduto(const Produto *produto);
const char *mensagemResultado(Resultado resultado);
#endif
