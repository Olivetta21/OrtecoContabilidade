#include "certificado.hpp"
#include <vector>
#include <fstream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <conio.h>
#include "E:/SINCRO GDRIVE/School/Codigos/CPP/ApenasNumeros/convertCharToNum.cpp"
#include "E:/SINCRO GDRIVE/School/Codigos/CPP/Cores/corObjetos.cpp"
#define Diretorio_ "diretorios.txt"
#define DadosBkp_ "BackUps/"
#define COLUNAS 4
#define Branco Cor(255,255,255)
#define Vermelho Cor(255,80,80)
#define Azul Cor(110,230,255)
#define Verde Cor(0,255,0)
#define Amarelo Cor(255,255,0)

Dados_* myLoc = nullptr; //Isso so eh definido em tempo de execucao, apenas uma vez.


bool pickDir(void) {
	ifstream dires("diretorios.txt");
	string tempLocal;

	if (!dires.is_open()) return 0;
	getline(dires, tempLocal);
	static Dados_ database(tempLocal);
	myLoc = &database;

	ifstream tryOpen(myLoc->getDados());
	if (!tryOpen.is_open()) return 0;
	else tryOpen.close();
	dires.close();
	
	string a = DadosBkp_; a += "teste.teste";
	ofstream createTest(a);
	if (!createTest.is_open()) return 0;
	else {
		createTest.close();
		remove(a.c_str());
	}

	return 1;
}

void cinWcleanBuff(string& str) {
	cin.clear();
	//cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << Branco;
	getline(cin, str);
}

Certificado::Certificado() {
	nome = lugar = "null\0";
	codigo = situacao = 0;
}

string Certificado::getNome() {
	return nome;
}

string Certificado::getLugar() {
	return lugar;
}

void Certificado::setLugar(const string& _lugar) {
	lugar = _lugar;
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
	cout << "\n\nARRUME ISSO\n\n";
	getline(is, cert.nome);
	getline(is, cert.lugar);
	if (cert.nome.length() > TAM) cert.nome.resize(TAM);
	if (cert.lugar.length() > TAM) cert.lugar.resize(TAM);
	return is;
}

ostream& operator<<(ostream& os, const Certificado& cert) {
	string tempPlace = cert.lugar;
	if (tempPlace[tempPlace.size() - 1] == '.') tempPlace.resize(tempPlace.size() - 1);
	string tempNome = cert.nome;
	if (tempNome[tempNome.size() - 1] == '.') tempNome.resize(tempNome.size() - 1);
	
	os << Azul << "|\tNome: " << Amarelo << tempNome
	<< Azul << "\n|\tOnde: " << Amarelo << tempPlace
	<< Azul << "\n|\tCodigo: " << Amarelo << cert.codigo;
	cout << Azul;
	return os;
}

bool Certificado::porNome(const string& nome_) {
	if (!getSit()) return 0;
	if (nome.find(nome_) != string::npos) return 1;
	return 0;
}

bool Certificado::porLugar(const string& lugar_) {
	if (!getSit()) return 0;
	if (lugar.find(lugar_) != string::npos) return 1;
	return 0;
}

bool Certificado::porCod(const int& _cod) {
	if (!getSit()) return 0;
	if (codigo == _cod) return 1;
	return 0;
}

void Certificado::importar(string& nom, string& lug, int cod, bool sit) {
	if (nom.length() > TAM) nom.resize(TAM);
	if (lug.length() > TAM) lug.resize(TAM);
	nome = nom;
	lugar = lug;
	codigo = cod;
	situacao = sit;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void delay(int sec) {
	system("cls");
	cout << "Aguarde \\\n";
	this_thread::sleep_for(chrono::seconds(sec));
	system("cls");
	cout << "Aguarde /\n";
	this_thread::sleep_for(chrono::seconds(sec));
}

string dataAtual() {
	time_t rawtime;
	struct tm timeinfo;

	// Obter a hora atual
	time(&rawtime);

	// Converter a hora para uma estrutura tm usando localtime_s
	if (localtime_s(&timeinfo, &rawtime) != 0) {
		cout << "Erro ao obter a data e hora.\n";
		system("pause");
		return "0";
	}

	string dat;
	int tmp = 0;
	dat += to_string(timeinfo.tm_year + 1900);

	tmp = timeinfo.tm_mon + 1;
	dat += tmp < 10 ? "0" + to_string(tmp) : to_string(tmp);

	tmp = timeinfo.tm_mday;
	dat += tmp < 10 ? "0" + to_string(tmp) : to_string(tmp);

	dat += "_";

	tmp = timeinfo.tm_hour;
	dat += tmp < 10 ? "0" + to_string(tmp) : to_string(tmp);

	tmp = timeinfo.tm_min;
	dat += tmp < 10 ? "0" + to_string(tmp) : to_string(tmp);

	return dat;
}

inline void strToUpprCase(string& str) {
	for (int i = 0, t = str.size(); i < t; i++) str[i] = toupper(str[i]);
}

void ler(Certificado* &cert, int& qntsCertificados) {
	ifstream dados;

	while (!dados.is_open()) {
		dados.open(myLoc->getDados());
		if (!dados.is_open()) {
			cout << myLoc->getDados() << " nao encontrado\n";
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
			strToUpprCase(subtraido);
			if (subtraido[subtraido.size() - 1] != '.') subtraido.push_back('.');
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

bool gravar(Certificado* &cert, const int& tam) {
	bool ok = 0;
	ifstream copia;
	copia.open(myLoc->getDados());
	if (copia.is_open()) {
		ofstream criarCopia;
		criarCopia.open(DadosBkp_+dataAtual()+".BKUP");
		if (criarCopia.is_open()) {
			string linha;
			while (getline(copia, linha)) {
				if (!copia.bad()) {
					criarCopia << linha << endl;
					if (!criarCopia.good()) { cout << "!criarCopia.good\n"; return 0; }
				}
				else { cout << "!copia.good\n"; return 0; }
			}
			if (copia.eof()) ok = true;
			criarCopia.close();
		}
		copia.close();
	}

	if (ok) {
		ofstream dados;
		dados.open(myLoc->getDados());
		dados << tam << endl;
		if (!dados.good()) return 0;
		for (int i = 0; i < tam; i++) {
			dados << cert[i].getNome() << ";" << cert[i].getLugar() << ";" << cert[i].getCodigo() << ";" << cert[i].getSit() << endl;
			if (!dados.good()) return 0;
		}
		return 1;
	}
	else return 0;
}

void pesquisar(Certificado* &cert, const int& tam, const vector<string> &pesqs, vector<Certificado*> &pesqCerts) {
	for (int i = 0; i < tam; i++) {
		for (const string& pesq : pesqs) {
			bool found = 0;
			if (cert[i].porLugar(pesq)) found = 1;
			else if (cert[i].porNome(pesq)) found = 1;
			if (found) {
				pesqCerts.push_back(&cert[i]);
				break;
			}
		}
	}
}

void pesqParaVector(vector<string>& _vec, string& _str) {
	strToUpprCase(_str);
	_str += ';';
	string temp;
	for (int i = 0, j = 0; i < _str.size(); i++) {
		if (_str[i] != ';') temp += _str[i];
		else if (temp != "") {
			_vec.push_back(temp);
			temp.erase();
		}
	}
}

void selecionar(Certificado*& cert, const int& tam, vector<string>& _vecStr, vector<Certificado*>& _vecCert) {
	int tamOfVecStr = _vecStr.size();
	for (int i = 0; i < tam && tamOfVecStr >= 1; i++) {
		for (string &vecStr : _vecStr) {
			if (cert[i].porCod(charToNum(vecStr))) {
				_vecCert.push_back(&cert[i]);
				vecStr = _vecStr[tamOfVecStr - 1];
				_vecStr.resize(tamOfVecStr - 1);
				break;
			}
		}
		tamOfVecStr = _vecStr.size();
	}
}

bool confirmacao(const string msg = "") {
	cout << Vermelho << msg;
	cout << Azul;
	char opt = _getch();
	cout << endl;
	if (opt == 's' || opt == 'S') return 1;
	return 0;
}

bool alteraLocal(Certificado* &certs, const int &tam, const string& _local, vector<Certificado*>& certSelecionados) {
	if (_local.find(';') != string::npos) {
		cout << "O caractere ';' eh proibido!\n";
		return 1;
	}
	cout << Vermelho << "Mover os certificados selecionados para \"" << Verde << _local << Vermelho << "\"";
	if (confirmacao(" ? (s / n) : ")) {
		for (Certificado*& vec : certSelecionados) vec->setLugar(_local);
		if (gravar(certs, tam)) cout << "Movido com sucesso!";
		else cout << "Erro na gravacao!";
	}
	else {
		if (confirmacao("Digitar outro local? (s/n): ")) return 1;
	}
	return 0;
}
