#ifndef FUNCIONARIOS_H
#define FUNCIONARIOS_H

typedef struct {
    char cpf[15];
    char nome[100];
    int idade;
} Funcionario;

void adicionarFuncionario();
void removerFuncionario();
void listarFuncionarios();
int buscaSequencialFuncionario(const char *cpf);
int buscaBinariaFuncionario(const char *cpf);
void ordenarFuncionariosPorCPF();
void gerarFuncionariosAleatorios(int quantidade);
int contarFuncionarios();
int buscarFuncionarioPorIndice(int indice, Funcionario *f);

#endif // FUNCIONARIOS_H
