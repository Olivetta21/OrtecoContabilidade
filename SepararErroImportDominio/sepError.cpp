#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <windows.h>

using namespace std;

#define TAMLINHA 1024

FILE* file = NULL;
string actLin;
char lin[TAMLINHA] = {0};
string arquivo;
int exec = 0;

int tryOpen(){
    static int times = 0;
    file = fopen(arquivo.c_str(), "r");

    if (file == NULL) cout << "procurando o arquivo... " << ++times << "\n-------------------------------";
    else{
     cout << "Arquivo encontrado!\n-------------------------------";
     times = 0;
     return 1;
    }

    Sleep(20);
    system("cls");
    return 0;
}



int main(int argc, char* argv[]){
    if (argc < 2) return 1;
    arquivo = argv[1];
    while (1){
        int n;

        vector<string> cfop_fAcum;
        vector<string> cfop_fContrap;

        while (!tryOpen()); //Ficar aqui ate abrir.
        cout << ++exec << "\n";

        while(fgets(lin, TAMLINHA, file)){    //O fgets retorna falso se ele chegar ao fim do arquivo.
            if (!ferror(file)){
                actLin = lin;

                int posEncontrado = actLin.find                               (" de acumulador para CFOP '");
                if (posEncontrado == string::npos)  posEncontrado = actLin.find("e acumulador para o CFOP '");
                if (posEncontrado != string::npos) cfop_fAcum.push_back(actLin.substr(posEncontrado + 26, 4));
                else {
                    posEncontrado = actLin.find("ada para o CFOP de sa");
                    if (posEncontrado == string::npos) posEncontrado = actLin.find("a para o CFOP de entr");
                    if (posEncontrado != string::npos) cfop_fContrap.push_back(actLin.substr(posEncontrado + 26, 4));
                    else continue;
                }
            }

        }


        vector<string> cfop2;
        for (string str : cfop_fAcum){
            bool n = false;
             for (string str2 : cfop2){
                 if (str == str2) n = true;
             }
            if (!n) cfop2.push_back(str);
        }


        if (cfop2.size() > 0) cout << "\n\nFalta configurar o acumulador para esses CFOPs:\n";
        for (string str2 : cfop2) cout << str2 <<endl;


        vector<string> cfop3;
        for (string str : cfop_fContrap){
            bool n = false;
             for (string str2 : cfop3){
                 if (str == str2) n = true;
             }
            if (!n) cfop3.push_back(str);
        }


        if (cfop3.size() > 0) cout << "\n\nEsta faltando a contrapartida desses CFOPs:\n";
        for (string str2 : cfop3) cout << str2 <<endl;

        if (file != NULL) fclose(file);
        cout << "\n\n";
        system("pause");
        system("cls");
    }
}
