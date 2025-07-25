#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funcionarios.h"

void adicionarFuncionario() {
    Funcionario novo;
    printf("\n--- Cadastro de Funcionario ---\n");
    printf("CPF: ");
    fgets(novo.cpf, sizeof(novo.cpf), stdin);
    novo.cpf[strcspn(novo.cpf, "\n")] = '\0';

    if (buscaBinariaFuncionario(novo.cpf) != -1) {
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

int buscaBinariaFuncionario(const char *cpf) {
    FILE *file = fopen("funcionarios.dat", "rb");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    int total = tamanho / sizeof(Funcionario);

    int inicio = 0;
    int fim = total - 1;

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;

        Funcionario f;
        fseek(file, meio * sizeof(Funcionario), SEEK_SET);
        fread(&f, sizeof(Funcionario), 1, file);

        int cmp = strcmp(cpf, f.cpf);
        if (cmp == 0) {
            fclose(file);
            return meio;
        }
        if (cmp < 0) {
            fim = meio - 1;
        } else {
            inicio = meio + 1;
        }
    }

    fclose(file);
    return -1;
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

    FILE *original = fopen("funcionarios.dat", "rb+");
    if (!original) {
        printf("Erro ao abrir arquivo para ordenacao!\n");
        return;
    }

    Funcionario f1, f2;
    int trocou;
    
    do {
        trocou = 0;
        rewind(original);
        
        for (int i = 0; i < total - 1; i++) {
            fseek(original, i * sizeof(Funcionario), SEEK_SET);
            fread(&f1, sizeof(Funcionario), 1, original);
            fread(&f2, sizeof(Funcionario), 1, original);
            
            if (strcmp(f1.cpf, f2.cpf) > 0) {
                fseek(original, i * sizeof(Funcionario), SEEK_SET);
                fwrite(&f2, sizeof(Funcionario), 1, original);
                fwrite(&f1, sizeof(Funcionario), 1, original);
                trocou = 1;
            }
        }
    } while (trocou);

    fclose(original);
    printf("Funcionarios ordenados por CPF!\n");
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

        if (buscaBinariaFuncionario(f.cpf) != -1) continue;

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
