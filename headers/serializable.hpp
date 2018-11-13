#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "modelo.hpp"

class ListaDeCobra;
class Comida;

struct DataContainer {
	int a;
	int b;
};

class RelevantData {
	private:
		Comida *food;
		ListaDeCobra *lista; // cobras
		std::vector<DataContainer > dados; // esse container vai ter realmente os dados finais que vou serializar
	public:
		RelevantData();
		void dados_print();
		RelevantData(Comida *food, ListaDeCobra *lista);
		std::vector<DataContainer > get_dados();
		void serialize(char *buffer_out,int ID, int perdeu, int comeu);
		void unserialize(char *bufferin);
		void clean();
		void dump();
		void update(ListaDeCobra *lista, Comida *food);
};

#endif
