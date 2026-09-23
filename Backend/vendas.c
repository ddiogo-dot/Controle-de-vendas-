#include "vendas.h"

void registrarVenda(void) {
    FILE *arquivo;
    FILE *temporario;
    FILE *vendas;
    Produto p;

    int codigo;
    int quantidadeVenda;
    int encontrado = 0;
    float faturamento;
    float custo;
    float lucro;

    printf("\n===== REGISTRAR VENDA =====\n");

    while (!lerInteiro(&codigo, "Digite o codigo do produto: ")) {
    }

    while (!lerInteiro(&quantidadeVenda, "Digite a quantidade vendida: ")) {
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

            if (quantidadeVenda > p.quantidade) {
                printf("\nQuantidade insuficiente em estoque.\n");

                fprintf(temporario, "%d %s %.2f %.2f %d\n",
                        p.codigo,
                        p.nome,
                        p.precoCompra,
                        p.precoVenda,
                        p.quantidade);
            } else {
                p.quantidade -= quantidadeVenda;

                faturamento = quantidadeVenda * p.precoVenda;
                custo = quantidadeVenda * p.precoCompra;
                lucro = faturamento - custo;

                fprintf(temporario, "%d %s %.2f %.2f %d\n",
                        p.codigo,
                        p.nome,
                        p.precoCompra,
                        p.precoVenda,
                        p.quantidade);

                vendas = fopen("vendas.txt", "a");

                if (vendas == NULL) {
                    printf("\nErro ao abrir arquivo de vendas.\n");
                    fclose(arquivo);
                    fclose(temporario);
                    remove("temp.txt");
                    return;
                }

                int numeroVenda = 1;
                int n;
                int codigoLido;
                int quantidadeLida;
                float faturamentoLido;
                float custoLido;
                float lucroLido;
                FILE *leituraVendas;

                leituraVendas = fopen("vendas.txt", "r");

                if (leituraVendas != NULL) {
                    while (fscanf(leituraVendas,
                                  "%d %d %d %f %f %f",
                                  &n,
                                  &codigoLido,
                                  &quantidadeLida,
                                  &faturamentoLido,
                                  &custoLido,
                                  &lucroLido) == 6) {
                        numeroVenda = n + 1;
                    }
                    fclose(leituraVendas);
                }

                fprintf(vendas, "%d %d %d %.2f %.2f %.2f\n",
                        numeroVenda,
                        codigo,
                        quantidadeVenda,
                        faturamento,
                        custo,
                        lucro);

                fclose(vendas);

                printf("\nVenda registrada com sucesso!\n");
                printf("Faturamento: R$ %.2f\n", faturamento);
                printf("Custo: R$ %.2f\n", custo);
                printf("Lucro: R$ %.2f\n", lucro);
            }
        } else {
            fprintf(temporario, "%d %s %.2f %.2f %d\n",
                    p.codigo,
                    p.nome,
                    p.precoCompra,
                    p.precoVenda,
                    p.quantidade);
        }
    }

    fclose(arquivo);
    fclose(temporario);

    remove("produtos.txt");
    rename("temp.txt", "produtos.txt");

    if (encontrado == 0) {
        printf("\nProduto nao encontrado.\n");
    }
}

void relatorioVendas(void) {
    FILE *arquivo;
    int numeroVenda;
    int codigo;
    int quantidade;
    float faturamento;
    float custo;
    float lucro;
    float totalFaturamento = 0;
    float totalCusto = 0;
    float totalLucro = 0;
    int encontrou = 0;

    arquivo = fopen("vendas.txt", "r");

    if (arquivo == NULL) {
        printf("\nNenhuma venda registrada.\n");
        return;
    }

    printf("\n===== RELATORIO DE VENDAS =====\n");

    while (fscanf(arquivo,
                  "%d %d %d %f %f %f",
                  &numeroVenda,
                  &codigo,
                  &quantidade,
                  &faturamento,
                  &custo,
                  &lucro) == 6) {

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

    if (encontrou) {
        printf("\n-----------------------------\n");
        printf("Faturamento total: R$ %.2f\n", totalFaturamento);
        printf("Custo total: R$ %.2f\n", totalCusto);
        printf("Lucro total: R$ %.2f\n", totalLucro);
    }

    fclose(arquivo);
}

void registrarPerda(void) {
    FILE *arquivo;
    FILE *temporario;
    FILE *perdas;
    Produto p;

    int codigo;
    int quantidadePerdida;
    int encontrado = 0;
    float valorPerda;

    printf("\n===== REGISTRAR PERDA =====\n");

    while (!lerInteiro(&codigo, "Digite o codigo do produto: ")) {
    }

    while (!lerInteiro(&quantidadePerdida, "Digite a quantidade perdida: ")) {
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

            if (quantidadePerdida > p.quantidade) {
                printf("\nQuantidade de perda maior que o estoque.\n");

                fprintf(temporario, "%d %s %.2f %.2f %d\n",
                        p.codigo,
                        p.nome,
                        p.precoCompra,
                        p.precoVenda,
                        p.quantidade);
            } else {
                p.quantidade -= quantidadePerdida;
                valorPerda = quantidadePerdida * p.precoCompra;

                fprintf(temporario, "%d %s %.2f %.2f %d\n",
                        p.codigo,
                        p.nome,
                        p.precoCompra,
                        p.precoVenda,
                        p.quantidade);

                perdas = fopen("perdas.txt", "a");

                if (perdas == NULL) {
                    printf("\nErro ao abrir arquivo de perdas.\n");
                    fclose(arquivo);
                    fclose(temporario);
                    remove("temp.txt");
                    return;
                }

                int numeroPerda = 1;
                int n;
                int codigoLido;
                int quantidadeLida;
                float valorLido;
                FILE *leituraPerdas;

                leituraPerdas = fopen("perdas.txt", "r");

                if (leituraPerdas != NULL) {
                    while (fscanf(leituraPerdas,
                                  "%d %d %d %f",
                                  &n,
                                  &codigoLido,
                                  &quantidadeLida,
                                  &valorLido) == 4) {
                        numeroPerda = n + 1;
                    }
                    fclose(leituraPerdas);
                }

                fprintf(perdas, "%d %d %d %.2f\n",
                        numeroPerda,
                        codigo,
                        quantidadePerdida,
                        valorPerda);

                fclose(perdas);

                printf("\nPerda registrada com sucesso!\n");
                printf("Valor da perda: R$ %.2f\n", valorPerda);
            }
        } else {
            fprintf(temporario, "%d %s %.2f %.2f %d\n",
                    p.codigo,
                    p.nome,
                    p.precoCompra,
                    p.precoVenda,
                    p.quantidade);
        }
    }

    fclose(arquivo);
    fclose(temporario);

    remove("produtos.txt");
    rename("temp.txt", "produtos.txt");

    if (encontrado == 0) {
        printf("\nProduto nao encontrado.\n");
    }
}

void relatorioPerdas(void) {
    FILE *arquivo;
    int numeroPerda;
    int codigo;
    int quantidade;
    float valorPerda;
    float totalPerdas = 0;

    arquivo = fopen("perdas.txt", "r");

    if (arquivo == NULL) {
        printf("\nNenhuma perda registrada.\n");
        return;
    }

    printf("\n===== RELATORIO DE PERDAS =====\n");

    while (fscanf(arquivo,
                  "%d %d %d %f",
                  &numeroPerda,
                  &codigo,
                  &quantidade,
                  &valorPerda) == 4) {

        printf("\nPerda: %d\n", numeroPerda);
        printf("Codigo do produto: %d\n", codigo);
        printf("Quantidade perdida: %d\n", quantidade);
        printf("Valor da perda: R$ %.2f\n", valorPerda);

        totalPerdas += valorPerda;
    }

    printf("\n-----------------------------\n");
    printf("Total de perdas: R$ %.2f\n", totalPerdas);

    fclose(arquivo);
}

void relatorioFinanceiro(void) {
    FILE *vendas;
    FILE *perdas;

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

    float totalFaturamento = 0;
    float totalCusto = 0;
    float totalLucro = 0;
    float totalPerdas = 0;

    vendas = fopen("vendas.txt", "r");

    if (vendas != NULL) {
        while (fscanf(vendas,
                      "%d %d %d %f %f %f",
                      &numeroVenda,
                      &codigoVenda,
                      &quantidadeVenda,
                      &faturamento,
                      &custo,
                      &lucro) == 6) {

            totalFaturamento += faturamento;
            totalCusto += custo;
            totalLucro += lucro;
        }
        fclose(vendas);
    }

    perdas = fopen("perdas.txt", "r");

    if (perdas != NULL) {
        while (fscanf(perdas,
                      "%d %d %d %f",
                      &numeroPerda,
                      &codigoPerda,
                      &quantidadePerda,
                      &valorPerda) == 4) {

            totalPerdas += valorPerda;
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
