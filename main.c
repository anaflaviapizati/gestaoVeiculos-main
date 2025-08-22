#include <stdio.h>
#include <stdlib.h>
#include "funcionarios.h"
#include "veiculos.h"
#include "locacoes.h"
#include "hash.h"

void menuPrincipal();
void menuFuncionarios();
void menuVeiculos();
void menuLocacoes();

int main() {
    menuPrincipal();
    return 0;
}

void menuPrincipal() {
    int opcao;
    do {
        printf("\n==== Sistema de Gestao de Veiculos ====");
        printf("\n1. Gerenciar Funcionarios");
        printf("\n2. Gerenciar Veiculos");
        printf("\n3. Gerenciar Locacoes");
        printf("\n0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();
        switch(opcao) {
            case 1:
                menuFuncionarios();
                break;
            case 2:
                menuVeiculos();
                break;
            case 3:
                menuLocacoes();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while(opcao != 0);
}

void menuFuncionarios() {
    int opcao;
    do {
        printf("\n--- Menu Funcionarios ---");
        printf("\n1. Adicionar Funcionario");
        printf("\n2. Remover Funcionario");
        printf("\n3. Listar Funcionarios");
        printf("\n4. Ordenar Funcionarios por CPF");
        printf("\n5. Gerar Funcionarios Aleatorios");
        printf("\n0. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();
        switch(opcao) {
            case 1: adicionarFuncionario(); break;
            case 2: removerFuncionario(); break;
            case 3: listarFuncionarios(); break;
            case 4: ordenarFuncionariosPorCPF(); break;
            case 5: {
                int qtd;
                printf("Quantos funcionarios deseja gerar? ");
                scanf("%d", &qtd);
                getchar();
                gerarFuncionariosAleatorios(qtd);
                break;
            }
            case 0: break;
            default: printf("Opcao invalida!\n");
        }
    } while(opcao != 0);
}

void menuVeiculos() {
    int opcao;
    do {
        printf("\n--- Menu Veiculos ---");
        printf("\n1. Adicionar Veiculo");
        printf("\n2. Remover Veiculo");
        printf("\n3. Listar Veiculos");
        printf("\n4. Ordenar Veiculos por Placa");
        printf("\n5. Gerar Veiculos Aleatorios");
        printf("\n6. Gerar Particoes Ordenadas (Selecao por Substituicao)");
        printf("\n7. Imprimir Particoes Geradas");
        printf("\n8. Intercalar Particoes (Arvore de Vencedores)");
        printf("\n9. Listar Veiculos Ordenados (arquivo final)");
        printf("\n10. Ordenar Veiculos por Selecao por Substituicao");
        printf("\n11. Inicializar Tabela Hash");
        printf("\n12. Buscar Veiculo por Placa (Hash)");
        printf("\n13. Inserir Veiculo no Hash");
        printf("\n14. Remover Veiculo do Hash");
        printf("\n15. Imprimir Tabela Hash");
        printf("\n0. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();

        switch(opcao) {
            case 1: adicionarVeiculo();
            break;
            case 2: removerVeiculo();
            break;
            case 3: listarVeiculos();
            break;
            case 4: ordenarVeiculosPorPlaca();
            break;
            case 5: {
                int qtd;
                printf("Quantos veiculos deseja gerar? ");
                scanf("%d", &qtd);
                getchar();
                gerarVeiculosAleatorios(qtd);
                break;
            }
            case 6: {
                FILE *arq = fopen("veiculos.dat", "rb");
                if (!arq) { printf("Erro ao abrir veiculos.dat\n"); break; }
                int qtd_particoes = selecao_por_substituicao_veiculos(arq, 5);
                fclose(arq);
                printf("%d particoes geradas com sucesso!\n", qtd_particoes);
                break;
            }
            case 7: {
                int qtd;
                printf("Quantas particoes deseja imprimir? ");
                scanf("%d", &qtd);
                getchar();
                imprime_particoes_veiculos(qtd);
                break;
            }
            case 8: {
                int qtd;
                printf("Quantas particoes deseja intercalar? ");
                scanf("%d", &qtd);
                getchar();
                intercalacao_arvore_vencedores(qtd, "veiculos_ordenado.dat");
                break;
            }
            case 9: {
                FILE *f = fopen("veiculos_ordenado.dat", "rb");
                if (!f) { printf("Arquivo veiculos_ordenado.dat nao encontrado.\n"); break; }
                Veiculo *v;
                printf("\n==== Veiculos Ordenados ====\n");
                while ((v = le_veiculo(f)) != NULL) {
                    imprime_veiculo(v);
                    free(v);
                }
                fclose(f);
                break;
            }
            case 10: ordenarVeiculosPorSelecaoSubstituicao();
            break;
            case 11: {
                FILE *tabHash = fopen("hash_veiculos.dat", "wb+");
                if (!tabHash) { printf("Erro ao criar tabela hash!\n"); break; }
                initTabela(tabHash);
                fclose(tabHash);
                FILE *listas = fopen("hash_listas.dat", "wb");
                if (listas) fclose(listas);
                printf("Tabela hash inicializada.\n");
                break;
            }
            case 12: {
                char placa[10];
                printf("Digite a placa para buscar: ");
                fgets(placa, sizeof(placa), stdin);
                placa[strcspn(placa, "\n")] = '\0';
                FILE *arq = fopen("hash_listas.dat", "rb");
                FILE *tabHash = fopen("hash_veiculos.dat", "rb");
                if (!arq || !tabHash) { if (arq) fclose(arq); if (tabHash) fclose(tabHash); printf("Arquivos nao encontrados!\n"); break; }
                TVeiculo v;
                if (buscaVeiculo(arq, tabHash, placa, &v)) {
                    printf("Veiculo encontrado: %s | %s | %s | %d | %s | %s\n",
                           v.placa, v.modelo, v.marca, v.ano, v.cor, v.cpf_funcionario);
                } else {
                    printf("Veiculo nao encontrado na hash.\n");
                }
                fclose(arq);
                fclose(tabHash);
                break;
            }
            case 13: {
                char placa[10];
                printf("Digite a placa do veiculo para inserir no hash: ");
                fgets(placa, sizeof(placa), stdin);
                placa[strcspn(placa, "\n")] = '\0';
                FILE *arq = fopen("hash_listas.dat", "rb+");
                FILE *tabHash = fopen("hash_veiculos.dat", "rb+");
                if (!arq || !tabHash) { if (arq) fclose(arq); if (tabHash) fclose(tabHash); printf("Arquivos nao encontrados!\n"); break; }
                TVeiculo vhash;
                if (buscaVeiculo(arq, tabHash, placa, &vhash)) {
                    printf("Veiculo ja existe na hash.\n");
                    fclose(arq);
                    fclose(tabHash);
                    break;
                }
                int idxV = buscaSequencialVeiculo(placa);
                if (idxV == -1) {
                    printf("Placa nao encontrada em veiculos.dat.\n");
                    fclose(arq);
                    fclose(tabHash);
                    break;
                }
                Veiculo vfile;
                if (buscarVeiculoPorIndice(idxV, &vfile) != 0) {
                    printf("Falha ao ler veiculo.\n");
                    fclose(arq);
                    fclose(tabHash);
                    break;
                }
                TVeiculo v;
                strcpy(v.placa, vfile.placa);
                strcpy(v.modelo, vfile.modelo);
                strcpy(v.marca, vfile.marca);
                v.ano = vfile.ano;
                strcpy(v.cor, vfile.cor);
                strcpy(v.cpf_funcionario, vfile.cpf_funcionario);
                v.prox = -1;
                v.ocupado = true;
                if (insereVeiculo(arq, tabHash, &v)) printf("Veiculo inserido no hash com sucesso!\n");
                else printf("Erro ao inserir veiculo no hash.\n");
                fclose(arq);
                fclose(tabHash);
                break;
            }
            case 14: {
                char placa[10];
                printf("Digite a placa do veiculo para remover do hash: ");
                fgets(placa, sizeof(placa), stdin);
                placa[strcspn(placa, "\n")] = '\0';
                FILE *arq = fopen("hash_listas.dat", "rb+");
                FILE *tabHash = fopen("hash_veiculos.dat", "rb+");
                if (!arq || !tabHash) { if (arq) fclose(arq); if (tabHash) fclose(tabHash); printf("Arquivos nao encontrados!\n"); break; }
                if (removeVeiculo(arq, tabHash, placa)) printf("Veiculo removido da hash com sucesso!\n");
                else printf("Veiculo nao encontrado na hash.\n");
                fclose(arq);
                fclose(tabHash);
                break;
            }
            case 15: {
                FILE *tabHash = fopen("hash_veiculos.dat", "rb");
                if (!tabHash) { printf("Tabela hash nao encontrada!\n"); break; }
                printaTabela(tabHash);
                fclose(tabHash);
                break;
            }
            case 0:
            break;
            default: printf("Opcao invalida!\n");
        }
    } while(opcao != 0);
}

void menuLocacoes() {
    int opcao;
    do {
        printf("\n--- Menu Locacoes ---");
        printf("\n1. Criar Locacao");
        printf("\n2. Finalizar Locacao");
        printf("\n3. Listar Todas as Locacoes");
        printf("\n4. Locacoes por Funcionario");
        printf("\n5. Locacoes por Veiculo");
        printf("\n6. Ordenar Locacoes por Data");
        printf("\n7. Ordenar Locacoes por CPF e Placa");
        printf("\n8. Gerar Locacoes Aleatorias");
        printf("\n0. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();
        switch(opcao) {
            case 1: criarLocacao(); break;
            case 2: finalizarLocacao(); break;
            case 3: listarLocacoes(); break;
            case 4: listarLocacoesPorFuncionario(); break;
            case 5: listarLocacoesPorVeiculo(); break;
            case 6: ordenarLocacoesPorData(); break;
            case 7: ordenarLocacoesPorCPFPlaca(); break;
            case 8: {
                int qtd;
                printf("Quantas locacoes deseja gerar? ");
                scanf("%d", &qtd);
                getchar();
                gerarLocacoesAleatorias(qtd);
                break;
            }
            case 0: break;
            default: printf("Opcao invalida!\n");
        }
    } while(opcao != 0);
}
