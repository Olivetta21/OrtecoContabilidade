create table visualizacao (id INTEGER PRIMARY KEY AUTOINCREMENT, id_cert INTEGER NOT NULL, autor TEXT DEFAULT 'sem autor', data DATETIME DEFAULT '0000-00-00 00:00:00', FOREIGN KEY (id_cert) REFERENCES certys(codi));
create table alterlocal (id INTEGER PRIMARY KEY AUTOINCREMENT, id_cert INTEGER NOT NULL, autor TEXT DEFAULT 'sem autor', newplace TEXT DEFAULT '*', data DATETIME DEFAULT '0000-00-00 00:00:00', FOREIGN KEY (id_cert) REFERENCES certys(codi));



//string command = "SELECT UPPER(nome), UPPER(loca), codi, venc, penultimovisto, ultimovisto FROM certys WHERE nome LIKE '";

select upper(c.nome), upper(c.loca), c.codi, c.venc, i.autor as ultimo, ii.autor as penultimo
from certys c left join visualizacao i on i.id = (
	select id from visualizacao where id_cert = c.codi order by id desc
)
left join visualizacao ii on ii.id = (
	select id from visualizacao where id_cert = c.codi and id < (
		select id from visualizacao where id_cert = c.codi order by id desc
	)	order by id desc
) where c.codi in (252,253,156,200);


string command = "select upper(c.nome), upper(c.loca), c.codi, c.venc, i.autor, ii.autor "
"from certys c left join informacao i on i.id = (select id from informacao where id_cert = c.codi order by id desc) "
"left join informacao ii on ii.id = ("
"select id from informacao where id_cert = c.codi and id < (select id from informacao where id_cert = c.codi order by id desc) order by id desc"
") where nome LIKE '";

