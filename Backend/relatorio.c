#include "relatorio.h"

void relatorioEstoque(void) {
    FILE *arquivo;
    Produto p;

    float totalInvestido = 0;
    float valorPotencial = 0;

    arquivo = fopen("produtos.txt", "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    printf("\n===== RELATORIO DE ESTOQUE =====\n");

    while (fscanf(arquivo, "%d %99s %f %f %d",
                  &p.codigo,
                  p.nome,
                  &p.precoCompra,
                  &p.precoVenda,
                  &p.quantidade) == 5) {

        float investimento;
        float potencial;

        investimento = p.quantidade * p.precoCompra;
        potencial = p.quantidade * p.precoVenda;

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

    printf("\n-----------------------------\n");
    printf("Total investido: R$ %.2f\n", totalInvestido);
    printf("Valor potencial de venda: R$ %.2f\n", valorPotencial);

    fclose(arquivo);
}
