# Sincronização do Metrô

Implementação 5 - Infraestrutura de software.
Aluno: Luiz Henrique Carvalho Vieira (lhcv).
Professor: Marcelo Lima Arcoverde. Turma: 3º período B.
Repositório: https://github.com/luizvieiraa/Sync

## Sistema operacional

Implementado e testado no Ubuntu 26.04 LTS via WSL2, com GCC 15.2.0 e GNU Make 4.4.1.
A implementação usa C e pthreads, para Linux, Unix ou macOS. A execução foi
verificada no Ubuntu; os demais sistemas não foram testados.

## Arquivos

| Arquivo | Responsabilidade |
| --- | --- |
| `sync_car.c` | Implementa `station_init`, `station_load_car`, `station_wait_for_car` e `station_on_board`. |
| `sync_car.h` | Define `struct station` e declara as quatro funções utilizadas pelo runner. |
| `sync_car-runner.c` | Código original fornecido pelo professor: contém `main` e testa a sincronização com 100 passageiros. |
| `Makefile` | Compila o programa e limpa os executáveis e objetos gerados. |
| `evidencias.log` | Registro automático de comandos e resultados durante o desenvolvimento. |
| `evidencias/erro_condicao.png` | Captura do retorno antecipado na versão com condição incorreta. |
| `evidencias/validacao_runner.png` | Captura da versão corrigida aprovada pelo runner. |

As capturas comparam uma versão alterada para depuração com a versão correta.
O log preserva o histórico, incluindo testes auxiliares realizados antes da
redução da entrega aos arquivos solicitados. Esses testes não integram o pacote final.

## Compilar

Dentro da pasta `lhcv`, execute:

```sh
make clean
make
```

O Makefile gera o executável `sync_car` a partir de `sync_car.c` e do runner.
O cabeçalho permite ao runner instanciar `struct station` sem alterar o original.
O aviso de parâmetro não utilizado é desativado somente ao compilar o runner;
os demais avisos são tratados como erros.

## Executar e testar

```sh
./sync_car
```

O runner não recebe argumentos nem arquivo de entrada. Ele cria 100 passageiros,
verifica carros sem passageiros ou sem vagas e usa capacidades aleatórias.
A quantidade de vagas e o número de carros variam entre execuções.
A saída apresenta `car entering station ...`, `car departed station ...` e,
quando o teste passa, `Looks good!`, com retorno zero. As mensagens e o formato
são os do arquivo original do professor. A implementação não acrescenta saídas.

## Limpar

```sh
make clean
```

Remove `sync_car` e os arquivos `.o`, preservando fontes e evidências.

## Sincronização

`station_init` inicializa um mutex, duas variáveis de condição e os contadores.
`station_wait_for_car` espera uma vaga e reserva um assento antes de retornar.
`station_on_board` confirma que o passageiro sentou e sinaliza a conclusão
quando o carro pode partir. `station_load_car` anuncia as vagas e espera até
todos os embarques autorizados terminarem e o carro estar cheio ou a fila acabar.

Todos os contadores são protegidos pelo único mutex da estação. As esperas
usam `pthread_cond_wait` em `while`; não há busy-wait nem semáforos.
O mutex é liberado antes do embarque físico para permitir embarques simultâneos.
Conforme o enunciado, há no máximo um carro na estação por vez e todos têm o mesmo destino.

## Registrar evidências

```sh
script -a evidencias.log
date
whoami
pwd
make clean
make
./sync_car
make clean
exit
```

## Entrega

Envie `lhcv.pdf`, `lhcv.tar` e o link do repositório na mesma atividade.
O `.tar` contém a pasta `lhcv/` com os fontes, README, Makefile e as evidências
pedidas pelo Guia de Relatórios. Para reproduzir: extraia com `tar -xf lhcv.tar`,
entre em `lhcv` e execute os comandos acima.
