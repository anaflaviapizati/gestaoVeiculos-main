#ifndef VEICULOS_H
#define VEICULOS_H

typedef struct {
    char placa[10];
    char modelo[50];
    char marca[50];
    int ano;
    char cor[20];
    char cpf_funcionario[15];
} Veiculo;

void adicionarVeiculo();
void removerVeiculo();
void listarVeiculos();
int buscaSequencialVeiculo(const char *placa);
int buscaBinariaVeiculo(const char *placa);
void ordenarVeiculosPorPlaca();
void ordenarVeiculosPorSelecaoSubstituicao();
void gerarVeiculosAleatorios(int quantidade);
int contarVeiculos();
int buscarVeiculoPorIndice(int indice, Veiculo *v);
int gerarParticoesVeiculos();
void imprime_particoes_veiculos(int qtd_particoes);
int selecao_por_substituicao_veiculos(FILE *arq, int mem_size);
void intercalacao_arvore_vencedores(int qtd_particoes, const char *saida_final);
Veiculo* le_veiculo(FILE *arq);
void imprime_veiculo(Veiculo *v);

typedef struct HashVeiculos HashVeiculos;
void construirIndiceHashVeiculos();
int buscarVeiculoHash(const char *placa);
void buscarVeiculoPorPlacaHash();

#endif // VEICULOS_H
