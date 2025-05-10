#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>


namespace fs = std::filesystem;

int main(){
    //LENDO O LOCAL
    std::string essaVersao;
    std::string atualizacao;
    std::ifstream ilocal("local.txt");
    std::string ilinha;
    std::vector<std::string> locais;
    if (!ilocal.is_open()) {
        std::cerr << "Erro ao abrir o arquivo local.txt" << std::endl;
        system("pause");
        return 1;
    }
    while (std::getline(ilocal, ilinha)) {
        locais.push_back(ilinha);
    }
    ilocal.close();
    if (locais.size() > 1){
        atualizacao = locais[0] + "versao.txt";
        essaVersao = locais[1];
    }
    else {
        std::cout << "\nsem linhas em local.txt\n";
        system("pause");
        return 1;
    }
    ////


    //LENDO A VERSAO
    std::ifstream versao(atualizacao);
    if (!versao.is_open()) {
        std::cerr << "Erro ao abrir o arquivo atualizacao.txt" << std::endl;
        system("pause");
        return 1;
    }
    std::vector<std::string> atts;
    std::string attsLinha;
    std::string numVer;
    while (std::getline(versao, attsLinha)) {
        atts.push_back(attsLinha);
    }
    if (atts.size() > 0){
        numVer = atts[atts.size() - 1];
        atts.pop_back();
    }
    else {
        std::cout << "\nsem linhas em atualizacao.txt\n";
        system("pause");
        return 1;
    }
    ////


    //COPIANDO A ATUALIZACAO
    if (essaVersao != numVer){
        std::string aqui = fs::current_path().string();

        for (const std::string& a : atts) {
            try {
                fs::path arq = a;
                fs::path dest = aqui + "\\" + arq.filename().string();

                if (fs::exists(dest)) {
                    fs::remove(dest);
                }

                fs::copy(arq, dest);
            } catch (const fs::filesystem_error& e) {
                std::cout << "erro - " << e.what();
            }
            std::cout << ".\n";
        }
    }
    ////


    //ABRINDO PROGRAMA
    int result = std::system("a.exe");

    if (result == 0) {
    } else {
        std::cerr << "Falha ao executar o programa. Código de erro: " << result << std::endl;
        system("pause");
    }
    ////




    return 0;
}
