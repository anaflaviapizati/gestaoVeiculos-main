#ifndef HASH_H
#define HASH_H
#include <stdio.h>
#include <stdbool.h>

#define TABLE_SIZE 10007
#define EMPTY_POS -1

typedef struct {
    char placa[10];
    int posArq;
    int prox;
} estHash;

typedef struct {
    char placa[10];
    char modelo[50];
    char marca[50];
    int ano;
    char cor[20];
    char cpf_funcionario[15];
    int prox;
    bool ocupado;
} TVeiculo;

int hash(const char *placa);
void initTabela(FILE *tabHash);
bool insereVeiculo(FILE *arqVeiculos, FILE *tabHash, TVeiculo *v);
bool buscaVeiculo(FILE *arqVeiculos, FILE *tabHash, const char *placa, TVeiculo *out);
bool removeVeiculo(FILE *arqVeiculos, FILE *tabHash, const char *placa);
void printaTabela(FILE *tabHash);

#endif
