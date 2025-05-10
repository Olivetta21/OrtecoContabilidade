#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>
#include <windows.h>

namespace fs = std::filesystem;

struct ArqsSeries {
    std::string localdoarquivo;
    std::string serierecuperada;
    std::string relative_path;  // Caminho relativo da subpasta
};

std::string getSerieFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }
    std::string line;
    std::regex serie_regex("<serie>(.*?)</serie>");
    std::smatch match;
    while (std::getline(file, line)) {
        if (std::regex_search(line, match, serie_regex)) {
            return match[1].str();
        }
    }
    return "";
}

void createDirectoryIfNotExists(const std::string& dir) {
    try {
        if (!fs::exists(dir)) {
            fs::create_directory(dir);
        }
    } catch (const fs::filesystem_error& e) {
        std::cout << "Whoops, arquivo " << dir << " nao criado!\n";
    }
}

void processFiles(bool preserve_structure) {
    std::string process_dir = "processados";
    std::cout << "Nome da pasta: ";
    std::cin.clear();
    std::getline(std::cin, process_dir);

    if (process_dir.size() > 2) createDirectoryIfNotExists(process_dir);
    else return;

    std::string arqs_dir = "arqs";
    std::string series_recuperadas_dir = process_dir + "/SeriesRecuperadas";
    std::string verificados_dir = process_dir + "/verificados";

    createDirectoryIfNotExists(series_recuperadas_dir);
    createDirectoryIfNotExists(verificados_dir);

    std::vector<ArqsSeries> arqs_series;
    std::unordered_set<std::string> found_series;

    for (const auto& entry : fs::recursive_directory_iterator(arqs_dir)) {
        if (entry.is_regular_file()) {
            std::string filepath = entry.path().string();
            std::string serie = getSerieFromFile(filepath);
            std::string relative_path = fs::relative(filepath, arqs_dir).parent_path().string();  // Caminho relativo

            if (!serie.empty() && found_series.find(serie) == found_series.end()) {
                arqs_series.push_back({ filepath, serie, relative_path });
                found_series.insert(serie);
            }
        }
    }

    // Move files with found series
    for (const auto& arqs : arqs_series) {
        std::string target_dir = series_recuperadas_dir;
        if (preserve_structure) {
            target_dir += "/" + arqs.relative_path;
        }
        createDirectoryIfNotExists(target_dir);
        std::string new_path = target_dir + "/serie-" + arqs.serierecuperada + ".xml";
        try {
            fs::rename(arqs.localdoarquivo, new_path);
            std::cout << "O arquivo \"" << arqs.localdoarquivo << "\" foi para: \"" << new_path << "\".\n";
        } catch (const fs::filesystem_error& e) {
            std::cout << "Whoops, arquivo nao movido!\n";
        }
    }

    // Move remaining files
    for (const auto& entry : fs::recursive_directory_iterator(arqs_dir)) {
        if (entry.is_regular_file()) {
            std::string filepath = entry.path().string();
            std::string filename = entry.path().filename().string();
            std::string relative_path = fs::relative(filepath, arqs_dir).parent_path().string();
            std::string target_dir = verificados_dir;
            if (preserve_structure) {
                target_dir += "/" + relative_path;
            }
            createDirectoryIfNotExists(target_dir);
            std::string new_path = target_dir + "/" + filename;
            try {
                fs::rename(filepath, new_path);
                std::cout << ".";
            } catch (const fs::filesystem_error& e) {
                std::cout << "X";
            }
        }
    }
}

int main() {
    bool running = true;
    while (running) {
        std::cout << "Iniciando verificação de arquivos..." << std::endl;

        char option;
        std::cout << "Deseja mover arquivos mantendo a estrutura de subpastas? (s/n): ";
        std::cin >> option;
        std::cin.ignore();
        bool preserve_structure = (option == 's' || option == 'S');

        processFiles(preserve_structure);

        std::cout << "\n\n\nVerificação concluída. Refazer? (s/n): ";
        std::cin >> option;
        std::cin.ignore();
        system("cls");
        if (option == 'n' || option == 'N') {
            running = false;
        }
    }
    return 0;
}
