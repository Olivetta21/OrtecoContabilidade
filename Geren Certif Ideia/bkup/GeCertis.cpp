#include "GeCertis.hpp"
#define vTam 2

void ler(Certificado* &, int&);
void pesquisar(Certificado*&, const int&);
vector<string>& pesqParaVector(vector<string>&, string&);

int main() {
	//string password;
	//cout << "Login: ";
	//cin >> password;
	//
	//if (password == "@leitura&*");
	//else if (password == "@escrita&*");

	short opt = 0;

	do {
		string pesquisa = "";
	
		system("cls");
		cout << "Realize a pesquisa, ou selecione os codigos diretamente usando \"+ 'codigo'\""
			<< ", utilize \";\" para separar os codigos.\n\n - Pesquisar por: ";
		cin >> pesquisa;

		vector<string> vecOfSearch;

		pesqParaVector(vecOfSearch, pesquisa);

		Certificado* certificados = nullptr;
		int tamanho;

		ler(certificados, tamanho);
		pesquisar(certificados, tamanho);

		cout << "\nretorne: ";  cin >> opt;
	} while (opt != 0);

}

