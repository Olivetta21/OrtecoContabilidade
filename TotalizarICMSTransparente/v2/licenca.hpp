#include <iostream>
#include <ctime>
#include <fstream>
#include <conio.h>
//#include <string>
//#define LOCAL "sysdb.curtime"
#define mAno 13461641.134345
#define mMes 94929352.236644
#define mDia 59382005.059384
#define mSum 51993558.932340
//#define GERARLICENCA

using namespace std;

string LOCAL = "sysdb.curtime";


int dataAtual(const char modoData = 'n') {
    time_t rawtime;
    struct tm timeinfo;

    // Obter a hora atual
    time(&rawtime);

    // Converter a hora para uma estrutura tm usando localtime_s
    if (localtime_s(&timeinfo, &rawtime) != 0) {
        cout << "Erro ao obter a data e hora.\n";
        system("pause");
        return 0;
    }

    switch (modoData) {
    case 'a': return timeinfo.tm_year + 1900;
    case 'm': return timeinfo.tm_mon + 1;
    case 'd': return timeinfo.tm_mday;
    default: return 0;
    }
}

bool grave(long double ano, long double mes, long double dia) {
    size_t tamanho = sizeof(long double);

    cout << "Gravando Data atual: " << ano << "|"
        << mes << "|" << dia << "\n" << LOCAL << "\n";

    ano *= mAno;
    mes *= mMes;
    dia *= mDia;

    ofstream saida(LOCAL, ios_base::binary);
    if (!saida.is_open()) {
        cout << "grave() -> sysdb.curtime nao encontrado\n";
        system("pause");
        return 0;
    }

    long double sum = (dia - ano * mes) * mSum;

#ifdef GERARLICENCA
    cout << "\n------------GRAVE---------\n";
    cout << "sum: " << sum << endl;
    cout << "ano: " << ano << endl;
    cout << "mes: " << mes << endl;
    cout << "dia: " << dia << endl;
    cout << "Verificacao: " << (dia - ano * mes) * mSum << endl;
    cout << "\n------------FGRAVE---------------\n";
#endif // GERARLICENCA



    saida.write(reinterpret_cast<const char*>(&sum), tamanho);
    saida.write(reinterpret_cast<const char*>(&ano), tamanho);
    saida.write(reinterpret_cast<const char*>(&mes), tamanho);
    saida.write(reinterpret_cast<const char*>(&dia), tamanho);

    saida.close();
    return 1;
}

int valida(string otherDic = LOCAL) {
    LOCAL = otherDic;
    ifstream data(LOCAL, ios_base::binary);
    if (!data.is_open()) {
        cout << "valida() -> sysdb.curtime nao encontrado\n";
        system("pause");
        return 1;
    }

    size_t tamanho = sizeof(long double);

    long double ano, mes, dia, sum;

    data.read(reinterpret_cast<char*>(&sum), tamanho);
    data.read(reinterpret_cast<char*>(&ano), tamanho);
    data.read(reinterpret_cast<char*>(&mes), tamanho);
    data.read(reinterpret_cast<char*>(&dia), tamanho);

#ifdef GERARLICENCA
    cout << "\n-------------VALIDA------------\n";
    cout << "sum: " << sum << endl;
    cout << "ano: " << ano << endl;
    cout << "mes: " << mes << endl;
    cout << "dia: " << dia << endl;
    cout << "Verificacao: " << (dia - ano * mes) * mSum << endl;
    cout << "\n------------FVALIDA--------------\n";
#endif





    if (sum != (dia - ano * mes) * mSum) return 2;

    ano /= mAno;
    mes /= mMes;
    dia /= mDia;

    int _ano = stoi(to_string(ano)), _mes = stoi(to_string(mes)), _dia = stoi(to_string(dia));

#ifdef GERARLICENCA
    cout << "\n-------------Apos------------\n";
    cout << "ano: " << _ano << endl;
    cout << "mes: " << _mes << endl;
    cout << "dia: " << _dia << endl;
    cout << "\n------------fApos--------------\n";

    cout << "\n-------------data------------\n";
    cout << "ano: " << dataAtual('a') << endl;
    cout << "mes: " << dataAtual('m') << endl;
    cout << "dia: " << dataAtual('d') << endl;
    cout << "\n------------fdata--------------\n";
#endif // GERARLICENCA

    if (dataAtual('a') < _ano) return 0;
    else if (dataAtual('a') == _ano) {
        if (dataAtual('m') < _mes) return 0;
        else if (dataAtual('m') == _mes) {
            if (dataAtual('d') <= _dia) return 0;
        }
    }

    return 3;
}


void genLicenca() {
	string password;
	char carac;
	do {
		cout << "Senha: ";
		for (int i = 0; i < password.size(); i++) cout << '*';
		carac = _getch();
		system("cls");
		if (carac == 8) { if (password.size() > 0) password.resize(password.size() - 1); }
		else password += carac;
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
