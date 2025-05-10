#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

string extrairConteudoEntre(const string& linha, const string& beforeCSTLinha, const string& afterCSTLinha) {
    size_t posInicio = linha.find(beforeCSTLinha);
    size_t posFim = linha.find(afterCSTLinha);
    if (posInicio != string::npos && posFim != string::npos) {
        posInicio += beforeCSTLinha.length();
        return linha.substr(posInicio, posFim - posInicio);
    }
    return "";
}

void removerDuplicatas(vector<int>& linhasCST) {
    // Ordena o vetor para que duplicatas estejam adjacentes
    sort(linhasCST.begin(), linhasCST.end());

    // Remove duplicatas consecutivas
    auto last = unique(linhasCST.begin(), linhasCST.end());

    // Redimensiona o vetor para remover o espaço "extra" deixado pelos duplicatas
    linhasCST.erase(last, linhasCST.end());
}

string modificarLinha(const string& linha, const string& modCST) {
    string novaLinha;
    int count = 0;
    size_t pos = 0, lastPos = 0;

    // Percorrer a linha e processar campo a campo, delimitado por '|'
    while ((pos = linha.find('|', lastPos)) != string::npos) {
        count++;

        // Copia o campo atual
        string campo = linha.substr(lastPos, pos - lastPos);

        // Se o campo for o 26º ou o 32º (conforme o exemplo), substituímos o valor por "modCST"
        if (count == 26 || count == 32) {
            campo = modCST;  // Substitui a palavra por "modCST"
        }

        // Constrói a nova linha com o campo (mantendo os delimitadores '|')
        novaLinha += campo + "|";

        lastPos = pos + 1;  // Avança para o próximo campo
    }

    // Adiciona o restante da linha, caso tenha conteúdo após o último '|'
    novaLinha += linha.substr(lastPos);

    return novaLinha;
}




int main() {
    // Arquivos de origem, destino e relatório
    string arqOrigem = "SPED.txt";
    string arqDestino = "resultados.txt";
    string arqRelatorio = "err.txt";

    string modCST = "73";

    string beforeCSTLinha = "Linha(s): ";
    string afterCSTLinha = " - CST d";

    vector<int> linhasCST;

    // Leitura do arquivo de relatório
    ifstream relatorio(arqRelatorio);
    string linha;

    std::cout << "Lendo relatorio.\n";
    while (getline(relatorio, linha)) {
        // Se encontrar a string "afterCSTLinha", extrai o número da linha
        if (linha.find(afterCSTLinha) != string::npos) {
            string conteudo = extrairConteudoEntre(linha, beforeCSTLinha, afterCSTLinha);
            if (!conteudo.empty()) {
                linhasCST.push_back(stoi(conteudo));  // Armazena o número da linha como inteiro
            }
        }
    }
    relatorio.close();

    // Ordena as linhas extraídas em ordem crescente
    sort(linhasCST.begin(), linhasCST.end());
    removerDuplicatas(linhasCST);

    for (int s : linhasCST){
        std::cout << s << "\n";
    }

    // Abre arquivos de origem e destino
    ifstream origem(arqOrigem);
    ofstream destino(arqDestino);

    // Copia o conteúdo, modificando as linhas mencionadas no relatório
    int numeroLinhaAtual = 1;
    int indiceCST = 0;

    std::cout << "\n\nCopiando.\n";
    int counter = 0;
    while (getline(origem, linha)) {
        if (indiceCST < linhasCST.size() && numeroLinhaAtual == linhasCST[indiceCST]) {
            // Modifica a linha correspondente
            std::cout << "FOUND at " << numeroLinhaAtual << "\n";
            destino << modificarLinha(linha, modCST) << endl;
            counter++;
            indiceCST++;
        } else {
            // Copia a linha sem modificar
            destino << linha << endl;
        }
        numeroLinhaAtual++;
    }

    std::cout << counter << " de " << linhasCST.size() << "\n";

    origem.close();
    destino.close();

    cout << "Processo concluído com sucesso!" << endl;
    return 0;
}
