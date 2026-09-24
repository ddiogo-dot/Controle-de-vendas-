#include "armazenamento.h"
#include "regras.h"
#include "produto.h"

int produtoExiste(int codigo) {
    FILE *arquivo;
    Produto p;
    char caminhoProduto[512];

    caminhoArquivo("produtos.txt", caminhoProduto, sizeof(caminhoProduto));
    arquivo = fopen(caminhoProduto, "r");

    if (arquivo == NULL) {
        return 0;
    }

    while (lerProdutoArquivo(arquivo, &p) == 1) {

        if (p.codigo == codigo) {
            if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");
            fclose(arquivo);
            return 1;
        }

        if (p.codigo <= 0) {
            printf("\nERRO: O codigo deve ser maior que zero.\n");
            continue;
        }
    }

    if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");

    fclose(arquivo);
    return 0;
}

void substituirEspacos(char *nome) {
    int i;

    for (i = 0; nome[i] != '\0'; i++) {
        if (nome[i] == ' ' || nome[i] == '\t') {
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

    while (1) {
        if (!lerFloat(&p->precoCompra, "Digite o preco de compra: ")) continue;
        if (p->precoCompra >= 0) break;
        printf("ERRO: O valor nao pode ser negativo.\n");
    }

    while (1) {
        if (!lerFloat(&p->precoVenda, "Digite o preco de venda: ")) continue;
        if (p->precoVenda >= 0) break;
        printf("ERRO: O valor nao pode ser negativo.\n");
    }

    while (1) {
        if (!lerInteiro(&p->quantidade, "Digite a quantidade em estoque: ")) continue;
        if (p->quantidade >= 0) break;
        printf("ERRO: O valor nao pode ser negativo.\n");
    }

    substituirEspacos(p->nome);
    return 1;
}

void salvarProduto(Produto p) {
    Resultado r = criarProduto(&p);
    printf("\n%s\n", mensagemResultado(r));
}

void listarProdutos(void) {
    FILE *arquivo;
    Produto p;
    char caminhoProduto[512];

    caminhoArquivo("produtos.txt", caminhoProduto, sizeof(caminhoProduto));
    arquivo = fopen(caminhoProduto, "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    printf("\n===== PRODUTOS CADASTRADOS =====\n");

    while (lerProdutoArquivo(arquivo, &p) == 1) {

        printf("\nCodigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("Preco de compra: R$ %.2f\n", p.precoCompra);
        printf("Preco de venda: R$ %.2f\n", p.precoVenda);
        printf("Quantidade: %d\n", p.quantidade);
    }

    if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");

    fclose(arquivo);
}

void pesquisarProduto(void) {
    FILE *arquivo;
    Produto p;
    int codigo;
    int encontrado = 0;
    char caminhoProduto[512];

    while (!lerInteiro(&codigo, "\nDigite o codigo do produto: ")) {
    }

    caminhoArquivo("produtos.txt", caminhoProduto, sizeof(caminhoProduto));
    arquivo = fopen(caminhoProduto, "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    while (lerProdutoArquivo(arquivo, &p) == 1) {

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

    if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");

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
    char caminhoProduto[512];
    char caminhoTemporario[512];

    while (!lerInteiro(&codigo, "\nDigite o codigo do produto que deseja alterar: ")) {
    }

    caminhoArquivo("produtos.txt", caminhoProduto, sizeof(caminhoProduto));
    arquivo = fopen(caminhoProduto, "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    if (!validarArquivoProdutos(arquivo)) {
        printf("ERRO: Arquivo de produtos invalido; nenhum dado foi alterado.\n");
        fclose(arquivo); return;
    }
    caminhoArquivo("temp.txt", caminhoTemporario, sizeof(caminhoTemporario));
    temporario = fopen(caminhoTemporario, "w");

    if (temporario == NULL) {
        printf("\nErro ao criar arquivo temporario.\n");
        if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");
        fclose(arquivo);
        return;
    }

    while (lerProdutoArquivo(arquivo, &p) == 1) {

        if (p.codigo == codigo) {
            printf("\n===== ALTERAR PRODUTO =====\n");

            while (!lerTexto(p.nome, sizeof(p.nome), "Novo nome: ")) {
            }

            while (1) {
                if (!lerFloat(&p.precoCompra, "Novo preco de compra: ")) continue;
                if (p.precoCompra >= 0) break;
                printf("ERRO: O valor nao pode ser negativo.\n");
            }

            while (1) {
                if (!lerFloat(&p.precoVenda, "Novo preco de venda: ")) continue;
                if (p.precoVenda >= 0) break;
                printf("ERRO: O valor nao pode ser negativo.\n");
            }

            while (1) {
                if (!lerInteiro(&p.quantidade, "Nova quantidade: ")) continue;
                if (p.quantidade >= 0) break;
                printf("ERRO: O valor nao pode ser negativo.\n");
            }

            substituirEspacos(p.nome);
            encontrado = 1;
        }

        fprintf(temporario, "%d %s %.2f %.2f %d\n",
                p.codigo,
                p.nome,
                p.precoCompra,
                p.precoVenda,
                p.quantidade);
    }

    if (!concluirProdutos(arquivo, temporario)) {
        printf("\nERRO: Produtos nao atualizados. Verifique os arquivos; preserve um backup.\n");
        return;
    }

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
    char caminhoProduto[512];
    char caminhoTemporario[512];

    while (!lerInteiro(&codigo, "\nDigite o codigo do produto que deseja excluir: ")) {
    }

    caminhoArquivo("produtos.txt", caminhoProduto, sizeof(caminhoProduto));
    arquivo = fopen(caminhoProduto, "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    if (!validarArquivoProdutos(arquivo)) {
        printf("ERRO: Arquivo de produtos invalido; nenhum dado foi alterado.\n");
        fclose(arquivo); return;
    }
    caminhoArquivo("temp.txt", caminhoTemporario, sizeof(caminhoTemporario));
    temporario = fopen(caminhoTemporario, "w");

    if (temporario == NULL) {
        printf("\nErro ao criar arquivo temporario.\n");
        if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");
        fclose(arquivo);
        return;
    }

    while (lerProdutoArquivo(arquivo, &p) == 1) {

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

    if (!concluirProdutos(arquivo, temporario)) {
        printf("\nERRO: Produtos nao atualizados. Verifique os arquivos; preserve um backup.\n");
        return;
    }

    if (encontrado) {
        printf("\nProduto excluido com sucesso!\n");
    } else {
        printf("\nProduto nao encontrado.\n");
    }
}
