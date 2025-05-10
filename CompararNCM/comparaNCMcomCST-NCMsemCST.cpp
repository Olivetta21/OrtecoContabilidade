#include <iostream>
#include <string>
using namespace std;

int main(){
	int tamPecas = 0;
	int tamMecan = 0;
	int indResp = 0;
	string** autoPecas;
	string** resp;
	string* mecaniBoa;

	while (true){
		system("cls");
		tamPecas = 0;
		tamMecan = 0;
		indResp = 0;
		cout << "NCMs que ja estao com o CST definido:\n\tQUANTIDADE: ";
		cin >> tamPecas;
		if (tamPecas < 1 || tamPecas > 1000) return 1;

		//Cria uma matriz de "n" x 2.
		autoPecas = new string*[tamPecas];
		for (int i = 0; i < tamPecas; i++){
			autoPecas[i] = new string[2];
		}

		cout << "\tNCMs:\n";
		for (int i = 0; i < tamPecas; i++){
			cout << i+1 << " ncm/cst: ";
			cin >> autoPecas[i][0];
			cin >> autoPecas[i][1];
		}
		cout << "\n\nNCMs que ainda nao tem CST:\n\tQUANTIDADE: ";
		cin >> tamMecan;
		if (tamMecan < 1 || tamMecan > 1000) return 1;

		int re = tamMecan-1; //Serve para gravar os que nao for igual a nenhum, de tras pra frente no vetor.

		mecaniBoa = new string[tamMecan]; //Cria um vetor de tamanho "n".

		//Cria uma matriz de "n" x 2.
		resp = new string*[tamMecan];
		for (int i = 0; i < tamMecan; i++){
			resp[i] = new string[2];
		}

		cout << "\tNCMs:\n";
		for (int i = 0; i < tamMecan; i++){
			cout << i+1 << " ncm: ";
			cin >> mecaniBoa[i];
		}
		system("cls");

		for (int i = 0; i < tamMecan; i++){
			for (int j = 0; j < tamPecas; j++){
				if (mecaniBoa[i] == autoPecas[j][0]){	//Se um NCM com CST ja definido exitir na tabela do que ainda nao tem CST definido: grave-o(NCM com CST) na resposta.
					resp[indResp][0] = autoPecas[j][0];
					resp[indResp++][1] = autoPecas[j][1];
					break;
				}
				else if (j+1 >= tamPecas){	//Se nao existir, verificar se ja olhou em todos, e se sim: gravar na resposta(NCM sem CST) de tras para frente no vetor.
					resp[re][0] = mecaniBoa[i];
					resp[re--][1] = "ND";
				}
			}
		}

		//Mostrar acima os NCM que ja tem CST definido, e na parte de baixo os que nao estao com CST.
		cout << "ND = Nao Definido:\n\n";
		for (int i = 0; i < tamMecan; i++){
			cout << resp[i][0] << ";" << resp[i][1] << endl;
		}

		cout << endl; system("pause");
		//Deleta tudo pra poder recomeçar.
		for (int i = 0; i < tamPecas; i++){
			delete[] autoPecas[i];
		}
		delete[] autoPecas;
		for (int i = 0; i < tamMecan; i++){
			delete[] resp[i];
		}
		delete[] resp;
		delete[] mecaniBoa;
	}
}
