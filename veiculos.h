#ifndef VEICULOS_H
#define VEICULOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_BASE_ARQUIVO   "hash_veiculos.dat"
#define HASH_LISTA_ARQUIVO  "hash_listas.dat"
#define TAM_TABELA          10007
#define NULO                -1


typedef struct {
    char placa[10];
    char modelo[50];
    char marca[50];
    int  ano;
    char cor[20];
    char cpf_funcionario[15];
} Veiculo;

typedef struct NoHash {
    Veiculo v;
    long long prox;  
} NoHash;

void adicionarVeiculo();
void removerVeiculo();
void listarVeiculos();
int  buscaSequencialVeiculo(const char *placa);
int  buscaBinariaVeiculo(const char *placa);
void ordenarVeiculosPorPlaca();
void ordenarVeiculosPorSelecaoSubstituicao();
void gerarVeiculosAleatorios(int quantidade);
int  contarVeiculos();
int  buscarVeiculoPorIndice(int indice, Veiculo *v);
int  gerarParticoesVeiculos();
void imprime_particoes_veiculos(int qtd_particoes);
int  selecao_por_substituicao_veiculos(FILE *arq, int mem_size);
void intercalacao_arvore_vencedores(int qtd_particoes, const char *saida_final);
Veiculo* le_veiculo(FILE *arq);
void imprime_veiculo(Veiculo *v);

unsigned int hashPlaca(const char *placa);
int  hash_inicializar();
int  hash_inserir(const Veiculo *v);
int  hash_buscar(const char *placa, Veiculo *out);
int  hash_remover(const char *placa);
void hash_listar();

#endif
