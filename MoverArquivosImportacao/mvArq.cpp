#include <iostream>
#include <filesystem>
#include <thread>
#include <regex>
#include <chrono>

namespace fs = std::filesystem;

// Função principal do programa
void processFiles(const fs::path& sourceDir, const fs::path& destDir) {
    std::regex pattern(R"((^[\w\s]+?)\s+(\d+(?:\s\d+)*)\.(.+)$)");  // Regex para capturar tipo, código e extensão
    std::smatch match;

    while (true) {
        std::cout << "Varrendo a pasta de origem\n";
        for (const auto& entry : fs::directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::cout << "\tVerificando: " << filename << "\n";

                // Verifica se o nome do arquivo corresponde ao padrão tipo + espaço + código + extensão
                if (std::regex_match(filename, match, pattern) && match.size() == 4) {
                    std::cout << "\t\tArquivo no padrão encontrado\n";
                    std::string tipo = match[1];
                    std::string codigo = match[2];
                    std::string extensao = match[3];

                    fs::path folderPath = destDir / codigo;
                    fs::create_directories(folderPath);  // Cria a pasta no destino, caso não exista

                    fs::path destFilePath = folderPath / (tipo + "." + extensao);

                    // Verifica se o arquivo já existe no destino e adiciona "_" ao nome se necessário
                    while (fs::exists(destFilePath)) {
                        destFilePath = folderPath / ("_" + destFilePath.filename().string());
                    }

                    // Tenta mover e renomear o arquivo
                    try {
                        fs::rename(entry.path(), destFilePath);
                        std::cout << "\t\tArquivo movido para: " << destFilePath << std::endl;
                    } catch (const fs::filesystem_error& e) {
                        // Caso o arquivo esteja em uso ou outro erro ocorra, ignora e tenta na próxima varredura
                        std::cerr << "\t\tErro ao mover o arquivo: " << e.what() << std::endl;
                    }
                }
            }
        }

        // Intervalo de 5 segundos antes da próxima varredura
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {
    fs::path sourceDir = "//192.168.0.200/e/IVANLUIZ/!mportações/2024/10/DownloadAqui"; // Substitua pelo caminho da origem
    fs::path destDir = "//192.168.0.200/e/IVANLUIZ/!mportações/2024/10/";  // Substitua pelo caminho do destino
    processFiles(sourceDir, destDir);
    return 0;
}
