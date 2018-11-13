
#include "../headers/serializable.hpp"
#include "../headers/modelo.hpp"
#include <string>
#include <cstring>
#include <ncurses.h>
#include <vector>



RelevantData::RelevantData() {
};

RelevantData::RelevantData(Comida *food, ListaDeCobra *lista) {
	this->food = food;
	this->lista = lista;
}
void RelevantData::dump() {
	int i, j;
	DataContainer aux;
	std::vector<DataContainer > posicoes = this->get_dados();
	int tamanho = posicoes[0].a;
	int num_cobras = posicoes[0].b;
	int perdeu = posicoes[2].a;
	int comeu = posicoes[2].b;
	
	mvprintw(15,0,"%d ",perdeu);
	mvprintw(16,0,"%d ",comeu);
	
	for(i = 1,  j = 3; i <= num_cobras; i++, j++){ // agora atribuo para as cobras
		aux = posicoes[j];
		mvprintw(10,10,"TAM E ID");
		mvprintw(15,j,"%d ",posicoes[j].a);
		mvprintw(16,j,"%d ",posicoes[j].b);
		for(int k = 0; k < aux.a; k++){ // para cada corpo pertencente a cobra
			j++; // posso variar o que for printar usando aux.b, mas deixar isso para depois de tudo OK
// 			mvprintw(15,i,"%d ",this->dados[i].a);
// 			mvprintw(16,i,"%d ",this->dados[i].b);
		}
	}
	
	
// 	for( int i = 0; i < this->dados.size(); i++){ // enfia corpo no buffer
// 		mvprintw(15,i,"%d ",this->dados[i].a);
// 		mvprintw(16,i,"%d ",this->dados[i].b);
// 		mvprintw(0,0,"%d",i);
// 	}
}

void RelevantData::dados_print(){
	std::vector<DataContainer > posicoes = this->dados;
	mvprintw(5,5,"COMIDA ESTA EM %d , %d",posicoes[1].a,posicoes[1].b);
}
void RelevantData::clean(){
		this->dados.clear();
}

std::vector<DataContainer > RelevantData::get_dados(){
	return this->dados;	
}

void RelevantData::serialize(char *bufferout, int ID, int perdeu, int comeu){
	DataContainer aux, init,food, audio;
	char corpo = 'A';
	this->dados.clear(); // limpo oque tinha escrito antes para montar novamente
	std::vector<ListaDeCorpos *> *l = this->lista->get_corpos(); // obtenho os corpos de cada cobra pra serializar
	init = {0,(int)l->size()};/* comeca cm tam. tot = 0, e com o numero de cobras ja sabido*/
	this->dados.push_back(init);
	food = {this->food->getcx(),this->food->getcy()};
	this->dados.push_back(food); // insiro comida no vector
	this->dados[0].a++;// ja incremento o numero de elem, i.e, ja tenho uma comida
	// para cada elemento na lista de Cobra_corpo
	audio = {perdeu,comeu};/* para cliente saber qd perdeu ou comeu e depois aplicar audio*/
	this->dados.push_back(audio); // insiro comida no vector
	this->dados[0].a++;// ja incremento o numero de elem, i.e, ja tenho uma comida
	
	for(int k = 0; k < l->size(); k++){
		std::vector<Cobra_corpo *> *c = (*l)[k]->get_corpos();// para cada cobra, pego seu corpo 
		// tenho que colocar antes do corpo em si, o numero de elem que o corpo tem
		this->dados[0].a++;
		aux.a = c->size();
		aux.b = k;
		this->dados.push_back(aux);
		for(int i = 0; i < c->size(); i++){ // para cada corpo do meu vector eu adicionarei-o no vector out
			this->dados[0].a++;
			aux.a = (*c)[i]->get_posicaoX();
			aux.b = (*c)[i]->get_posicaoY();
			this->dados.push_back(aux);
		}
	}
	// agora tenho que copiar esses dados para buffer e depois retira-los
	for( int i = 0; i < this->dados.size(); i++){ // enfia corpo no buffer
		DataContainer envia = this->dados[i];
		std::memcpy(&bufferout[i*sizeof(DataContainer)], &envia, sizeof(DataContainer));
	}
}
void RelevantData::update(ListaDeCobra *lista, Comida *food){
	this->lista = lista;
	this->food = food;
}

void RelevantData::unserialize(char *bufferin) {
	DataContainer init,aux;
	int num_cobras;
	int j;
	int i;
	std::memcpy(&init, &bufferin[0], sizeof(DataContainer)); // fazendo isso, consigo pegar num total da string e num de cobras
	num_cobras = init.b;
	
	for( i = 0; i <= init.a; i++){
		std::memcpy(&aux, &bufferin[i*sizeof(DataContainer)], sizeof(DataContainer));
		this->dados.push_back(aux);
	}
}
