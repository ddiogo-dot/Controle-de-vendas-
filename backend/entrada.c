#include "produto.h"
#include <ctype.h>
#include <math.h>

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* Somente a interface de terminal encerra o programa ao receber EOF. */
static int lerLinha(char *buffer, size_t tamanho, const char *mensagem) {
    printf("%s", mensagem);
    fflush(stdout);
    if (!fgets(buffer, (int)tamanho, stdin)) {
        if (ferror(stdin)) {
            fprintf(stderr, "\nErro ao ler a entrada.\n");
            exit(EXIT_FAILURE);
        }
        printf("\nEntrada encerrada. Operacao cancelada.\n");
        exit(EXIT_SUCCESS);
    }
    char *fim = strchr(buffer, '\n');
    if (fim) *fim = '\0';
    else if (!feof(stdin)) {
        int c = getchar();
        if (c != '\n' && c != EOF) {
            limparBuffer();
            printf("ERRO: Entrada muito longa.\n");
            return 0;
        }
    }
    return 1;
}

static int somenteEspacos(const char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return *s == '\0';
}

int lerInteiro(int *valor, const char *mensagem) {
    char entrada[128], *fim;
    if (!lerLinha(entrada, sizeof entrada, mensagem)) return 0;
    errno = 0;
    long n = strtol(entrada, &fim, 10);
    if (fim == entrada || errno == ERANGE || !somenteEspacos(fim) || n < INT_MIN || n > INT_MAX) {
        printf("ERRO: Digite um numero inteiro valido.\n");
        return 0;
    }
    *valor = (int)n;
    return 1;
}

int lerFloat(float *valor, const char *mensagem) {
    char entrada[128], *fim;
    if (!lerLinha(entrada, sizeof entrada, mensagem)) return 0;
    /* Aceita 12,50 ou 12.50, sem separador de milhar. */
    for (size_t i = 0; entrada[i]; i++) if (entrada[i] == ',') entrada[i] = '.';
    errno = 0;
    float n = strtof(entrada, &fim);
    if (fim == entrada || errno == ERANGE || !somenteEspacos(fim) || !isfinite(n)) {
        printf("ERRO: Digite um numero finito valido.\n");
        return 0;
    }
    *valor = n;
    return 1;
}

int lerTexto(char *destino, size_t tamanho, const char *mensagem) {
    char buffer[512];
    if (!tamanho || !lerLinha(buffer, sizeof buffer, mensagem)) return 0;
    char *inicio = buffer;
    while (isspace((unsigned char)*inicio)) inicio++;
    size_t len = strlen(inicio);
    while (len && isspace((unsigned char)inicio[len-1])) inicio[--len] = '\0';
    int temLetra = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)inicio[i];
        if (isalpha(c) || c >= 128) temLetra = 1;
        if (iscntrl(c) && c != '\t') {
            printf("ERRO: Nome com caractere de controle.\n");
            return 0;
        }
    }
    if (!len || !temLetra || len >= tamanho) {
        printf("ERRO: Use um nome com letras e no maximo %zu bytes.\n", tamanho - 1);
        return 0;
    }
    memcpy(destino, inicio, len + 1);
    return 1;
}
