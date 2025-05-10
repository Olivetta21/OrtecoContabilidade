#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>

using namespace std;

void defineColumn(string& linha, const string& linhaArq, const int& inicio, const int& tam){
    for (int i = 0; i < tam; i++) linha[i] = linhaArq[i+inicio];
}

void reset(string& sttr, const int& tam){
    sttr = "\0\0\0\0\0\0\0\0\0";
    sttr.erase();
    sttr.clear();
    sttr.resize(tam);
}

int main(){
    int qnts = 0;
    ifstream file("sped.txt");
    ofstream outFile("saidaSped.txt");

    string tipoC = "|C190|";
    string cst = "|000|";
    string cfop = "|5101|";
    string vals = "|0,00|0,00|";

    string linha;

    while(getline(file, linha)){
        string temp;
        reset(temp, 6);
        defineColumn(temp, linha, 0, 6);
        if (temp == tipoC){
            reset(temp, 5);
            defineColumn(temp, linha, 5, 5);
            if (temp == cst){
                reset(temp, 6);
                defineColumn(temp, linha, 9, 6);
                if (temp == cfop){
                    reset(temp, 11);
                    defineColumn(temp, linha, 14, 11);
                    if (temp == vals){
                        qnts++;
                        reset(temp, 0);
                        temp = linha.substr(0, 25);
                        temp += "0,00";
                        temp += linha.substr(linha.find("|", 25));
                        cout << temp <<"\n";
                        linha = temp;
                    }
                }
            }
        }
        outFile << linha << "\n";
    }
    cout << "\n\nTotal: " << qnts << "\n\n";
}
