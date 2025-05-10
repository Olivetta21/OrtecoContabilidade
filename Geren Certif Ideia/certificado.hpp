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
	string getNome();
	void setLugar(const string&);
	string getLugar();
	void setCodigo(int);
	int getCodigo();
	void setSituacao(bool);
	int getSit();
	friend istream& operator>>(istream&, Certificado&);
	friend ostream& operator<<(ostream&, const Certificado&);
	bool porCod(const int&);
	bool porNome(const string&);
	bool porLugar(const string&);
	void importar(string&, string&, int, bool);
	bool cancelarCertificado(int cod);
};

class Dados_ {
private:
	string local;
public:
	Dados_(string loc) : local(loc) {}
	inline string getDados(void) { return this->local; }
};