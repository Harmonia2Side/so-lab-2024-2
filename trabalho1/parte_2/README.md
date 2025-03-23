# Parte 2: Controle de Pedidos de Medicamentos

## Como executar o programa (Quickstart)

### Linux

- Para compilar e executar o código, execute o script 'run.sh' com o comando "./run.sh". Verifique se o arquivo é executável.
- Para apenas compilar código, execute o script 'make.sh' com o comando "./make.sh". Verifique se o arquivo é executável.

### Windows

- Compile o arquivo main.c com os comandos apropriados
- Execute o arquivo main.exe

## Descrição da estrutura de arquivos

- `arquivos_teste`: diretório com os vários casos de teste do programa.
- `teste_N_descrição`: diretórios cada um descrevendo um caso de teste diferente. Exemplo: teste_1_leitura_de_arquivos
- `make.log` e `run.log`: Registros da última compilação e última execução, respectivamente. `Observe que o script run.sh também executa make.sh`

## Observações sobre parâmetros passados programa

- O executável `main.exe` recebe como parâmetro o caminho para um diretório, relativo ao diretório atual. Dentro do diretório passado como parâmetro devem haver 4 arquivos, nomeados conforme padrão a seguir:

```
arquivo-1.txt
arquivo-2.txt
arquivo-3.txt
arquivo-4.txt
```

- Cada arquivo deve conter informações sobre pedidos de medicamentos de acordo com o seguinte padrão

```
nome de algum paciente
id do medicamento correspondente
quantidade
nome de algum paciente
id do medicamento correspondente
quantidade
nome de algum paciente
id do medicamento correspondente
quantidade
```

- Exemplos existentes podem ser encontrados nos diretórios dentro de `arquivos_teste`
- Caminhos absolutos de diretórios não são suportados
