#include <string>
#include <filesystem>
#include "G:/Outros computadores/3050/School/Codigos/CPP/Orteco/licenca.hpp"

//using namespace std;
namespace fs = std::filesystem; 

string defineName(string resp) {
    reverse(resp.begin(), resp.end());
    resp = resp.substr(0, resp.find("\\"));
    reverse(resp.begin(), resp.end());
    return resp;
}

int main() {
    //if (!valida()) { cout << "Arquivo de bando de dados corrompido!\n"; system("pause"); return -9; }

    ifstream diretorios("diretorios.txt");
    if (!diretorios.is_open()) {
        cout << "Impossivel abrir \"diretorios.txt\"\n";
        system("pause");
        return 1;
    }
    string dirSaida;
    getline(diretorios, dirSaida);
    string dirEntrada = dirSaida;
    getline(diretorios, dirSaida);
    diretorios.close();

    dirSaida += ((dirSaida[dirSaida.size() - 1] == '\\') ? "" : "\\");
    cout << "entrada: -" << dirEntrada << "-\nsaida: -" << dirSaida << "-\n";
    system("pause");

    if (!fs::exists(dirSaida) || !fs::exists(dirEntrada)) {
        cout << "Diretorios inexistentes!\n";
        system("pause");
        return 1;
    }
    if (dirEntrada == "" || dirSaida == "") {
        cout << "Local invalido!\n";
        system("pause");
        return 1;
    }
    else if (dirSaida.find(dirEntrada) != string::npos) {
        cout << "Saida nao pode estar na pasta de entrada!\n";
        system("pause");
        return 1;
    }
    //Os codigos acima servem para qualquer outro.

    string anoArquivo;
    string anoPretendido;
    cout << "Ano do arquivo e pretendido: ";
    cin >> anoArquivo >> anoPretendido;


    ofstream outputFile(dirSaida);
    if (!outputFile.is_open()) {
        cout << "Impossivel criar o arquivo de saida!";
        system("pause");
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(dirEntrada)) {
        if (fs::is_regular_file(entry)) {
            string caminhoArqv = entry.path().string();
            ifstream inputFile(caminhoArqv);

            if (!inputFile.is_open()) {
                cout << "Impossivel abrir o arquivo de entrada!\n";
                system("pause");
                return 1;
            }

            string strName = defineName(caminhoArqv);

            cout << strName << ":\n";
            string line;

            size_t dataEncontrada;

            while (getline(inputFile, line)) {
                size_t posi = 0;
                int qntsDatasNaMesmaLinha = -1;
                do {
                    qntsDatasNaMesmaLinha++;
                    dataEncontrada = line.find(anoArquivo + '|', posi);
                    posi = dataEncontrada + 1;
                } while (dataEncontrada != string::npos);


                if (qntsDatasNaMesmaLinha > 0) {
                    if (line[dataEncontrada - 5] == '|') {

                    }
                    else outputFile << line
                }
                else {
                    outputFile << line;
                }
            }


            inputFile.seekg(0);
            linhaAtual = 0;
            emitEncontrado = 0;
            destEncontrado = 0;
            cnpjEncontrado = 0;
            while (getline(inputFile, line)) {
                linhaAtual++;
                if (linhaAtual == posLinhaPrimeira) {
                    emitEncontrado = line.find("<CNPJ>");
                    if (emitEncontrado != string::npos) {
                        outputFile << line.substr(0, emitEncontrado + 6);
                        outputFile << linhaUltima;
                        if (posLinhaPrimeira != posLinhaUltima) outputFile << line.substr(emitEncontrado + 6 + 14) << endl;
                        else cnpjEncontrado = emitEncontrado + 6;
                    }
                }
                if (linhaAtual == posLinhaUltima) {
                    destEncontrado = line.find("<CNPJ>", cnpjEncontrado);
                    if (destEncontrado != string::npos) {
                        if (posLinhaPrimeira != posLinhaUltima) {
                            outputFile << line.substr(0, destEncontrado + 6);
                            outputFile << linhaPrimeira;
                            outputFile << line.substr(destEncontrado + 6 + 14) << endl;
                        }
                        else {
                            outputFile << line.substr(emitEncontrado + 6 + 14, destEncontrado - emitEncontrado - 14);
                            outputFile << linhaPrimeira;
                            outputFile << line.substr(destEncontrado + 6 + 14);
                        }
                    }
                }
                else if (linhaAtual != posLinhaPrimeira) outputFile << line << endl;
            }

            //std::cout << "1 cnpj: " << linhaPrimeira << "| " << posLinhaPrimeira << "\n2 cnpj: " << linhaUltima << "| " << posLinhaUltima << "\n\n";

            inputFile.close();
            outputFile.close();
        }
    }

    cout << "Arquivos gerados\n";
    system("pause");
}
