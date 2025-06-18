#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <algorithm>
#define CONFIGDIR ".Configuracao/"

namespace fs = std::filesystem;

std::string linebreak() {
    return std::string(50, '-');
}

void pressToContinue() {
    std::cout << "\npressione para voltar...\n";
    _getch();
}

class UserVerifier {
private:
    std::string user;
    std::string domain;
    std::string psExecDir = CONFIGDIR "PsExec.exe";

    std::string getCurrentDomain() {
        char domain[256];
        DWORD size = sizeof(domain);
        if (GetEnvironmentVariableA("USERDOMAIN", domain, size)) {
            return std::string(domain);
        }
        return "";
    }

    std::string getCurrentUsername() {
        char username[256];
        DWORD size = sizeof(username);
        if (GetEnvironmentVariableA("USERNAME", username, size)) {
            return std::string(username);
        }
        return "";
    }

    std::string askPassword() {
        std::string typed_password;
        char ch;
        std::cout << "Digite sua senha: ";
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (!typed_password.empty()) {
                    typed_password.pop_back();
                    std::cout << "\b \b";
                }
            } else {
                typed_password.push_back(ch);
                std::cout << '*';
            }
        }
        std::cout << std::endl;
        return typed_password;
    }

    DWORD testPassword(const std::string& password) {
        std::ostringstream cmdBuilder;
        cmdBuilder << psExecDir << " -accepteula -nobanner -u \"" << getFullUser() //o PsExec está na mesma pasta do executável
                  << "\" -p \"" << password
                  << "\" cmd /c exit";
        // Estruturas para CreateProcess
        STARTUPINFOA si = { sizeof(STARTUPINFOA) };
        PROCESS_INFORMATION pi;

        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;  // Esconde a janela

        // Cria o processo
        if (!CreateProcessA(
            NULL,                   // Sem aplicativo específico
            &cmdBuilder.str()[0],            // Linha de comando
            NULL,                   // Atributos de segurança do processo
            NULL,                   // Atributos de segurança da thread
            FALSE,                  // Sem herança de handles
            CREATE_NO_WINDOW,       // Sem janela
            NULL,                   // Ambiente pai
            NULL,                   // Diretório atual
            &si,                    // Informações de inicialização
            &pi))                   // Informações do processo
        {
            std::cerr << "Falha ao iniciar o processo: " << GetLastError() << std::endl;
            return -1; // Retorna erro
        }

        // Aguarda o processo terminar
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Obtém o código de saída
        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        // Fecha os handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return exitCode;
    }



public:
    UserVerifier () {
        user = getCurrentUsername();
        domain = getCurrentDomain();
    }

    std::string getFullUser() const {
        if (user.empty() || domain.empty()) {
            std::cerr << "Erro ao obter domínio ou usuário." << std::endl;
            return "";
        }
        return domain + "\\" + user;
    }

    bool verify() {
        std::string password = askPassword();
        return (0 == testPassword(password));
    }
};



class Voto {
private:
    std::string pasta;
    std::string arquivoVotos;
    std::string destinoImgVencedora;
    
    struct VotoDB {
        std::string usuario;
        std::string imagem;
    };
    
    std::vector<std::string> imagens;
    std::vector<VotoDB> votos;
    

    void carregarImagens() {
        if (imagens.size() > 0) {
            return; // Já carregou as imagens
        }
        std::vector<std::string> imagens;
        for (const auto& entry : fs::directory_iterator(pasta)) {
            if (entry.is_regular_file()) {
                auto ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".png") {
                    imagens.push_back(entry.path().filename().string());
                }
            }
        }
        std::sort(imagens.begin(), imagens.end());
        this->imagens = imagens;
    }



    void carregarVotos() {
        if (votos.size() > 0) {
            return; // Já carregou os votos
        }
        std::vector<VotoDB> votos;
        std::ifstream fin(arquivoVotos);
        std::string linha;
        while (std::getline(fin, linha)) {
            size_t pos = linha.find('|');
            if (pos != std::string::npos) {
                VotoDB v;
                v.usuario = linha.substr(0, pos);
                v.imagem = linha.substr(pos + 1);
                votos.push_back(v);
            }
        }
        this->votos = votos;
    }

    void salvarVoto(const std::string& usuario, const std::string& imagem) {
        std::ofstream fout(arquivoVotos, std::ios::app);
        fout << usuario << "|" << imagem << "\n";
    }

    std::map<std::string, int> resultadoParcial() {
        std::map<std::string, int> contagem;
        for (const auto& v : votos) {
            contagem[v.imagem]++;
        }
        return contagem;
    }

    std::pair<std::string, int> resultadoGanhador() {
        std::map<std::string, int> contagem;
        for (const auto& v : votos) {
            contagem[v.imagem]++;
        }

        if (!contagem.empty()) {
            auto max = std::max_element(contagem.begin(), contagem.end(),
                [](const auto& a, const auto& b) {
                    return a.second < b.second;
                });
            return { max->first, max->second };
        } 
        return { "", 0 };
    }

public:

    Voto() {
        pasta = "Candidatas_PNG";
        arquivoVotos = CONFIGDIR "votos.txt";
        destinoImgVencedora = "wallpaper.png";
    }

    bool usuarioJaVotou(const std::string& usuario) {
        carregarVotos();
        for (const auto& v : votos) {
            if (v.usuario == usuario)
                return true;
        }
        return false;
    }

    void listarOpcoes() {
        carregarImagens();
        std::cout << "Imagens disponíveis para votação:\n";
        for (size_t i = 0; i < imagens.size(); ++i) {
            std::cout << i << ": " << imagens[i] << "\n";
        }
    }

    void votar(const std::string& usuario) {
        carregarVotos();
        carregarImagens();

        int escolha;
        std::cout << "Digite o número da imagem que deseja votar: ";
        std::cin >> escolha;

        if (escolha < 0 || escolha >= (int)imagens.size()) {
            std::cout << "ID inválido!\n";
            return;
        }

        salvarVoto(usuario, imagens[escolha]);
        std::cout << "Voto registrado com sucesso!\n";
    }
    
    void verResultados() {
        carregarVotos();

        std::cout << linebreak() << "\n";

        //Parcial
        std::cout << "Resultados parciais:\n";
        auto votosParciais = resultadoParcial();
        for (const auto& [imagem, qtd] : votosParciais) {
            std::cout << imagem << ": " << qtd << " voto(s)\n";
        }

        std::cout << "\n";

        //Ganhador
        auto [imagemGanhadora, votosGanhadora] = resultadoGanhador();
        std::cout << "Imagem mais votada: " << imagemGanhadora << " com " << votosGanhadora << " voto(s).\n";


        std::cout << linebreak() << "\n";
    }

    std::string pickUserVote(const std::string& user) {
        carregarVotos();
        
        for (const VotoDB& v : votos) {
            if (v.usuario == user) {
                return v.imagem;
            }
        }
        return "";
    }


    void resetVotos() {
        carregarVotos();

        std::string imagemVencedora = resultadoGanhador().first;
        if (imagemVencedora.empty()) {
            std::cout << "Nenhum voto registrado.\n";
            return;
        }

        // Copia a imagem vencedora para o destino
        std::string origem = pasta + "/" + imagemVencedora;
        std::string destino = destinoImgVencedora;

        std::ifstream src(origem, std::ios::binary);
        std::ofstream dst(destino, std::ios::binary);
        dst << src.rdbuf();


        std::cout << "Imagem vencedora '" << imagemVencedora << "' copiada para '" << destinoImgVencedora << "'.\n";
        votos.clear();
        std::ofstream fout(arquivoVotos, std::ios::trunc);
        fout.close();


        std::cout << "Votos reiniciados com sucesso!\n";
    }

};

void fazerVotacao() {
    UserVerifier userV;
    std::string fullUser = userV.getFullUser();
    Voto voto;

    voto.listarOpcoes();
    std::cout << linebreak() << "\n";

    if (!voto.usuarioJaVotou(fullUser)) {
        if (userV.verify()) voto.votar(fullUser);
        else std::cout << "Falha na autenticação. Verifique seu usuário e senha.\n";
    }
    else {
        std::cout << "O usuário '" << fullUser << "' já votou em: '" << voto.pickUserVote(fullUser) << "'\n";
    }
}



int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");

    if (argc > 1) {
        if (std::string(argv[1]) == "/reset") {
            Voto().resetVotos();
        }
        return 0;
    }

    std::cout << "Bem-vindo!\n";
    while (true) {
        std::cout << "1 - Fazer votação\n";
        std::cout << "2 - Ver resultados\n";
        std::cout << "Tecle um numero: ";

        char opcao;
        opcao = _getch();

        system("cls");

        switch (opcao)
        {
        case '1':
            fazerVotacao();
            break;
        case '2':
            Voto().verResultados();
            break;
        default:
            std::cout << "Opção inválida!\n";
            break;
        }
        pressToContinue();
        system("cls");
    }

    return 0;
}
