#include <iostream>
#include <filesystem>
#include <Windows.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]){
	if (argc > 1 && fs::is_directory(argv[1])) std::cout << argv[1] << std::endl; 
	else { std::cout << "Preciso de uma pasta para funcionar!\n"; system("pause"); return -1; }

	while (true){
		std::cout << "Testando...\n";
		int qnts = 0;
		for (const auto& arqs : fs::directory_iterator(argv[1])) {
			if (qnts > 1) break;
			qnts++;
		}
		if (qnts > 1) PlaySound(TEXT("SystemDefault"), NULL, SND_ALIAS);
		else { system("cls"); std::cout << "OK\n"; }
		Sleep(2000);
	}
}
