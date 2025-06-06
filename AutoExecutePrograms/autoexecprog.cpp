#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <random>
#include <windows.h>

#define BANCO_PATH "./banco.txt"
#define PASTA_PUBLICA "//arq.orteco.ctb/Arquivo/#Todos/Executaveis/"
#define PASTA_PRIVADA "//arq.orteco.ctb/Arquivo/#Utilitarios/AutoExecutePrograms/ToInstall/"
#define DESTINO "C:/Program Files/AutoExecuteProgramsOrteco/"
#define SENHA_ADM .env

namespace fs = std::filesystem;


struct Registro {
    std::string codigo;
    std::string caminho;
};

std::string criptografar(const std::string& senha) {
    std::string criptografada = senha;
    for (char& c : criptografada) {
        c = c + 2;  // Simples deslocamento de caractere
    }
    return criptografada;
}
std::string desCriptografar(const std::string& senha) {
    std::string criptografada = senha;
    for (char& c : criptografada) {
        c = c - 2;  // Simples deslocamento de caractere
    }
    return criptografada;
}

// Função para ler o banco
std::vector<Registro> lerBanco() {
    std::vector<Registro> registros;
    std::ifstream arquivo(BANCO_PATH);
    std::string linha;
    while (std::getline(arquivo, linha)) {
        std::istringstream iss(linha);
        std::string codigo, caminho;
        if (std::getline(iss, codigo, '@') && std::getline(iss, caminho)) {
            registros.push_back({codigo, caminho});
        }
    }
    return registros;
}

// Função para adicionar ao banco
void adicionarAoBanco(const std::string& codigo, const std::string& caminho) {
    std::ofstream arquivo(BANCO_PATH, std::ios::app);
    arquivo << codigo << "@" << caminho << std::endl;
}

// Gera código aleatório
std::string gerarCodigo(int tamanho = 8) {
    const char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(caracteres) - 2);

    std::string codigo;
    for (int i = 0; i < tamanho; ++i) {
        codigo += caracteres[dis(gen)];
    }
    return codigo;
}

// Executa o arquivo
void executarArquivo(const std::string& caminho) {
    ShellExecuteA(NULL, "open", caminho.c_str(), NULL, NULL, SW_SHOW);
}

bool createDirectory(const std::string& dir){
    try {
        fs::create_directory(dir);
        return 0;
    } catch (const fs::filesystem_error& e) {
        std::cout << "Whoops: " << e.what() << std::endl;
        return 1;
    }
    return 1;
}

// Copia arquivo
bool copiarArquivo(const std::string& origem, const std::string& destino) {
    std::cout << "Origem: " << origem << std::endl << "Destino: " << destino << std::endl;
    try {
        fs::copy_file(origem, destino, fs::copy_options::overwrite_existing);
        return true;
    } catch (const fs::filesystem_error& e) {
        std::cout << "Whoops: " << e.what() << std::endl;
        return false;
    }
}

// Modo administrador
int modoAdministrador() {
    std::string senha;
    std::cout << "Digite a senha de administrador: ";
    std::cin >> senha;
    if (criptografar(senha) != SENHA_ADM) {
        std::cout << "Acesso negado!" << std::endl;
        return 5;
    }
    system("cls");

    std::vector<std::string> arquivos;
    std::cout << "\nArquivos disponíveis em " << PASTA_PUBLICA << ":\n";
    int id = 0;

    for (const auto& entry : fs::directory_iterator(PASTA_PUBLICA)) {
        if (entry.is_regular_file() && entry.path().extension() == ".exe") {
            arquivos.push_back(entry.path().string());
            std::cout << id << ": " << entry.path().filename().string() << std::endl;
            ++id;
        }
    }

    int escolha;
    std::cout << "Digite o ID do executável para adicionar: ";
    std::cin >> escolha;

    if (escolha == -999) {
        std::string descstr;
        std::cout << "Digite o ID do executável: ";
        std::cin >> descstr;
        std::cout << desCriptografar(descstr) << std::endl;
        system("pause");
        return 0;
    }

    if (arquivos.empty()) {
        std::cout << "Nenhum executável encontrado." << std::endl;
        return 6;
    }

    if (escolha < 0 || escolha >= arquivos.size()) {
        std::cout << "ID inválido." << std::endl;
        return 7;
    }

    std::string selecionado = arquivos[escolha];
    std::string nomeArquivo = fs::path(selecionado).filename().string();
    std::string novoCaminho = std::string(PASTA_PRIVADA) + nomeArquivo;

    // Move o arquivo
    try {
        fs::rename(selecionado, novoCaminho);
    } catch (...) {
        std::cout << "Erro ao mover o arquivo." << std::endl;
        return 8;
    }

    std::string codigo = gerarCodigo();
    adicionarAoBanco(criptografar(codigo), novoCaminho);

    std::cout << "Arquivo movido para privado e registrado com o código: " << codigo << std::endl;
    system("pause");
    return 0;
}

// Modo usuário
int modoUsuario(const std::string& codigo) {
    auto registros = lerBanco();
    for (const auto& reg : registros) {
        if (reg.codigo == codigo) {
            if (!fs::exists(reg.caminho)) {
                std::cout << "Arquivo não encontrado: " << reg.caminho << std::endl;
                return 1;
            }
            std::string destino = std::string(DESTINO) + fs::path(reg.caminho).filename().string();

            if (createDirectory(std::string(DESTINO))) return 10;

            if (copiarArquivo(reg.caminho, destino)) {
                std::cout << "Executando..." << std::endl;
                executarArquivo(destino);
                return 0;
            } else {
                std::cout << "Erro ao copiar o arquivo." << std::endl;
                return 2;
            }
            return 3;
        }
    }
    std::cout << "Código não encontrado no banco." << std::endl;
    return 4;
}

int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");

    std::string entrada;
    std::cout << "Digite o código de instalação: ";
    std::cin >> entrada;

    int withError = 0;
    if (entrada == "/add") {
        withError = modoAdministrador();
    } else {
        withError = modoUsuario(criptografar(entrada));
    }

    if (withError > 0) system("pause");

    return 0;
}
