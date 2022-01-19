lotto_server : lotto_server.o ../funzioni.o
	gcc lotto_server.o ../funzioni.o -o lotto_server
	mkdir consuntivi
	mkdir file_registro
lotto_server.o : lotto_server.c ../funzioni.h
	gcc -c -Wall lotto_server.c -o lotto_server.o
../funzioni.o : ../funzioni.c ../funzioni.h
	gcc -c -Wall ../funzioni.c -o ../funzioni.o

clean:
	rm -f lotto_server
	rm -f *.o
	rm -f buffer.txt
	rm -f buffer2.txt
	rm -f buffer3.txt
	rm -f lista_utenti.txt
	rm -f IP_bannati.txt
	rm -f file_estrazioni.txt
	rm -f consuntivi/*
	rm -f file_registro/*
	rm -f ../*.o
	rmdir consuntivi
	rmdir file_registro
	echo "pulizia completata"
