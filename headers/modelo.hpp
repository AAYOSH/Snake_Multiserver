


#ifndef modelo_hpp
#define modelo_hpp

#include <thread>
#include <vector>
#include "serializable.hpp"
// acho que completo


class RelevantData;

class Cobra_corpo{
      private:
	    int velocidadeX;
	    int velocidadeY;
	    int posicaoX;
	    int posicaoY;
	    int oldX;
	    int oldY;
	    
      public:
	    Cobra_corpo(int velocidadex,int velocidadey, int posicaoX, int posicaoY);
	    void update(int nova_posicaoX, int nova_posicaoY);
	    void update_vel(int nova_velX, int nova_velY);
	    void update_old(int posicaoX, int posicaoY);
	    int get_velocidadeX();
	    int get_velocidadeY();
	    int get_posicaoX();
	    int get_posicaoY();
	    int get_oldX();
	    int get_oldY();
};



//classe que agrega varios Cobra_corpos
class ListaDeCorpos {
private:
      std::vector<Cobra_corpo *> *corpos;
      
public:
      ListaDeCorpos();
      void add_corpo(Cobra_corpo *c);
      std::vector<Cobra_corpo *> *get_corpos();

};
// clase que agrega varias cobras
class ListaDeCobra {
private:
	std::vector<ListaDeCorpos *> *cobras;
public:
	ListaDeCobra();
	void add_corpo(int i,Cobra_corpo *c);
	int maior_cobra();
	void add_cobra(ListaDeCorpos *l);
	std::vector<ListaDeCorpos *> *get_corpos();
	
};

// classe que verifica se a cobra em questao se colidiu
class Choque {
      private:
	    ListaDeCobra *cobra;
      
      public:
	    Choque(ListaDeCobra *cobra);
	    bool colisao(ListaDeCobra *cobra, int ID);
	    bool colisao_cobras(ListaDeCobra *cobra, int ID);
      
};

// classe que dispoem comida na tela
class Comida{
      private:
	    int posicaoX;
	    int posicaoY;
      public:
	    Comida();
	    int getcx();
	    int getcy();
	    void update();
	    void update_serial(int nova_posicaoX, int nova_posicaoY);
};

// classe responsavel pelo movimento dos corpos
class Movimento {
      private:
	    Comida *food;
	    ListaDeCobra *cobra;
      public:
	    Movimento( Comida *food, ListaDeCobra *cobra);
	    int update(void);
};



void threadfun (char *keybuffer, int *control, int connection , char *buffer);

class Teclado{
      private:
	    char ultima_captura;
	    int rodando;
	    int fd;
	    char buffer[2];
	    std::thread kb_thread;
      public:
	    Teclado();
	    ~Teclado();
	    void stop();
	    void init();
	    void init_socket(int connection);
	    char getchar();
};

class Tela{
      private:
	    ListaDeCorpos *lista;
	    Comida *food;
	    RelevantData *D1;
	    ListaDeCobra *cobra;
	    
      public:
	    Tela(ListaDeCorpos *ldc,Comida *food, RelevantData *D1, ListaDeCobra *cobra);
	    ~Tela();
	    void stop();
	    void init();
	    void update();
};

class Tela_client{
private:
	RelevantData *D1;
public:
	
	Tela_client(RelevantData *D1);
	~Tela_client();
	void stop_client();
	void init_client();
	int update_client();
	
	
};












#endif