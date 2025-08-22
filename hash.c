#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "hash.h"
#include "veiculos.h"

int hash(const char *placa) {
    unsigned int h = 0;
    for (int i = 0; placa[i] != '\0'; i++) {
        h = (h * 31 + placa[i]) % TABLE_SIZE;
    }
    return h % TABLE_SIZE;
}

void initTabela(FILE *tabHash) {
    estHash slot;
    slot.posArq = EMPTY_POS;
    slot.prox = EMPTY_POS;
    strcpy(slot.placa, "");
    rewind(tabHash);
    for (int i = 0; i < TABLE_SIZE; i++) {
        fwrite(&slot, sizeof(estHash), 1, tabHash);
    }
}

bool insereVeiculo(FILE *arqVeiculos, FILE *tabHash, TVeiculo *v) {
    if (v == NULL) return false;

    TVeiculo tmp;
    if (buscaVeiculo(arqVeiculos, tabHash, v->placa, &tmp))
        return false;

    fseek(arqVeiculos, 0, SEEK_END);
    int nova_pos = ftell(arqVeiculos) / sizeof(TVeiculo);

    v->prox = -1;
    v->ocupado = true;
    fwrite(v, sizeof(TVeiculo), 1, arqVeiculos);

    int idx = hash(v->placa);
    estHash slot;

    fseek(tabHash, idx * sizeof(estHash), SEEK_SET);
    fread(&slot, sizeof(estHash), 1, tabHash);

    if (slot.posArq == EMPTY_POS) {
        strcpy(slot.placa, v->placa);
        slot.posArq = nova_pos;
        slot.prox = -1;
        fseek(tabHash, idx * sizeof(estHash), SEEK_SET);
        fwrite(&slot, sizeof(estHash), 1, tabHash);
        return true;
    }

    int pos_atual = slot.posArq;
    int pos_anterior = pos_atual;
    TVeiculo atual_v;

    while (pos_atual != -1) {
        fseek(arqVeiculos, pos_atual * sizeof(TVeiculo), SEEK_SET);
        fread(&atual_v, sizeof(TVeiculo), 1, arqVeiculos);
        pos_anterior = pos_atual;
        pos_atual = atual_v.prox;
    }

    atual_v.prox = nova_pos;
    fseek(arqVeiculos, pos_anterior * sizeof(TVeiculo), SEEK_SET);
    fwrite(&atual_v, sizeof(TVeiculo), 1, arqVeiculos);

    return true;
}

bool buscaVeiculo(FILE *arqVeiculos, FILE *tabHash, const char *placa, TVeiculo *out) {
    int idx = hash(placa);
    estHash slot;

    fseek(tabHash, idx * sizeof(estHash), SEEK_SET);
    fread(&slot, sizeof(estHash), 1, tabHash);

    if (slot.posArq == EMPTY_POS)
        return false;

    int pos_atual = slot.posArq;
    while (pos_atual != -1) {
        fseek(arqVeiculos, pos_atual * sizeof(TVeiculo), SEEK_SET);
        fread(out, sizeof(TVeiculo), 1, arqVeiculos);

        if (strcmp(out->placa, placa) == 0 && out->ocupado)
            return true;

        pos_atual = out->prox;
    }
    return false;
}

bool removeVeiculo(FILE *arqVeiculos, FILE *tabHash, const char *placa) {
    int idx = hash(placa);
    estHash slot;

    fseek(tabHash, idx * sizeof(estHash), SEEK_SET);
    fread(&slot, sizeof(estHash), 1, tabHash);

    if (slot.posArq == EMPTY_POS)
        return false;

    int pos_anterior = -1;
    int pos_atual = slot.posArq;
    TVeiculo atual_v, anterior_v;

    while (pos_atual != -1) {
        fseek(arqVeiculos, pos_atual * sizeof(TVeiculo), SEEK_SET);
        fread(&atual_v, sizeof(TVeiculo), 1, arqVeiculos);

        if (strcmp(atual_v.placa, placa) == 0 && atual_v.ocupado) {
            atual_v.ocupado = false;
            fseek(arqVeiculos, pos_atual * sizeof(TVeiculo), SEEK_SET);
            fwrite(&atual_v, sizeof(TVeiculo), 1, arqVeiculos);

            if (pos_anterior == -1) {
                if (atual_v.prox == -1) {
                    slot.posArq = EMPTY_POS;
                    strcpy(slot.placa, "");
                } else {
                    slot.posArq = atual_v.prox;
                }
                fseek(tabHash, idx * sizeof(estHash), SEEK_SET);
                fwrite(&slot, sizeof(estHash), 1, tabHash);
            } else {
                fseek(arqVeiculos, pos_anterior * sizeof(TVeiculo), SEEK_SET);
                fread(&anterior_v, sizeof(TVeiculo), 1, arqVeiculos);
                anterior_v.prox = atual_v.prox;
                fseek(arqVeiculos, pos_anterior * sizeof(TVeiculo), SEEK_SET);
                fwrite(&anterior_v, sizeof(TVeiculo), 1, arqVeiculos);
            }
            return true;
        }
        pos_anterior = pos_atual;
        pos_atual = atual_v.prox;
    }
    return false;
}

void printaTabela(FILE *tabHash) {
    estHash slot;
    rewind(tabHash);
    printf("-------------------\n");
    printf("| Indice | Posicao|\n");
    printf("-------------------\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (fread(&slot, sizeof(estHash), 1, tabHash) != 1) {
            perror("Erro ao ler slot da tabela hash");
            return;
        }
        printf("| %-6d | %-6d |\n", i, slot.posArq == -1 ? -1 : slot.posArq);
    }
    printf("-------------------\n");
}
