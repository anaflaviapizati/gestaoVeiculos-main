# Algoritmos e Tipos de Busca Usados

## Funcionarios
- **Busca Sequencial**: Usada em `buscaSequencialFuncionario()` para encontrar funcionarios por CPF
- **Busca Binaria**: Usada em `buscaBinariaFuncionario()` para encontrar funcionarios por CPF (CORRETAMENTE IMPLEMENTADA)
- **Ordenacao**: Usada em `ordenarFuncionariosPorCPF()` para ordenar funcionarios por CPF (Bubble Sort)
- **Operacoes**: Adicionar, remover, listar, buscar por CPF (sequencial e binaria), ordenar por CPF, gerar aleatorios

## Veiculos
- **Busca Sequencial**: Usada em `buscaSequencialVeiculo()` para encontrar veiculos por placa
- **Busca Binaria**: Usada em `buscaBinariaVeiculo()` para encontrar veiculos por placa (CORRETAMENTE IMPLEMENTADA)
- **Ordenacao**: Usada em `ordenarVeiculosPorPlaca()` para ordenar veiculos por placa (Bubble Sort)
- **Operacoes**: Adicionar, remover, listar, buscar por placa (sequencial e binaria), ordenar por placa, gerar aleatorios

## Locacoes
- **Busca Sequencial**: Usada em `buscaSequencialLocacao()` para encontrar locacoes por CPF e placa
- **Busca Binaria**: Usada em `buscaBinariaLocacao()` para encontrar locacoes por CPF e placa (CORRETAMENTE IMPLEMENTADA)
- **Ordenacao por Data**: Usada em `ordenarLocacoesPorData()` para ordenar locacoes por data de inicio (Bubble Sort)
- **Ordenacao por CPF e Placa**: Usada em `ordenarLocacoesPorCPFPlaca()` para ordenar locacoes por CPF e placa (Bubble Sort)
- **Operacoes**: Criar, finalizar, listar todas, listar por funcionario, listar por veiculo, ordenar por data, ordenar por CPF e placa, buscar (sequencial e binaria), gerar aleatorias

## Caracteristicas Gerais
- **Armazenamento**: Todos os dados sao armazenados em arquivos .dat no disco
- **Busca Sequencial**: Implementada em todos os modulos - O(n)
- **Busca Binaria**: Implementada em todos os modulos - O(log n) (AGORA FUNCIONANDO CORRETAMENTE)
- **Ordenacao**: Bubble Sort implementado em todos os modulos - O(n²)
- **Simplicidade**: Codigo com algoritmos basicos de ordenacao e busca
- **Performance**: 
  - Busca sequencial: O(n) para todos os modulos
  - Busca binaria: O(log n) para todos os modulos (quando dados estao ordenados)
  - Ordenacao: O(n²) para todos os modulos
- **Uso Correto**: Busca binaria agora e usada apos ordenacao dos dados, garantindo funcionamento correto
- **Sem Memoria RAM**: Todas as operacoes sao feitas diretamente no disco usando fseek() e fread() 