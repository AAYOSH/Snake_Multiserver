# Snake_multiserver

# Instalação
Para poder jogar é necessário possuir as bibliotecas ncurses e portaudio. Clone o repositório em seu computador
e rode o Makefile. O jogo está configura para 2 clientes, e também é necessário atualizar o ip do servidor em sockets.cpp.


# Regras
Use os caracteres **w** para mover para cima, **s** para baixo, **a** e **d** para a esquerda e direita respectivamente.
Colisões com o próprio corpo, com outras cobras ou com as paredes resultam em derrota.


# Objetivo:
O jogo acaba após 1m40s, quem possuir maior corpo após esse tempo, ou for o último sobrevivente da arena, ganha o jogo. Em casos
de colisões com a cabeça a cobra morre, se ambas colidirem com a cabeça e forem as únicas na arena, é declarado empate.