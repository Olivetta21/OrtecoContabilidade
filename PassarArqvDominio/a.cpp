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
#define MAXS 5000
#define MINS 1000
#define INUSE "wait_someone_to_end_the_task"
#define NOTINUSE "you_can_do_the_task"
#define CONFIG_ARQ_LOC "local.txt"
#define INSTALL_LOC "C:\\Program Files\\AtualizacoesDominio\\"

namespace fs = std::filesystem;

std::vector<std::string> arqs;
std::string logLocal;
std::string jaAttLoc;

std::string nomeUser;
std::string nomePC;




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


std::vector<std::string> getNamesPC(){
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

    while (names.size() < 2) names.push_back("ERRO");

    return names;
}


void gravaLogs(std::string text){
    int tentativas = 3;
    init:
    tentativas--;

    std::ofstream file(logLocal, std::ios::app);
    if (file.is_open()) {
        file << nomeUser << " - " << nomePC << " - " << obterHorarioAtual() << " - " << text << "\n";
        file.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo de log: " << tentativas << std::endl;
        if (tentativas){
            Sleep(1000);
            goto init;
        }
    }
}


void gravaJaAtt(std::string jaAttPlace, std::string text){
    int tentativas = 3;
    init:
    tentativas--;

    std::ofstream file(jaAttPlace, std::ios::app);
    if (file.is_open()) {
        file << text << "\n";
        file.close();
    } else {
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


int copy_with_progress(const std::filesystem::path& src, const std::filesystem::path& dest) {
    std::ifstream source(src, std::ios::binary);
    std::ofstream destination(dest, std::ios::binary);

    if (!source.is_open() || !destination.is_open()) {
        std::cerr << "Erro ao abrir os arquivos de origem e/ou destino.\n";
        return 1;
    }

    const std::uintmax_t total_size = std::filesystem::file_size(src);
    std::uintmax_t copied_size = 0;

    // Tenta um buffer maior primeiro (1MB)
    const std::size_t buffer_size = 1 << 20; // 1MB
    std::unique_ptr<char[]> buffer(new (std::nothrow) char[buffer_size]);

    if (!buffer) {
        std::cerr << "Falha ao alocar buffer de memória.\n";
        return 2;
    }

    std::cout << "Iniciando cópia de " << total_size << " bytes...\n";

    int last_progress = -1;
    while (!source.eof()) {
        source.read(buffer.get(), buffer_size);
        std::streamsize bytes_read = source.gcount();

        if (bytes_read <= 0) break;

        destination.write(buffer.get(), bytes_read);
        copied_size += bytes_read;

        int progress = static_cast<int>((100 * copied_size) / total_size);
        if (progress != last_progress) {
            std::cout << "\rProgresso: " << progress << "%";
            std::cout.flush();
            last_progress = progress;
        }
    }

    destination.flush();
    std::cout << "\rProgresso: 100%\nConcluído com sucesso.\n";
    return 0;
}


////////////////////////////////////////////////
int abrirPrograma(std::string local) {
    std::string nomeProgam = fs::path(local).filename().string();

    // Estruturas necessárias para criar o processo
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Inicializa as estruturas
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Caminho para o programa que você quer abrir
    LPCSTR programPath = local.c_str();

    // Cria o processo
    if (!CreateProcess(
        programPath,   // Caminho do programa
        NULL,          // Argumentos da linha de comando
        NULL,          // Atributos de segurança do processo
        NULL,          // Atributos de segurança do thread
        FALSE,         // Herança de handles
        0,             // Flags de criação
        NULL,          // Variáveis de ambiente
        NULL,          // Diretório de trabalho atual
        &si,           // Informações sobre o startup
        &pi            // Informações sobre o processo
    )) {
        std::cerr << "Falha ao iniciar:" << nomeProgam << "! Código de erro: " << GetLastError() << std::endl;
        return 1;
    }

    // Aguarda o término do processo
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Obtém o código de saída do processo
    DWORD exitCode;
    if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
        std::cout << "Código de saída: " << exitCode << std::endl;
    } else {
        std::cerr << "Falha ao obter o código de saída do processo!" << std::endl;
        return 1;
    }

    // Fecha os handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
////////////////////////////////////////////////


int arqToStrings(std::string arq, std::vector<std::string> &vec, int tentativas = 1){

    vec.clear();

    while (tentativas--){
        std::ifstream file(arq);
        std::string linha;
        if (!file.is_open()) {
            if (tentativas){
                Sleep(1000);
                continue;
            }
            return 1;
        }

        while (std::getline(file, linha)) {
            vec.push_back(linha);
        }

        file.close();

        if (vec.size() < 1) return 2;

        return 0;
    }

    return 1;
}

void showResult(std::string add = ""){
    std::cout
    << "      @@@@@@@@@@@@    @@   @@@                    \n"
    << "      @@        @@    @@  @@                      \n"
    << "      @@        @@    @@@@@                       \n"
    << "      @@        @@    @@@@                        \n"
    << "      @@        @@    @@  @@                      \n"
    << "      @@@@@@@@@@@@    @@   @@@                    \n"
    << "                                                  \n";
    std::cout << add << "\n\n\n";
}

void cabecalho(){
    system("cls");
    std::cout << "\n"
    << "______________________________________________________________ \n"
    << "  @@@  @@@@@@@@ @@  @@   @@@   @@    @@ @@@@@@@   @@@   @@@@@@ \n"
    << " @@ @@    @@    @@  @@  @@ @@  @@           @@   @@ @@  @@  @@ \n"
    << "@@   @@   @@    @@  @@ @@   @@ @@    @@   @@@   @@   @@ @@@@   \n"
    << "@@@@@@@   @@    @@  @@ @@@@@@@ @@    @@  @@     @@@@@@@ @@ @@  \n"
    << "@@   @@   @@     @@@@  @@   @@ @@@@@ @@ @@@@@@@ @@   @@ @@  @@ \n"
    << "          NÃO FECHE ESSA TELA ANTES TERMINAR TUDO!!!           \n"
    << "______________________________________________________________ \n";
}


int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");


    if (createDirectoryIfNotExists(INSTALL_LOC)) {
        gravaLogs("Não foi possivel criar o diretorio de instalação");

        std::cout << "diretorio de instalação não criado!\n";
        system("pause");
        return 1;
    }


    cabecalho();

    {
        std::vector<std::string> nomespc = getNamesPC();
        nomeUser = nomespc[0];
        nomePC = nomespc[1];
    }


    //LENDO OS LOCAIS DE CONFIGURACAO E ARQUIVOS
    std::string execLocal;
    std::string checkLocal;
    std::vector<std::string> locais;

    switch(arqToStrings(CONFIG_ARQ_LOC, locais, 2)){
        case 1:
            std::cerr << "err1| Erro ao abrir o arquivo: " << CONFIG_ARQ_LOC << std::endl;
            system("pause");
            return 1;
        case 2:
            std::cout << "\nSem linhas em: " << CONFIG_ARQ_LOC << "\n";
            system("pause");
            return 1;
        case 0:
            if (fs::exists(locais[0])){
                execLocal = locais[0] + "executaveis.txt";
                logLocal = locais[0] + "logs.txt";
                checkLocal = locais[0] + "checks.txt";
            }
            else{
                std::cout << "O caminho: " << locais[0] << " não foi encontrado.\n";
                system("pause");
                return 1;
            }
            break;
    }
    ///////////////////////


    gravaLogs("Abriu o Programa");



    //OBTENDO LINHAS DE ONDE TEM OS ARQUIVOS DE TRANSFERENCIA
    switch(arqToStrings(execLocal, arqs, 2)){
        case 1:
            std::cerr << "Erro ao abrir o arquivo: " << execLocal << std::endl;
            system("pause");
            return 1;
        case 2:
            std::cout << "\nSem linhas em: " << execLocal << "\n";
            system("pause");
            return 1;
        case 0:
            if (arqs.size() < 2) {
                std::cout << "OPS, não foi definido nenhum arquivo de atualização para ser transferido!\n";
                system("pause");
                return 1;
            }
            for (int i = 0; i < arqs.size() - 1; i++){
                if (!fs::exists(arqs[i])){
                    std::cout << "Arquivo inexistente: " << arqs[i] << "\n";
                    system("pause");
                    return 1;
                }
            }
            break;
    }

    ///////////////////////
    //PEGANDO O NOME E CRIANDO A PASTA DA NOVA VERSAO
    std::string versaoNova = arqs[arqs.size() - 1];
    arqs.pop_back();


    versaoNova = INSTALL_LOC + versaoNova + "\\";

    if (fs::exists(versaoNova)){
        gravaLogs("ja tem a pasta da nova versao");
    }
    else {
        if (createDirectoryIfNotExists(versaoNova)) {
            gravaLogs("nao conseguiu criar a pasta da nova versao.");

            std::cout << "pasta da nova versao nao criada!\n";
            system("pause");
            return 1;
        }
    }
    ///////////////////////


    //PARA VERIFICAR SE EXISTE ALGUM ARQUIVO JA ESTA ATUALIZADO
    jaAttLoc = versaoNova + "jaAtualizado.txt";
    std::vector<std::string> jaArqs;
    arqToStrings(jaAttLoc, jaArqs, 2);
    ///////////////////////


    //MOVENDO OS AQUIVOS PARA A NOVA PASTA
    std::vector<std::string> novosDestinos;

    gravaLogs("iniciou");
    std::cout << "\nMovendo arquivos...\n\n";
    int restante = arqs.size();
    for (const std::string& s : arqs) {
        std::cout << s << " - " << restante-- << "\nResultado:";
        try {
            fs::path origem = s;

            std::string nomeArquivo = origem.filename().string();
            gravaLogs("copiando: " + nomeArquivo);

            fs::path destino = versaoNova + nomeArquivo;



            //ATUALIZADO ?////////////////
            bool estaAtualizado = false;
            for (std::string str : jaArqs){
                if (str == nomeArquivo){
                    estaAtualizado = true;
                    break;
                }
            }
            if (estaAtualizado) {
                gravaLogs("ja esta atualizado");
                continue;
            }
            //////////////////////////////



            // Copia o arquivo para o novo diretório
            if (!fs::exists(destino)) {
                if(copy_with_progress(origem, destino)){
                    gravaLogs("erro copiar: " + nomeArquivo);
                    std::cout << "Erro ao copiar o arquivo: " << nomeArquivo << "\n";
                    system("pause");
                    return 1;
                }
            }
            else {
                std::cout << "você já tem esse arquivo";
                gravaLogs("ja tem o arquivo");
            }

            novosDestinos.push_back(destino.string());

        } catch (const fs::filesystem_error& e) {
            std::cout << "erro - " << e.what();
        }
        std::cout << ".\n";
    }
    cabecalho();
    std::cout << "\nTRANSFERÊNCIA CONCLUÍDA, AGORA ESPERE\n TODAS AS ATUALIZAÇÕES SEREM EXECUTADAS.\nNÃO FECHE ESSA JANELA AINDA!!!!\n\n\n";
    ///////////////////////



    gravaLogs("finalizou");


    bool err = 0;
    for (std::string a : novosDestinos){
        std::string nome = fs::path(a).filename().string();

        std::cout << "Atenção, o programa de atualização \"" << nome << "\" irá abrir automaticamente, aguarde isso acontecer.\n";

        if (abrirPrograma(a)) {
            std::cout << "Ocorreram erros ao abrir o arquivo. Abortando...\n";
            err = 1;
            break;
        }
        else {
            gravaJaAtt(jaAttLoc, nome);
            gravaLogs("atualizou: " + nome);
        }
    }

    if (!err){
        cabecalho();
        showResult("SISTEMA ATUALIZADO! PODE FECHAR ESSA JANELA E ENTRAR NO SISTEMA AGORA!\n");
    }



    gravaLogs("fechou o programa.");
    std::cout << "Por favor, pressione ENTER para FECHAR o programa.\n";
    system("pause");
    return 0;
}
