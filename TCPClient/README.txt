Dieses *.tar-Archiv beinhaltet unsere eigene Implementierung der folgenden 
beiden Programme der Lehrveranstaltung VCS simple_message_client und 
simple_message_server. Das Archiv beinhaltet folgende Files:

 -) README.txt
 -) simple_message_client.c
 -) simple_message_server.c
 -) Makefile

Compilieren mit folgenden Optionen:

 -) make ... compiliert server und client
 -) make cleanall ... löscht alle generierten Files (inkl. *.html und *.png)
 -) make clean_r ... löscht *.html und *.png Files
 -) make clean ... löscht alle aus den c-Files generierten Files

Die Aufrufparameter sind folgende:
 
 -) simple_message_client -s server -p port -u user [-i image URL] -m message [-v] [-h]
 -) simple_message_server -p port [-h]
  
 Christian Mödlhammer
 Harald Partmann