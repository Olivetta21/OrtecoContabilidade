#include <iostream>
#include <iomanip>
#include <locale>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <sstream>

#define UNLEN 256
#define MAXS 5000
#define MINS 1000
#define CONFIG_ARQ_LOC "local.txt"
#define INSTALL_LOC "C:\\Program Files\\AtualizacoesDominio\\"

namespace fs = std::filesystem;

// Classe para gerenciar informa��es do sistema e usu�rio
class SystemInfo {
private:
    std::string nomeUser;
    std::string nomePC;

public:
    SystemInfo() {
        initializeNames();
    }

    void initializeNames() {
        // Captura o nome do usu�rio
        char username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        GetUserNameA(username, &username_len);

        // Captura o nome do computador
        char computerName[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD computerName_len = MAX_COMPUTERNAME_LENGTH + 1;
        GetComputerNameA(computerName, &computerName_len);

        nomeUser = username;
        nomePC = computerName;
    }

    std::string getUserName() const { return nomeUser; }
    std::string getComputerName() const { return nomePC; }

    static std::string obterHorarioAtual() {
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
};

// Classe para gerenciar logs
class Logger {
private:
    std::string logPath;
    SystemInfo& sysInfo;

public:
    Logger(const std::string& path, SystemInfo& info) : logPath(path), sysInfo(info) {}

    bool gravaLogs(const std::string& text, int maxTentativas = 3) {
        int tentativas = maxTentativas;

        while (tentativas > 0) {
            std::ofstream file(logPath, std::ios::app);
            if (file.is_open()) {
                file << sysInfo.getUserName() << " - " << sysInfo.getComputerName() << " - "
                     << SystemInfo::obterHorarioAtual() << " - " << text << "\n";
                file.close();
                return true;
            } else {
                std::cerr << "Erro ao abrir o arquivo de log. Tentativas restantes: " << tentativas << std::endl;
                tentativas--;
                Sleep(1000);
            }
        }

        return false;
    }

    bool gravaJaAtt(const std::string& text, int maxTentativas = 3) {
        int tentativas = maxTentativas;

        while (tentativas > 0) {
            std::ofstream file(logPath, std::ios::app);
            if (file.is_open()) {
                file << text << "\n";
                file.close();
                return true;
            } else {
                tentativas--;
                Sleep(1000);
            }
        }

        return false;
    }
};

// Classe para gerenciar o sistema de arquivos
class FileSystem {
public:
    static bool createDirectoryIfNotExists(const std::string& dir) {
        try {
            if (!fs::exists(dir)) {
                fs::create_directory(dir);
                return true;
            }
            return true;
        } catch (const fs::filesystem_error& e) {
            std::cout << "Erro ao criar diret�rio " << dir << ": " << e.what() << "\n";
            return false;
        }
    }

    static bool createDirectory(const std::string& dir) {
        try {
            fs::create_directory(dir);
            return true;
        } catch (const fs::filesystem_error& e) {
            std::cout << "Erro ao criar diret�rio " << dir << ": " << e.what() << "\n";
            return false;
        }
    }

    static void deleteFoldersExcept(const std::string& installLoc, const std::string& exclude) {
        try {
            for (const auto& entry : fs::directory_iterator(installLoc)) {
                if (entry.is_directory()) {
                    std::string folderName = entry.path().filename().string();

                    if (folderName != exclude) {
                        std::cout << "Deletando pasta: " << entry.path() << std::endl;
                        fs::remove_all(entry.path());
                    } else {
                        std::cout << "Mantendo pasta: " << entry.path() << std::endl;
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Erro ao acessar o sistema de arquivos: " << e.what() << std::endl;
        }
    }

    static int arqToStrings(const std::string& arq, std::vector<std::string>& vec, int tentativas = 1) {
        vec.clear();

        while (tentativas--) {
            std::ifstream file(arq);
            std::string linha;
            if (!file.is_open()) {
                if (tentativas) {
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

    static int copy_with_progress(const std::filesystem::path& src, const std::filesystem::path& dest) {
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
            std::cerr << "Falha ao alocar buffer de mem�ria.\n";
            return 2;
        }

        std::string size_lbl = "Gb";
        double tot = (double) total_size / (double)(1024*1024*1024);
        if (tot < 1) {
            tot = (double)total_size / (double)(1024*1024);
            size_lbl = "Mb";
        }

        std::cout << "Iniciando c�pia de " << std::fixed << std::setprecision(2) << tot << size_lbl << "\n";

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
        std::cout << "\rProgresso: 100%\nConclu�do com sucesso.\n";
        return 0;
    }
};

// Classe para gerenciar os execut�veis
class ExecutableManager {
public:
    static int abrirPrograma(const std::string& local) {
        std::string nomeProgam = fs::path(local).filename().string();

        // Estruturas necess�rias para criar o processo
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        // Inicializa as estruturas
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // Caminho para o programa que voc� quer abrir
        LPCSTR programPath = local.c_str();

        // Cria o processo
        if (!CreateProcessA(
            programPath,   // Caminho do programa
            NULL,          // Argumentos da linha de comando
            NULL,          // Atributos de seguran�a do processo
            NULL,          // Atributos de seguran�a do thread
            FALSE,         // Heran�a de handles
            0,             // Flags de cria��o
            NULL,          // Vari�veis de ambiente
            NULL,          // Diret�rio de trabalho atual
            &si,           // Informa��es sobre o startup
            &pi            // Informa��es sobre o processo
        )) {
            std::cerr << "Falha ao iniciar:" << nomeProgam << "! C�digo de erro: " << GetLastError() << std::endl;
            return 1;
        }

        // Aguarda o t�rmino do processo
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Obt�m o c�digo de sa�da do processo
        DWORD exitCode;
        if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
            std::cout << "C�digo de sa�da: " << exitCode << std::endl;
        } else {
            std::cerr << "Falha ao obter o c�digo de sa�da do processo!" << std::endl;
            return 1;
        }

        // Fecha os handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return 0;
    }
};

// Classe para gerenciar a interface com o usu�rio
class UserInterface {
public:
    static void cabecalho() {
        system("cls");
        std::cout << "\n"
        << "v3____________________________________________________________ \n"
        << "  @@@  @@@@@@@@ @@  @@   @@@   @@    @@ @@@@@@@   @@@   @@@@@@ \n"
        << " @@ @@    @@    @@  @@  @@ @@  @@           @@   @@ @@  @@  @@ \n"
        << "@@   @@   @@    @@  @@ @@   @@ @@    @@   @@@   @@   @@ @@@@   \n"
        << "@@@@@@@   @@    @@  @@ @@@@@@@ @@    @@  @@     @@@@@@@ @@ @@  \n"
        << "@@   @@   @@     @@@@  @@   @@ @@@@@ @@ @@@@@@@ @@   @@ @@  @@ \n"
        << "          N�O FECHE ESSA TELA ANTES TERMINAR TUDO!!!           \n"
        << "______________________________________________________________ \n";
    }

    static void showResult(const std::string& add = "") {
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

    static std::string obterResposta(const std::string& pergunta) {
        std::string resposta;
        std::cout << pergunta;
        std::cin >> resposta;
        return resposta;
    }
};

// Classe para transfer�ncia de arquivos
class FileTransfer {
private:
    SystemInfo& sysInfo;
    Logger& logger;

public:
    FileTransfer(SystemInfo& info, Logger& logMain)
        : sysInfo(info), logger(logMain) {}

    bool transferirArquivos(const std::vector<std::string>& arquivos,
                           const std::string& pastaDestino,
                           const std::vector<std::string>& arquivosJaAtualizados,
                           std::vector<std::string>& destinosFinais) {

        logger.gravaLogs("iniciou transfer�ncia");
        std::cout << "\nMovendo arquivos...\n\n";

        int total = arquivos.size();
        int atual = 1;

        for (const std::string& origem : arquivos) {
            try {
                fs::path caminhoOrigem = origem;
                std::string nomeArquivo = caminhoOrigem.filename().string();
                fs::path destino = pastaDestino + nomeArquivo;

                logger.gravaLogs("copiando: " + nomeArquivo);
                std::cout << nomeArquivo << " - " << atual++ << " de " << total << "\nResultado: ";

                // Verifica se j� est� atualizado
                if (!verificarAtualizacao(nomeArquivo, arquivosJaAtualizados)) {
                    continue;
                }

                // Copia o arquivo para o novo diret�rio
                if (fs::exists(destino) && arquivos_iguais(caminhoOrigem.string(), destino.string())) {
                    std::cout << "Ok.\n";
                } else if (FileSystem::copy_with_progress(caminhoOrigem, destino)) {
                    logger.gravaLogs("erro ao copiar: " + nomeArquivo);
                    std::cout << "Erro ao copiar o arquivo: " << nomeArquivo << "\n";
                    system("pause");
                    return false;
                }

                destinosFinais.push_back(destino.string());
            }
            catch (const fs::filesystem_error& e) {
                std::cout << "Erro - " << e.what();
                return false;
            }
            std::cout << ".\n";
        }

        return true;
    }

private:
    bool arquivos_iguais(const std::string& caminho1, const std::string& caminho2) {
        std::cout << "Comparando arquivos, aguarde...\n";
        std::ifstream f1(caminho1, std::ios::binary);
        std::ifstream f2(caminho2, std::ios::binary);

        if (!f1 || !f2) {
            std::cerr << "Erro ao abrir um dos arquivos.\n";
            return false;
        }

        // Verifica tamanhos primeiro
        f1.seekg(0, std::ios::end);
        f2.seekg(0, std::ios::end);
        if (f1.tellg() != f2.tellg())
            return false;

        // Volta para o in�cio dos arquivos
        f1.seekg(0);
        f2.seekg(0);

        // Compara conte�do byte a byte
        char c1, c2;
        while (f1.get(c1) && f2.get(c2)) {
            if (c1 != c2)
                return false;
        }

        return true;
    }

    bool verificarAtualizacao(const std::string& nomeArquivo,
                             const std::vector<std::string>& arquivosJaAtualizados) {
        // Verifica se j� est� atualizado
        for (const std::string& arqAtualizado : arquivosJaAtualizados) {
            //Garante que o usuario s� saia do loop se ele responder corretamente
            bool jaAtualizado = (arqAtualizado == nomeArquivo);
            while (jaAtualizado) {
                // Se o arquivo j� foi atualizado, pergunta ao usu�rio se deseja atualizar novamente
                std::string resposta = UserInterface::obterResposta(
                    "Foi identificado que o instalador \"" + nomeArquivo +
                    "\" j� foi executado uma vez. Deseja execut�-lo novamente? (S/N): ");

                if (resposta == "s" || resposta == "S") {
                    // Continua com a atualiza��o sem registrar novamente
                    return true;
                }
                else if (resposta == "n" || resposta == "N") {
                    logger.gravaLogs("j� est� atualizado: " + nomeArquivo);
                    return false;
                }
                else {
                    std::cout << "Resposta inv�lida. Por favor, responda com 'S' ou 'N'.\n";
                }
            }
        }

        // Registra que o arquivo foi atualizado
        //attLogger.gravaJaAtt(nomeArquivo);
        return true;
    }
};

// Classe para execu��o de atualiza��es
class UpdateExecutor {
private:
    Logger& logger;
    Logger& attLogger;

public:
    UpdateExecutor(Logger& log, Logger& logAtt) : logger(log), attLogger(logAtt) {}

    bool executarAtualizacoes(const std::vector<std::string>& arquivos, const std::vector<std::string>& arquivosJaAtualizados = {}) {
        for (const std::string& arquivo : arquivos) {
            std::string nome = fs::path(arquivo).filename().string();

            std::cout << "Aten��o, o programa de atualiza��o \"" << nome
                     << "\" ir� abrir automaticamente, aguarde isso acontecer.\n";

            if (ExecutableManager::abrirPrograma(arquivo)) {
                std::cout << "Ocorreram erros ao abrir o arquivo. Abortando...\n";
                return false;
            }

            logger.gravaLogs("atualizou: " + nome);

            bool jaAtualizado = false;
            for (const std::string& arqAtualizado : arquivosJaAtualizados) {
                if (arqAtualizado == nome) jaAtualizado = true;
            }

            if (!jaAtualizado) attLogger.gravaJaAtt(nome);
        }

        return true;
    }
};

// Classe principal que gerencia a aplica��o
class AtualizadorApp {
private:
    SystemInfo sysInfo;
    std::string logLocal;
    std::string jaAttLoc;
    std::unique_ptr<Logger> logger;
    std::unique_ptr<Logger> attLogger;
    std::unique_ptr<FileTransfer> transferidor;
    std::unique_ptr<UpdateExecutor> executor;

    std::vector<std::string> arquivosTransferidos;

public:
    AtualizadorApp() {

    }

    bool inicializar() {
        // L� as configura��es iniciais
        std::vector<std::string> locais;

        if (!carregarConfiguracoes(locais)) {
            return false;
        }

        // Inicializa o logger
        logger = std::make_unique<Logger>(logLocal, sysInfo);
        logger->gravaLogs("Abriu o Programa");

        if (!FileSystem::createDirectory(INSTALL_LOC)) {
            logger->gravaLogs("N�o foi poss�vel criar o diret�rio de instala��o");
            std::cout << "Diret�rio de instala��o n�o criado!\n";
            system("pause");
            return false;
        }

        return true;
    }

    bool carregarConfiguracoes(std::vector<std::string>& locais) {
        switch (FileSystem::arqToStrings(CONFIG_ARQ_LOC, locais, 2)) {
            case 1:
                std::cerr << "Erro ao abrir o arquivo: " << CONFIG_ARQ_LOC << std::endl;
                system("pause");
                return false;
            case 2:
                std::cout << "\nSem linhas em: " << CONFIG_ARQ_LOC << "\n";
                system("pause");
                return false;
            case 0:
                if (fs::exists(locais[0])) {
                    logLocal = locais[0] + "logs.txt";
                } else {
                    std::cout << "O caminho: " << locais[0] << " n�o foi encontrado.\n";
                    system("pause");
                    return false;
                }
                break;
        }
        return true;
    }

    bool executar() {
        if (!inicializar()) {
            return false;
        }

        UserInterface::cabecalho();

        // Obt�m os caminhos dos arquivos a serem transferidos
        std::vector<std::string> arqs;
        std::string execLocal = obterCaminhoExecutaveis();

        if (execLocal.empty() || !obterArquivosTransferencia(execLocal, arqs)) {
            return false;
        }

        // Obt�m e cria a pasta da nova vers�o
        std::string pastaNova = arqs[arqs.size() - 1];
        arqs.pop_back();
        std::string versaoNova = INSTALL_LOC + pastaNova + "\\";

        if (!configurarPastaNova(versaoNova)) {
            return false;
        }

        // Configura o logger de atualiza��es
        jaAttLoc = versaoNova + "jaAtualizado.txt";
        attLogger = std::make_unique<Logger>(jaAttLoc, sysInfo);

        // Verifica arquivos j� atualizados
        std::vector<std::string> jaArqs;
        FileSystem::arqToStrings(jaAttLoc, jaArqs, 2);

        // Inicializa o transferidor e o executor de atualiza��es
        transferidor = std::make_unique<FileTransfer>(sysInfo, *logger);
        executor = std::make_unique<UpdateExecutor>(*logger, *attLogger);

        // Move os arquivos para a nova pasta
        if (!transferidor->transferirArquivos(arqs, versaoNova, jaArqs, arquivosTransferidos)) {
            return false;
        }

        // Executa os arquivos de atualiza��o
        UserInterface::cabecalho();
        std::cout << "\nTRANSFER�NCIA CONCLU�DA, AGORA ESPERE\n TODAS AS ATUALIZA��ES SEREM EXECUTADAS.\nN�O FECHE ESSA JANELA AINDA!!!!\n\n\n";
        logger->gravaLogs("finalizou transfer�ncia");

        if (!executor->executarAtualizacoes(arquivosTransferidos, jaArqs)) {
            return false;
        }

        // Limpa as vers�es antigas e finaliza
        system("cls");
        std::cout << "Deletando arquivos de instala��o antigos...\n";
        FileSystem::deleteFoldersExcept(INSTALL_LOC, pastaNova);
        std::cout << "\n\n\n";

        UserInterface::showResult("SISTEMA ATUALIZADO! PODE FECHAR ESSA JANELA E ENTRAR NO SISTEMA AGORA!\n");
        logger->gravaLogs("fechou o programa.");

        return true;
    }

private:
    std::string obterCaminhoExecutaveis() {
        std::vector<std::string> locais;
        switch(FileSystem::arqToStrings(CONFIG_ARQ_LOC, locais, 2)) {
            case 0:
                if (fs::exists(locais[0])) {
                    return locais[0] + "executaveis.txt";
                }
                std::cout << "O caminho: " << locais[0] << " n�o foi encontrado.\n";
                break;
            default:
                std::cerr << "Erro ao ler o arquivo de configura��o: " << CONFIG_ARQ_LOC << std::endl;
                break;
        }
        system("pause");
        return "";
    }

    bool obterArquivosTransferencia(const std::string& execLocal, std::vector<std::string>& arqs) {
        switch(FileSystem::arqToStrings(execLocal, arqs, 2)) {
            case 1:
                std::cerr << "Erro ao abrir o arquivo: " << execLocal << std::endl;
                system("pause");
                return false;
            case 2:
                std::cout << "\nSem linhas em: " << execLocal << "\n";
                system("pause");
                return false;
            case 0:
                if (arqs.size() < 2) {
                    std::cout << "OPS, n�o foi definido nenhum arquivo de atualiza��o para ser transferido!\n";
                    system("pause");
                    return false;
                }
                // Verifica se todos os arquivos existem, exceto o �ltimo (nome da pasta)
                for (int i = 0; i < arqs.size() - 1; i++) {
                    if (!fs::exists(arqs[i])) {
                        std::cout << "Arquivo inexistente: " << arqs[i] << "\n";
                        system("pause");
                        return false;
                    }
                }
                break;
        }
        return true;
    }

    bool configurarPastaNova(const std::string& versaoNova) {
        if (fs::exists(versaoNova)) {
            logger->gravaLogs("j� tem a pasta da nova vers�o");
        } else {
            if (!FileSystem::createDirectoryIfNotExists(versaoNova)) {
                logger->gravaLogs("n�o conseguiu criar a pasta da nova vers�o.");
                std::cout << "pasta da nova vers�o n�o criada!\n";
                system("pause");
                return false;
            }
        }
        return true;
    }
};

int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");

    AtualizadorApp app;
    if (app.executar()) {
        system("pause");
        return 0;
    }
    return 1;
}
