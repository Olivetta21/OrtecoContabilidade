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

namespace fs = std::filesystem;

class UserVerifier {
private:
    std::string user;
    std::string domain;
    
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
        cmdBuilder << "PsExec.exe -accepteula -nobanner -u \"" << getFullUser() //o PsExec está na mesma pasta do executável
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
            return false;
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
    std::vector<std::string> imagens;
    
    struct VotoDB {
        std::string usuario;
        std::string imagem;
    };

    std::vector<VotoDB> votos;
    
    bool usuarioJaVotou(const std::vector<VotoDB>& votos, const std::string& usuario) {
        for (const auto& v : votos) {
            if (v.usuario == usuario)
                return true;
        }
        return false;
    }

    
    std::vector<std::string> listarImagens(const std::string& caminho) {
        std::vector<std::string> imagens;
        for (const auto& entry : fs::directory_iterator(caminho)) {
            if (entry.is_regular_file()) {
                auto ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".png") {
                    imagens.push_back(entry.path().filename().string());
                }
            }
        }
        std::sort(imagens.begin(), imagens.end());
        return imagens;
    }

    
    
    std::vector<VotoDB> carregarVotos(const std::string& arquivo) {
        std::vector<VotoDB> votos;
        std::ifstream fin(arquivo);
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
        return votos;
    }

public:

    Voto() {
        pasta = "imagens";
        arquivoVotos = "votos.txt";
        imagens = listarImagens(pasta);
        if (imagens.empty()) {
            std::cout << "Nenhuma imagem .png encontrada na pasta '" << pasta << "'.\n";
        }
        votos = carregarVotos(arquivoVotos);
    }

    void listarOpcoes() {
        std::cout << "Imagens disponíveis para votação:\n";
        for (size_t i = 0; i < imagens.size(); ++i) {
            std::cout << i << ": " << imagens[i] << "\n";
        }
    }

    void votar(const std::string& usuario) {
        if (usuarioJaVotou(votos, usuario)) {
            std::cout << "Você já votou! Apenas um voto por usuário é permitido.\n";
            return;
        }

        int escolha;
        std::cout << "Digite o número da imagem que deseja votar: ";
        std::cin >> escolha;

        if (escolha < 0 || escolha >= (int)imagens.size()) {
            std::cout << "ID inválido!\n";
            return;
        }

        salvarVoto(arquivoVotos, usuario, imagens[escolha]);
        std::cout << "Voto registrado com sucesso!\n";

        votos = carregarVotos(arquivoVotos); // recarrega para incluir novo voto
    }

    void mostrarResultado() {
        std::map<std::string, int> contagem;
        for (const auto& v : votos) {
            contagem[v.imagem]++;
        }

        std::cout << "\nResultado parcial:\n";
        for (const auto& [imagem, qtd] : contagem) {
            std::cout << imagem << ": " << qtd << " voto(s)\n";
        }

        if (!contagem.empty()) {
            auto max = std::max_element(contagem.begin(), contagem.end(),
                [](const auto& a, const auto& b) {
                    return a.second < b.second;
                });
            std::cout << "\nImagem mais votada: " << max->first << " com " << max->second << " voto(s).\n";
        }
    }

    void salvarVoto(const std::string& arquivo, const std::string& usuario, const std::string& imagem) {
        std::ofstream fout(arquivo, std::ios::app);
        fout << usuario << "|" << imagem << "\n";
    }
};



int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");

    UserVerifier userV;
    Voto voto;

    voto.listarOpcoes();
    
    if (userV.verify()) {

        voto.votar(userV.getFullUser());
    }
    else {
        std::cout << "Usuário ou senha inválidos.\n";
        return 1;
    }

    system("pause");
    return 0;
}