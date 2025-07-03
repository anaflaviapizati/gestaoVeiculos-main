#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funcionarios.h"
int totalFuncionarios = 0;

void atualizarTotalFuncionarios() {
    totalFuncionarios = contarFuncionarios();
}

void adicionarFuncionario() {
    Funcionario novo;
    printf("\n--- Cadastro de Funcionario ---\n");
    printf("CPF: ");
    fgets(novo.cpf, sizeof(novo.cpf), stdin);
    novo.cpf[strcspn(novo.cpf, "\n")] = '\0';

    if (buscaSequencialFuncionario(novo.cpf) != -1) {
        printf("Ja existe um funcionario com este CPF!\n");
        return;
    }

    printf("Nome: ");
    fgets(novo.nome, sizeof(novo.nome), stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';
    printf("Idade: ");
    scanf("%d", &novo.idade);
    getchar();

    FILE *file = fopen("funcionarios.dat", "ab");
    if (!file) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }
    fwrite(&novo, sizeof(Funcionario), 1, file);
    fclose(file);
    printf("Funcionario cadastrado com sucesso!\n");
}

void listarFuncionarios() {
    FILE *file = fopen("funcionarios.dat", "rb");
    if (!file) {
        printf("Nenhum funcionario cadastrado.\n");
        return;
    }

    printf("\n--- Lista de Funcionarios ---\n");
    Funcionario f;
    int i = 1;
    while (fread(&f, sizeof(Funcionario), 1, file)) {
        printf("%d) CPF: %s | Nome: %s | Idade: %d\n", i++, f.cpf, f.nome, f.idade);
    }

    fclose(file);
}

void removerFuncionario() {
    char cpf[15];
    printf("Informe o CPF a remover: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = '\0';

    FILE *orig = fopen("funcionarios.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (!orig || !temp) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    Funcionario f;
    int removido = 0;
    while (fread(&f, sizeof(Funcionario), 1, orig)) {
        if (strcmp(f.cpf, cpf) != 0) {
            fwrite(&f, sizeof(Funcionario), 1, temp);
        } else {
            removido = 1;
        }
    }

    fclose(orig);
    fclose(temp);

    remove("funcionarios.dat");
    rename("temp.dat", "funcionarios.dat");

    if (removido)
        printf("Funcionario removido com sucesso!\n");
    else
        printf("Funcionario nao encontrado.\n");
}

int buscaSequencialFuncionario(const char *cpf) {
    FILE *file = fopen("funcionarios.dat", "rb");
    if (!file) return -1;

    Funcionario f;
    int index = 0;
    while (fread(&f, sizeof(Funcionario), 1, file)) {
        if (strcmp(f.cpf, cpf) == 0) {
            fclose(file);
            return index;
        }
        index++;
    }
    fclose(file);
    return -1;
}

int buscarFuncionarioPorIndice(int indice, Funcionario *f) {
    FILE *file = fopen("funcionarios.dat", "rb");
    if (!file) return -1;

    fseek(file, indice * sizeof(Funcionario), SEEK_SET);
    if (fread(f, sizeof(Funcionario), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

void ordenarFuncionariosPorCPF() {
    FILE *file = fopen("funcionarios.dat", "rb");
    if (!file) {
        printf("Arquivo de funcionarios nao encontrado!\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long total = ftell(file) / sizeof(Funcionario);
    fclose(file);

    if (total == 0) {
        printf("Nenhum funcionario para ordenar.\n");
        return;
    }

    FILE *original = fopen("funcionarios.dat", "rb");
    FILE *ordenado = fopen("ordenado.dat", "wb+");
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
        Funcionario menor;
        long posMenor = -1;

        for (int j = 0; j < total; j++) {
            if (usados[j]) continue;

            Funcionario atual;
            fseek(original, j * sizeof(Funcionario), SEEK_SET);
            fread(&atual, sizeof(Funcionario), 1, original);

            if (posMenor == -1 || strcmp(atual.cpf, menor.cpf) < 0) {
                menor = atual;
                posMenor = j;
            }
        }

        fseek(ordenado, i * sizeof(Funcionario), SEEK_SET);
        fwrite(&menor, sizeof(Funcionario), 1, ordenado);
        usados[posMenor] = 1;
    }

    free(usados);
    fclose(original);
    fclose(ordenado);

    remove("funcionarios.dat");
    rename("ordenado.dat", "funcionarios.dat");

    printf("Funcionarios ordenados!\n");
}

void gerarFuncionariosAleatorios(int quantidade) {
    FILE *file = fopen("funcionarios.dat", "ab");
    if (!file) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    const char *nomes[] = {"Ana", "Bruno", "Carlos", "Daniela", "Eduardo", "Fernanda", "Gabriel", "Helena", "Igor", "Juliana"};
    const char *sobrenomes[] = {"Silva", "Santos", "Oliveira", "Souza", "Costa"};
    int totalNomes = sizeof(nomes) / sizeof(nomes[0]);
    int totalSobrenomes = sizeof(sobrenomes) / sizeof(sobrenomes[0]);

    srand(time(NULL));
    int gerados = 0;

    while (gerados < quantidade) {
        Funcionario f;
        sprintf(f.cpf, "%03d.%03d.%03d-%02d", rand() % 1000, rand() % 1000, rand() % 1000, rand() % 100);

        if (buscaSequencialFuncionario(f.cpf) != -1) continue;

        snprintf(f.nome, sizeof(f.nome), "%s %s", nomes[rand() % totalNomes], sobrenomes[rand() % totalSobrenomes]);
        f.idade = 18 + rand() % 43;

        fwrite(&f, sizeof(Funcionario), 1, file);
        gerados++;
    }

    fclose(file);
    printf("%d funcionarios gerados com sucesso!\n", quantidade);
}

int contarFuncionarios() {
    FILE *file = fopen("funcionarios.dat", "rb");
    if (!file) return 0;
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fclose(file);
    return tamanho / sizeof(Funcionario);
}
