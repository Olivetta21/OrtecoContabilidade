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
    novo = fopen((arquivo + ".txt").c_str(), "w");

    if (novo == NULL) cout << "\nao foi possivel criar o arquivo '" << arquivo << "'\n";
    else return 1;

    return 0;
}

int main(int argc, char* argv[]){
    if (argc < 2) return 1;
    string leftlinha;
    string rightlinha;
    string cnpj;

    vector<string> localidades;

    vector<string> arquivos;
    for (int i = 1; i < argc; i++)
        arquivos.push_back(argv[i]);

    for (const string& arc : arquivos) {
        while (!tryOpen(arc)); //Ficar aqui ate abrir.
        cout << "\n\n";

        bool cnpjfound = 0, razaofound = 0, ruafound = 0, munfound = 0, uffound = 0;

        string cnpjEmit = "", razaoEmit = "", rua= "", mun = "", UF = "";


        int exec = 0;

        while(fgets(lin, TAMLINHA, file)){    //O fgets retorna falso se ele chegar ao fim do arquivo.
            if (!ferror(file)){
                actLin = lin;

                int posCnpjEmit = 0, posInRazaoEmit = 0, posOutRazaoEmit = 0, posInRua = 0, posOutRua = 0, posInMun = 0, posOutMun = 0, posUf = 0;

                if (!cnpjfound){
                    posCnpjEmit = actLin.find("<emit><CNPJ>");
                    if (posCnpjEmit == string::npos){
						cout << arc << "       -       cnpjin" << endl;
						continue;
					}
					posCnpjEmit += 12;
                    cnpjEmit = actLin.substr(posCnpjEmit, 14);
                    cout << "CNPJ: " << posCnpjEmit << endl;
                    cnpjfound = true;
                }

                if (!razaofound){
                    posInRazaoEmit = actLin.find("<xFant>", posCnpjEmit);
                    if (posInRazaoEmit == string::npos){
						cout << arc << "       -      fantin " << endl;
						continue;
					}
					posInRazaoEmit +=7;

                    posOutRazaoEmit = actLin.find("</xFant>", posInRazaoEmit);
                    if (posOutRazaoEmit == string::npos){
						cout << arc << "       -       fantout" << endl;
						continue;
					}
                    razaoEmit = actLin.substr(posInRazaoEmit, posOutRazaoEmit - posInRazaoEmit);
                    cout << "RAZIN: " << posInRazaoEmit << "-RAZOUT: " << posOutRazaoEmit << endl;
                    razaofound = true;
                }

                if (!ruafound){
                    posInRua = actLin.find("<xLgr>");
                    if (posInRua == string::npos){
						cout << arc << "       -       lgrin" << endl;
						continue;
					}
					posInRua += 6;

                    posOutRua = actLin.find("</xLgr>", posInRua);
                    if (posOutRua == string::npos){
						cout << arc << "       -       lgrout" << endl;
						continue;
					}
                    rua = actLin.substr(posInRua, posOutRua - posInRua);
                    cout << "RUAIN: " << posInRua << " RUAOUT: " << posOutRua << endl;
                    ruafound = true;
                }

                if (!munfound){
                    posInMun = actLin.find("<xMun>");
                    if (posInMun == string::npos){
						cout << arc << "       -       munin" << endl;
						continue;
					}
					posInMun += 6;

                    posOutMun = actLin.find("</xMun>", posInMun);
                    if (posOutMun == string::npos){
						cout << arc << "       -       munout" << endl;
						continue;
					}
                    mun = actLin.substr(posInMun, posOutMun - posInMun);
                    cout << "MUNIN: " << posInMun << " MUNOUT: " << posOutMun << endl;
                    munfound = true;
                }

                if (!uffound){
                    posUf = actLin.find("<UF>", posOutRazaoEmit);
                    if (posUf == string::npos){
						cout << arc << "       -       ufin" << endl;
						continue;
					}
					posUf += 4;

                    UF = actLin.substr(posUf, 2);
                    cout << "UFIN: " << posUf << endl;
                    uffound = true;
                }

            }
        }
        string mesclado = cnpjEmit + string(";") + razaoEmit + string(";") + UF + string(";") + mun + string(";") + rua + string("\n");

        //cout << mesclado;
        localidades.push_back(mesclado);
    }

    if(createNew("locais")){
        for (string & l : localidades){
            fputs(l.c_str(), novo);
        }
    }

    system("pause");
}
