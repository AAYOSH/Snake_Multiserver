#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include "portaudio.h"
#include <iostream>
#include <chrono>
#include "../headers/sockets.hpp"
#include "../headers/serializable.hpp"
#include "../headers/01-playback.hpp"
int aux_print(RelevantData *D1){
	int tocar = 0;
	DataContainer aux;
	int foodx,foody,oldx,oldy;
	int total, num_cobras;
	int corpo;
	char imprimir;
	/* dados em minha classe D1 estao no formato como enviados*/
	std::vector<DataContainer > posicoes = D1->get_dados(); // aloco posicoes para receber os dados
	// 	mvprintw(10,10,"%d e %d",posicoes[1].a,posicoes[1].b);
	foodx = posicoes[1].a, foody = posicoes[1].b;
	if(foodx != oldx || foody != oldy){
		tocar++; ///< coloco som da comida
	}
	oldx = foodx, oldy = foody;
	
	total = posicoes[0].a, num_cobras = posicoes[0].b;
	int i,j;
	// ver som de perda
	
	for(i = 1,  j = 2; i <= num_cobras; i++, j++){ // agora atribuo para as cobras
		aux = posicoes[j];
		corpo = aux.b;
		if(aux.b == -1){
			tocar = tocar + 2;
			break;
		}
		imprimir = (aux.b);
		for(int k = 0; k < aux.a; k++){ // para cada corpo pertencente a cobra
			j++; // posso variar o que for printar usando aux.b, mas deixar isso para depois de tudo OK
		}
	}
	return tocar;
}
using namespace std::chrono;
uint64_t get_now_ms() {
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main() {
    
	char a[2];
	
	RelevantData *D1 =  new RelevantData();
	int som;
	Sockets *cliente = new Sockets(D1);
	cliente->init_cliente();
	int socket = cliente->get_socketfd();
	
	Teclado *teclado = new Teclado();
	teclado->init(); // inicializa PAD do teclado
//	inicializa sons
	Audio::Sample *asample;
	asample = new Audio::Sample();
	asample->load("Audios/You-Lose-Sound-Effect.dat");
	      
	Audio::Sample *asample1;
	asample1 = new Audio::Sample();
	asample1->load("Audios/blip.dat");
	      
	Audio::Sample *asample2;
	asample2 = new Audio::Sample();
	asample2->load("Audios/Bite.dat");
	      
	Audio::Player *player;
	player = new Audio::Player();
	player->init();
	std::this_thread::sleep_for (std::chrono::milliseconds(1100)); // evitar que erros do som aparecam na tela, ou redirecionar erros a outro arquivo
	
	Tela_client *tela = new Tela_client(D1);
	tela->init_client();
	
	/* aguardo mensagem de inicio para comecar o jogo*/
	mvprintw(15,45,"AGUARDANDO JOGADORES");
	refresh();
	int teste = -1;
	while((teste) < 0 || (teste) > 4){
		recv(socket, &teste , sizeof(int), 0);
	}
	clear();
	mvprintw(16,45,"CORPO  DE SUA SNAKE: %d",(teste));
	refresh();
	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	while(1){
		a[0] = cliente->getchar();
		if (a[0] != 0) {// aqui estou enviando dado do teclado para o servidor
			send(socket, a, sizeof(a) , 0); 
			if(a[0] == 'q') {
				mvprintw(16,45,"Voce Saiu do Jogo ");
				refresh();
				std::this_thread::sleep_for (std::chrono::milliseconds(1000));
				break;
			}
			// toco som de movimento
			std::this_thread::sleep_for (std::chrono::milliseconds(100));
			asample1->set_position(0); // caso tenha comido
			player->play(asample1);
			
		}
		D1->clean(); // recebo do servidor os dados e do upload
		
 		cliente->unserial();
 		som = tela->update_client();
// 		D1->dump();
		if(som == 1){ /* tocar comida*/
			std::this_thread::sleep_for (std::chrono::milliseconds(100));
			asample2->set_position(0); // caso tenha comido
			player->play(asample2);
		}
		else if ( som == 2){/* tocar perda*/
			mvprintw(16,45,"Voce Perdeu o Jogo ");
			refresh();
			asample->set_position(0); // caso tenha comida
			player->play(asample);
			std::this_thread::sleep_for (std::chrono::milliseconds(5000));
			break;
		}
		else if( som == 4){/* tocar vitoria*/
			mvprintw(16,45,"Voce ganhou o Jogo ");
			refresh();
			std::this_thread::sleep_for (std::chrono::milliseconds(5000));
			break;
		}
		else if(som == 3){
			mvprintw(16,45,"EMPATE");
			refresh();
			std::this_thread::sleep_for (std::chrono::milliseconds(5000));
			break;
		}
		std::this_thread::sleep_for (std::chrono::milliseconds(100));// necessário para esta thread n ficar consumindo a todo tempo o processador
	}

	player->stop();
 	tela->stop_client();
	cliente->stop_socket();
	return 0;
}	

