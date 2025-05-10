#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct Produto {
    std::string nome;
    std::string valor;
};

struct NotaFiscal {
    std::string numero;
    std::vector<Produto> produtos;
};

int main() {
    std::ifstream arquivo("sped_fiscal.txt");
    std::string linha;
    std::vector<NotaFiscal> notasFiscais;

    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
        return 1;
    }

    NotaFiscal notaAtual;
    bool lendoProdutos = false;

    std::cout << "\nIniciando a leitura linha a linha...\n";
    while (std::getline(arquivo, linha)) {
        std::stringstream ss(linha);
        std::string item;
        std::vector<std::string> colunas;

        if (linha.size() < 1) continue;
        else item = "";

        bool firstGoes = false;
        while (std::getline(ss, item, '|')) {
            if (firstGoes) colunas.push_back(item);
            else firstGoes = true;

        }

        if (colunas[0] == "C100") {
            if (lendoProdutos) {
                notasFiscais.push_back(notaAtual);
                notaAtual = NotaFiscal();
            }
            notaAtual.numero = colunas[7];
            lendoProdutos = true;
        } else if (colunas[0] == "C170" && lendoProdutos) {
            Produto produto;
            produto.nome = colunas[3];
            produto.valor = colunas[6];
            notaAtual.produtos.push_back(produto);
        }
    }

    if (lendoProdutos) {
        notasFiscais.push_back(notaAtual);
    }

    arquivo.close();

    std::cout << "\n\n\nResultado: \n\n";
    std::ofstream arqsaida("resultado.csv");
    for (const auto& nota : notasFiscais) {
        //std::cout << "Nota Fiscal: " << nota.numero << std::endl;

        for (const auto& produto : nota.produtos) {
            //std::cout << "\tProduto: " << produto.nome << "\n\t\tValor: " << produto.valor << std::endl;
            arqsaida << nota.numero << ";\"" << produto.nome << "\";" << produto.valor << "\n";
        }
    }

    return 0;
}
