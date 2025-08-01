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

int buscaBinariaVeiculo(const char *placa) {
    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    int total = tamanho / sizeof(Veiculo);

    int inicio = 0;
    int fim = total - 1;

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;

        Veiculo v;
        fseek(file, meio * sizeof(Veiculo), SEEK_SET);
        fread(&v, sizeof(Veiculo), 1, file);

        int cmp = strcmp(placa, v.placa);
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

    if (buscaBinariaVeiculo(novo.placa) != -1) {
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

    if (buscaBinariaFuncionario(novo.cpf_funcionario) == -1) {
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

void ordenarVeiculosPorSelecaoSubstituicao() {
    FILE *arq = fopen("veiculos.dat", "rb");
    if (!arq) {
        printf("Arquivo de veiculos nao encontrado!\n");
        return;
    }
    FILE *log = fopen("log_ordenacao.txt", "a");
    if (log) {
        fprintf(log, "Iniciando ordenação por seleção por substituição...\n");
    }
    int mem_size = 5; // Tamanho da memória, pode ser ajustado
    int qtd_particoes = selecao_por_substituicao_veiculos(arq, mem_size);
    fclose(arq);
    printf("%d particoes geradas com sucesso!\n", qtd_particoes);
    if (log) {
        fprintf(log, "%d partições geradas com sucesso.\n", qtd_particoes);
        fprintf(log, "Iniciando intercalação ótima (árvore de vencedores)...\n");
    }

    // Intercala as partições geradas em um arquivo final ordenado
    intercalacao_arvore_vencedores(qtd_particoes, "veiculos_ordenado.dat");
    printf("Arquivo veiculos_ordenado.dat gerado com sucesso!\n");
    if (log) {
        fprintf(log, "Arquivo veiculos_ordenado.dat gerado com sucesso.\n");
    }

    // Substitui o arquivo original pelo ordenado
    if (remove("veiculos.dat") == 0) {
        if (rename("veiculos_ordenado.dat", "veiculos.dat") == 0) {
            printf("Arquivo veiculos.dat foi substituido pelo arquivo ordenado com sucesso!\n");
            if (log) {
                fprintf(log, "Arquivo veiculos.dat foi substituído pelo arquivo ordenado com sucesso.\n");
            }
        } else {
            printf("Erro ao renomear veiculos_ordenado.dat para veiculos.dat\n");
            if (log) {
                fprintf(log, "Erro ao renomear veiculos_ordenado.dat para veiculos.dat.\n");
            }
        }
    } else {
        printf("Erro ao remover veiculos.dat original. O arquivo ordenado foi salvo como veiculos_ordenado.dat\n");
        if (log) {
            fprintf(log, "Erro ao remover veiculos.dat original. O arquivo ordenado foi salvo como veiculos_ordenado.dat.\n");
        }
    }
    if (log) {
        fprintf(log, "---\n");
        fclose(log);
    }
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

            if (buscaBinariaVeiculo(v.placa) != -1) {
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

 int gerarParticoesVeiculos() {
       FILE *entrada = fopen("veiculos.dat", "rb");
       if (!entrada) {
           printf("Erro ao abrir veiculos.dat\n");
           return 0;
       }
       // Implementa��o completa da fun��o aqui
       // (use o c�digo que eu te mostrei anteriormente)
       fclose(entrada);
       return 1; // Sucesso
   }

