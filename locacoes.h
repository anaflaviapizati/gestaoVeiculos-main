#ifndef LOCACOES_H
#define LOCACOES_H

typedef struct {
    char cpf_funcionario[15];
    char placa_veiculo[10];
    char data_inicio[11];
    char data_fim[11];
    char status[20];
} Locacao;

void criarLocacao();
void finalizarLocacao();
void listarLocacoes();
void listarLocacoesPorFuncionario();
void listarLocacoesPorVeiculo();
int buscaSequencialLocacao(const char *cpf, const char *placa);
int buscaBinariaLocacao(const char *cpf, const char *placa);
void ordenarLocacoesPorData();
void gerarLocacoesAleatorias(int quantidade);
int contarLocacoes();

#endif // LOCACOES_H
