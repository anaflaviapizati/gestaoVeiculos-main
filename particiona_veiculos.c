#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculos.h"

Veiculo* le_veiculo(FILE *arq) {
    Veiculo *v = malloc(sizeof(Veiculo));
    if (fread(v, sizeof(Veiculo), 1, arq) != 1) {
        free(v);
        return NULL;
    }
    return v;
}

void salva_veiculo(Veiculo *v, FILE *arq) {
    fwrite(v, sizeof(Veiculo), 1, arq);
}

void imprime_veiculo(Veiculo *v) {
    printf("Placa: %s\n", v->placa);
    printf("Modelo: %s\n", v->modelo);
    printf("Marca: %s\n", v->marca);
    printf("Ano: %d\n", v->ano);
    printf("Cor: %s\n", v->cor);
    printf("CPF Funcionario: %s\n", v->cpf_funcionario);
}

int menor_nao_congelado(Veiculo **mem, int *congelado, int tamanho) {
    int idx_menor = -1;
    for (int i = 0; i < tamanho; i++) {
        if (!congelado[i] && mem[i] != NULL) {
            if (idx_menor == -1 || strcmp(mem[i]->placa, mem[idx_menor]->placa) < 0) {
                idx_menor = i;
            }
        }
    }
    return idx_menor;
}

int selecao_por_substituicao_veiculos(FILE *arq, int mem_size) {
    rewind(arq);

    Veiculo **memoria = malloc(mem_size * sizeof(Veiculo *));
    int *congelado = calloc(mem_size, sizeof(int));

    int qtd_particoes = 0;

    for (int i = 0; i < mem_size; i++) {
        memoria[i] = le_veiculo(arq);
        if (memoria[i] == NULL) {
            mem_size = i;
            break;
        }
        congelado[i] = 0;
    }

    char ultima_placa_gravada[10] = "";

    while (mem_size > 0) {
        char nome_part[30];
        sprintf(nome_part, "particao_%d.dat", qtd_particoes++);
        FILE *saida = fopen(nome_part, "wb");
        if (!saida) {
            perror("Erro ao criar partição");
            break;
        }

        for (int i = 0; i < mem_size; i++)
            congelado[i] = 0;

        strcpy(ultima_placa_gravada, "");

        while (1) {
            int idx = menor_nao_congelado(memoria, congelado, mem_size);
            if (idx == -1) break;

            salva_veiculo(memoria[idx], saida);
            strcpy(ultima_placa_gravada, memoria[idx]->placa);

            Veiculo *novo = le_veiculo(arq);
            if (novo != NULL) {
                if (strcmp(novo->placa, ultima_placa_gravada) < 0)
                    congelado[idx] = 1;

                free(memoria[idx]);
                memoria[idx] = novo;
            } else {
                free(memoria[idx]);
                for (int j = idx + 1; j < mem_size; j++) {
                    memoria[j-1] = memoria[j];
                    congelado[j-1] = congelado[j];
                }
                mem_size--;
            }
        }

        fclose(saida);
    }

    for (int i = 0; i < mem_size; i++) {
        free(memoria[i]);
    }
    free(memoria);
    free(congelado);

    return qtd_particoes;
}

void imprime_particoes_veiculos(int qtd_particoes) {
    char nome[30];

    for (int i = 0; i < qtd_particoes; i++) {
        sprintf(nome, "particao_%d.dat", i);
        FILE *f = fopen(nome, "rb");
        if (!f) {
            printf("Erro ao abrir %s\n", nome);
            continue;
        }

        printf("\n=========== PARTIÇÃO %d ===========\n", i);
        Veiculo *v;
        while ((v = le_veiculo(f)) != NULL) {
            imprime_veiculo(v);
            free(v);
        }

        fclose(f);
    }
}

typedef struct {
    Veiculo *v;
    int part_idx;
    int ativo;
} NodoVencedor;

void atualiza_arvore_vencedores(NodoVencedor *nodos, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        int menor = i;
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        if (esq < n && nodos[esq].ativo && nodos[menor].ativo && strcmp(nodos[esq].v->placa, nodos[menor].v->placa) < 0)
            menor = esq;
        if (dir < n && nodos[dir].ativo && nodos[menor].ativo && strcmp(nodos[dir].v->placa, nodos[menor].v->placa) < 0)
            menor = dir;
        if (menor != i) {
            NodoVencedor tmp = nodos[i];
            nodos[i] = nodos[menor];
            nodos[menor] = tmp;
        }
    }
}

void intercalacao_arvore_vencedores(int qtd_particoes, const char *saida_final) {
    FILE **particoes = malloc(qtd_particoes * sizeof(FILE*));
    NodoVencedor *nodos = malloc(qtd_particoes * sizeof(NodoVencedor));
    int ativos = 0;

    for (int i = 0; i < qtd_particoes; i++) {
        char nome[30];
        sprintf(nome, "particao_%d.dat", i);
        particoes[i] = fopen(nome, "rb");
        if (particoes[i]) {
            nodos[i].v = le_veiculo(particoes[i]);
            nodos[i].part_idx = i;
            nodos[i].ativo = (nodos[i].v != NULL);
            if (nodos[i].ativo) ativos++;
        } else {
            nodos[i].v = NULL;
            nodos[i].ativo = 0;
        }
    }

    FILE *saida = fopen(saida_final, "wb");
    if (!saida) {
        printf("Erro ao criar arquivo de saída %s\n", saida_final);
        for (int i = 0; i < qtd_particoes; i++) if (particoes[i]) fclose(particoes[i]);
        free(particoes);
        free(nodos);
        return;
    }

    while (ativos > 0) {
        int idx_menor = -1;
        for (int i = 0; i < qtd_particoes; i++) {
            if (nodos[i].ativo) {
                if (idx_menor == -1 || strcmp(nodos[i].v->placa, nodos[idx_menor].v->placa) < 0) {
                    idx_menor = i;
                }
            }
        }
        if (idx_menor == -1) break;

        salva_veiculo(nodos[idx_menor].v, saida);
        free(nodos[idx_menor].v);
        nodos[idx_menor].v = le_veiculo(particoes[idx_menor]);
        if (nodos[idx_menor].v == NULL) {
            nodos[idx_menor].ativo = 0;
            ativos--;
        }
    }

    fclose(saida);
    for (int i = 0; i < qtd_particoes; i++) if (particoes[i]) fclose(particoes[i]);
    free(particoes);
    free(nodos);
    printf("Arquivo %s gerado com sucesso!\n", saida_final);
}

