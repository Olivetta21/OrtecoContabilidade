#include "G:\Outros computadores\3050\School\Codigos\CPP\Orteco\GeradorLicencas\licenca.hpp"

int main() {
	cout << "Ano|Mes|Dia: ";
	short ano, mes, dia;
	cin >> ano >> mes >> dia;
	if (grave(ano, mes, dia)) cout << "Sucesso!";
	else cout << "Ocorreram erros!";
	system("pause");
}
