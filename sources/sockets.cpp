

#include "../headers/sockets.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>



int adicionar_conexao(int new_connection_fd,Sock *estr_client) {
	int i;
	for (i=0; i<MAX_CONEXOES; i++) {
		if (estr_client->connection_usada[i] == 0) {
			estr_client->connection_usada[i] = 1;
			estr_client->connection_client[i] = new_connection_fd;
			return i;
		}
	}
	return -1;
}

int Sockets::remover_conexao() {
	for(int user = 0; user < MAX_CONEXOES; user++)
		if (this->estr_client.connection_usada[user]==1) {
			this->estr_client.connection_usada[user] = 0;
			close(this->estr_client.connection_client[user]);
		}
	return 1;
}

void * wait_connections(int *connection, Sock *estr_client) {
	struct sockaddr_in client;
	socklen_t client_size = (socklen_t)sizeof(client);
	int conn_fd;
	int user_id;
	int running = 1; // temporario
	int i = 0;
	while(running) {
		conn_fd = accept(*connection, (struct sockaddr*)&client, &client_size);
		user_id = adicionar_conexao(conn_fd,estr_client);
		i++;
	//	mvprintw(10,i,"Cliente : %d conectado %d", i,conn_fd);
		// a partir daqui sei que existe a connectionfd e tenho que criar uma thread para o client 
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
	return NULL;
}

//essa thread é feita para enviar caracteres do servidor ao cliente
void threadservidor2( int *control,char *buffer, int connection) // preciso saber o socket tb
{
	int x;
	// para cada client, usar send com seu id connection e enviar os dados a serem serializados
      while ((*control) == 1) {// CASO ESTEJA RODANDO
		x = send(connection, buffer, 1000, 0);
	    std::this_thread::sleep_for (std::chrono::milliseconds(10));
      }
      return;
}
// essa thread é feita para enviar caracteres do cliente ao servidor
void threadcliente1(char *keybuffer, int *control)
{
      char c;
      while ((*control) == 1) {
	    c = getch();
	    if (c!=ERR) (*keybuffer) = c;
	    else (*keybuffer) = 0;
	    std::this_thread::sleep_for (std::chrono::milliseconds(10));
      }
      return;
}
// thread que recebe os dados do model do servidor em input_buffer1
void threadcliente2( int *control, int *connection, char *buffer)
{
      char c;
      while ((*control) == 1) {
            c = recv(*connection, buffer, 1000, 0); /* c indica o numero de caracteres recebidos*/
	    std::this_thread::sleep_for (std::chrono::milliseconds(10));
      }
      return;
}


Sockets::Sockets() {
}

Sockets::Sockets(RelevantData *D1){
	this->D1 = D1;
}

void Sockets::unserial(){
	this->D1->unserialize(this->input_buffer1);
}
/*
void Sockets::serial(RelevantData *D1, int ID){
	this->D1 = D1;
	this->D1->serialize(this->input_buffer1,ID);
	
}*/
void Sockets::init_cliente(){ 

    struct sockaddr_in target;

    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket criado\n");

    target.sin_family = AF_INET;
    target.sin_port = htons(3001);//192.168.0.34
    inet_aton("177.220.34.186", &(target.sin_addr));
    printf("Tentando conectar\n");
    if (connect(this->socket_fd, (struct sockaddr*)&target, sizeof(target)) != 0) { // conect: 
           printf("Problemas na conexao\n");
	   exit(0);
    }
    printf("Conectei ao servidor\n");
    // inicializa thread, cliente envia dados do teclado a servidor
     this->rodando = 1;
     std::thread newthread(threadcliente1, &(this->ultima_captura), &(this->rodando));
     (this->sk1_thread).swap(newthread);
    // inicializa thread, cliente recebe dados do servidor
     std::thread newthread1(threadcliente2, &(this->rodando),&(this->socket_fd),(this->input_buffer1));
     (this->sk2_thread).swap(newthread1);
}

void Sockets::init_servidor() { /* consegue se conectar*/
	
    // inicializcao do sockets
        struct sockaddr_in myself;

        this-> socket_fd = socket(AF_INET, SOCK_STREAM, 0); // sockstream = TCP
        printf("Socket criado\n");
        myself.sin_family = AF_INET;
        myself.sin_port = htons(3001);
	inet_aton("177.220.34.186", &(myself.sin_addr));
        printf("Tentando abrir porta 3001\n");
        if (bind(this->socket_fd, (struct sockaddr*)&myself, sizeof(myself)) != 0) {
            printf("Problemas ao abrir porta\n");
        }
        printf("Abri porta 3001!\n");
        listen(this->socket_fd, 2);
        printf("Estou ouvindo na porta 3001!\n");
	
	for(int i = 0; i < MAX_CONEXOES; i++)
		this->estr_client.connection_usada[i] = 0;
	
	this->rodando = 1;
	std::thread thread_nova(wait_connections,&(this->socket_fd),&(this->estr_client)); // colocar parm necess  
	(this->server_thread).swap(thread_nova);
	
}

int Sockets::play(){ /*comeca a rodar a cobra se pelo menos 1 cliente esta utilizando*/
	int k = 0;
	for( int i = 0; i< MAX_CONEXOES; i++)
		if(this->estr_client.connection_usada[i] != 0)
			k++;
	return k;
}

int Sockets::get_fd(int i){
	return this->estr_client.connection_client[i];
}

int Sockets::get_socketfd() {
    return this->socket_fd;
}

int Sockets::ultimo_cliente(){
		int k = 0;
		for( int i = 0; i< MAX_CONEXOES; i++){
			if(this->estr_client.connection_usada[i] != 0)
				k++;
		} /* retorna numero de clientes*/
		return k;
}

void Sockets::stop_socket() {
      this->rodando = 0;
      (this->sk1_thread).join();
      close(this->socket_fd);
}
void Sockets::remover_cliente(int id) {
	this->estr_client.connection_usada[id] = 0;
	close(this->estr_client.connection_client[id]);
}

char Sockets::getchar() {
      char c = this->ultima_captura;
      this->ultima_captura = 0;
      return c;
}
