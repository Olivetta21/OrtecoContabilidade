#include <iostream>
#include <locale.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

#include <cstdlib>  // Para std::rand e std::srand
#include <ctime>    // Para std::time

#define UNLEN 25
#define MAXS 30000
#define MINS 10000
#define INUSE "wait_someone_to_end_the_task"
#define NOTINUSE "you_can_do_the_task"

namespace fs = std::filesystem;

std::vector<std::string> arqs;
std::string logLocal;
std::string checkLocal;



std::string obterHorarioAtual() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    std::stringstream ss;
    ss << (st.wDay < 10 ? "0" : "") << st.wDay << "/"
       << (st.wMonth < 10 ? "0" : "") << st.wMonth << "/"
       << st.wYear << " "
       << (st.wHour < 10 ? "0" : "") << st.wHour << ":"
       << (st.wMinute < 10 ? "0" : "") << st.wMinute << ":"
       << (st.wSecond < 10 ? "0" : "") << st.wSecond;

    return ss.str();
}


std::vector<std::string> getNames(){
     // Captura o nome do usuário
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserName(username, &username_len);

    // Captura o nome do computador
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computerName_len = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(computerName, &computerName_len);


    std::vector<std::string> names;
    names.push_back(username);
    names.push_back(computerName);

    return names;
}


void gravaLogs(std::string text){
    int tentativas = 3;
    init:
    tentativas--;

    std::ofstream file(logLocal, std::ios::app);
    if (file.is_open()) {
        for (auto n : getNames()){
            file << n << " - ";
        }
        file << obterHorarioAtual() << " - " << text << "\n";
        file.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo de log: " << tentativas << std::endl;
        if (tentativas){
            Sleep(1000);
            goto init;
        }
    }
}



bool createDirectoryIfNotExists(const std::string& dir) {
    try {
        if (!fs::exists(dir)) {
            fs::create_directory(dir);
            return 0;
        }
    } catch (const fs::filesystem_error& e) {
        std::cout << "Whoops, diretório " << dir << " não criado!\n";
        return 1;
    }
    return 1;
}

bool createDirectory(const std::string& dir){
    try {
        fs::create_directory(dir);
        return 0;
    } catch (const fs::filesystem_error& e) {
        std::cout << "Whoops, diretório " << dir << " não criado!\n";
        return 1;
    }
    return 1;
}


void showResult(){
    system("cls");
    std::cout
    << "      @@@@@@@@@@@@    @@   @@@                    \n"
    << "      @@        @@    @@  @@                      \n"
    << "      @@        @@    @@@@@                       \n"
    << "      @@        @@    @@@@                        \n"
    << "      @@        @@    @@  @@                      \n"
    << "      @@@@@@@@@@@@    @@   @@@                    \n"
    << "                                                  \n"
    << "   VOCÊ PODE SAIR DO PROGRAMA E EXECUTAR          \n"
    << " AS ATUALIZAÇÕES AGORA!                           \n"
    << "                                                  \n";
}

int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");



    //LENDO OS LOCAIS DE CONFIGURACAO E ARQUIVOS
    std::string execLocal;
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
    if (locais.size() > 0){
        execLocal = locais[0] + "executaveis.txt";
        logLocal = locais[0] + "logs.txt";
        checkLocal = locais[0] + "checks.txt";
    }
    else {
        std::cout << "\nsem linhas em local.txt\n";
        system("pause");
        return 1;
    }
    ///////////////////////



    gravaLogs("Abriu o Programa");



    //VERIFICA SE TEM ALGUEM USANDO.
    std::srand(std::time(nullptr));
    tryCheck:
    std::fstream check(checkLocal, std::ios::in | std::ios::out);
    int random_number = MINS + std::rand() % (MAXS - MINS + 1);
    std::cout << "'" << random_number << "' Checando possibilidade...\n";

    if (!check.is_open()) {
        std::cout << "Tentando abrir checks.\n" << checkLocal << "\n";
        Sleep(random_number);
        goto tryCheck;
    }
    else {
        std::string checkLinha;
        std::getline(check, checkLinha);
        if (checkLinha == INUSE){
            std::cout << "Outra pessoa está transferindo, aguardando ela terminar.\n";
            Sleep(random_number);
            goto tryCheck;
        }
        else if (checkLinha == NOTINUSE){
            check.clear(); // Limpa flags de erro
            check.seekp(0, std::ios::beg);
            check << INUSE;
            std::cout << "Tudo certo, realizando a transferencia...\n";
        }
        else {
            std::cout << "Opa, tag não identificada. Por favor, avise o IVAN\n";
            Sleep(random_number);
            goto tryCheck;
        }
    }
    check.close();
    ///////////////////////



    //OBTENDO LINHAS DE ONDE TEM OS ARQUIVOS DE TRANSFERENCIA
    std::ifstream arquivo(execLocal);
    std::string linha;
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << execLocal << std::endl;
        goto toClose;
    }
    while (std::getline(arquivo, linha)) {
        arqs.push_back(linha);
    }
    arquivo.close();
    ///////////////////////



    //PEGANDO O NOME E CRIANDO A PASTA DA NOVA VERSAO
    std::string versaoNova = arqs[arqs.size() - 1];
    arqs.pop_back();
    if (fs::exists(versaoNova)){
        gravaLogs("ja tem a pasta da nova versao");
    }
    else {
        if (createDirectoryIfNotExists(versaoNova)) {
            gravaLogs("nao conseguiu criar a pasta da nova versao.");
            goto toClose;
        }
    }
    versaoNova = fs::current_path().string() + "\\" + versaoNova + "\\";
    ///////////////////////



    //MOVENDO OS AQUIVOS PARA A NOVA PASTA
    gravaLogs("iniciou");
    std::cout << "\nMovendo arquivos...\n\n";
    int restante = arqs.size();
    for (const std::string& s : arqs) {
        std::cout << s << " - " << restante-- << "\nResultado:";
        try {
            fs::path origem = s;
            fs::path destino = versaoNova + origem.filename().string();

            gravaLogs(origem.filename().string());

            // Copia o arquivo para o novo diretório
            if (!fs::exists(destino)) {
                fs::copy(origem, destino);
                std::cout << "sucesso";
            }
            else {
                std::cout << "você já tem esse arquivo";
                gravaLogs("ja tem o arquivo");
            }
        } catch (const fs::filesystem_error& e) {
            std::cout << "erro - " << e.what();
        }
        std::cout << ".\n";
    }
    showResult();
    ///////////////////////



    //COLOCANDO A FLAG NOTINUSE NO CHECKS
    toClose:
    std::ofstream checkToClose(checkLocal);
    if (!checkToClose.is_open()) {
        goto toClose;
    }
    checkToClose << NOTINUSE;
    checkToClose.close();
    ///////////////////////



    gravaLogs("finalizou");
    system("pause");
    return 0;
}
