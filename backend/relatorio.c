#include "armazenamento.h"
#include "regras.h"
#include "relatorio.h"

void relatorioEstoque(void) {
    FILE *arquivo;
    Produto p;
    char caminhoProduto[512];

    double totalInvestido = 0;
    double valorPotencial = 0;

    caminhoArquivo("produtos.txt", caminhoProduto, sizeof(caminhoProduto));
    arquivo = fopen(caminhoProduto, "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    printf("\n===== RELATORIO DE ESTOQUE =====\n");

    while (lerProdutoArquivo(arquivo, &p) == 1) {

        double investimento;
        double potencial;

        investimento = (double)p.quantidade * p.precoCompra;
        potencial = (double)p.quantidade * p.precoVenda;

        totalInvestido += investimento;
        valorPotencial += potencial;

        printf("\nCodigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("Quantidade: %d\n", p.quantidade);
        printf("Valor investido: R$ %.2f\n", investimento);
        printf("Valor potencial de venda: R$ %.2f\n", potencial);

        if (p.quantidade <= 5) {
            printf("ATENCAO: estoque baixo!\n");
        }
    }

    if (erroLeituraProdutos()) {
        printf("ERRO: Arquivo de produtos invalido; relatorio incompleto.\n");
        fclose(arquivo); return;
    }
    printf("\n-----------------------------\n");
    printf("Total investido: R$ %.2f\n", totalInvestido);
    printf("Valor potencial de venda: R$ %.2f\n", valorPotencial);

    if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");

    fclose(arquivo);
}

void relatorioVendas(void) {
    FILE *arquivo;
    int numeroVenda;
    int codigo;
    int quantidade;
    float faturamento;
    float custo;
    float lucro;
    double totalFaturamento = 0;
    double totalCusto = 0;
    double totalLucro = 0;
    int encontrou = 0;
    char caminhoVendas[512];

    caminhoArquivo("vendas.txt", caminhoVendas, sizeof(caminhoVendas));
    arquivo = fopen(caminhoVendas, "r");

    if (arquivo == NULL) {
        printf("\nNenhuma venda registrada.\n");
        return;
    }

    printf("\n===== RELATORIO DE VENDAS =====\n");

    int leitura;
    while ((leitura = lerHistorico(arquivo, 6, &numeroVenda, &codigo, &quantidade, &faturamento, &custo, &lucro)) == 1) {

        encontrou = 1;

        printf("\nVenda: %d\n", numeroVenda);
        printf("Codigo do produto: %d\n", codigo);
        printf("Quantidade: %d\n", quantidade);
        printf("Faturamento: R$ %.2f\n", faturamento);
        printf("Custo: R$ %.2f\n", custo);
        printf("Lucro: R$ %.2f\n", lucro);

        totalFaturamento += faturamento;
        totalCusto += custo;
        totalLucro += lucro;
    }
    if (leitura < 0) {
        printf("ERRO: Historico invalido; relatorio incompleto.\n");
        fclose(arquivo); return;
    }

    if (encontrou) {
        printf("\n-----------------------------\n");
        printf("Faturamento total: R$ %.2f\n", totalFaturamento);
        printf("Custo total: R$ %.2f\n", totalCusto);
        printf("Lucro total: R$ %.2f\n", totalLucro);
    }

    if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");

    fclose(arquivo);
}

void relatorioPerdas(void) {
    FILE *arquivo;
    int numeroPerda;
    int codigo;
    int quantidade;
    float valorPerda;
    double totalPerdas = 0;
    char caminhoPerdas[512];

    caminhoArquivo("perdas.txt", caminhoPerdas, sizeof(caminhoPerdas));
    arquivo = fopen(caminhoPerdas, "r");

    if (arquivo == NULL) {
        printf("\nNenhuma perda registrada.\n");
        return;
    }

    printf("\n===== RELATORIO DE PERDAS =====\n");

    int leitura;
    while ((leitura = lerHistorico(arquivo, 4, &numeroPerda, &codigo, &quantidade, &valorPerda, NULL, NULL)) == 1) {

        printf("\nPerda: %d\n", numeroPerda);
        printf("Codigo do produto: %d\n", codigo);
        printf("Quantidade perdida: %d\n", quantidade);
        printf("Valor da perda: R$ %.2f\n", valorPerda);

        totalPerdas += valorPerda;
    }
    if (leitura < 0) {
        printf("ERRO: Historico invalido; relatorio incompleto.\n");
        fclose(arquivo); return;
    }

    printf("\n-----------------------------\n");
    printf("Total de perdas: R$ %.2f\n", totalPerdas);

    if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");

    fclose(arquivo);
}

void relatorioFinanceiro(void) {
    FILE *vendas;
    FILE *perdas;
    char caminhoVendas[512];
    char caminhoPerdas[512];

    int numeroVenda;
    int codigoVenda;
    int quantidadeVenda;
    float faturamento;
    float custo;
    float lucro;

    int numeroPerda;
    int codigoPerda;
    int quantidadePerda;
    float valorPerda;

    double totalFaturamento = 0;
    double totalCusto = 0;
    double totalLucro = 0;
    double totalPerdas = 0;

    caminhoArquivo("vendas.txt", caminhoVendas, sizeof(caminhoVendas));
    vendas = fopen(caminhoVendas, "r");

    if (vendas != NULL) {
        int leitura;
    while ((leitura = lerHistorico(vendas, 6, &numeroVenda, &codigoVenda, &quantidadeVenda, &faturamento, &custo, &lucro)) == 1) {

            totalFaturamento += faturamento;
            totalCusto += custo;
            totalLucro += lucro;
        }
    if (leitura < 0) {
        printf("ERRO: Historico invalido; relatorio incompleto.\n");
        fclose(vendas); return;
    }
        fclose(vendas);
    }

    caminhoArquivo("perdas.txt", caminhoPerdas, sizeof(caminhoPerdas));
    perdas = fopen(caminhoPerdas, "r");

    if (perdas != NULL) {
        int leitura;
    while ((leitura = lerHistorico(perdas, 4, &numeroPerda, &codigoPerda, &quantidadePerda, &valorPerda, NULL, NULL)) == 1) {

            totalPerdas += valorPerda;
        }
    if (leitura < 0) {
        printf("ERRO: Historico invalido; relatorio incompleto.\n");
        fclose(perdas); return;
    }
        fclose(perdas);
    }

    printf("\n===== RELATORIO FINANCEIRO =====\n");
    printf("\nFaturamento total: R$ %.2f\n", totalFaturamento);
    printf("Custo total das vendas: R$ %.2f\n", totalCusto);
    printf("Lucro total das vendas: R$ %.2f\n", totalLucro);
    printf("Total de perdas: R$ %.2f\n", totalPerdas);
    printf("\nResultado apos perdas: R$ %.2f\n",
           totalLucro - totalPerdas);
}