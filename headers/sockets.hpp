
#include "serializable.hpp"
#include <thread>
#include <mutex>
#define MAX_CONEXOES 2
typedef struct sock{
	int connection_client[MAX_CONEXOES]; 
	int connection_usada[MAX_CONEXOES];	
}Sock;

void threadservidor1 (char *keybuffer, int *control, int connection, char *buffer);// thread para receber dados do teclado de cliente
void threadservidor2(int *control,char *buffer, int connection);
void threadcliente1  (char *keybuffer, int *control);// thread para enviar dados do teclado para servidor
void threadcliente2  ( int *control, int *connection, char *buffer);
void *wait_connections(int *connection, Sock *estr_client);/* retorna 1 se houver uma ou mais conexoes*/
int adicionar_conexao(int new_connection_fd,Sock *estr_client);

class Sockets{
private:
	RelevantData *D1;
        int connection_fd;
        char input_buffer1[1024];
	char output_buffer1[50];
        int socket_fd;
	Sock estr_client;
        char ultima_captura;
        int rodando;
	std::thread sk1_thread,sk2_thread,server_thread;
      
public:
	Sockets();
	int ultimo_cliente();
	Sockets(RelevantData *D1);
	void init_servidor();
	void remover_cliente(int id);
	int remover_conexao();
	int play();
	int get_fd(int i);
	void init_cliente();
	void stop_socket();
	int get_socketfd();
	char getchar();
	void unserial();
	void serial(RelevantData *D1, int ID);
	void init_sockets(int fd, std::thread t1, std::thread t2);
	
};