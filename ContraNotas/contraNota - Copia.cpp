#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main() {
    ofstream outputFile("G:/Outros computadores/3050/School/Codigos/CPP/Orteco/saida.xml");

    //for (const auto& entry : fs::directory_iterator("C:/Users/Usuario/Desktop/Novapasta")) {
    //    //if (fs::is_regular_file(entry)) {
    //    string caminhocorreto = entry.path().string();
    //        
    //
    //}



    ifstream inputFile("G:/Outros computadores/3050/School/Codigos/CPP/Orteco/entrada.xml");


    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Erro ao abrir arquivos." << std::endl;
        return 1;
    }

    string linhaPrimeira = "*";
    string linhaUltima = "*";
    string line;
    size_t emitEncontrado = string::npos;
    bool _emit = 0;
    size_t destEncontrado = string::npos;
    bool _dest = 0;
    size_t cnpjEncontrado = string::npos;

    unsigned int posLinhaPrimeira = 0;
    unsigned int posLinhaUltima = 0;
    unsigned int linhaAtual = 0;

    while (getline(inputFile, line)) {
        linhaAtual++;
        if (linhaPrimeira == "*" && !_emit) {
            emitEncontrado = line.find("<emit>");
            if (emitEncontrado != string::npos) _emit = 1;
        }

        if (_emit) {
            cnpjEncontrado = line.find("<CNPJ>", emitEncontrado);
            if (cnpjEncontrado != string::npos) {
                linhaPrimeira = line.substr(cnpjEncontrado + 6, 14);
                posLinhaPrimeira = linhaAtual;
                _emit = 0;
                cnpjEncontrado = string::npos;
            }
            emitEncontrado = 0;
        }

        if (linhaPrimeira != "*" && !_dest) {
            destEncontrado = line.find("<dest>");
            if (destEncontrado != string::npos) _dest = 1;
        }

        if (linhaUltima == "*" && _dest) {
            cnpjEncontrado = line.find("<CNPJ>", destEncontrado);
            if (cnpjEncontrado != string::npos) {
                linhaUltima = line.substr(cnpjEncontrado + 6, 14);
                posLinhaUltima = linhaAtual;
                break;
            }
            destEncontrado = 0;
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
        if (linhaAtual != posLinhaPrimeira && linhaAtual != posLinhaUltima) outputFile << line << endl;
    }

    cout << "\n" << linhaPrimeira << "\n" << linhaUltima << "\n" << posLinhaPrimeira << "\n" << posLinhaUltima << "\n\n\n\n";

    inputFile.close();
    outputFile.close();
}
