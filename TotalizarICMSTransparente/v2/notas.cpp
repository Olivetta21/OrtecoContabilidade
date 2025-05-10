#include <iostream>
#include <locale.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <gmp.h>
#include "licenca.hpp"


using namespace std;
namespace fs = filesystem;

class NFent{
public:
	string tot, base, icms;
public:

	string chave, ied, iee, rzSocial, inscrFed, nNf, emiss, UF, sit;

	void setValues(string _tot, string _base, string _icms){
		vector<string> _values{_tot, _base, _icms};

		for (auto &v : _values){
			string temp;
			for (auto ch : v){
				if (ch == ',') temp.push_back('.');
				else if (ch != '.') temp.push_back(ch);
			}
			v = temp;
		}

		tot =  _values[0];
		base = _values[1];
		icms = _values[2];
	}

	/*friend ostream &operator << (ostream &os, const NFent &nf){
        os << "T:" << nf.tot << " B:" << nf.base << " I:" << nf.icms;
        return os;
	}*/

};



// Função para dividir uma string em um vetor de strings usando '\n' como delimitador
vector<string> splitIntoColumns(const string& input) {
    std::vector<std::string> columns;
    std::stringstream ss(input);
    std::string line;



    while (std::getline(ss, line, '\t')) {
        columns.push_back(line);
    }

    // Verifica se o vetor possui exatamente 12 colunas
    if (columns.size() != 12 && columns.size() != 11) {
        std::cerr << "A string não possui colunas certeiras." << std::endl;
        // Caso necessário, você pode lidar com esse erro de acordo com a sua necessidade
    }

    return columns;
}



string calc(string n1, string n2){
    mpf_set_default_prec(256); // Define a precisão (em bits) para ponto flutuante
    mpf_t num1, num2, result;

    // Inicializa as variáveis
    mpf_init(num1);
    mpf_init(num2);
    mpf_init(result);

    // Define os números de ponto flutuante
    mpf_set_str(num1, n1.c_str(), 10); // Base 10
    mpf_set_str(num2, n2.c_str(), 10); // Base 10

    // Soma os números
    mpf_add(result, num1, num2);

    // Imprime o resultado
    //gmp_printf("Soma: %.50Ff\n", result); // Imprime com 50 casas decimais

    // Libera a memória
    mpf_clear(num1);
    mpf_clear(num2);
    mpf_clear(result);

    char buffer[1024]; // Buffer suficientemente grande para armazenar o resultado
    gmp_sprintf(buffer, "%.Ff", result); // Converte o número para string no buffer

    return string(buffer);

}


void trocaPonto(string &str){
    for (auto &ch : str) if (ch == '.') ch = ',';
}

string licdic = "//192.168.0.200/e/IVANLUIZ/MeusProgramas/Lic/sysdb.curtime";
int main(int argc, char *argv[]) {
	if (valida(licdic)) {
		cout << "error: unrecognized database!\nPress ENTER to exit.";
		string pesq;
		getline(cin, pesq);
		if (pesq == "/renewlicence") {
			genLicenca(); return 0;
		}
		return -99;
	}

    setlocale(LC_ALL, "Portuguese_Brazil.1252");
    setlocale(LC_NUMERIC, "C");

    cout << "Versao 1\n";

    string arquivoLeitura = "transparente.txt";
    if (argc > 1 && argv[1]) arquivoLeitura = argv[1];
    cout << "Arquivo: " << arquivoLeitura << "\n\n";
    fs::path arqResultado = arquivoLeitura;

    string dataIN, dataOUT, ES;
    int qntNFs = 0;


    vector<string> LinhasArquivo;
    ifstream transparente(arquivoLeitura);  //Abrindo arquivo colado do transparente.

    if (!transparente.is_open()) {
        cerr << "Erro ao abrir o arquivo do transparente" << endl;
        system("pause");
        return 1;
    }

    string linhaAtual;
    while (getline(transparente, linhaAtual)) { //Colocando todas as linhas na memoria.
        for (auto &ch : linhaAtual){
            if ((ch & 0x80) != 0) { // Caractere não ASCII
                ch = '?';
            }
        }

        LinhasArquivo.push_back(linhaAtual);
        //cout << linhaAtual << "\n";
    }


    dataIN = LinhasArquivo[17];
    dataOUT = LinhasArquivo[19];
    if (LinhasArquivo[31] == "I.E. Emit.\t ") ES = "entrada";
    else if (LinhasArquivo[31] == "Raz??o Social Dest.\t ") ES = "saida";
    {
        int pos = LinhasArquivo[26].find(" de ");
        if (pos != string::npos){
            //cout << LinhasArquivo[26] << "\n";
            qntNFs = stod(LinhasArquivo[26].substr(pos+4));
        }
    }

    cout << "DataIN." << dataIN << ".\nDataOut." << dataOUT << ".\nES." << ES << ".\nQnts." << qntNFs << ".\n.";

    vector<NFent> notasEntradas;
    if (ES == "entrada" || 1){

        for (auto &str : LinhasArquivo){
            if (str.size() > 100){
                vector<string> colunas = splitIntoColumns(str);
                if (colunas.size() == 12) {
                    //chave, ied, iee, rzSocial, inscrFed, nNf, emiss, UF, tot, base, icms, sit;

                    NFent nf;
                    nf.chave    = colunas[0];
                    nf.ied      = colunas[1];
                    nf.iee      = colunas[2];
                    nf.rzSocial = colunas[3];
                    nf.inscrFed = colunas[4];
                    nf.nNf      = colunas[5];
                    nf.emiss    = colunas[6];
                    nf.UF       = colunas[7];
                    //nf.tot  = colunas[8];
                    //nf.base = colunas[9];
                    //nf.icms = colunas[10];
                    nf.sit      = colunas[11];
                    nf.setValues(colunas[8], colunas[9], colunas[10]);

                    notasEntradas.push_back(nf);
                }
                else if (colunas.size() == 11) {
                    //chave, ied, iee, rzSocial, inscrFed, nNf, emiss, UF, tot, base, icms, sit;

                    NFent nf;
                    nf.chave    = colunas[0];
                    nf.ied      = colunas[1];
                    nf.iee      = " ";
                    nf.rzSocial = colunas[2];
                    nf.inscrFed = colunas[3];
                    nf.nNf      = colunas[4];
                    nf.emiss    = colunas[5];
                    nf.UF       = colunas[6];
                    //nf.tot  = colunas[8];
                    //nf.base = colunas[9];
                    //nf.icms = colunas[10];
                    nf.sit      = colunas[10];
                    nf.setValues(colunas[7], colunas[8], colunas[9]);

                    notasEntradas.push_back(nf);
                }
            }
        }

    }


    cout << "\nTotal de notas: informada-" << qntNFs << " localizada-" << notasEntradas.size() << "\n-----------------------\n";


    int tentativas = 20;
    init:
    tentativas--;

    ofstream file(arqResultado.filename().string() + ".csv");
    if (file.is_open()) {
        string t("0"),b("0"),i("0"),ct("0"),cb("0"),ci("0");
        for (auto &nf : notasEntradas){
            if (nf.sit == "A"){
                t = calc(t, nf.tot);
                b = calc(b, nf.base);
                i = calc(i, nf.icms);
            }
            else {
                ct = calc(ct, nf.tot);
                cb = calc(cb, nf.base);
                ci = calc(ci, nf.icms);
            }
            //file << "=\"" << nf.chave << "\";\"" << nf.tot << "\";\"" << nf.base << "\";\"" << nf.icms << "\";\n";
            string temp = ("=\"" + nf.chave + "\";=\"" + nf.ied + "\";=\"" + nf.iee + "\";=\"" + nf.rzSocial + "\";=\"" + nf.nNf + "\";" + nf.emiss + ";\"" + nf.UF + "\";" + nf.tot + ";" + nf.base + ";" + nf.icms + ";=\"" + nf.sit + "\";\n");
            trocaPonto(temp);
            file << temp;
            //file << "=\"" << nf.chave << "\";" << nf.tot << ";" << nf.base << ";" << nf.icms << ";\n";
        }
        string temp = ("\"Autorizadas Total:\";;;;;;;\"" + t + "\";\"" + b + "\";\"" + i + "\";\n"
                       "\"Canceladas/Denegadas Total:\";;;;;;;\"" + ct + "\";\"" + cb + "\";\"" + ci + "\";\n"
                       );
        trocaPonto(temp);
        file << temp;
        //file << "\"Total:\";" << "\"" << t << "\";\"" << b << "\";\"" << i << "\";\n";
        file.close();
    } else {
        cerr << "Erro ao criar o arquivo" << tentativas << endl;
        if (tentativas){
            Sleep(250);
            goto init;
        }
    }


    /*for (auto &nf : notasEntradas){
        cout << b << " + " << nf.base << " = ";
        t = calc(t, nf.tot);
        b = calc(b, nf.base);
        i = calc(i, nf.icms);
        cout << b << "\n";
    }
    cout << "Total\n" << "T:" << t << " B:" << b << " I:" << i;*/

    cout << "\n----------\n";
    system("pause");

    return 0;
}
