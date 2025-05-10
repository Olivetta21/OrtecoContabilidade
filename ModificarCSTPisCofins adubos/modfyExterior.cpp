#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

int counter = 0;

// Fun��o para modificar a linha, substituindo campos 25 e 31 se forem "INV"
string verificarEModificarLinha(const string& linha, const string& modCST) {
    string novaLinha;
    int count = 0;
    size_t pos = 0, lastPos = 1;
    string primeiroCampo;

    // Percorre a linha e processa campo a campo, delimitado por '|'
    while ((pos = linha.find('|', lastPos)) != string::npos) {
        count++;

        // Extrai o campo atual
        string campo = linha.substr(lastPos, pos - lastPos);

        // Armazena o primeiro campo para verificar se � "C170"
        if (count == 1) {
            primeiroCampo = campo;
        }

        // Se o primeiro campo for "C170", verificamos os campos 25 e 31
        if (primeiroCampo == "C100") {
            if ((count == 3) && (campo == "1")) {
                counter++;
                std::cout << "\nP: " << count << " - " << campo;
                campo = modCST;  // Substitui o campo 25 se for "INV"
            }
            else std::cout << "x";
        }
        else std::cout << ".";

        // Constr�i a nova linha com o campo (mantendo os delimitadores '|')
        novaLinha += campo + "|";

        lastPos = pos + 1;  // Avan�a para o pr�ximo campo
    }

    // Adiciona o restante da linha, caso tenha conte�do ap�s o �ltimo '|'
    novaLinha += linha.substr(lastPos);

    return "|" + novaLinha;
}

int main() {
    string arqOrigem = "origem.txt";
    string arqDestino = "destino.txt";
    string modCST = "0";

    ifstream origem(arqOrigem);
    ofstream destino(arqDestino);

    if (!origem.is_open() || !destino.is_open()) {
        cerr << "Erro ao abrir os arquivos!" << endl;
        return 1;
    }

    string linha;
    while (getline(origem, linha)) {
        // Verifica e modifica a linha, se necess�rio
        string linhaModificada = verificarEModificarLinha(linha, modCST);

        // Escreve a linha (modificada ou n�o) no arquivo de destino
        destino << linhaModificada << endl;
    }

    origem.close();
    destino.close();

    cout << counter << " Processo conclu�do com sucesso!" << endl;
    system("pause");
    return 0;
}
