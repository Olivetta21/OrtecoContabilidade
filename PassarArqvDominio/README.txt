1- local.txt:
	* Na posição [0] fica o local das configurações do programa.
		executaveis.txt; logs.txt; checks.txt; versao.txt.
	* Na posição [1] fica o codigo da versão atual.

2- executaveis.txt:
	* Todas as posições, exceto a [size -1] é um caminho para o executável
		que será transferido.
	* Na posição [size -1] fica o nome da pasta que será criada, e também
		onde irá todos os executáveis.
		
3- logs.txt:
	* Simples log do que está acontecendo.
	
4- checks.txt:
	* É aqui que fica o meio de comunicação de quem está transferindo, com
		os que estão aguardando a transferência terminar.
	* Esse arquivo deve ter sempre somente dois modos: INUSE e NOTINUSE. 
	* Em ambos os modos, a posição [0] é referente à essa TAG.
	* Quando ele está em NOTINUSE, qualquer um pode iniciar a transferência.
	* Quando ele está em INUSE a posição [1] pode(e deve) conter o nome do
		usuario e do computador que está transferindo neste momento, e então
		o usuario atual fica verificando até que surja a tag NOTINUSE novamente.
	* OBS: Mesmo tendo várias tentativas de impedir que isso aconteça, pode ser que
		em algum momento o usuario feche o programa antes dele definir o NOTINUSE no
		arquivo, desta maneira, tudo ficará bloqueado, e ninguem mais poderá atualizar,
		pois o programa sempre achará que tem alguém atualizando. A solução é simples:
		basta ver quem "está atualizando" e executar novamente o programa (se for o
		mesmo nome e computador o programa deixará passar por essa barreira), mas se
		não for possível ver quem está atualizando, digite a tag: "you_can_do_the_task"
		sem as aspas, na posicão [0].
	
5- versao.txt:
	* Esse arquivo será utilizado somente pelo atualizador, na abertura, onde ele irá pegar 
		o codigo da versão localizado em [size -1] e comparar com o codigo da versão 
		localizado em local.txt[1], se forem diferentes, ele ira puxar os arquivos dos locais
		que estarão definidos entre a posição inicial e [size -1].
	* Lembre-se: ao copiar o novo arquivo "local.txt" certifique-se que o codigo da versao
		dentro dele, seja a mesma que o que está dentro de "versao.txt", se não, obviamente
		ele ira "atualizar" sempre que o programa for iniciado.