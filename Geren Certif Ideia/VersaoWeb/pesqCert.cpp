int pesquisarCertificado(int tipodaconsulta) {	//1=nome 2=codi 3=local
	//Montando o comando usando o conteudo de _pesqs, nessa funcao o _pesqs sempre sera um vetor de nome e/ou local.
	//string command = "SELECT UPPER(nome), UPPER(loca), codi, venc, penultimovisto, ultimovisto FROM certys WHERE nome LIKE '";
	string command = "select upper(c.nome), upper(c.loca), c.codi, c.venc, i.autor, ii.autor "
		"from certys c left join visualizacao i on i.id = (select id from visualizacao where id_cert = c.codi order by id desc) "
		"left join visualizacao ii on ii.id = ("
		"select id from visualizacao where id_cert = c.codi and id < (select id from visualizacao where id_cert = c.codi order by id desc) order by id desc"
		") where";

	bool prim = 1;

	switch (tipodaconsulta){
	case 1: {	//Consulta por nome.
		command += " c.nome LIKE '";
		for (string& p : _pesqs) {
			string e = "%";

			if (p[p.size() - 1] == '=') {
				e = "";
				p.pop_back();
			}

			if (prim) {
				command += e + p + e + "'";
				prim = 0;
			}
			else {
				command += " OR c.nome LIKE '" + e + p + e + "'";
			}
		}
		command += " ORDER BY UPPER(c.nome);";
		break;
	}
	case 2: {	//Consulta por codi.
		//Nessa funcao o _pesqs sempre sera um vetor de codigos.
		command += " c.codi IN (";
		for (const string& p : _pesqs) {
			int num = charToNum(p);

			if (prim) {
				command += to_string(num);
				prim = 0;
			}
			else {
				command += ", " + to_string(num);
			}
		}
		command += ") ORDER BY UPPER(c.nome);";
		break;
	}
	case 3: {
		command += " c.loca LIKE '";
		for (string& p : _pesqs) {
			string e = "%";

			if (p[p.size() - 1] == '=') {
				e = "";
				p.pop_back();
			}

			if (prim) {
				command += e + p + e + "'";
				prim = 0;
			}
			else {
				command += " OR c.loca LIKE '" + e + p + e + "'";
			}
		}
		command += " ORDER BY UPPER(c.nome);";
		break;
	}
	default:
		break;
	}

#ifdef SYSLOG
	cout << AzulEsc << command << "\n";
	system("pause");
#endif

	if (prim) {
		cout << Vermelho << "Nao houve parametros para pesquisa!\n";
		return 1;
	}
	return prepareAndSearch(command);
}