#define _POSIX_C_SOURCE 200809L
#include "armazenamento.h"
#include "regras.h"
#include <ctype.h>
#include <math.h>
#include <unistd.h>

static int erroLeitura;
int erroLeituraProdutos(void) { return erroLeitura; }

/* Conversoes limitadas evitam overflow de scanf em arquivos corrompidos. */
static int inteiro(const char *s, int *n) {
    char *fim;
    errno = 0;
    long v = strtol(s, &fim, 10);
    if (fim == s || *fim || errno == ERANGE || v < INT_MIN || v > INT_MAX) return 0;
    *n = (int)v; return 1;
}
static int real(const char *s, float *n) {
    char *fim;
    errno = 0;
    float v = strtof(s, &fim);
    if (fim == s || *fim || errno == ERANGE) return 0;
    *n = v; return 1;
}
int lerProdutoArquivo(FILE *a, Produto *p) {
    char linha[512];
    erroLeitura = 0;
    if (!fgets(linha, sizeof linha, a)) {
        erroLeitura = ferror(a) ? 1 : 0;
        return erroLeitura ? -1 : 0;
    }
    if (!strchr(linha, '\n') && !feof(a)) { erroLeitura = 1; return -1; }
    char *campos[6];
    size_t n = 0;
    char *t = strtok(linha, " \t\r\n");
    while (t && n < 6) { campos[n++] = t; t = strtok(NULL, " \t\r\n"); }
    if (n != 5 || t || strlen(campos[1]) >= sizeof p->nome ||
        !inteiro(campos[0], &p->codigo) || !real(campos[2], &p->precoCompra) ||
        !real(campos[3], &p->precoVenda) || !inteiro(campos[4], &p->quantidade)) {
        erroLeitura = 1; return -1;
    }
    strcpy(p->nome, campos[1]);
    if (validarProduto(p) != RESULTADO_OK) { erroLeitura = 1; return -1; }
    return 1;
}

static int compararCodigos(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}
int validarArquivoProdutos(FILE *a) {
    Produto p;
    int r, *codigos = NULL;
    size_t n = 0, capacidade = 0;
    while ((r = lerProdutoArquivo(a, &p)) == 1) {
        if (n == capacidade) {
            size_t nova = capacidade ? capacidade * 2 : 32;
            if (nova < capacidade || nova > (size_t)-1 / sizeof(int)) { free(codigos); return 0; }
            int *memoria = realloc(codigos, nova * sizeof(int));
            if (!memoria) { free(codigos); return 0; }
            codigos = memoria; capacidade = nova;
        }
        codigos[n++] = p.codigo;
    }
    if (r < 0) { free(codigos); return 0; }
    if (n > 1) qsort(codigos, n, sizeof(int), compararCodigos);
    for (size_t i = 1; i < n; i++) {
        if (codigos[i] == codigos[i-1]) { free(codigos); return 0; }
    }
    free(codigos);
    return fseek(a, 0, SEEK_SET) == 0;
}

int fecharGravacao(FILE *a) {
    int ok = !ferror(a);
    if (fflush(a) != 0) ok = 0;
    if (ok && fsync(fileno(a)) != 0) ok = 0;
    if (fclose(a) != 0) ok = 0;
    return ok;
}

int concluirProdutos(FILE *origem, FILE *temporario) {
    int ok = !erroLeitura && !ferror(origem);
    if (fclose(origem) != 0) ok = 0;
    if (!fecharGravacao(temporario)) ok = 0;
    /* No Linux, rename substitui o destino sem remove-lo primeiro. */
    if (ok && rename("temp.txt", "produtos.txt") == 0) return 1;
    remove("temp.txt");
    return 0;
}

Resultado criarProduto(const Produto *dados) {
    if (validarProduto(dados) != RESULTADO_OK) return DADOS_INVALIDOS;
    Produto novo = *dados, p;
    substituirEspacos(novo.nome);
    FILE *a = fopen("produtos.txt", "r");
    if (!a && errno != ENOENT) return ERRO_ARQUIVO;
    if (a && !validarArquivoProdutos(a)) { fclose(a); return ERRO_ARQUIVO; }
    FILE *t = fopen("temp.txt", "w");
    if (!t) { if (a) fclose(a); return ERRO_ARQUIVO; }
    int r = 0;
    if (a) {
        while ((r = lerProdutoArquivo(a, &p)) == 1) {
            if (p.codigo == novo.codigo) {
                fclose(a); fclose(t); remove("temp.txt");
                return PRODUTO_DUPLICADO;
            }
            fprintf(t, "%d %s %.2f %.2f %d\n", p.codigo, p.nome, p.precoCompra, p.precoVenda, p.quantidade);
        }
        if (r < 0) { fclose(a); fclose(t); remove("temp.txt"); return ERRO_ARQUIVO; }
    }
    fprintf(t, "%d %s %.2f %.2f %d\n", novo.codigo, novo.nome, novo.precoCompra, novo.precoVenda, novo.quantidade);
    if (a) return concluirProdutos(a, t) ? RESULTADO_OK : ERRO_ARQUIVO;
    int ok = fecharGravacao(t);
    if (ok && rename("temp.txt", "produtos.txt") == 0) return RESULTADO_OK;
    remove("temp.txt");
    return ERRO_ARQUIVO;
}

int lerHistorico(FILE *a, int campos, int *id, int *codigo, int *q,
                 float *receita, float *custo, float *lucro) {
    char linha[512], *tokens[7];
    size_t n = 0;
    if (!fgets(linha, sizeof linha, a)) return ferror(a) ? -1 : 0;
    /* Registros completos sempre terminam com nova linha. */
    if (!strchr(linha, '\n')) return -1;
    char *t = strtok(linha, " \t\r\n");
    while (t && n < 7) { tokens[n++] = t; t = strtok(NULL, " \t\r\n"); }
    if ((campos != 4 && campos != 6) || n != (size_t)campos || t ||
        !inteiro(tokens[0], id) || !inteiro(tokens[1], codigo) || !inteiro(tokens[2], q) ||
        *id <= 0 || *codigo <= 0 || *q <= 0 || !real(tokens[3], receita) ||
        !isfinite(*receita) || *receita < 0) return -1;
    if (campos == 6 && (!real(tokens[4], custo) || !real(tokens[5], lucro) ||
        !isfinite(*custo) || !isfinite(*lucro) || *custo < 0)) return -1;
    return 1;
}
