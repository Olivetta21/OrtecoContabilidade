# Arquivos
## No local do executável
* **local.txt:**
	* Na posição [0] fica o local das configurações do programa.
		```
		ex:
		\\remoto\atualizador\
		
		lá: executaveis.txt; logs.txt;
		```
## No local remoto
* **executaveis.txt:**
	* Todas as posições, exceto a [size -1] é um caminho para o executável que será transferido.
		```
		ex:
		->  [0] \\remoto\atualizador\arquivo1.exe
		->  [1] \\remoto\atualizador\arquivo2.exe
		    [2] Versao100ABC
		```
	* Na posição [size -1] fica o nome da pasta que será criada, e também onde irá todos os executáveis.
		```
		ex:
		    [0] \\remoto\atualizador\arquivo1.exe
		    [1] \\remoto\atualizador\arquivo2.exe
		->  [2] Versao100ABC
		```
* **logs.txt:**
	* Simples log do que está acontecendo.
## No local de destino (criado pelo programa)
* **Local fixo:** `C:\Program Files\AtualizacoesDominio\`
	* Pasta de instalação local
		```
		ex:
		C:\Program Files\AtualizacoesDominio\Versao100ABCxyz\
		```
	* Dentro da pasta da versão:
		* Todos os arquivos executáveis transferidos.
		* **jaAtualizado.txt:** Lista dos instaladores já executados.
			```
			ex:
			arquivo1.exe
			arquivo2.exe
			```
# Configurações do Sistema
* **Permissões necessárias:**
	* Executar como administrador para criar pastas em `C:\Program Files\`
	* Permissão de leitura nos caminhos remotos especificados
