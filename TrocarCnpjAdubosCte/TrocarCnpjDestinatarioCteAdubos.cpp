#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <windows.h>

using namespace std;

#define TAMLINHA 262144

FILE* file = NULL;
FILE* novo = NULL;
string actLin;
char lin[TAMLINHA] = {0};
int exec = 0;

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
    novo = fopen((arquivo + ".xml").c_str(), "w");

    if (novo == NULL) cout << "\nao foi possivel criar o arquivo '" << arquivo << "'\n";
    else return 1;

    return 0;
}

int main(int argc, char* argv[]){
    if (argc < 2) return 1;
    string leftlinha;
    string rightlinha;
    string cnpj;
    int numerolinha= 0;



    vector<string> arquivos;
    for (int i = 1; i < argc; i++)
        arquivos.push_back(argv[i]);

    for (const string& arc : arquivos) {
        while (!tryOpen(arc)); //Ficar aqui ate abrir.
        cout << "\n\n";

        numerolinha = 0;
        while(fgets(lin, TAMLINHA, file)){    //O fgets retorna falso se ele chegar ao fim do arquivo.
            if (!ferror(file)){
                actLin = lin;
                numerolinha++;
                int posEncontrado = actLin.find("<dest><CNPJ>");
                if (posEncontrado == string::npos){
                    cout << "\"<dest><CNPJ>\" de \"" << arc << " nao encontrado!\n";
                    continue;
                }
                else {
                    leftlinha = actLin.substr(0, posEncontrado+12);
                    rightlinha = actLin.substr(posEncontrado+26);
                    cnpj = actLin.substr(posEncontrado+12, 14);

                    //cout << "\n\n\n linhaesquerda:\n" << leftlinha << "\n\n\n";
                    //cout << "linhadireita:\n" << rightlinha << "\n\n\n";
                    //cout << "cnpj:\n" << cnpj << "\n\n\n";

                    if(createNew(arc+"mod")){
                        string mesclado = leftlinha + "09122329000361" + rightlinha;
                        fputs(mesclado.c_str(), novo);
                    }

                }
            }

        }
    }

    system("pause");
}
