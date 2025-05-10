#include <iostream>
#include <ctime>
#include <fstream>
//#include <string>
#define LOCAL "sysdb.curtime"
#define mAno 13461641.134345
#define mMes 94929352.236644
#define mDia 59382005.059384
#define mSum 51993558.932340

using namespace std;

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

    long double sum = (ano - dia * mes) * mSum;

    saida.write(reinterpret_cast<const char*>(&sum), tamanho);
    saida.write(reinterpret_cast<const char*>(&ano), tamanho);
    saida.write(reinterpret_cast<const char*>(&mes), tamanho);
    saida.write(reinterpret_cast<const char*>(&dia), tamanho);

    saida.close();
    return 1;
}

bool valida() {
    ifstream data(LOCAL, ios_base::binary);
    if (!data.is_open()) {
        cout << "valida() -> sysdb.curtime nao encontrado\n";
        system("pause");
        return 0;
    }
    cout << "1\n";

    size_t tamanho = sizeof(long double);

    long double ano, mes, dia, sum;

    data.read(reinterpret_cast<char*>(&sum), tamanho);
    data.read(reinterpret_cast<char*>(&dia), tamanho);
    data.read(reinterpret_cast<char*>(&mes), tamanho);
    data.read(reinterpret_cast<char*>(&ano), tamanho);

    if (sum != (dia - ano * mes) * mSum) return 0;
    cout << "2\n";

    ano /= mDia;
    mes /= mMes;
    dia /= mAno;

    if (dataAtual('a') < dia) return 1;
    else if (dataAtual('a') == dia) {
        if (dataAtual('m') < mes) return 1;
        else if (dataAtual('m') == mes) {
            if (dataAtual('d') <= ano) return 1;
        }
    }

    cout << "3\n";
    return 0;
}
