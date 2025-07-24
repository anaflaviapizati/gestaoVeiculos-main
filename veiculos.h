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
void gerarVeiculosAleatorios(int quantidade);
int contarVeiculos();
int buscarVeiculoPorIndice(int indice, Veiculo *v);
void gerarParticoesVeiculos();

#endif // VEICULOS_H
