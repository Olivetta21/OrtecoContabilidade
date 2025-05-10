#include "certificado.hpp"
#include <vector>
#include <fstream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "E:\SINCRO GDRIVE\School\Codigos\CPP\ApenasNumeros\convertCharToNum.cpp"
#define Dados_ "E:\SINCRO GDRIVE\School\Codigos\CPP\Orteco\GerenciarCertificados\certs.txt"
#define COLUNAS 4

Certificado::Certificado() {
	nome = lugar = "null\0";
	codigo = situacao = 0;
}

void Certificado::setCodigo(int cod) {
	codigo = cod;
}

int Certificado::getCodigo() {
	return codigo;
}

void Certificado::setSituacao(bool sit) {
	situacao = sit;
}

int Certificado::getSit() {
	return situacao;
}

istream& operator>>(istream& is, Certificado& cert) {
	getline(is, cert.nome);
	getline(is, cert.lugar);
	if (cert.nome.length() > TAM) cert.nome.resize(TAM);
	if (cert.lugar.length() > TAM) cert.lugar.resize(TAM);
	return is;
}

ostream& operator<<(ostream& os, const Certificado& cert) {
	os << "|\tNome: " << cert.nome << "\n|\tOnde: " << cert.lugar << "\n|\tCodigo: " << cert.codigo;
	return os;
}

void Certificado::importar(string& nom, string& lug, int cod, bool sit) {
	if (nom.length() > TAM) nom.resize(TAM);
	if (lug.length() > TAM) lug.resize(TAM);
	nome = nom;
	lugar = lug;
	codigo = cod;
	situacao = sit;
}



vector<string>& pesqParaVector(vector<string>& _vec, string& _str) {
	cout << endl << "str" << _str;
	string temp;
	temp.resize(_str.size());
	for (int i = 0, j = 0 ; i < _str.size(); i--) {
		int test = _str[i];
		if ((test >= 65 && test <= 90 ) || (test >= 97 && test <= 122) || (test >= 48 && test <= 57)) temp[j++] = _str[i];
	}

	cout << endl << "tem" << temp;

	system("pause");

	return _vec;
}






























void delay(int sec) {
	system("cls");
	cout << "Aguarde \\\n";
	this_thread::sleep_for(chrono::seconds(sec/2));
	system("cls");
	cout << "Aguarde /\n";
	this_thread::sleep_for(chrono::seconds(sec/2));
}

void ler(Certificado* &cert, int& qntsCertificados) {
	ifstream dados;

	while (!dados.is_open()) {
		dados.open(Dados_);
		if (!dados.is_open()) {
			cout << Dados_ << " nao encontrado\n";
			delay(1);
		}
	}

	string linha;
	getline(dados, linha);

	qntsCertificados = charToNum(linha);
	delete[] cert;
	cert = new Certificado[qntsCertificados];

	string nome;
	string lugar;
	int codigo;
	bool situacao;

	for (int i = 0; i < qntsCertificados; i++) {
		if (!dados.good()) continue;
		getline(dados, linha);
		for (int i = 0, pos = -1, ult; i < COLUNAS; i++) {
			ult = pos+1;
			pos = linha.find(";", ult);
			string subtraido = linha.substr(ult, pos - ult);
			switch (i) {
			case 0: {nome = subtraido; break; }
			case 1: {lugar = subtraido; break; }
			case 2: {codigo = charToNum("0" + subtraido); break; }
			case 3: {situacao = charToNum("0" + subtraido); break; }
			}
		}
		cert[i].importar(nome, lugar, codigo, situacao);
	}
	dados.close();
}

void porCod(Certificado*& cert, const int& tam, const int& cod) {
	for (int i = 0; i < tam; i++) {
		if (cert[i].codigo == cod) {
			if (cert[i].getSit()) cout << "---------------------------\n" << cert[i] << "\n---------------------------\n";
		}
	}
}
void porNome(Certificado*& cert, const int& tam, const string& nome_) {
	for (int i = 0; i < tam; i++) {
		int pos = cert[i].nome.find(nome_);
		if (pos != string::npos) {
			if (cert[i].getSit()) cout << "---------------------------\n" << cert[i] << "\n---------------------------\n";
		}
	}
}

void pesquisar(Certificado* &cert, const int& tam) {
	cout << "Pesquisar Certificado\nuse apenas \"#\" para mostrar todos os certificados\nuse #\"numero\" para pesquisar por codigo\nou apenas digite uma parte do nome\n\nPesquisa: ";
	string pesq;
	cin >> pesq;

	int* possiblePositions = nullptr;

	cout << "Resultado:\n\n";

	if (pesq == "#") {
		for (int i = 0; i < tam; i++) {
			if (cert[i].getSit()) cout << i + 1 << ":\n---------------------------\n" << cert[i] << "\n---------------------------\n";;
		}
	}
	else if (pesq[0] == '#') {
		porCod(cert, tam, charToNum(pesq));
	}
	else porNome(cert, tam, pesq);
}