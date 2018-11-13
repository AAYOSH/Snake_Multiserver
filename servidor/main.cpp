/*
 * 
 * Nome : André de Morais Yoshida
 * RA: 166241
 * 
 * Primeiro projeto da disciplina EA872
 * 
 * 
 * 
 * ************SNAKE GAME***************
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <ncurses.h>
#include "portaudio.h"
#include <mutex>
#include "../headers/01-playback.hpp"
#include "../headers/modelo.hpp"
#include "../headers/sockets.hpp"
#include "../headers/serializable.hpp"



using namespace std::chrono;
uint64_t get_now_ms() {
      return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}


typedef struct multi_cliente{
	int connection_fd;
	int ID;
	ListaDeCobra *L;
	Comida *c;
	Sockets *S;
	int *rodando;
}multi_cliente;

std::mutex mtx;

void aux_thread(multi_cliente  dados_cliente);
int main()
{	
	
  
      int i;
      // cria classe comida
      Comida *food = new Comida();
      
      //cria classe snake, crio maximo 4 cobras 
      Cobra_corpo *snake1 = new Cobra_corpo(1, 0, 1, 12);// cria cabeca da cobra 
      Cobra_corpo *snake2 = new Cobra_corpo(-1, 0, 100 , 12);// cria cabeca da cobra    ////<<<<<<< mudar as posicoes iniciais e velocidades iniciais
      Cobra_corpo *snake3 = new Cobra_corpo(1, 0, 10, 37);// cria cabeca da cobra
      Cobra_corpo *snake4 = new Cobra_corpo(-1, 0, 100, 37);// cria cabeca da cobra
//       
      // cria classe lista de corpos vazia 
      ListaDeCorpos *l1 = new ListaDeCorpos();
      l1->add_corpo(snake1);// adiciona cabeca na lista de corpos
      
      ListaDeCorpos *l2 = new ListaDeCorpos();
      l2->add_corpo(snake2);// adiciona cabeca na lista de corpos
      
      ListaDeCorpos *l3 = new ListaDeCorpos();
      l3->add_corpo(snake3);// adiciona cabeca na lista de corpos
      
      ListaDeCorpos *l4 = new ListaDeCorpos();
      l4->add_corpo(snake4);// adiciona cabeca na lista de corpos
      // criando lista com 4 cobras para os clientes a vir jogar
      ListaDeCobra *C = new ListaDeCobra();
      C->add_cobra(l1);
      C->add_cobra(l2);
//       C->add_cobra(l3);
//       C->add_cobra(l4);
      // cria estrutura para serializar os dados
      RelevantData *D1 = new RelevantData(food,C);// 
      // cria socket, inicializa servidor
      Sockets *servidor = new Sockets(D1);
      servidor->init_servidor();
      
    //  cria classe tela
      Tela *tela = new Tela(l1, food, D1, C); //<<<<<<<<<<<<<<<, esse l1 é desnecessario
      tela->init();
      
      int max_x;
      int max_y;
      getmaxyx(stdscr,max_y,max_x); 
//       espero conexoes serem feitas
      while(servidor->play() < MAX_CONEXOES){
// 	        printf("AGUARDANDO CONEXOES\n");
		mvprintw(15,45,"Esperando Conexoes Dos Jogadores");
		refresh();
      }
      int players[MAX_CONEXOES];
      for(i = 0; i < MAX_CONEXOES; i++)
	      players[i] = 0;
      
      
      clear();
      // clientes se conectaram, agora obtenho infos deles
      multi_cliente dados_clientes[MAX_CONEXOES];
      for(i = 0; i < MAX_CONEXOES;i++){
	      dados_clientes[i].ID = i;
	      dados_clientes[i].L = C; 
	      dados_clientes[i].c = food; 
	      dados_clientes[i].connection_fd = servidor->get_fd(i);
	      dados_clientes[i].S = servidor;
	      dados_clientes[i].rodando = players;
      }

      // a partir daqui, sei que vector connections cm client esta preenchido e logo posso criar threads especificas para cada um
      std::vector<std::thread> connection_thread(MAX_CONEXOES);
      for( i = 0; i < MAX_CONEXOES; i++){
		std::thread new_thread(aux_thread, dados_clientes[i]);
		connection_thread[i].swap(new_thread);
      }
      
      
      // jogo snake_game
      int fd;
      while(1){ // descobrir qd acabar
		D1->update(C,food);
		tela->update(); // atualiza tela
		if(servidor->play() < 2){ /* caso so tenha 1 cliente conectado ao jogo, mando mensagem de vitoria à ele e entao acabo o programa */ 
			while(servidor->play()); //< espera o ultimo cliente desconectar
			break;  
		}
	    std::this_thread::sleep_for (std::chrono::milliseconds(500));
      }
      // fecho threads
      for (int i = 0; i < MAX_CONEXOES; i++){
	      connection_thread[i].join();
      }
      servidor->remover_conexao(); // removo todas as conects com os clients
      servidor->stop_socket(); // paro as threads e variaveis
      tela->stop();
 
      return 0;
}


//sons ficarao no client
void aux_thread(multi_cliente  dados_cliente){ //<< mandar mensagens de inicio
	int max_x;
	int max_y;
// 	getmaxyx(stdscr,max_y,max_x); 
	char buffer[1024];
	// obtendo dados da struct 
	ListaDeCobra *this_cobra =  dados_cliente.L;
	Comida *this_food = dados_cliente.c;
	int this_ID = dados_cliente.ID;
	int this_fd = dados_cliente.connection_fd;
	Sockets *this_socket = dados_cliente.S; //<<<<<<<<<<<<,,,,, nAO ESTOU USANDO PARA NDA
	int *jogadores = dados_cliente.rodando;
	std::vector<ListaDeCorpos *> *l = this_cobra->get_corpos(); //<< obtenho lista de cobra
	std::vector<Cobra_corpo *> *lista = (*l)[this_ID]->get_corpos();  //<< obtenho lista de corpos da cobra desejada
	// cria classe teclado
	Teclado *teclado = new Teclado();
 	teclado->init();
	teclado->init_socket(this_fd);
	
	RelevantData *D1 = new RelevantData(this_food,this_cobra);
	
//	cria classe Movimento
	Movimento *m = new Movimento(this_food,this_cobra);
	
//	cria classe Choque
	Choque *choque = new Choque(this_cobra);
//	mensagem de inicializacao para cada cliente
	jogadores[this_ID] = 1;
	
	int i;
	int envia = this_ID;
	for( i = 0; i < 2; i++){
	if(send(this_fd, &envia , sizeof(int), 0)) break;
	}
	
	i=0;
	while(i < MAX_CONEXOES){
		if(jogadores[i] != 0 ) i++;
	}
	int comida;
	int ultimo = 0;
	char c;
	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	/* loop principal*/
	int choque1,choque2;
	
	uint64_t t1;
	uint64_t T;
	
	T = get_now_ms();
	t1 = T;
	int vencedor;
	while(1){
		t1 = get_now_ms();
		if ( (t1-T) > 100000 ) { /* passado um tempo, jogo acaba e descobre qual maior cobra*/
			mtx.lock();
			D1->update(this_cobra,this_food);
			vencedor = this_cobra->maior_cobra();
			if(vencedor == -1){ /* empate*/
				D1->serialize(buffer,this_ID,3 ,0);
			}
			else if(this_ID == vencedor){
				D1->serialize(buffer,this_ID,2,0);
			}
			else{
				
				D1->serialize(buffer,this_ID,1,0);
			}
			mtx.unlock();
			send(this_fd, buffer, 1000, 0);
			break;	
		}
		
		comida = 0;
		char c = teclado->getchar(); // < servidor muda para cada thread
		if (c=='q') 
			break;
		if(c == 's'){
			(*lista)[0]->update_vel(0,1); ///<<<< mexo a cabeca da cobra
		}
		if(c == 'w'){
			(*lista)[0]->update_vel(0,-1);
		}
		if(c == 'd'){
			(*lista)[0]->update_vel(1,0); 
		}
		if(c == 'a'){
			(*lista)[0]->update_vel(-1,0);
		}
		if(m->update()){// atualizado movimento do corpo, retorna 1 se cobra comer
			comida = 1;
		}
		mtx.lock();
		choque1 = choque->colisao(this_cobra,this_ID);
		choque2 = choque->colisao_cobras(this_cobra,this_ID);
		mtx.unlock();
		D1->update(this_cobra,this_food);
		D1->serialize(buffer,this_ID,0,comida);
		if(send(this_fd, buffer, 1000, 0) <= 0) break; /* cliente desconectou*/
		ultimo = this_socket->ultimo_cliente();
		if(choque1 || choque2){ // caso haja colisao, programa é encerrado
			mtx.lock();
			std::this_thread::sleep_for (std::chrono::milliseconds(750));
			D1->update(this_cobra,this_food);
			D1->serialize(buffer,this_ID,1,0);///<<< manda com ID igual a -1 quando perde
			send(this_fd, buffer, 1000, 0);
			mtx.unlock();
			break;
		}
		
		if(ultimo == 1){
			mtx.lock();
			D1->update(this_cobra,this_food);
			D1->serialize(buffer,this_ID,2,0);
			mtx.unlock();
			send(this_fd, buffer, 1000, 0);
			break;
		}
		mvprintw(0,this_ID,"%d",this_ID);
		std::this_thread::sleep_for (std::chrono::milliseconds(240));
		refresh();
	}
	
	this_socket->remover_cliente(this_ID);	
	teclado->stop();
	return;
}