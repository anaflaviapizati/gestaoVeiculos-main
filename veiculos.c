#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "veiculos.h"
#include "funcionarios.h"

int carregarTotalVeiculos() {
    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) return 0;
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fclose(file);
    return tamanho / sizeof(Veiculo);
}

int buscaSequencialVeiculo(const char *placa) {
    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) return -1;

    Veiculo v;
    int idx = 0;
    while (fread(&v, sizeof(Veiculo), 1, file)) {
        if (strcmp(v.placa, placa) == 0) {
            fclose(file);
            return idx;
        }
        idx++;
    }
    fclose(file);
    return -1;
}

int buscarVeiculoPorIndice(int indice, Veiculo *v) {
    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) return -1;
    fseek(file, indice * sizeof(Veiculo), SEEK_SET);
    if (fread(v, sizeof(Veiculo), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

void adicionarVeiculo() {
    Veiculo novo;
    printf("\n--- Cadastro de Veiculo ---\n");
    printf("Placa: ");
    fgets(novo.placa, sizeof(novo.placa), stdin);
    novo.placa[strcspn(novo.placa, "\n")] = '\0';

    if (buscaSequencialVeiculo(novo.placa) != -1) {
        printf("Ja existe um veiculo com essa placa!\n");
        return;
    }

    printf("Modelo: ");
    fgets(novo.modelo, sizeof(novo.modelo), stdin);
    novo.modelo[strcspn(novo.modelo, "\n")] = '\0';

    printf("Marca: ");
    fgets(novo.marca, sizeof(novo.marca), stdin);
    novo.marca[strcspn(novo.marca, "\n")] = '\0';

    printf("Ano: ");
    scanf("%d", &novo.ano);
    getchar();

    printf("Cor: ");
    fgets(novo.cor, sizeof(novo.cor), stdin);
    novo.cor[strcspn(novo.cor, "\n")] = '\0';

    printf("CPF do funcionario responsavel: ");
    fgets(novo.cpf_funcionario, sizeof(novo.cpf_funcionario), stdin);
    novo.cpf_funcionario[strcspn(novo.cpf_funcionario, "\n")] = '\0';

    // Verifica se funcionário existe no arquivo funcionarios.dat
    if (buscaSequencialFuncionario(novo.cpf_funcionario) == -1) {
        printf("Funcionario com esse CPF nao encontrado!\n");
        return;
    }

    FILE *file = fopen("veiculos.dat", "ab");
    if (!file) {
        printf("Erro ao salvar veiculo!\n");
        return;
    }
    fwrite(&novo, sizeof(Veiculo), 1, file);
    fclose(file);
    printf("Veiculo salvo com sucesso!\n");
}

void listarVeiculos() {
    printf("\n--- Lista de Veiculos ---\n");
    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) {
        printf("Nenhum veiculo cadastrado.\n");
        return;
    }

    Veiculo v;
    int i = 1;
    while (fread(&v, sizeof(Veiculo), 1, file)) {
        printf("%d) Placa: %s | Modelo: %s | Marca: %s | Ano: %d | Cor: %s | CPF Resp.: %s\n",
               i++, v.placa, v.modelo, v.marca, v.ano, v.cor, v.cpf_funcionario);
    }
    fclose(file);
}

void buscarVeiculoPorPlaca() {
    char placa[10];
    printf("\n--- Busca por Placa ---\n");
    printf("Qual placa voce quer? ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = '\0';

    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) {
        printf("Arquivo de veiculos nao encontrado!\n");
        return;
    }

    Veiculo v;
    int encontrado = 0;
    while (fread(&v, sizeof(Veiculo), 1, file)) {
        if (strcmp(v.placa, placa) == 0) {
            printf("Placa: %s\nModelo: %s\nMarca: %s\nAno: %d\nCor: %s\nCPF do responsavel: %s\n",
                   v.placa, v.modelo, v.marca, v.ano, v.cor, v.cpf_funcionario);
            encontrado = 1;
            break;
        }
    }
    if (!encontrado) {
        printf("Placa %s nao foi encontrada!\n", placa);
    }
    fclose(file);
}

void removerVeiculo() {
    char placa[10];
    printf("\n--- Remover Veiculo ---\n");
    printf("Digite a placa que quer remover: ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = '\0';

    FILE *original = fopen("veiculos.dat", "rb");
    FILE *temp = fopen("temp_veiculos.dat", "wb");
    if (!original || !temp) {
        printf("Erro abrindo os arquivos!");
        if (original) fclose(original);
        if (temp) fclose(temp);
        return;
    }

    Veiculo v;
    int removido = 0;
    while (fread(&v, sizeof(Veiculo), 1, original)) {
        if (strcmp(v.placa, placa) == 0) {
            removido = 1;
            continue;
        }
        fwrite(&v, sizeof(Veiculo), 1, temp);
    }
    fclose(original);
    fclose(temp);

    if (removido) {
        remove("veiculos.dat");
        rename("temp_veiculos.dat", "veiculos.dat");
        printf("Veiculo removido com sucesso!\n");
    } else {
        remove("temp_veiculos.dat");
        printf("Placa %s nao encontrada!\n", placa);
    }
}

void editarVeiculo() {
    char placa[10];
    printf("\n--- editar veiculo ---\n");
    printf("placa do bicho q vc quer mudar: ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = '\0';

    FILE *original = fopen("veiculos.dat", "rb");
    FILE *temp = fopen("temp_veiculos.dat", "wb");
    if (!original || !temp) {
        printf("deu ruim abrindo os arquivos\n");
        if (original) fclose(original);
        if (temp) fclose(temp);
        return;
    }

    Veiculo v;
    int encontrado = 0;
    while (fread(&v, sizeof(Veiculo), 1, original)) {
        if (strcmp(v.placa, placa) == 0) {
            encontrado = 1;
            printf("modelo: ");
            fgets(v.modelo, sizeof(v.modelo), stdin);
            v.modelo[strcspn(v.modelo, "\n")] = '\0';

            printf("marca: ");
            fgets(v.marca, sizeof(v.marca), stdin);
            v.marca[strcspn(v.marca, "\n")] = '\0';

            printf("ano: ");
            scanf("%d", &v.ano);
            getchar();

            printf("cor: ");
            fgets(v.cor, sizeof(v.cor), stdin);
            v.cor[strcspn(v.cor, "\n")] = '\0';

            printf("cpf do responsavel: ");
            fgets(v.cpf_funcionario, sizeof(v.cpf_funcionario), stdin);
            v.cpf_funcionario[strcspn(v.cpf_funcionario, "\n")] = '\0';
        }
        fwrite(&v, sizeof(Veiculo), 1, temp);
    }
    fclose(original);
    fclose(temp);

    if (encontrado) {
        remove("veiculos.dat");
        rename("temp_veiculos.dat", "veiculos.dat");
        printf("mudou tudo, prontinho\n");
    } else {
        remove("temp_veiculos.dat");
        printf("placa %s nao encontrada...\n", placa);
    }
}

void ordenarVeiculosPorPlaca() {
    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) {
        printf("Arquivo de veiculos nao encontrado!\n");
        return;
    }
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    int total = tamanho / sizeof(Veiculo);
    fclose(file);

    if (total == 0) {
        printf("Nenhum veiculo para ordenar.\n");
        return;
    }


    FILE *original = fopen("veiculos.dat", "rb+");
    if (!original) {
        printf("Erro ao abrir arquivo para ordenacao\n");
        return;
    }

    Veiculo v1, v2;
    int trocou;
    
    do {
        trocou = 0;
        rewind(original);
        
        for (int i = 0; i < total - 1; i++) {
            fseek(original, i * sizeof(Veiculo), SEEK_SET);
            fread(&v1, sizeof(Veiculo), 1, original);
            fread(&v2, sizeof(Veiculo), 1, original);
            
            if (strcmp(v1.placa, v2.placa) > 0) {
                fseek(original, i * sizeof(Veiculo), SEEK_SET);
                fwrite(&v2, sizeof(Veiculo), 1, original);
                fwrite(&v1, sizeof(Veiculo), 1, original);
                trocou = 1;
            }
        }
    } while (trocou);

    fclose(original);
    printf("Veiculos ordenados por placa!\n");
}

void gerarVeiculosAleatorios(int quantidade) {
    if (quantidade <= 0) return;

    FILE *file = fopen("veiculos.dat", "ab");
    if (!file) {
        printf("Erro ao abrir arquivo para gravar veiculos aleatorios\n");
        return;
    }

    srand(time(NULL));
    const char *modelos[] = {"Gol", "Civic", "Uno", "Corolla", "Palio", "Focus", "Fiesta", "HB20", "Sandero"};
    const char *marcas[] = {"Volkswagen", "Honda", "Fiat", "Toyota", "Ford", "Hyundai", "Renault"};
    const char *cores[] = {"Preto", "Branco", "Vermelho", "Azul", "Prata", "Cinza", "Verde"};
    int numModelos = sizeof(modelos)/sizeof(modelos[0]);
    int numMarcas = sizeof(marcas)/sizeof(marcas[0]);
    int numCores = sizeof(cores)/sizeof(cores[0]);

    int totalFuncionarios = contarFuncionarios();
    if (totalFuncionarios == 0) {
        printf("Nenhum funcionario cadastrado para associar veiculo.\n");
        fclose(file);
        return;
    }

    for (int i = 0; i < quantidade; ) {
        Veiculo v;

        int valido;
        do {
            valido = 1;
            for (int j = 0; j < 3; j++) {
                v.placa[j] = 'A' + (rand() % 26);
            }
            for (int j = 3; j < 7; j++) {
                v.placa[j] = '0' + (rand() % 10);
            }
            v.placa[7] = '\0';

            if (buscaSequencialVeiculo(v.placa) != -1) {
                valido = 0;
            }
        } while (!valido);

        strcpy(v.modelo, modelos[rand() % numModelos]);
        strcpy(v.marca, marcas[rand() % numMarcas]);
        v.ano = 2000 + rand() % 23;
        strcpy(v.cor, cores[rand() % numCores]);

        int idxFunc = rand() % totalFuncionarios;
        Funcionario f;
        if (buscarFuncionarioPorIndice(idxFunc, &f) == -1) {
            printf("Erro ao carregar funcionario para veiculo aleatorio.\n");
            fclose(file);
            return;
        }
        strcpy(v.cpf_funcionario, f.cpf);

        fwrite(&v, sizeof(Veiculo), 1, file);
        i++;
    }

    fclose(file);
    printf("%d veiculos aleatorios gerados com sucesso!\n", quantidade);
}

int contarVeiculos() {
    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) return 0;
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fclose(file);
    return tamanho / sizeof(Veiculo);
}

