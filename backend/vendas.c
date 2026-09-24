#include "armazenamento.h"
#include "regras.h"
#include "vendas.h"

void registrarVenda(void) {
    FILE *arquivo;
    FILE *temporario;
    FILE *vendas;
    Produto p;
    char caminhoProduto[512];
    char caminhoTemporario[512];
    char caminhoVendas[512];

    int codigo;
    int quantidadeVenda;
    int encontrado = 0;
    int registrado = 0;
    float faturamento;
    float custo;
    float lucro;

    printf("\n===== REGISTRAR VENDA =====\n");

    while (!lerInteiro(&codigo, "Digite o codigo do produto: ")) {
    }

    while (1) {
        if (!lerInteiro(&quantidadeVenda, "Digite a quantidade vendida: ")) continue;
        if (quantidadeVenda > 0) break;
        printf("ERRO: A quantidade deve ser maior que zero.\n");
    }

    caminhoArquivo("produtos.txt", caminhoProduto, sizeof(caminhoProduto));
    arquivo = fopen(caminhoProduto, "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    if (!validarArquivoProdutos(arquivo)) {
        printf("ERRO: Arquivo de produtos invalido; nenhum dado foi alterado.\n");
        fclose(arquivo);
        return;
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

            if (quantidadeVenda > p.quantidade) {
                printf("\nQuantidade insuficiente em estoque.\n");

                fprintf(temporario, "%d %s %.2f %.2f %d\n",
                        p.codigo,
                        p.nome,
                        p.precoCompra,
                        p.precoVenda,
                        p.quantidade);
            } else {
                Resultado r = calcularVenda(&p, quantidadeVenda, &faturamento, &custo, &lucro);
                if (r != RESULTADO_OK) {
                    printf("%s\n", mensagemResultado(r));
                    fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                }
                p.quantidade -= quantidadeVenda;

                fprintf(temporario, "%d %s %.2f %.2f %d\n",
                        p.codigo,
                        p.nome,
                        p.precoCompra,
                        p.precoVenda,
                        p.quantidade);

                caminhoArquivo("vendas.txt", caminhoVendas, sizeof(caminhoVendas));
                vendas = fopen(caminhoVendas, "a");

                if (vendas == NULL) {
                    printf("\nErro ao abrir arquivo de vendas.\n");
                    if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");
                    fclose(arquivo);
                    fclose(temporario);
                    remove(caminhoTemporario);
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

                leituraVendas = fopen(caminhoVendas, "r");

                if (leituraVendas == NULL) {
                    printf("ERRO: Nao foi possivel ler o historico.\n");
                    fclose(vendas); fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                }
                if (leituraVendas != NULL) {
                    int leitura;
                    while ((leitura = lerHistorico(leituraVendas, 6, &n, &codigoLido, &quantidadeLida, &faturamentoLido, &custoLido, &lucroLido)) == 1) {
                        if (n <= 0 || n == INT_MAX) {
                            printf("ERRO: Identificador invalido no historico.\n");
                            fclose(leituraVendas); fclose(vendas);
                            fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                        }
                        if (n >= numeroVenda) numeroVenda = n + 1;
                    }
                    if (leitura < 0) {
                        printf("ERRO: Historico invalido; operacao cancelada.\n");
                        fclose(leituraVendas); fclose(vendas);
                        fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
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

                if (!fecharGravacao(vendas)) {
                    printf("ERRO: Falha ao gravar historico. Estoque nao atualizado; confira o historico antes de tentar novamente.\n");
                    fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                }


                registrado = 1;
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

    if (!concluirProdutos(arquivo, temporario)) {
        printf("\nERRO: Estoque nao atualizado. O historico pode conter a operacao; confira os arquivos antes de repetir.\n");
        return;
    }

    if (registrado) printf("Operacao registrada com sucesso!\n");
    if (encontrado == 0) {
        printf("\nProduto nao encontrado.\n");
    }
}

void registrarPerda(void) {
    FILE *arquivo;
    FILE *temporario;
    FILE *perdas;
    Produto p;
    char caminhoProduto[512];
    char caminhoTemporario[512];
    char caminhoPerdas[512];

    int codigo;
    int quantidadePerdida;
    int encontrado = 0;
    int registrado = 0;
    float valorPerda;

    printf("\n===== REGISTRAR PERDA =====\n");

    while (!lerInteiro(&codigo, "Digite o codigo do produto: ")) {
    }

    while (1) {
        if (!lerInteiro(&quantidadePerdida, "Digite a quantidade perdida: ")) continue;
        if (quantidadePerdida > 0) break;
        printf("ERRO: A quantidade deve ser maior que zero.\n");
    }

    caminhoArquivo("produtos.txt", caminhoProduto, sizeof(caminhoProduto));
    arquivo = fopen(caminhoProduto, "r");

    if (arquivo == NULL) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    if (!validarArquivoProdutos(arquivo)) {
        printf("ERRO: Arquivo de produtos invalido; nenhum dado foi alterado.\n");
        fclose(arquivo);
        return;
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

            if (quantidadePerdida > p.quantidade) {
                printf("\nQuantidade de perda maior que o estoque.\n");

                fprintf(temporario, "%d %s %.2f %.2f %d\n",
                        p.codigo,
                        p.nome,
                        p.precoCompra,
                        p.precoVenda,
                        p.quantidade);
            } else {
                Resultado r = calcularPerda(&p, quantidadePerdida, &valorPerda);
                if (r != RESULTADO_OK) {
                    printf("%s\n", mensagemResultado(r));
                    fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                }
                p.quantidade -= quantidadePerdida;

                fprintf(temporario, "%d %s %.2f %.2f %d\n",
                        p.codigo,
                        p.nome,
                        p.precoCompra,
                        p.precoVenda,
                        p.quantidade);

                caminhoArquivo("perdas.txt", caminhoPerdas, sizeof(caminhoPerdas));
                perdas = fopen(caminhoPerdas, "a");

                if (perdas == NULL) {
                    printf("\nErro ao abrir arquivo de perdas.\n");
                    if (erroLeituraProdutos()) printf("\nERRO: Arquivo de produtos invalido ou ilegivel.\n");
                    fclose(arquivo);
                    fclose(temporario);
                    remove(caminhoTemporario);
                    return;
                }

                int numeroPerda = 1;
                int n;
                int codigoLido;
                int quantidadeLida;
                float valorLido;
                FILE *leituraPerdas;

                leituraPerdas = fopen(caminhoPerdas, "r");

                if (leituraPerdas == NULL) {
                    printf("ERRO: Nao foi possivel ler o historico.\n");
                    fclose(perdas); fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                }
                if (leituraPerdas != NULL) {
                    int leitura;
                    while ((leitura = lerHistorico(leituraPerdas, 4, &n, &codigoLido, &quantidadeLida, &valorLido, NULL, NULL)) == 1) {
                        if (n <= 0 || n == INT_MAX) {
                            printf("ERRO: Identificador invalido no historico.\n");
                            fclose(leituraPerdas); fclose(perdas);
                            fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                        }
                        if (n >= numeroPerda) numeroPerda = n + 1;
                    }
                    if (leitura < 0) {
                        printf("ERRO: Historico invalido; operacao cancelada.\n");
                        fclose(leituraPerdas); fclose(perdas);
                        fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                    }
                    fclose(leituraPerdas);
                }

                fprintf(perdas, "%d %d %d %.2f\n",
                        numeroPerda,
                        codigo,
                        quantidadePerdida,
                        valorPerda);

                if (!fecharGravacao(perdas)) {
                    printf("ERRO: Falha ao gravar historico. Estoque nao atualizado; confira o historico antes de tentar novamente.\n");
                    fclose(arquivo); fclose(temporario); remove(caminhoTemporario); return;
                }


                registrado = 1;
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

    if (!concluirProdutos(arquivo, temporario)) {
        printf("\nERRO: Estoque nao atualizado. O historico pode conter a operacao; confira os arquivos antes de repetir.\n");
        return;
    }

    if (registrado) printf("Operacao registrada com sucesso!\n");
    if (encontrado == 0) {
        printf("\nProduto nao encontrado.\n");
    }
}

