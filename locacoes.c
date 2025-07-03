#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "locacoes.h"
#include "funcionarios.h"
#include "veiculos.h"

void criarLocacao() {
    Locacao nova;
    printf("\n--- Criar Nova Locacao ---\n");

    printf("CPF do funcionario: ");
    fgets(nova.cpf_funcionario, sizeof(nova.cpf_funcionario), stdin);
    nova.cpf_funcionario[strcspn(nova.cpf_funcionario, "\n")] = '\0';

    int idx_func = buscaSequencialFuncionario(nova.cpf_funcionario);
    if (idx_func == -1) {
        printf("Funcionario nao encontrado!\n");
        return;
    }

    printf("Placa do veiculo: ");
    fgets(nova.placa_veiculo, sizeof(nova.placa_veiculo), stdin);
    nova.placa_veiculo[strcspn(nova.placa_veiculo, "\n")] = '\0';

    int idx_veic = buscaSequencialVeiculo(nova.placa_veiculo);
    if (idx_veic == -1) {
        printf("Veiculo nao encontrado!\n");
        return;
    }

    FILE *file = fopen("locacoes.dat", "rb");
    if (file) {
        Locacao l;
        while (fread(&l, sizeof(Locacao), 1, file)) {
            if (strcmp(l.placa_veiculo, nova.placa_veiculo) == 0 &&
                strcmp(l.status, "ATIVA") == 0) {
                printf("Veiculo ja esta em locacao ativa!\n");
                fclose(file);
                return;
            }
        }
        fclose(file);
    }

    printf("Data de inicio (DD/MM/AAAA): ");
    fgets(nova.data_inicio, sizeof(nova.data_inicio), stdin);
    nova.data_inicio[strcspn(nova.data_inicio, "\n")] = '\0';

    printf("Data de fim (DD/MM/AAAA): ");
    fgets(nova.data_fim, sizeof(nova.data_fim), stdin);
    nova.data_fim[strcspn(nova.data_fim, "\n")] = '\0';

    strcpy(nova.status, "ATIVA");

    FILE *arquivo = fopen("locacoes.dat", "ab");
    if (!arquivo) {
        printf("Erro ao abrir arquivo de locacoes!\n");
        return;
    }
    fwrite(&nova, sizeof(Locacao), 1, arquivo);
    fclose(arquivo);

    printf("Locacao criada com sucesso!\n");
}

void finalizarLocacao() {
    char cpf[15], placa[10];
    printf("\n--- Finalizar Locacao ---\n");
    printf("CPF do funcionario: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = '\0';

    printf("Placa do veiculo: ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = '\0';

    FILE *original = fopen("locacoes.dat", "rb");
    FILE *temp = fopen("temp_locacoes.dat", "wb");
    if (!original || !temp) {
        printf("Erro ao abrir arquivos!\n");
        return;
    }

    Locacao l;
    int finalizada = 0;
    while (fread(&l, sizeof(Locacao), 1, original)) {
        if (strcmp(l.cpf_funcionario, cpf) == 0 &&
            strcmp(l.placa_veiculo, placa) == 0 &&
            strcmp(l.status, "ATIVA") == 0) {
            strcpy(l.status, "FINALIZADA");
            finalizada = 1;
        }
        fwrite(&l, sizeof(Locacao), 1, temp);
    }

    fclose(original);
    fclose(temp);

    if (finalizada) {
        remove("locacoes.dat");
        rename("temp_locacoes.dat", "locacoes.dat");
        printf("Locacao finalizada com sucesso!\n");
    } else {
        remove("temp_locacoes.dat");
        printf("Locacao ativa nao encontrada!\n");
    }
}

void listarLocacoes() {
    printf("\n--- Lista de Todas as Locacoes ---\n");
    FILE *file = fopen("locacoes.dat", "rb");
    if (!file) {
        printf("Nenhuma locacao cadastrada.\n");
        return;
    }

    Locacao l;
    int i = 1;
    while (fread(&l, sizeof(Locacao), 1, file)) {
        printf("%d) CPF: %s | Placa: %s | Inicio: %s | Fim: %s | Status: %s\n",
            i++,
            l.cpf_funcionario,
            l.placa_veiculo,
            l.data_inicio,
            l.data_fim,
            l.status);
    }
    fclose(file);
}

void listarLocacoesPorFuncionario() {
    char cpf[15];
    printf("\n--- Locacoes por Funcionario ---\n");
    printf("CPF do funcionario: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = '\0';

    FILE *file = fopen("locacoes.dat", "rb");
    if (!file) {
        printf("Nenhuma locacao cadastrada.\n");
        return;
    }

    int encontrou = 0;
    Locacao l;
    while (fread(&l, sizeof(Locacao), 1, file)) {
        if (strcmp(l.cpf_funcionario, cpf) == 0) {
            if (!encontrou) {
                printf("Locacoes do funcionario %s:\n", cpf);
                encontrou = 1;
            }
            printf("- Placa: %s | Inicio: %s | Fim: %s | Status: %s\n",
                l.placa_veiculo,
                l.data_inicio,
                l.data_fim,
                l.status);
        }
    }
    if (!encontrou) {
        printf("Nenhuma locacao encontrada para este funcionario.\n");
    }
    fclose(file);
}

void listarLocacoesPorVeiculo() {
    char placa[10];
    printf("\n--- Locacoes por Veiculo ---\n");
    printf("Placa do veiculo: ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = '\0';

    FILE *file = fopen("locacoes.dat", "rb");
    if (!file) {
        printf("Nenhuma locacao cadastrada.\n");
        return;
    }

    int encontrou = 0;
    Locacao l;
    while (fread(&l, sizeof(Locacao), 1, file)) {
        if (strcmp(l.placa_veiculo, placa) == 0) {
            if (!encontrou) {
                printf("Locacoes do veiculo %s:\n", placa);
                encontrou = 1;
            }
            printf("- CPF: %s | Inicio: %s | Fim: %s | Status: %s\n",
                l.cpf_funcionario,
                l.data_inicio,
                l.data_fim,
                l.status);
        }
    }
    if (!encontrou) {
        printf("Nenhuma locacao encontrada para este veiculo.\n");
    }
    fclose(file);
}

int buscaSequencialLocacao(const char *cpf, const char *placa) {
    FILE *file = fopen("locacoes.dat", "rb");
    if (!file) return -1;

    Locacao l;
    int index = 0;
    while (fread(&l, sizeof(Locacao), 1, file)) {
        if (strcmp(l.cpf_funcionario, cpf) == 0 &&
            strcmp(l.placa_veiculo, placa) == 0) {
            fclose(file);
            return index;
        }
        index++;
    }
    fclose(file);
    return -1;
}

int buscaBinariaLocacao(const char *cpf, const char *placa) {
    return buscaSequencialLocacao(cpf, placa);
}

void ordenarLocacoesPorData() {
    FILE *file = fopen("locacoes.dat", "rb");
    if (!file) {
        printf("Arquivo de locacoes nao encontrado!\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long total = ftell(file) / sizeof(Locacao);
    fclose(file);

    if (total == 0) {
        printf("Nenhuma locacao para ordenar.\n");
        return;
    }

    FILE *original = fopen("locacoes.dat", "rb");
    FILE *ordenado = fopen("ordenado_locacoes.dat", "wb+");
    if (!original || !ordenado) {
        printf("Erro ao abrir arquivos!\n");
        return;
    }

    int *usados = calloc(total, sizeof(int));
    if (!usados) {
        printf("Erro de memoria!\n");
        fclose(original);
        fclose(ordenado);
        return;
    }

    for (int i = 0; i < total; i++) {
        Locacao menor;
        long posMenor = -1;

        for (int j = 0; j < total; j++) {
            if (usados[j]) continue;

            Locacao atual;
            fseek(original, j * sizeof(Locacao), SEEK_SET);
            fread(&atual, sizeof(Locacao), 1, original);

            if (posMenor == -1 || strcmp(atual.data_inicio, menor.data_inicio) < 0) {
                menor = atual;
                posMenor = j;
            }
        }

        fseek(ordenado, i * sizeof(Locacao), SEEK_SET);
        fwrite(&menor, sizeof(Locacao), 1, ordenado);
        usados[posMenor] = 1;
    }

    free(usados);
    fclose(original);
    fclose(ordenado);

    remove("locacoes.dat");
    rename("ordenado_locacoes.dat", "locacoes.dat");

    printf("Locacoes ordenadas por data!\n");
}

void gerarLocacoesAleatorias(int quantidade) {
    if (quantidade <= 0) return;

    int totalFuncionarios = contarFuncionarios();
    int totalVeiculos = contarVeiculos();

    if (totalFuncionarios == 0 || totalVeiculos == 0) {
        printf("Nao existem funcionarios ou veiculos cadastrados para gerar locacoes.\n");
        return;
    }

    FILE *file = fopen("locacoes.dat", "ab");
    if (!file) {
        printf("Erro ao abrir arquivo de locacoes!\n");
        return;
    }

    srand(time(NULL));
    int criadas = 0;

    for (int i = 0; i < quantidade; i++) {
        Locacao nova;

        int idxFunc = rand() % totalFuncionarios;
        Funcionario f;
        if (buscarFuncionarioPorIndice(idxFunc, &f) == -1) {
            printf("Erro ao carregar funcionario para locacao aleatoria.\n");
            continue;
        }
        strcpy(nova.cpf_funcionario, f.cpf);

        int idxVeic = rand() % totalVeiculos;
        Veiculo v;
        if (buscarVeiculoPorIndice(idxVeic, &v) == -1) {
            printf("Erro ao carregar veiculo para locacao aleatoria.\n");
            continue;
        }
        strcpy(nova.placa_veiculo, v.placa);

        int ocupado = 0;
        FILE *check = fopen("locacoes.dat", "rb");
        if (check) {
            Locacao l;
            while (fread(&l, sizeof(Locacao), 1, check)) {
                if (strcmp(l.placa_veiculo, nova.placa_veiculo) == 0 &&
                    strcmp(l.status, "ATIVA") == 0) {
                    ocupado = 1;
                    break;
                }
            }
            fclose(check);
        }
        if (ocupado) {
            i--;
            continue;
        }

        int dia_inicio = 1 + rand() % 28;
        int mes_inicio = 1 + rand() % 12;
        int ano_inicio = 2023;
        sprintf(nova.data_inicio, "%02d/%02d/%d", dia_inicio, mes_inicio, ano_inicio);

        int duracao = 1 + rand() % 10;
        int dia_fim = dia_inicio + duracao;
        int mes_fim = mes_inicio;
        int ano_fim = ano_inicio;
        if (dia_fim > 28) {
            dia_fim -= 28;
            mes_fim++;
            if (mes_fim > 12) {
                mes_fim = 1;
                ano_fim++;
            }
        }
        sprintf(nova.data_fim, "%02d/%02d/%d", dia_fim, mes_fim, ano_fim);

        strcpy(nova.status, "ATIVA");
        fwrite(&nova, sizeof(Locacao), 1, file);
        criadas++;
    }

    fclose(file);
    printf("%d locacoes aleatorias criadas com sucesso!\n", criadas);
}

int contarLocacoes() {
    FILE *file = fopen("locacoes.dat", "rb");
    if (!file) return 0;
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fclose(file);
    return tamanho / sizeof(Locacao);
}


