#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include "produto.h"
#include "relatorio.h"
#include "vendas.h"

void menu(void) {
    int opcao;

    do {
        printf("\n====================================\n");
        printf("          SISTEMA DA LOJA\n");
        printf("====================================\n");
        printf("1  - Cadastrar produto\n");
        printf("2  - Listar produtos\n");
        printf("3  - Pesquisar produto\n");
        printf("4  - Alterar produto\n");
        printf("5  - Excluir produto\n");
        printf("6  - Registrar venda\n");
        printf("7  - Relatorio de estoque\n");
        printf("8  - Relatorio de vendas\n");
        printf("9  - Relatorio financeiro\n");
        printf("10 - Registrar perda\n");
        printf("11 - Relatorio de perdas\n");
        printf("0  - Sair\n");
        printf("====================================\n");

        while (!lerInteiro(&opcao, "Escolha uma opcao: ")) {
        }

        switch (opcao) {
            case 1: {
                Produto produto;
                if (cadastrarProduto(&produto)) {
                    salvarProduto(produto);
                }
                break;
            }
            case 2:
                listarProdutos();
                break;
            case 3:
                pesquisarProduto();
                break;
            case 4:
                alterarProduto();
                break;
            case 5:
                excluirProduto();
                break;
            case 6:
                registrarVenda();
                break;
            case 7:
                relatorioEstoque();
                break;
            case 8:
                relatorioVendas();
                break;
            case 9:
                relatorioFinanceiro();
                break;
            case 10:
                registrarPerda();
                break;
            case 11:
                relatorioPerdas();
                break;
            case 0:
                printf("\nSaindo do sistema...\n");
                break;
            default:
                printf("\nOpcao invalida!\n");
        }
    } while (opcao != 0);
}

int main(void) {
    int lock = open(".controle.lock", O_CREAT | O_RDWR, 0600);
    if (lock < 0 || flock(lock, LOCK_EX | LOCK_NB) != 0) {
        fprintf(stderr, "Outro processo esta usando os dados, ou nao foi possivel obter o bloqueio.\n");
        if (lock >= 0) close(lock);
        return EXIT_FAILURE;
    }
    menu();
    close(lock);
    return 0;
}
