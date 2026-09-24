#include "regras.h"
#include <ctype.h>
#include <math.h>

Resultado validarProduto(const Produto *p) {
    if (!p || p->codigo <= 0 || p->quantidade < 0 ||
        !memchr(p->nome, '\0', sizeof p->nome) || !p->nome[0] ||
        !isfinite(p->precoCompra) || !isfinite(p->precoVenda) ||
        p->precoCompra < 0 || p->precoVenda < 0) return DADOS_INVALIDOS;
    int letra = 0;
    for (size_t i = 0; p->nome[i]; i++) {
        unsigned char c = (unsigned char)p->nome[i];
        if (iscntrl(c) && c != '\t') return DADOS_INVALIDOS;
        if (isalpha(c) || c >= 128) letra = 1;
    }
    return letra ? RESULTADO_OK : DADOS_INVALIDOS;
}

Resultado calcularVenda(const Produto *p, int q, float *f, float *c, float *l) {
    if (!f || !c || !l || validarProduto(p) != RESULTADO_OK || q <= 0) return DADOS_INVALIDOS;
    if (q > p->quantidade) return ESTOQUE_INSUFICIENTE;
    float receita = q * p->precoVenda, custo = q * p->precoCompra;
    float lucro = receita - custo;
    if (!isfinite(receita) || !isfinite(custo) || !isfinite(lucro)) return DADOS_INVALIDOS;
    *f = receita; *c = custo; *l = lucro;
    return RESULTADO_OK;
}

Resultado calcularPerda(const Produto *p, int q, float *valor) {
    if (!valor || validarProduto(p) != RESULTADO_OK || q <= 0) return DADOS_INVALIDOS;
    if (q > p->quantidade) return ESTOQUE_INSUFICIENTE;
    float total = q * p->precoCompra;
    if (!isfinite(total)) return DADOS_INVALIDOS;
    *valor = total;
    return RESULTADO_OK;
}

const char *mensagemResultado(Resultado r) {
    switch (r) {
        case RESULTADO_OK: return "Operacao concluida com sucesso!";
        case DADOS_INVALIDOS: return "ERRO: Dados invalidos.";
        case ESTOQUE_INSUFICIENTE: return "ERRO: Quantidade insuficiente em estoque.";
        case PRODUTO_DUPLICADO: return "ERRO: Codigo de produto ja cadastrado.";
        default: return "ERRO: Falha de leitura ou gravacao; operacao nao concluida.";
    }
}
