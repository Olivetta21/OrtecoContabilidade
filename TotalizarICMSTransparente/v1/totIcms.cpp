#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>


long double stringToLongDouble(const std::string& input) {
    std::string filtered;
    bool decimalFound = false;

    for (char c : input) {
        if (std::isdigit(c)) {
            filtered += c;
        } else if (c == ',' && !decimalFound) {
            filtered += '.';
            decimalFound = true;
        }
    }

    std::istringstream iss(filtered);
    long double result;
    iss >> result;

    return result;
}


long double stringToNumber(const std::string& str) {
    std::string temp = str;

    long double a = std::stod(temp);
    std::cout << "1valor: " << str << " -- ";

    // Remove pontos (.)
    temp.erase(std::remove(temp.begin(), temp.end(), '.'), temp.end());

    a = std::stod(temp);
    std::cout << "2valor: " << temp << " -- ";

    // Substituir vírgula (,) por ponto (.)
    std::replace(temp.begin(), temp.end(), ',', '.');

    a = std::stod(temp);
    std::cout << "3valor: " << temp << "\n";


    long double res = std::stod(temp);
    //std::cout << "\nOriginal: " <<str << " - Modificado: " << temp << " - Res: " << res;
    // Converter para double
    return res;
}


struct NotaFiscal {
    std::string  chave
                ,ie
                ,razDest
                ,cnpjEmit
                ,nf
                ,emiss
                ,UFDest
                ,total
                ,base
                ,icms
                ,sit;
    NotaFiscal(std::vector<std::string> cols){
            chave    = cols[0],
            ie       = cols[1],
            razDest  = cols[2],
            cnpjEmit = cols[3],
            nf       = cols[4],
            emiss    = cols[5],
            UFDest   = cols[6],
            total    = cols[7],
            base     = cols[8],
            icms     = cols[9],
            sit      = cols[10];
    }
};

int main() {
    std::ifstream arquivo("notas.txt");
    std::string linha;
    std::vector<NotaFiscal> notasFiscais;

    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
        return 1;
    }


    std::cout << "\nIniciando a leitura linha a linha...\n";
    while (std::getline(arquivo, linha)) {
        std::stringstream ss(linha);
        std::string item;
        std::vector<std::string> colunas;

        if (linha.size() < 100) continue;

        std::getline(ss, item, '\t');
        if (item.size() == 44){
            colunas.push_back(item);
            //std::cout << "Chave: " << item << "\n";
            while (std::getline(ss, item, '\t')) {
                colunas.push_back(item);
                //std::cout << item << " - ";
            }
        }

        if (colunas.size() == 11 && colunas[0].size() == 44){
            notasFiscais.push_back(colunas);
        }
    }


    arquivo.close();




    std::cout << "\n\n\nResultado: \n";

    std::ofstream arqsaida("resultado.csv");
    arqsaida << "\"Chave de Acesso\";\"I.E. Dest\";\"Razão Social Dest\";\"CNPJ/CPF Emit\";\"N NF\";\"Dt. Emissao Fuso MS\";\"UF Dest.\";\"Total NF\";\"Base Calc. ICMS\";\"Valor ICMS\";\"Sit.\"\n";

    long double tt = 0.0f;
    long double totalNotas = 0.01f;
    long double res = 0.01f;
    for (const auto& nota : notasFiscais) {
        arqsaida << "\"." << nota.chave << "\";\"." << nota.ie << "\";\"" << nota.razDest << "\";\"." << nota.cnpjEmit << "\";" << nota.nf << ";" << nota.emiss << ";\"" << nota.UFDest << "\";" << nota.total << ";" << nota.base << ";" << nota.icms << ";\"" << nota.sit << "\"\n";
        res = stringToLongDouble(nota.total);
        tt += res;
        std::cout << "\t\tSoma: " << totalNotas << " + " << res << " = " << tt << "\n";
    }


    std::cout << "\nTotal NOTAS: " << totalNotas;
    std::string str;

    std::cout << "\nDigite um numero: ";
    std::cin >> str;
    std::cout << "res: " << stringToLongDouble(str);

    return 0;
}
