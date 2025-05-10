#include <iostream>
#include <fstream>
#include <string>
#define TAM 70
using namespace std;

class Certificado {
private:
	string nome;
	string lugar;
	int codigo;
	bool situacao;
public:
	Certificado();
	void setCodigo(int);
	int getCodigo();
	void setSituacao(bool);
	int getSit();
	friend istream& operator>>(istream&, Certificado&);
	friend ostream& operator<<(ostream&, const Certificado&);
	friend void porCod(Certificado*&, const int&, const int&);
	friend void porNome(Certificado*&, const int&, const string&);
	void importar(string&, string&, int, bool);
	bool cancelarCertificado(int cod);
};