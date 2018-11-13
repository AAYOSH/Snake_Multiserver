

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#include "../headers/modelo.hpp"
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <ncurses.h>
#include<time.h> // para gerar comidas aleatorias
#include<stdlib.h>
#include "../headers/serializable.hpp"
#include "portaudio.h"
#include "../headers/01-playback.hpp"

using namespace std::chrono;

////////////////////////********* classe lista de cobra***********//////////////////

ListaDeCobra::ListaDeCobra() {
	this->cobras = new std::vector<ListaDeCorpos *>(0);
}


void ListaDeCobra::add_cobra(ListaDeCorpos *l) {
	(this->cobras)->push_back(l);
}

std::vector<ListaDeCorpos *> *ListaDeCobra::get_corpos() {
	return (this->cobras);
}
int ListaDeCobra::maior_cobra(){
	std::vector<ListaDeCorpos *> *l = this->cobras;
	int maior = 0;
	for(int i = 1; i < (*l).size();i++){
		std::vector<Cobra_corpo *> *C = (*l)[maior]->get_corpos();
		std::vector<Cobra_corpo *> *c = (*l)[i]->get_corpos();
			if((*c).size() > (*C).size()){
				maior = i;
			}
			else if( (*c).size() == (*C).size()){
				maior = -1;
			}
	}
	return maior;
}


/////////////////*******classe choque********************//////////////////////
// montador
Choque::Choque(ListaDeCobra *cobra){
      this->cobra = cobra;
}
// verifica colisao da cabeca ou com os extremos ou com proprio corpo, retorna verdadeiro caso haja colisao
// agora sera necessario ver colisoes entre cobras tb
bool Choque::colisao(ListaDeCobra *cobra, int ID){
     
      int pos_x,pos_y;
      int max_x,max_y;
      std::vector<ListaDeCorpos *> *l = this->cobra->get_corpos(); //<< obtenho lista de cobra
      std::vector<Cobra_corpo *> *c = (*l)[ID]->get_corpos();  //<< obtenho lista de corpos da cobra desejada
      getmaxyx(stdscr,max_y,max_x); // obtenho extremos da minha tela.
      
      // obtenho posicao da cabeca
      pos_x = (*c)[0]->get_posicaoX();
      pos_y = (*c)[0]->get_posicaoY();
      
      // colisao com os extremos
      if(pos_x >= max_x || pos_x < 0) 
	    return true;
      if(pos_y >= max_y|| pos_y < 0)
	    return true;
      
      // agora avalio caso haja alguma colisao contra o proprio corpo
      for(int i = 1; i < (*c).size(); i++){  
	    if(pos_x == (*c)[i]->get_posicaoX() && pos_y == (*c)[i]->get_posicaoY())
		  return true;
      }
      
      
      return false;
      
}

bool Choque::colisao_cobras(ListaDeCobra *cobra, int ID){
	int j,cabeca_x,cabeca_y,corpo_x,corpo_y;
	std::vector<ListaDeCorpos *> *l = cobra->get_corpos();
	std::vector<Cobra_corpo *> *c = (*l)[ID]->get_corpos();
	int tamanho = (*l).size(); // obtenho quantas cobras tenho no jogo
	std::vector<Cobra_corpo *> *alvo;
	cabeca_x = (*c)[0]->get_posicaoX();
	cabeca_y = (*c)[0]->get_posicaoY();
	
	for(int i = 0; i < tamanho; i++){
		if( i == ID) continue; // nao verifico com a mesma cobra
		alvo = (*l)[i]->get_corpos(); // obtenho o corpo da cobra i
		for(j = 0; j < (*alvo).size(); j++){ // para cada corpo da cobra, verifico se ocorre a colisa
			corpo_x = (*alvo)[j]->get_posicaoX();
			corpo_y = (*alvo)[j]->get_posicaoY();
			if( corpo_x == cabeca_x && corpo_y == cabeca_y)
				return true;
		}
	}
	
	return false;
}

////////******************** classe movimento****************////////////////
// construtor
Movimento::Movimento(Comida *food, ListaDeCobra *cobra){
      this->food = food;
      this->cobra = cobra;
  
}
// atualiza as posicoes de cada parte da cobra, retorna 1 caso comeu a comida
int Movimento::update(void){
      int a = 0;
      std::vector<ListaDeCorpos *> *l = this->cobra->get_corpos();// obtenho as cobras que estao no jogo
      int x = (*l).size();
//       printf("numero de cobras no jogo %d \n", x);
      for(int i = 0; i < x; i++){ // para cada cobra
		std::vector<Cobra_corpo *> *c = (*l)[i]->get_corpos();// obtenho os corpos da cobra i
		// acredito que restante do algoritmo ainda funcione
		// aqui faco o movimento para a cabeca, separado pois é a unica que possui velocidade
		int velx = (*c)[0]->get_velocidadeX();// obtem velocidade
		int vely = (*c)[0]->get_velocidadeY();
		(*c)[0]->update_old((*c)[0]->get_posicaoX(),(*c)[0]->get_posicaoY()); // guarda posicao antiga antes de atualizar
		int new_posx = (*c)[0]->get_posicaoX() + velx; //significa in/decrementar 1;
		int new_posy = (*c)[0]->get_posicaoY() + vely;
		
// 		printf("cabeca_cobra %d esta com posx:%d e posy:%d \n",i,new_posx,new_posy);
		// aqui eu evito movimentos nao permitidos,ex: a cobra vai para direita e jogador pressiona tecla esquerda -> IMPOSSIVEL caso ela possua tamanho maior que 1
		if((*c).size() > 1 ){
			// só faco a comparacao com o corpo logo atras da cabeca, que ja é suficiente
			int pos_x = (*c)[1]->get_posicaoX();
			int pos_y = (*c)[1]->get_posicaoY();
		
			if(new_posx == pos_x){ // caso a nova posicao da cobra seja igual a da parte anterior
				new_posx = pos_x - 2*velx; 
			}
			if(new_posy == pos_y)
				new_posy = pos_y - 2*vely;
			}
			
		(*c)[0]->update(new_posx,new_posy); // atualizado finalmente posicao da cabeca
		
		// faco o restante dos corpos seguirem a cabeca
		for(int i = 1; i < (*c).size(); i++){  
			(*c)[i]->update_old((*c)[i]->get_posicaoX(),(*c)[i]->get_posicaoY());
			int new_posx = (*c)[i-1]->get_oldX();
			int new_posy = (*c)[i-1]->get_oldY();
			(*c)[i]->update(new_posx,new_posy);
			
		}
		
		
		int foodx = food->getcx();
		int foody = food->getcy();
			// verificando caso a cobra pegue a comida, no caso somente a cabeca
		if( foodx == (*c)[0]->get_posicaoY() && foody == (*c)[0]->get_posicaoX() ){
			int aux = (*c).size();
		// 	    mvprintw(foodx,foody ," ");
			food->update();
			Cobra_corpo *nova = new Cobra_corpo( 1, 0,(*c)[aux-1]->get_oldX(), (*c)[aux-1]->get_oldY() ); // adiciona nova parte ao corpo da cobra
 			(*l)[i]->add_corpo(nova);
			// atualizar lista de corpos agora
			a = 1;
		}
	
      }
	return a;
}
     
/////*************************** CLASS Comida **********************88//////////////////
// construtor
Comida::Comida(){
      int max_x,max_y;
      getmaxyx(stdscr,max_x,max_y); // obtenho extremos da minha tela.
      
      srand(time(NULL)); // gero a comida em posicoa aleatoria
      this->posicaoY =  random()%(max_y) +10 ;
      this->posicaoX = random()%(max_x) +10 ;
      
}

// funcao que da update na posicao da comida apos ela ser consumida
void Comida::update(){
      
      int max_x,max_y;
      getmaxyx(stdscr,max_x,max_y); // obtenho extremos da minha tela.
      
      
      srand(time(NULL)); // posiciona comida em novo lugar random
      this->posicaoY =  random()%(max_y);
      this->posicaoX = random()%(max_x);
}

int Comida::getcx(){
      return this->posicaoX;
}
int Comida::getcy(){
      return this->posicaoY;
}

void Comida::update_serial(int nova_posicaoX, int nova_posicaoY){
	this->posicaoX = nova_posicaoX;
	this->posicaoY = nova_posicaoY;
	
}
/////***************** Class Lista de Corpos*********************//////////////////////
// construtor
ListaDeCorpos::ListaDeCorpos() {
      this->corpos = new std::vector<Cobra_corpo *>(0);
}


void ListaDeCorpos::add_corpo(Cobra_corpo *c) {
      (this->corpos)->push_back(c);
}

std::vector<Cobra_corpo *> *ListaDeCorpos::get_corpos() {
      return (this->corpos);
}

////*****************************CLASS Cobra_corpo******************************////////////////////

Cobra_corpo::Cobra_corpo( int velocidadex, int velocidadey, int posicaoX, int posicaoY){
      this->velocidadeX = velocidadex;
      this->velocidadeY = velocidadey;
      this->posicaoX = posicaoX;
      this->posicaoY = posicaoY;
}

void Cobra_corpo::update(int nova_posicaoX, int nova_posicaoY) {
      this->posicaoX = nova_posicaoX;
      this->posicaoY = nova_posicaoY;
}

void Cobra_corpo::update_old(int posicaoX, int posicaoY) {
      this->oldX = posicaoX;
      this->oldY = posicaoY;
}

void Cobra_corpo::update_vel(int nova_velX, int nova_velY) {
      this->velocidadeX = nova_velX;
      this->velocidadeY = nova_velY;
}

int Cobra_corpo::get_velocidadeX() {
      return this->velocidadeX;
}
int Cobra_corpo::get_velocidadeY() {
      return this->velocidadeY;
}
int Cobra_corpo::get_posicaoX() {
      return this->posicaoX;
}

int Cobra_corpo::get_posicaoY() {
      return this->posicaoY;
}
int Cobra_corpo::get_oldX() {
      return this->oldX;
}
int Cobra_corpo::get_oldY() {
      return this->oldY;
}

/////////*************CLASS TELA_CLIENT***********************8//////////////////////

Tela_client::Tela_client(RelevantData *D1) {
	
	this->D1 = D1;
	
}
int Tela_client::update_client(){
	int tocar = 0;
	DataContainer aux;
	int foodx,foody,oldx,oldy;
	int total, num_cobras;
	int imprimir;
	clear(); ///// <<< vou usar a estrategia de limpar a tela assim, so coloco os objetos na posicao nova e ja era, caso de errado, usar
	// estrategia de limpar a pos antiga
	
	/* dados em minha classe D1 estao no formato como enviados*/
	std::vector<DataContainer > posicoes = this->D1->get_dados(); // aloco posicoes para receber os dados
// 	mvprintw(10,10,"%d e %d",posicoes[1].a,posicoes[1].b);
	foodx = posicoes[1].a, foody = posicoes[1].b;
	mvprintw(foodx,foody,"*"); // agora comida ja esta printada
	
	total = posicoes[0].a, num_cobras = posicoes[0].b;
	int i,j;
	// ver som de perda
	if(posicoes[2].a == 1)/* perdeu*/
		tocar =  2;
	if(posicoes[2].b == 1) /* comeu*/
		tocar =  1;
	if(posicoes[2].a == 2) /* venceu*/
		tocar = 4;
	if(posicoes[2].a == 3) /*empate*/
		tocar = 3;
	
	for(i = 1,  j = 3; i <= num_cobras; i++, j++){ // agora atribuo para as cobras
		aux = posicoes[j];
 		imprimir = (aux.b);
		for(int k = 0; k < aux.a; k++){ // para cada corpo pertencente a cobra
			j++; // posso variar o que for printar usando aux.b, mas deixar isso para depois de tudo OK
			mvprintw(posicoes[j].b,posicoes[j].a,"%d",imprimir); // printo S na pos da cobra
		}
	}
	
	refresh();
	return tocar;
}



void Tela_client::init_client() {
	initscr();			       /* Start curses mode 		*/
	raw();				         /* Line buffering disabled	*/
	curs_set(0);           /* Do not display cursor */
	noecho();
}

void Tela_client::stop_client() {
	endwin();
}


Tela_client::~Tela_client() {
	this->stop_client();;
}





////************************************* CLASS TELA**********************//////////////////////////////

//construtor
Tela::Tela(ListaDeCorpos *ldc, Comida *food,RelevantData *D1, ListaDeCobra *cobra) {
      this->lista = ldc;
      this->food = food;
      this->D1 = D1;
      this->cobra = cobra;
}
void Tela::update() {
      int foodx,foody;
      std::vector<ListaDeCorpos *> *l = this->cobra->get_corpos();
      clear();
      // laco que atualiza os corpos na tela
      for (int i = 0; i < (*l).size(); i++){ // para cada cobra
	        std::vector<Cobra_corpo *> *c = (*l)[i]->get_corpos();// obtenho os corpos da cobra i
		for (int k=0; k< (*c).size(); k++)
		{		
			int xo = (*c)[k]->get_oldX();
			int yo = (*c)[k]->get_oldY();
			move(yo, xo);   /* Move cursor to position */
			echochar(' ');  /* Prints character, advances a position */
			
			
			int x = (*c)[k]->get_posicaoX();
			int y = (*c)[k]->get_posicaoY();
			mvprintw(y, x,"s"); // similar às duas instrucoes acima, fazer um para cada cobra
			
			
		}
		// printa comida
		int oldfoodx = foodx;
		int oldfoody = foody;
		foodx = food->getcx();
		foody = food->getcy();
		if(foodx != oldfoodx  || foody != oldfoody)
			mvprintw(oldfoodx,oldfoody ," ");
		mvprintw(foodx,foody,"*");
		
		// Atualiza tela
		refresh();
      }
}



void Tela::init() {
      initscr();			       /* Start curses mode 		*/
      raw();				         /* Line buffering disabled	*/
      curs_set(0);           /* Do not display cursor */
}

void Tela::stop() {
      endwin();
}


Tela::~Tela() {
      this->stop();;
}
/////////////////////////////*****************CLASS TECLADO*******************************//////////////////
void threadfun (char *keybuffer, int *control, int connection , char *buffer)
{
	
	char c;
	while ((*control) == 1) {
		c = recv(connection, buffer, sizeof(buffer), 0);
		(*keybuffer) = buffer[0];
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
	return;
	
}

Teclado::Teclado() {
}

Teclado::~Teclado() {
}

void Teclado::init() {
      // Inicializa ncurses
      raw();				         /* Line buffering disabled	*/
      keypad(stdscr, TRUE);	 /* We get F1, F2 etc..		*/
      noecho();			         /* Don't echo() while we do getch */
      curs_set(0);           /* Do not display cursor */
}

void Teclado::stop() {
      this->rodando = 0;
      (this->kb_thread).join();
}

char Teclado::getchar() {
      char c = this->ultima_captura;
      this->ultima_captura = 0;
      return c;
}


void Teclado::init_socket(int connection){
	this->fd = (connection);
	this->rodando = 1;
	
	std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando), this->fd, this->buffer);
	(this->kb_thread).swap(newthread);
	
}



