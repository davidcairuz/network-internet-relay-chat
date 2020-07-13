# Redes - SSC0142, Professora Kalinka

## Membros

- David Cairuz da Silva - 10830061
- João Guilherme Madeira Araújo - 9725165
- Luísa Souza Moura - 10692179

## Sistema Operacional

- Linux Ubuntu 19.10

## Compilador

- g++ 9.2.1

## Instruções

O Makefile tem os seguintes comandos:

- make all: compila todos os arquivos, gerando executáveis para o cliente e para o servidor
- make client: compila os arquivos relacionados ao cliente
- make servidor: compila os arquivos relacionados ao servidor
- make socket: compila os arquivos relacionados ao socket
- make error: compila os arquivos relacionados ao error
- make clean: remove os arquivos binários e executáveis da pasta

Para utilizar o sistema deve-se executar os arquivos na seguinte ordem:

- Primeiramente é preciso executar o servidor, para isso é possível utilizar o comando ./server
- Em seguida executa-se o cliente. Deve ser fornecido o endereço de IP do servidor ao qual se deseja conectar. Para isso é possível utilizar o comando ./client ip_address
