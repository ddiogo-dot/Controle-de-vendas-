#include "produto.h"

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int lerInteiro(int *valor, const char *mensagem) {
    char entrada[64];
    char *fim = NULL;
    long numero;

    printf("%s", mensagem);
    if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
        limparBuffer();
        return 0;
    }

    entrada[strcspn(entrada, "\n")] = '\0';

    if (entrada[0] == '\0') {
        printf("\nERRO: Digite um numero inteiro valido.\n");
        return 0;
    }

    errno = 0;
    numero = strtol(entrada, &fim, 10);

    if (errno == ERANGE || fim == entrada || *fim != '\0' || numero < INT_MIN || numero > INT_MAX) {
        printf("\nERRO: Digite um numero inteiro valido.\n");
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

int lerFloat(float *valor, const char *mensagem) {
    char entrada[64];
    char *fim = NULL;
    float numero;

    printf("%s", mensagem);
    if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
        limparBuffer();
        return 0;
    }

    entrada[strcspn(entrada, "\n")] = '\0';

    if (entrada[0] == '\0') {
        printf("\nERRO: Digite um numero valido.\n");
        return 0;
    }

    errno = 0;
    numero = strtof(entrada, &fim);

    if (errno == ERANGE || fim == entrada || *fim != '\0') {
        printf("\nERRO: Digite um numero valido.\n");
        return 0;
    }

    *valor = numero;
    return 1;
}

int lerTexto(char *destino, size_t tamanho, const char *mensagem) {
    char buffer[256];
    size_t len;
    int i;
    int temLetra = 0;

    if (tamanho == 0) {
        return 0;
    }

    printf("%s", mensagem);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 0;
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    while (*buffer == ' ' || *buffer == '\t') {
        memmove(buffer, buffer + 1, strlen(buffer));
    }

    len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == ' ' || buffer[len - 1] == '\t')) {
        buffer[--len] = '\0';
    }

    if (len == 0) {
        printf("\nERRO: O texto nao pode estar vazio.\n");
        return 0;
    }

    for (i = 0; buffer[i] != '\0'; i++) {
        if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z')) {
            temLetra = 1;
            break;
        }
    }

    if (!temLetra) {
        printf("\nERRO: O nome do produto nao pode ser numerico.\n");
        return 0;
    }

    if (len >= tamanho) {
        printf("\nERRO: Texto muito longo. Maximo %zu caracteres.\n", tamanho - 1);
        limparBuffer();
        return 0;
    }

    strcpy(destino, buffer);
    return 1;
}

int produtoExiste(int codigo) {
    FILE *arquivo;
    Produto p;

    arquivo = fopen("produtos.txt", "r");

    if (arquivo == NULL) {
        return 0;
    }

    while (fscanf(arquivo, "%d %99s %f %f %d",
                  &p.codigo,
                  p.nome,
                  &p.precoCompra,
                  &p.precoVenda,
                  &p.quantidade) == 5) {

        if (p.codigo == codigo) {
            fclose(arquivo);
            return 1;
        }

        if (p.codigo <= 0) {
            printf("\nERRO: O codigo deve ser maior que zero.\n");
            continue;
        }
    }

    fclose(arquivo);
    return 0;
}

void substituirEspacos(char *nome) {
    int i;

    for (i = 0; nome[i] != '\0'; i++) {
        if (nome[i] == ' ') {
            nome[i] = '_';
        }
    }
}

int cadastrarProduto(Produto *p) {
    while (1) {
        if (!lerInteiro(&p->codigo, "Digite o codigo do produto: ")) {
            continue;
        }

        if (p->codigo <= 0) {
            printf("\nERRO: O codigo deve ser maior que zero.\n");
            continue;
        }

        if (produtoExiste(p->codigo)) {
            printf("\nERRO: Ja existe um produto com esse codigo.\n");
        } else {
            break;
        }
    }

    while (!lerTexto(p->nome, sizeof(p->nome), "Digite o nome do produto: ")) {
    }

    do {
        if (!lerFloat(&p->precoCompra, "Digite o preco de compra: ")) {
            continue;
        }
        substituirEspacos(p->nome);

        if (p->precoCompra < 0) {
            printf("ERRO: O preco nao pode ser negativo.\n");
        }
    } while (p->precoCompra < 0);

    do {
        if (!lerFloat(&p->precoVenda, "Digite o preco de venda: ")) {
            continue;
        }

        if (p->precoVenda < 0) {
            printf("ERRO: O preco nao pode ser negativo.\n");
        }
    } while (p->precoVenda < 0);

    do {
        if (!lerInteiro(&p->quantidade, "Digite a quantidade em estoque: ")) {
            continue;
        }

        if (p->quantidade < 0) {
            printf("ERRO: A quantidade nao pode ser negativa.\n");
        }
    } while (p->quantidade < 0);

    return 1;
}

void salvarProduto(Produto p) {
    FILE *arquivo;

    arquivo = fopen("produtos.txt", "a");

    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo.\n");
        return;
    }

    fprintf(arquivo, "%d %s %.2f %.2f %d\n",
            p.codigo,
            p.nome,
            p.precoCompra,
            p.precoVenda,
            p.quantidade);

    fclose(arquivo);
    printf("\nProduto salvo com sucesso!\n");
}

void listarProdutos(void) {
    FILE *arquivo;
    Produto p;

    arquivo = fopen("produtos.txt", "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    printf("\n===== PRODUTOS CADASTRADOS =====\n");

    while (fscanf(arquivo, "%d %99s %f %f %d",
                  &p.codigo,
                  p.nome,
                  &p.precoCompra,
                  &p.precoVenda,
                  &p.quantidade) == 5) {

        printf("\nCodigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("Preco de compra: R$ %.2f\n", p.precoCompra);
        printf("Preco de venda: R$ %.2f\n", p.precoVenda);
        printf("Quantidade: %d\n", p.quantidade);
    }

    fclose(arquivo);
}

void pesquisarProduto(void) {
    FILE *arquivo;
    Produto p;
    int codigo;
    int encontrado = 0;

    while (!lerInteiro(&codigo, "\nDigite o codigo do produto: ")) {
    }

    arquivo = fopen("produtos.txt", "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    while (fscanf(arquivo, "%d %99s %f %f %d",
                  &p.codigo,
                  p.nome,
                  &p.precoCompra,
                  &p.precoVenda,
                  &p.quantidade) == 5) {

        if (p.codigo == codigo) {
            printf("\n===== PRODUTO ENCONTRADO =====\n");
            printf("Codigo: %d\n", p.codigo);
            printf("Nome: %s\n", p.nome);
            printf("Preco de compra: R$ %.2f\n", p.precoCompra);
            printf("Preco de venda: R$ %.2f\n", p.precoVenda);
            printf("Quantidade: %d\n", p.quantidade);

            encontrado = 1;
            break;
        }
    }

    fclose(arquivo);

    if (encontrado == 0) {
        printf("\nProduto nao encontrado.\n");
    }
}

void alterarProduto(void) {
    FILE *arquivo;
    FILE *temporario;
    Produto p;
    int codigo;
    int encontrado = 0;

    while (!lerInteiro(&codigo, "\nDigite o codigo do produto que deseja alterar: ")) {
    }

    arquivo = fopen("produtos.txt", "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    temporario = fopen("temp.txt", "w");

    if (temporario == NULL) {
        printf("\nErro ao criar arquivo temporario.\n");
        fclose(arquivo);
        return;
    }

    while (fscanf(arquivo, "%d %99s %f %f %d",
                  &p.codigo,
                  p.nome,
                  &p.precoCompra,
                  &p.precoVenda,
                  &p.quantidade) == 5) {

        if (p.codigo == codigo) {
            printf("\n===== ALTERAR PRODUTO =====\n");

            while (!lerTexto(p.nome, sizeof(p.nome), "Novo nome: ")) {
            }

            do {
                if (!lerFloat(&p.precoCompra, "Novo preco de compra: ")) {
                    continue;
                }
                if (p.precoCompra < 0) {
                    printf("ERRO: O preco nao pode ser negativo.\n");
                }
            } while (p.precoCompra < 0);

            do {
                if (!lerFloat(&p.precoVenda, "Novo preco de venda: ")) {
                    continue;
                }
                if (p.precoVenda < 0) {
                    printf("ERRO: O preco nao pode ser negativo.\n");
                }
            } while (p.precoVenda < 0);

            do {
                if (!lerInteiro(&p.quantidade, "Nova quantidade: ")) {
                    continue;
                }
                if (p.quantidade < 0) {
                    printf("ERRO: A quantidade nao pode ser negativa.\n");
                }
            } while (p.quantidade < 0);

            encontrado = 1;
        }

        fprintf(temporario, "%d %s %.2f %.2f %d\n",
                p.codigo,
                p.nome,
                p.precoCompra,
                p.precoVenda,
                p.quantidade);
    }

    fclose(arquivo);
    fclose(temporario);

    remove("produtos.txt");
    rename("temp.txt", "produtos.txt");

    if (encontrado) {
        printf("\nProduto alterado com sucesso!\n");
    } else {
        printf("\nProduto nao encontrado.\n");
    }
}

void excluirProduto(void) {
    FILE *arquivo;
    FILE *temporario;
    Produto p;
    int codigo;
    int encontrado = 0;

    while (!lerInteiro(&codigo, "\nDigite o codigo do produto que deseja excluir: ")) {
    }

    arquivo = fopen("produtos.txt", "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    temporario = fopen("temp.txt", "w");

    if (temporario == NULL) {
        printf("\nErro ao criar arquivo temporario.\n");
        fclose(arquivo);
        return;
    }

    while (fscanf(arquivo, "%d %99s %f %f %d",
                  &p.codigo,
                  p.nome,
                  &p.precoCompra,
                  &p.precoVenda,
                  &p.quantidade) == 5) {

        if (p.codigo == codigo) {
            encontrado = 1;
            continue;
        }

        fprintf(temporario, "%d %s %.2f %.2f %d\n",
                p.codigo,
                p.nome,
                p.precoCompra,
                p.precoVenda,
                p.quantidade);
    }

    fclose(arquivo);
    fclose(temporario);

    remove("produtos.txt");
    rename("temp.txt", "produtos.txt");

    if (encontrado) {
        printf("\nProduto excluido com sucesso!\n");
    } else {
        printf("\nProduto nao encontrado.\n");
    }
}
