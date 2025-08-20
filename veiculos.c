#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "veiculos.h"
#include "funcionarios.h"

typedef struct {
    char placa[10];
    int indice;
    int ocupado;
} HashEntryVeiculo;

static HashEntryVeiculo *tabelaHashVeiculos = NULL;
static int tamanhoTabelaHashVeiculos = 0;
static int quantidadeInseridaHashVeiculos = 0;

static void invalidarIndiceHashInterno() {
    if (tabelaHashVeiculos != NULL) {
        free(tabelaHashVeiculos);
        tabelaHashVeiculos = NULL;
    }
    tamanhoTabelaHashVeiculos = 0;
    quantidadeInseridaHashVeiculos = 0;
}

static int ehPrimo(int n) {
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

static int proximoPrimo(int n) {
    if (n <= 2) return 2;
    if (n % 2 == 0) n++;
    while (!ehPrimo(n)) n += 2;
    return n;
}

static unsigned int hashPlaca(const char *placa) {
    unsigned int h = 0u;
    for (const unsigned char *p = (const unsigned char *)placa; *p; ++p) {
        h = h * 131u + (unsigned int)(*p);
    }
    return h;
}

static void inserirNaHash(const char *placa, int indice) {
    unsigned int h = hashPlaca(placa);
    int pos = (int)(h % (unsigned int)tamanhoTabelaHashVeiculos);
    while (tabelaHashVeiculos[pos].ocupado) {
        if (strcmp(tabelaHashVeiculos[pos].placa, placa) == 0) {
            tabelaHashVeiculos[pos].indice = indice;
            return;
        }
        pos = (pos + 1) % tamanhoTabelaHashVeiculos;
    }
    strncpy(tabelaHashVeiculos[pos].placa, placa, sizeof(tabelaHashVeiculos[pos].placa));
    tabelaHashVeiculos[pos].placa[sizeof(tabelaHashVeiculos[pos].placa) - 1] = '\0';
    tabelaHashVeiculos[pos].indice = indice;
    tabelaHashVeiculos[pos].ocupado = 1;
    quantidadeInseridaHashVeiculos++;
}

void construirIndiceHashVeiculos() {
    invalidarIndiceHashInterno();
    FILE *file = fopen("veiculos.dat", "rb");
    if (!file) {
        printf("Nenhum veiculo para indexar (veiculos.dat nao encontrado).\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long tamanhoBytes = ftell(file);
    int total = (int)(tamanhoBytes / (long)sizeof(Veiculo));
    if (total <= 0) {
        fclose(file);
        printf("Nenhum veiculo para indexar.\n");
        return;
    }
    int capacidade = proximoPrimo(total * 2 + 1);
    tabelaHashVeiculos = (HashEntryVeiculo *)calloc((size_t)capacidade, sizeof(HashEntryVeiculo));
    if (!tabelaHashVeiculos) {
        fclose(file);
        printf("Falha ao alocar memoria para indice hash de veiculos.\n");
        return;
    }
    tamanhoTabelaHashVeiculos = capacidade;
    quantidadeInseridaHashVeiculos = 0;

    // Varre o arquivo e insere na tabela
    Veiculo v;
    int idx = 0;
    rewind(file);
    while (fread(&v, sizeof(Veiculo), 1, file) == 1) {
        inserirNaHash(v.placa, idx);
        idx++;
    }
    fclose(file);
    printf("Indice hash construido: %d registros em %d buckets.\n", quantidadeInseridaHashVeiculos, tamanhoTabelaHashVeiculos);
}

int buscarVeiculoHash(const char *placa) {
    if (!tabelaHashVeiculos || tamanhoTabelaHashVeiculos == 0) {
        construirIndiceHashVeiculos();
        if (!tabelaHashVeiculos || tamanhoTabelaHashVeiculos == 0) return -1;
    }
    unsigned int h = hashPlaca(placa);
    int pos = (int)(h % (unsigned int)tamanhoTabelaHashVeiculos);
    int start = pos;
    while (tabelaHashVeiculos[pos].ocupado) {
        if (strcmp(tabelaHashVeiculos[pos].placa, placa) == 0) {
            return tabelaHashVeiculos[pos].indice;
        }
        pos = (pos + 1) % tamanhoTabelaHashVeiculos;
        if (pos == start) break;
    }
    return -1;
}

void buscarVeiculoPorPlacaHash() {
    char placa[10];
    printf("\n--- Busca por Placa (Hash) ---\n");
    printf("Qual placa voce quer? ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = '\0';

    int indice = buscarVeiculoHash(placa);
    if (indice == -1) {
        printf("Placa %s nao foi encontrada!\n", placa);
        return;
    }
    Veiculo v;
    if (buscarVeiculoPorIndice(indice, &v) == 0) {
        printf("Placa: %s\nModelo: %s\nMarca: %s\nAno: %d\nCor: %s\nCPF do responsavel: %s\n",
            v.placa, v.modelo, v.marca, v.ano, v.cor, v.cpf_funcionario);
    } else {
        printf("Erro ao carregar veiculo no indice %d.\n", indice);
    }
}

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

