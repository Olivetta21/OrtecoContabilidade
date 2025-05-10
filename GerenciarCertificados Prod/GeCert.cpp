#include <iostream>
#include <Windows.h>
#include "sqlite3.h"
#include <string>
#include <conio.h>
#include <vector>
#include "licenca.hpp"
#include "E:/SINCRO GDRIVE/School/Codigos/CPP/ApenasNumeros/convertCharToNum.cpp"
#include "E:/SINCRO GDRIVE/School/Codigos/CPP/Cores/corObjetos.cpp"
//#define GERARLICENCA
//#define Branco Cor(255,255,255)
#define Laranja Cor(255, 165, 0)
#define Vermelho Cor(255,80,80)
#define Azul Cor(110,230,255)
#define AzulEsc Cor(105,115,225)
#define Verde Cor(0,255,0)
#define Amarelo Cor(255,255,0)
#define Cinza Cor(40,40,40)
#define LENGH_NOME 70
#define LENGH_LOCAL 16
//#define SYSLOG
//using namespace std;


sqlite3* db = nullptr;	//Banco de dados.
vector<string> _pesqs;	//Vetor onde podera ficar os nomes/locais ou codigos.


int encontrado = 0;	//Numero da quandidade de certificados que aparece na tela a cada consulta.
bool canShowLoca = 0;
string autorModific = "-*";

void espc(int qnt, const Cor &cor, const char ch = '_') {
	//Imprimir underlines (para padronizar as colunas).
	cout << cor;
	while (qnt-- > 0) cout << ch;
}



//-HORARIO-//
std::string twoDig(short data) {
	std::string dataStr;
	if (data < 10) dataStr = "0";
	dataStr += std::to_string(data);
	return dataStr;
}
std::string getTime_now(int info) {
	static SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	std::string tmpTime;

	switch (info) {
	case 1:
		tmpTime += std::to_string(sysTime.wYear) + "-" + twoDig(sysTime.wMonth) + "-" + twoDig(sysTime.wDay) + " ";
	case 2:
		tmpTime += twoDig(sysTime.wHour) + ":" + twoDig(sysTime.wMinute) + ":" + twoDig(sysTime.wSecond);
		break;
	case 3:
		tmpTime += std::to_string(sysTime.wYear) + "-" + twoDig(sysTime.wMonth) + "-" + twoDig(sysTime.wDay);
		break;
	default:
		break;
	}

	return tmpTime;
}


void mostrarCert(sqlite3_stmt* &stmt, const Cor &cor) {
	//Mostar as colunas fazendo o calculo para deixar elas com o mesmo tamanho.
	cout << cor << "Nome: " << Amarelo << sqlite3_column_text(stmt, 0);
	espc(LENGH_NOME - strlen((const char*)sqlite3_column_text(stmt, 0)), cor);
	
	string local = canShowLoca ? (const char*)sqlite3_column_text(stmt, 1) : "*****";
	cout << cor << " Local: " << Amarelo << local;
	espc(LENGH_LOCAL - local.size(), cor);
	
	cout << cor << " Codigo: " << Amarelo << sqlite3_column_int(stmt, 2) << '\n';

	//Arrumando quem visualizou, e testando se nao eh um nullptr.
	string vistos = (sqlite3_column_text(stmt, 4) == nullptr ? "" : (const char*)sqlite3_column_text(stmt, 4));
	(sqlite3_column_text(stmt, 5) == nullptr ? "" : vistos += " e " + string((const char*)sqlite3_column_text(stmt, 5)));

	string infosAdicional = to_string(encontrado) + "- Vencimento: " + string((const char*)sqlite3_column_text(stmt, 3)) + " - Visto por: " + vistos;
	cout << Cinza << infosAdicional;	//Mostrando as informacoes adicionais.
	espc(112 - (infosAdicional.size()), Cinza, '-');
}


int prepareAndSearch(const string& command) {

	_pesqs.clear();	//Limpando, pois usaremos para armazenar os codigos dos certificados que aparecer na pesquisa.
	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(db, command.c_str(), -1, &stmt, 0) != SQLITE_OK) {
		cout << Vermelho << "Erro ao preparar consulta: " << sqlite3_errmsg(db) << endl;
		return 1; // Retorna um valor negativo para indicar erro
	}

	encontrado = 0; //Resetar a contagem.

	espc(112, Cinza, '-');
	cout << endl;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		encontrado++;
		Cor cor;
		//Definindo a cor da linha, para ficar melhor para ler.
		if (encontrado % 2 == 0) cor = AzulEsc;
		else cor = Azul;
		mostrarCert(stmt, cor);	//Mostrar o certificado com a cor definida anteriormente.
		_pesqs.push_back((const char*)sqlite3_column_text(stmt, 2));	//Sempre que mostrar um certificado, armazenar o codigo dele no vetor _pesqs.
		cout << endl;
	}
	sqlite3_finalize(stmt);

	if (encontrado <= 0) {
		cout << Vermelho << "Nenhum certificado foi encontrado!\n";
		return 2;
	}

	return 0;
}


int prepareAndUpdate(const string& command) {
	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(db, command.c_str(), -1, &stmt, 0) != SQLITE_OK) {
		cout << Vermelho << "Erro ao preparar consulta: " << sqlite3_errmsg(db) << endl;
		return 1; // Retorna um valor negativo para indicar erro
	}

	if (sqlite3_step(stmt)) {
		sqlite3_finalize(stmt);
		return 0;
	}

	cout << Vermelho << "Nao foi possivel fazer essa alteracao!\n";
	return 2;
}


int pesquisarCertificado(int tipodaconsulta) {	//1=nome 2=codi 3=local
	//Montando o comando usando o conteudo de _pesqs, nessa funcao o _pesqs sempre sera um vetor de nome e/ou local.
	//string command = "SELECT UPPER(nome), UPPER(loca), codi, venc, penultimovisto, ultimovisto FROM certys WHERE nome LIKE '";
	string command = "select upper(c.nome), upper(c.loca), c.codi, c.venc, i.autor, ii.autor "
		"from certys c left join visualizacao i on i.id = (select id from visualizacao where id_cert = c.codi order by id desc) "
		"left join visualizacao ii on ii.id = ("
		"select id from visualizacao where id_cert = c.codi and id < (select id from visualizacao where id_cert = c.codi order by id desc) order by id desc"
		") where";

	bool prim = 1;

	switch (tipodaconsulta){
	case 1: {	//Consulta por nome.
		command += " c.nome LIKE '";
		for (string& p : _pesqs) {
			string e = "%";

			if (p[p.size() - 1] == '=') {
				e = "";
				p.pop_back();
			}

			if (prim) {
				command += e + p + e + "'";
				prim = 0;
			}
			else {
				command += " OR c.nome LIKE '" + e + p + e + "'";
			}
		}
		command += " ORDER BY UPPER(c.nome);";
		break;
	}
	case 2: {	//Consulta por codi.
		//Nessa funcao o _pesqs sempre sera um vetor de codigos.
		command += " c.codi IN (";
		for (const string& p : _pesqs) {
			int num = charToNum(p);

			if (prim) {
				command += to_string(num);
				prim = 0;
			}
			else {
				command += ", " + to_string(num);
			}
		}
		command += ") ORDER BY UPPER(c.nome);";
		break;
	}
	case 3: {
		command += " c.loca LIKE '";
		for (string& p : _pesqs) {
			string e = "%";

			if (p[p.size() - 1] == '=') {
				e = "";
				p.pop_back();
			}

			if (prim) {
				command += e + p + e + "'";
				prim = 0;
			}
			else {
				command += " OR c.loca LIKE '" + e + p + e + "'";
			}
		}
		command += " ORDER BY UPPER(c.nome);";
		break;
	}
	default:
		break;
	}

#ifdef SYSLOG
	cout << AzulEsc << command << "\n";
	system("pause");
#endif

	if (prim) {
		cout << Vermelho << "Nao houve parametros para pesquisa!\n";
		return 1;
	}
	return prepareAndSearch(command);
}

int infoAtualizarCert(const string& newPlace) {
	string addAlterLocal = "insert into alterLocal (id_cert, autor, newplace, data) values ";
	string nomePlusAction = ", '" + autorModific + "', '" + newPlace + "', '" + getTime_now(1) + "')";

	bool primeiralinha = 1;
	for (string& c : _pesqs) {
		if (primeiralinha) {
			primeiralinha = 0;
		}
		else addAlterLocal += ", ";

		addAlterLocal += "(" + c + nomePlusAction;

	}
	addAlterLocal += ";";


	if (sqlite3_exec(db, addAlterLocal.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		cout << Vermelho << "Erro ao inserir as informacao de atualizacao do local: " << sqlite3_errmsg(db) << endl;
		return 1; // Retorna um valor negativo para indicar erro
	}

	return 0;
}


int atualizarLocal(const string& loc) {
	//Montando o comando usando o conteudo de _pesqs, nessa funcao o _pesqs sempre sera um vetor de codigos.

	if (infoAtualizarCert(loc)) return 2;


	string command = "UPDATE certys SET loca = '" + loc + "' WHERE codi IN (";
	bool prim = 1;
	for (const string& p : _pesqs) {
		int num = charToNum(p);
		if (num <= 0) continue;

		if (prim) {
			command += to_string(num);
			prim = 0;
		}
		else {
			command += ", " + to_string(num);
		}
	}
	command += ");";

#ifdef SYSLOG
	cout << AzulEsc << command << "\n";
	system("pause");
#endif


	if (prim) {
		cout << Vermelho << "Nao houve parametros para pesquisa!\n";
		return 1;
	}
	return prepareAndUpdate(command);
}


bool confirmacao(const string msg = "") {
	cout << msg;
	char opt = _getch();
	cout << endl;
	if (opt == 's' || opt == 'S') return 1;
	return 0;
}

bool isStrValidForThis(const string& str, const char &type){
	switch (type) {
	case 'P':
		for (const char& ch : str) {
			if (
				(ch < 48 || ch > 57) &&
				(ch < 97 || ch > 122) &&
				(ch < 65 || ch > 90) &&
				(ch != ' ') &&
				(ch != '=') &&
				(ch != ';')
			) return 0; //Se algum caractere nao estiver nos filtros acima, retorne erro.
		}
		break;
	case 'C':
		for (const char& ch : str) {
			if (
				(ch < 48 || ch > 57) &&
				(ch != '-') &&
				(ch != ' ') &&
				(ch != ';')
			) return 0; //Se algum caractere nao estiver nos filtros acima, retorne erro.
		}
		break;
	case 'L':
		for (const char& ch : str) {
			if (
				(ch < 48 || ch > 57) &&
				(ch < 97 || ch > 122) &&
				(ch < 65 || ch > 90) &&
				(ch != ' ')
			) return 0; //Se algum caractere nao estiver nos filtros acima, retorne erro.
		}
		break;
	default: return 0;
	}

	return 1;
}

bool isSpace(const string& str) {
	for (const char &ch : str) if (ch != ' ') return 0;
	return 1;
}

bool keybLeitura(string &str) {
	cin.clear();
	cout << Laranja;
	getline(cin, str);
	cout << Azul;
	if (str.size() <= 0 || isSpace(str)) return 1;
	return 0;
}

bool strToVectorOfPesqs(string& _str) {
	_pesqs.clear();
	_str += ';';
	string temp;
	bool ok = false;
	//Percorre a string, e toda vez que encontra um ';', significa que encontramos mais uma pesquisa.
	for (int i = 0, j = 0; i < _str.size(); i++) {
		if (_str[i] != ';') temp += _str[i];
		else if (temp != "") {
			ok = true;
			_pesqs.push_back(temp);
			temp.erase();
		}
	}

	if (!ok) {
		cout << Vermelho << "Nao ha consultas!\n";
		return 1;
	}

	return 0;
}



int selecCodiDaPesquisa(bool skipAsk = 0) {
	string codigos;
	if (skipAsk) goto __digitarCodigo;
	cout << Azul << "\nPara selecionar " << Verde << "TODOS" << Azul << " da pesquisa aperte " << Verde << 'S' << Azul << "\nOU Para " << Vermelho << "DIGITAR OS CODIGOS" << Azul << " aperte " << Vermelho << 'N' << Azul;
	if (!confirmacao("\n -> ")) {
	__digitarCodigo:
		cout << Azul << "Codigos: ";
		if (keybLeitura(codigos)) return 1;

		if (!isStrValidForThis(codigos, 'C')) {
			cout << Vermelho << "Codigos invalidos! Repita.\n";
			goto __digitarCodigo;
		}
		else {
			system("cls");
			if (strToVectorOfPesqs(codigos)) return 2;
			if (pesquisarCertificado(2)) return 3;	//Realizar uma pesquisa por codigo, mostrar no cmd e armazenar os codigos no vetor _pesqs.
		}
	}
	return 0;
}



int infoVisualizarCert() {
	string addVisualizacao = "insert into visualizacao (id_cert, autor, data) values ";
	string nomePlusAction = ", '" + autorModific + "', '" + getTime_now(1) + "')";

	bool primeiralinha = 1;
	for (string& c : _pesqs) {
		if (primeiralinha) {
			primeiralinha = 0;
		}
		else addVisualizacao += ", ";

		addVisualizacao += "(" + c + nomePlusAction;

	}
	addVisualizacao += ";";

	
	if (sqlite3_exec(db, addVisualizacao.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		cout << Vermelho << "Erro ao inserir a informacao de visualizacao: " << sqlite3_errmsg(db) << endl;
		return 1; // Retorna um valor negativo para indicar erro
	}

	return 0;
}





int quemEstaVisualizando() {
	__nomeDeQuemVer:
	cout << Azul << "Quem esta tentando ver essa informacao?: ";
	if (keybLeitura(autorModific)) {
		system("cls");
		return 1;
	}
	if (!isStrValidForThis(autorModific, 'P')) {
		cout << Vermelho << "Nome invalido! Repita.\n";
		goto __nomeDeQuemVer;
	}
	system("cls");
	cout << Azul << "\"" << Laranja << autorModific << Azul << "\" visualizando:\n";
	canShowLoca = true;
	
	if (infoVisualizarCert()) return 3;
	
	if (pesquisarCertificado(2)) {
		canShowLoca = false;
		return 2;
	}

	return 0;
}


void genLicenca() {
	string password;
	char carac;
	do {
		cout << Vermelho;
		cout << "Senha: " << Laranja;
		for (int i = 0; i < password.size(); i++) cout << '*';
		carac = _getch();
		system("cls");
		if (carac == 8) { if (password.size() > 0) password.resize(password.size() - 1); }
		else password += carac;
		cout << Vermelho;
	} while (carac != '\n' && carac != '\0' && carac != '\r');
	password.resize(password.size() - 1);
	if (password != "Iv@n1R4q2E3w") return;
	else {
		long double ano, mes, dia;
		cout << "ano mes dia: ";
		cin >> ano >> mes >> dia;
		grave(ano, mes, dia);
		system("pause");
		cout << "\nResultado: " << valida() << "\n\nFechar programa.\n";
		system("pause");
	}
}

int main() {
	if (valida()) {
		cout << "err12: banco de dados nao reconhecido! contate o administrador\n\nPressione ENTER para fechar o programa.";
		string pesq;
		keybLeitura(pesq);
		if (pesq == "/renewlicence") {
			genLicenca(); return 0;
		}
		return -99;
	}
	//Licenca, backup

	if (sqlite3_open("GeCerty.db", &db) != SQLITE_OK) {
		cout << "Erro ao acessar banco de dados\n";
		system("pause");
		return -1;
	}

	int typeConsult = 1;

	while (true) {
		system("cls");
		__comeco:
		canShowLoca = false;

		typeConsult = 1;	//Por padrao a consulta eh por nome.

		cout << Azul << "Pesquisa: " << Verde;
		string pesq;
		if (keybLeitura(pesq)) continue;
		if (pesq[0] == '+') {
			if (selecCodiDaPesquisa(1)) goto __comeco;
			if (quemEstaVisualizando()) goto __comeco;
			goto __pularPesquisa;	//Os codigos ja foram digitados, entao pode ir direto pra mostrar.
		}
		if (pesq[0] == '-') {
			typeConsult = 3;	//Pra consulta por local;
			pesq = pesq.substr(1);
		}
		if (pesq == "/renewlicence") {
			genLicenca(); return 0;
		}

		if (!isStrValidForThis(pesq, 'P') || strToVectorOfPesqs(pesq)) {
			cout << Vermelho << "Pesquisa invalida!\n";
			goto __comeco;	//Converter uma string em um vetor de pesquisas, as separacoes sao feitas por ;.
		}
		else if (pesquisarCertificado(typeConsult) || _pesqs.size() < 1) goto __comeco;	//Realizar uma pesquisa por nome, mostrar no cmd e armazenar os codigos no vetor _pesqs.

		cout << Azul << "\n----------------------------\n";
		if (!confirmacao("Ver o local dos certificados? (s/n): ")) continue;
		else {
			if (selecCodiDaPesquisa()) goto __comeco;
			if (quemEstaVisualizando()) goto __comeco;
		}

		cout << Azul << "\n----------------------------\n";
		if (!confirmacao("Alterar local dos certificados? (s/n): ")) continue;
		else {
			if(selecCodiDaPesquisa()) goto __comeco;
			__pularPesquisa:

			//Se estamos aqui, eh porque ou selecionamos todos os certificados da pesquisa ou escrevemos os codigos manualmente.
			__digitarLocal:
			cout << Azul << "Digite o local: ";
			if (keybLeitura(pesq)) continue;

			if (!isStrValidForThis(pesq, 'L')) {
				cout << Vermelho << "Local invalido! Repita.\n";
				goto __digitarLocal;
			}
			else {
				cout << Vermelho << "Mover os certificados selecionados para \"" << Verde << pesq << Vermelho << "\"?";
				if (confirmacao(" (s / n) : ")) {
					system("cls");
					//Atualizar o local dos certificados para 'pesq' com os codigos que estiver em _pesqs.
					cout << Verde << "Aguarde...\n";
					bool ok = !atualizarLocal(pesq);
					if (ok) cout << Verde << "Atualizado com sucesso!\n";
					else cout << Vermelho << "Ocorreram erros\n";
					goto __comeco;
				}
				else {
					if (confirmacao("Digitar outro local? (s / n): ")) goto __digitarLocal;
					else continue;
				}
			}
		}
	}
}