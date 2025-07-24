#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculos.h"

#define TAM_HEAP 5

// Função para comparar placas (critério de ordenação)
int comparaVeiculo(const Veiculo *a, const Veiculo *b) {
    return strcmp(a->placa, b->placa);
}

// Função para trocar dois veículos
void troca(Veiculo *a, Veiculo *b) {
    Veiculo temp = *a;
    *a = *b;
    *b = temp;
}

// Função para ajustar o heap mínimo
void heapify(Veiculo heap[], int n, int i) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < n && comparaVeiculo(&heap[esq], &heap[menor]) < 0)
        menor = esq;
    if (dir < n && comparaVeiculo(&heap[dir], &heap[menor]) < 0)
        menor = dir;
    if (menor != i) {
        troca(&heap[i], &heap[menor]);
        heapify(heap, n, menor);
    }
}

// Função principal de geração de partições ordenadas
void gerarParticoesVeiculos() {
    FILE *entrada = fopen("veiculos.dat", "rb");
    if (!entrada) {
        printf("Erro ao abrir veiculos.dat\n");
        return;
    }

    Veiculo heap[TAM_HEAP];
    int usados[TAM_HEAP]; // 0 = disponível, 1 = marcado para próxima partição
    int i, nHeap = 0, particao = 1, fimArquivo = 0;
    Veiculo novo, ultimoGravado;
    char nomeParticao[32];
    FILE *saida;
    int limiteParticoes = 1000; // Limite de segurança

    // Inicializa heap
    for (i = 0; i < TAM_HEAP; i++) {
        if (fread(&heap[i], sizeof(Veiculo), 1, entrada) == 1) {
            usados[i] = 0;
            nHeap++;
        } else {
            fimArquivo = 1;
            break;
        }
    }

    while (nHeap > 0) {
        sprintf(nomeParticao, "particao%d.dat", particao);
        saida = fopen(nomeParticao, "wb");
        if (!saida) {
            printf("Erro ao criar %s\n", nomeParticao);
            break;
        }
        printf("Gerando %s...\n", nomeParticao);

        int primeiro = 1;
        while (nHeap > 0) {
            // Heap mínimo na posição 0
            Veiculo menor = heap[0];
            if (primeiro) {
                ultimoGravado = menor;
                primeiro = 0;
            }
            fwrite(&menor, sizeof(Veiculo), 1, saida);

            // Tenta ler novo registro do arquivo
            if (!fimArquivo && fread(&novo, sizeof(Veiculo), 1, entrada) == 1) {
                // Se novo >= último gravado, substitui no heap
                if (comparaVeiculo(&novo, &ultimoGravado) >= 0) {
                    heap[0] = novo;
                } else {
                    // Marca para próxima partição
                    usados[0] = 1;
                }
            } else {
                fimArquivo = 1;
                usados[0] = 1;
            }

            // Se marcou para próxima partição, move para o final do heap
            if (usados[0]) {
                troca(&heap[0], &heap[nHeap-1]);
                int temp = usados[0];
                usados[0] = usados[nHeap-1];
                usados[nHeap-1] = temp;
                nHeap--;
            }

            // Ajusta heap mínimo apenas com os disponíveis
            heapify(heap, nHeap, 0);
        }
        fclose(saida);
        particao++;

        // Recarrega heap apenas com os marcados para próxima partição
        int novoHeap = 0;
        for (i = 0; i < TAM_HEAP; i++) {
            if (usados[i]) {
                heap[novoHeap] = heap[i];
                usados[novoHeap] = 0;
                novoHeap++;
            }
        }
        // Se não há mais registros para ler e não há mais registros marcados, encerra
        if (fimArquivo && novoHeap == 0) {
            break;
        }
        nHeap = novoHeap;
    }
    fclose(entrada);
    printf("Particionamento concluído!\n");
}
