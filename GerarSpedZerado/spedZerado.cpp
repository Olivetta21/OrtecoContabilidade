#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <windows.h>

#include <random>

using namespace std;




#define TAMLINHA 1024

FILE* file = NULL;
FILE* novo = NULL;






int aleat() {
    // Obtendo a hora atual
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);

    // Calculando os segundos desde 1900
    FILETIME fileTime;
    SystemTimeToFileTime(&currentTime, &fileTime);
    ULARGE_INTEGER uli;
    uli.LowPart = fileTime.dwLowDateTime;
    uli.HighPart = fileTime.dwHighDateTime;

    const ULONGLONG epochDiff = 116444736000000000ULL;
    ULONGLONG secondsSince1900 = (uli.QuadPart - epochDiff) / 10000000ULL;

    return secondsSince1900;
}


mt19937 gen(aleat());
uniform_int_distribution<int> distribution(0, 1000000);


int tryOpen(const string& arquivo){
    if (file != NULL){
        fclose(file);
        file = NULL;
    }
    file = fopen(arquivo.c_str(), "r");

    if (file == NULL) cout << "\narquivo '" << arquivo << "' nao encontrado\n";
    else return 1;

    return 0;
}

int createNew(const string& arquivo) {
    if (novo != NULL) {
        fclose(novo);
        novo = NULL;
    }
    novo = fopen((arquivo + "_SEMMOVIMENTO.sped").c_str(), "w");

    if (novo == NULL) cout << "\nao foi possivel criar o arquivo '" << arquivo << "'\n";
    else return 1;

    return 0;
}

vector<string> datas;

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;

    int qntsGravados = 0;

    string datainicio;
    string datafim;
    cout << "Data inicial (ddmmaaaa nao use separadores!): ";
    cin >> datainicio;
    cout << "Data final (ddmmaaaa nao use separadores!): ";
    cin >> datafim;

    char inv = 'n';
    cout << "Inventario? (s/n): ";
    cin >> inv;

    if (inv == 's' || inv == 'S'){
        string fimInvent;
        cout << "Informe o final do periodo (ddmmaaaa nao use separadores!: ";
        cin >> fimInvent;

        datas = {
            "|0000|018|0|" + datainicio + "|" + datafim,
            "|0100|JAFE CANDIDO DA CUNHA|31323472134|1-MS-004055/O-0||74000000|RUA JOAO ROSA GOES|167||CENTRO|6734164600||jafe@terra.com.br|5003702|",
            "|0990|5|",
            "|B001|1|",
            "|B990|2|",
            "|C001|1|",
            "|C990|2|",
            "|D001|1|",
            "|D990|2|",
            "|E001|0|",
            "|E100|" + datainicio + "|" + datafim + "|",
            "|E110|0|0|0|0|0|0|0|0|0|0|0|0|0|0|",
            "|E300|MS|" + datainicio + "|" + datafim + "|",
            "|E310|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|",
            "|E990|6|",
            "|G001|1|",
            "|G990|2|",
            "|H001|0|",
            "|H005|" + fimInvent + "|0|01|",
            "|H990|3|",
            "|K001|1|",
            "|K990|2|",
            "|1001|0|",
            "|1010|N|N|N|N|N|N|N|N|N|N|N|N|N|",
            "|1990|3|",
            "|9001|0|",
            "|9900|0000|1|",
            "|9900|0001|1|",
            "|9900|0005|1|",
            "|9900|0100|1|",
            "|9900|0990|1|",
            "|9900|1001|1|",
            "|9900|1010|1|",
            "|9900|1990|1|",
            "|9900|9001|1|",
            "|9900|9990|1|",
            "|9900|9999|1|",
            "|9900|B001|1|",
            "|9900|B990|1|",
            "|9900|C001|1|",
            "|9900|C990|1|",
            "|9900|D001|1|",
            "|9900|D990|1|",
            "|9900|E001|1|",
            "|9900|E100|1|",
            "|9900|E110|1|",
            "|9900|E300|1|",
            "|9900|E310|1|",
            "|9900|E990|1|",
            "|9900|G001|1|",
            "|9900|G990|1|",
            "|9900|H001|1|",
            "|9900|H005|1|",
            "|9900|H990|1|",
            "|9900|K001|1|",
            "|9900|K990|1|",
            "|9900|9900|31|",
            "|9990|34|",
            "|9999|61|"
        };

    }
    else {
            cout << "\nSem Inventario:\n";
        datas = {
            "|0000|018|0|" + datainicio + "|" + datafim,
            "|0100|JAFE CANDIDO DA CUNHA|31323472134|1-MS-004055/O-0||74000000|RUA JOAO ROSA GOES|167||CENTRO|6734164600||jafe@terra.com.br|5003702|",
            "|0990|5|",
            "|B001|1|",
            "|B990|2|",
            "|C001|1|",
            "|C990|2|",
            "|D001|1|",
            "|D990|2|",
            "|E001|0|",
            "|E100|" + datainicio + "|" + datafim + "|",
            "|E110|0|0|0|0|0|0|0|0|0|0|0|0|0|0|",
            "|E300|MS|" + datainicio + "|" + datafim + "|",
            "|E310|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|",
            "|E990|6|",
            "|G001|1|",
            "|G990|2|",
            "|H001|1|",
            "|H990|2|",
            "|K001|1|",
            "|K990|2|",
            "|1001|0|",
            "|1010|N|N|N|N|N|N|N|N|N|N|N|N|N|",
            "|1990|3|",
            "|9001|0|",
            "|9900|0000|1|",
            "|9900|0001|1|",
            "|9900|0005|1|",
            "|9900|0100|1|",
            "|9900|0990|1|",
            "|9900|1001|1|",
            "|9900|1010|1|",
            "|9900|1990|1|",
            "|9900|9001|1|",
            "|9900|9990|1|",
            "|9900|9999|1|",
            "|9900|B001|1|",
            "|9900|B990|1|",
            "|9900|C001|1|",
            "|9900|C990|1|",
            "|9900|D001|1|",
            "|9900|D990|1|",
            "|9900|E001|1|",
            "|9900|E100|1|",
            "|9900|E110|1|",
            "|9900|E300|1|",
            "|9900|E310|1|",
            "|9900|E990|1|",
            "|9900|G001|1|",
            "|9900|G990|1|",
            "|9900|H001|1|",
            "|9900|H990|1|",
            "|9900|K001|1|",
            "|9900|K990|1|",
            "|9900|9900|30|",
            "|9990|33|",
            "|9999|59|"
        };
    }



    vector<string> arquivos;

    for (int i = 1; i < argc; i++)
        arquivos.push_back(argv[i]);


    for (const string& arc : arquivos) {
        if (tryOpen(arc)) {
            string s1;
            string s2;
            string s3;

            char lin[TAMLINHA] = { 0 };
            int numLinha = 0;
            while (fgets(lin, TAMLINHA, file) && numLinha++ < 3) {    //O fgets retorna falso se ele chegar ao fim do arquivo.
                if (!ferror(file)) {
                    string actLin = lin;

                    switch (numLinha) {
                    case 1: {
                        int qntFound = 0;
                        int i = -1;
                        while (++i < actLin.size() && qntFound < 6) {
                            if (actLin[i] == '|') qntFound++;
                        }
                        s1 = actLin.substr(--i, actLin.size() - i);
                        break;
                    }
                    case 2: {
                        s2 = actLin;
                        break;
                    }
                    case 3: {
                        s3 = actLin;
                        break;
                    }
                    }
                }
            }


            string teste = to_string(distribution(gen));

            if (createNew(arc + teste)){
                fputs(datas[0].c_str(), novo); fputs((s1 + '\n').c_str(), novo);
                fputs((s2).c_str(), novo);
                fputs((s3).c_str(), novo);
                for (int i = 1; i < datas.size(); i++) {
                    fputs((datas[i] + '\n').c_str(), novo);
                }
                qntsGravados++;
            }
        }
    }


    cout << "\n\n" << qntsGravados << " arquivos gravados!\n\n";
    system("pause");
}
