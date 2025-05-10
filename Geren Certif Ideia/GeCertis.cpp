#include "GeCertis.hpp"
#define vTam 2

void selecionar(Certificado*&, const int&, vector<string>&, vector<Certificado*>&);
void ler(Certificado* &, int&);
bool gravar(Certificado*&, const int&);
void pesquisar(Certificado*&, const int&, const vector<string>&, vector<Certificado*>&);
void pesqParaVector(vector<string>&, string&);
bool alteraLocal(Certificado*&, const int&, const string&, vector<Certificado*>&);
bool confirmacao(const string);
void cinWcleanBuff(string&);

void showCerts(vector<Certificado*>& certs) {
	int t = certs.size();
	cout << Azul << "+------------------------------------------\n";
	for (int i = 0; i < t; i++) {
		cout << *certs[i] << endl << i+1 << "------------------------------------------\n";
	}
}

int main() {
	cout << Branco << "GeCert\n";

	{
		string password;
		char carac;
		do {
			cout << "Senha: " << Cor(255, 0, 0);
			for (int i = 0; i < password.size(); i++) cout << '*';
			carac = _getch();
			system("cls");
			if (carac == 8) { if (password.size() > 0) password.resize(password.size() - 1); }
			else password += carac;
			cout << Branco;
		} while (carac != '\n' && carac != '\0' && carac != '\r');
		password.resize(password.size() - 1);
		if (password != "@leitura&*") return -444;
	}

	if (!pickDir()) {
		cout << endl;
		system("pause");
		return -5;
	}
	
	Certificado* certificados = nullptr;
	int tamanho;
	
	short opt = 0;
	do {
		__comeco:
		ler(certificados, tamanho);

		string pesquisa = "";
		vector<string> vecOfSearch;
		vector<Certificado*> certsPesqs;
		system("cls");
		cout << Azul << "Realize a pesquisa, ou selecione os codigos diretamente usando: '+' e os codigos..."
			<< "\n - Utilize " << Amarelo << ';' << Azul << " para separar os codigos."
			<< "\n - Utilize " << Amarelo << '.' << Azul << " para definir uma pesquisa exata."
			<< "\n\n - Pesquisar por : ";
		cinWcleanBuff(pesquisa);
		if (pesquisa == "#todos#") {
			int onlySitGood = 0;
			for (int i = 0; i < tamanho; i++) if (certificados[i].getSit()) onlySitGood++;
			certsPesqs.resize(onlySitGood);
			for (int i = 0, j = 0; j < onlySitGood; i++) {
				if (certificados[i].getSit()) certsPesqs[j++] = &certificados[i];
			}
			goto __skipInputButShowThem;
		}
		else if (pesquisa == ".") goto __comeco;
		pesqParaVector(vecOfSearch, pesquisa);


		if (pesquisa[0] != '+') {
			pesquisar(certificados, tamanho, vecOfSearch, certsPesqs);
			cout << "Resultado da pesquisa:\n";
			if (certsPesqs.size() < 1) cout << "Nenhum certificado encontrado!\n";
			else showCerts(certsPesqs);
			
			if (!confirmacao("Alterar local dos certificados? (s/n): ") || certsPesqs.size() < 1) goto __comeco;
			else {
				cout << "\nPara selecionar TODOS da pesquisa aperte " << Verde << 'S' << Azul << "\nOU\nAperte "<< Verde << 'N' << Azul << " para digitar os codigos.";
				if (!confirmacao("\n -> ")) {
					cout << "Codigos:\n -> ";
					cinWcleanBuff(pesquisa);
					pesqParaVector(vecOfSearch, pesquisa);
				}
				else {
					goto __skipInputOfCodigos;
				}
			}
		}

		certsPesqs.clear();
		certsPesqs.resize(0);

		__skipInputButShowThem:
		selecionar(certificados, tamanho, vecOfSearch, certsPesqs);
		system("cls");
		cout << "Certificados selecionados:\n";
		if (certsPesqs.size() < 1) {
			cout << "\tNenhum certificado selecionado!\n";
			goto __fim;
		}
		showCerts(certsPesqs);
		
		
		if (confirmacao("Confirma selecao? (s/n): ")) {
			__skipInputOfCodigos:
			string novoLocal;
			__redefineLocal:
			cout << "Escreva para onde o certificado ira:\n -> ";
			cinWcleanBuff(novoLocal);
			if (alteraLocal(certificados, tamanho, novoLocal, certsPesqs)) goto __redefineLocal;
		}
		else goto __comeco;

		__fim:
		confirmacao("\n\nPressione qualquer tecla para voltar...");
	} while (true);

	cout << "\n\nsaiu corretamente";
}

