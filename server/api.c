#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <microhttpd.h>
#include <cjson/cJSON.h>

#include "armazenamento.h"

#define PORTA 8080

/* Envia uma resposta JSON para quem fez a requisicao. */
static enum MHD_Result responderJSON(
    struct MHD_Connection *conexao,
    unsigned int status,
    const char *json
) {
    struct MHD_Response *resposta =
        MHD_create_response_from_buffer(
            strlen(json),
            (void *)json,
            MHD_RESPMEM_MUST_COPY
        );

    if (resposta == NULL) {
        return MHD_NO;
    }

    MHD_add_response_header(
        resposta,
        "Content-Type",
        "application/json; charset=utf-8"
    );

    MHD_add_response_header(
        resposta,
        "Cache-Control",
        "no-store"
    );

    if (status == 405) {
        MHD_add_response_header(resposta, "Allow", "GET");
    }

    enum MHD_Result resultado =
        MHD_queue_response(conexao, status, resposta);

    MHD_destroy_response(resposta);

    return resultado;
}

/* Usa as funcoes de leitura do seu backend C. */
static enum MHD_Result listarProdutosJSON(
    struct MHD_Connection *conexao,
    const char *caminho
) {
    FILE *arquivo = fopen(caminho, "r");

    if (arquivo == NULL) {
        return responderJSON(
            conexao,
            500,
            "{\"erro\":\"Nao foi possivel abrir o arquivo de produtos.\"}"
        );
    }

    if (!validarArquivoProdutos(arquivo)) {
        fclose(arquivo);

        return responderJSON(
            conexao,
            500,
            "{\"erro\":\"Arquivo de produtos invalido.\"}"
        );
    }

    cJSON *lista = cJSON_CreateArray();

    if (lista == NULL) {
        fclose(arquivo);
        return responderJSON(
            conexao, 500, "{\"erro\":\"Memoria insuficiente.\"}"
        );
    }

    Produto produto;
    int leitura;

    while ((leitura = lerProdutoArquivo(arquivo, &produto)) == 1) {
        /*
         * O frontend usa centavos inteiros.
         * Esta conversao adapta os precos float do backend atual.
         */
        double compraCentavos =
            round((double)produto.precoCompra * 100.0);

        double vendaCentavos =
            round((double)produto.precoVenda * 100.0);

        /* Limite de inteiros exatos do JavaScript. */
        if (compraCentavos > 9007199254740991.0 ||
            vendaCentavos > 9007199254740991.0) {
            leitura = -1;
            break;
        }

        cJSON *item = cJSON_CreateObject();

        if (item == NULL ||
            cJSON_AddNumberToObject(
                item, "codigo", produto.codigo
            ) == NULL ||
            cJSON_AddStringToObject(
                item, "nome", produto.nome
            ) == NULL ||
            cJSON_AddNumberToObject(
                item, "precoCompraCentavos", compraCentavos
            ) == NULL ||
            cJSON_AddNumberToObject(
                item, "precoVendaCentavos", vendaCentavos
            ) == NULL ||
            cJSON_AddNumberToObject(
                item, "quantidade", produto.quantidade
            ) == NULL ||
            !cJSON_AddItemToArray(lista, item)) {

            cJSON_Delete(item);
            leitura = -1;
            break;
        }
    }

    if (fclose(arquivo) != 0) {
        leitura = -1;
    }

    if (leitura < 0) {
        cJSON_Delete(lista);

        return responderJSON(
            conexao,
            500,
            "{\"erro\":\"Nao foi possivel montar a lista completa.\"}"
        );
    }

    char *json = cJSON_PrintUnformatted(lista);
    cJSON_Delete(lista);

    if (json == NULL) {
        return responderJSON(
            conexao, 500, "{\"erro\":\"Memoria insuficiente.\"}"
        );
    }

    enum MHD_Result resultado =
        responderJSON(conexao, 200, json);

    cJSON_free(json);

    return resultado;
}

/* Envia um arquivo do frontend para o navegador. */
static enum MHD_Result servirArquivo(
    struct MHD_Connection *conexao,
    const char *caminho,
    const char *tipo
) {
    int arquivo = open(caminho, O_RDONLY);

    if (arquivo < 0) {
        return responderJSON(
            conexao, 500,
            "{\"erro\":\"Nao foi possivel abrir o arquivo do frontend.\"}"
        );
    }

    struct stat informacoes;

    if (fstat(arquivo, &informacoes) != 0 ||
        !S_ISREG(informacoes.st_mode) ||
        informacoes.st_size < 0) {
        close(arquivo);
        return responderJSON(
            conexao, 500,
            "{\"erro\":\"Arquivo do frontend invalido.\"}"
        );
    }

    struct MHD_Response *resposta = MHD_create_response_from_fd(
        (uint64_t)informacoes.st_size,
        arquivo
    );

    if (resposta == NULL) {
        close(arquivo);
        return MHD_NO;
    }

    MHD_add_response_header(resposta, "Content-Type", tipo);
    MHD_add_response_header(resposta, "Cache-Control", "no-store");

    enum MHD_Result resultado =
        MHD_queue_response(conexao, 200, resposta);

    MHD_destroy_response(resposta);
    return resultado;
}

/* Decide qual funcao executar para cada endereco. */
static enum MHD_Result atenderRequisicao(
    void *contexto,
    struct MHD_Connection *conexao,
    const char *url,
    const char *metodo,
    const char *versao,
    const char *dados,
    size_t *tamanhoDados,
    void **estado
) {
    (void)versao;
    (void)dados;
    (void)tamanhoDados;
    (void)estado;

    const char *caminhoProdutos = contexto;

    if (strcmp(metodo, "GET") != 0) {
        return responderJSON(
            conexao,
            405,
            "{\"erro\":\"Nesta etapa, somente GET e permitido.\"}"
        );
    }

    if (strcmp(url, "/api/status") == 0) {
        return responderJSON(
            conexao,
            200,
            "{\"status\":\"online\",\"loja\":\"Bar 335 Universitario\"}"
        );
    }

    if (strcmp(url, "/api/produtos") == 0) {
        return listarProdutosJSON(conexao, caminhoProdutos);
    }
    if (strcmp(url, "/") == 0 ||
        strcmp(url, "/index.html") == 0) {
        return servirArquivo(
            conexao, "frontend/index.html",
            "text/html; charset=utf-8"
        );
    }

    if (strcmp(url, "/styles.css") == 0) {
        return servirArquivo(
            conexao, "frontend/styles.css",
            "text/css; charset=utf-8"
        );
    }

    if (strcmp(url, "/model.js") == 0) {
        return servirArquivo(
            conexao, "frontend/model.js",
            "text/javascript; charset=utf-8"
        );
    }

    if (strcmp(url, "/app.js") == 0) {
        return servirArquivo(
            conexao, "frontend/app.js",
            "text/javascript; charset=utf-8"
        );
    }

    return responderJSON(
        conexao,
        404,
        "{\"erro\":\"Endereco nao encontrado.\"}"
    );
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(
            stderr,
            "Uso: %s caminho/para/produtos.txt\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }

    /* Escuta localmente; o Codespaces encaminha essa porta. */
    struct sockaddr_in endereco = {0};

    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(PORTA);
    endereco.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    struct MHD_Daemon *servidor = MHD_start_daemon(
        MHD_USE_INTERNAL_POLLING_THREAD,
        PORTA,
        NULL,
        NULL,
        atenderRequisicao,
        argv[1],
        MHD_OPTION_SOCK_ADDR,
        (struct sockaddr *)&endereco,
        MHD_OPTION_END
    );

    if (servidor == NULL) {
        fprintf(stderr, "Nao foi possivel iniciar a API.\n");
        return EXIT_FAILURE;
    }

    printf("API: http://localhost:%d\n", PORTA);
    printf("Arquivo de produtos: %s\n", argv[1]);
    printf("Pressione Enter para encerrar.\n");

    getchar();

    MHD_stop_daemon(servidor);

    return EXIT_SUCCESS;
}