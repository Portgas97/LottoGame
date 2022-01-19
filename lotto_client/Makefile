lotto_client : lotto_client.o ../funzioni.o
	gcc lotto_client.o ../funzioni.o -o lotto_client
lotto_client.o : lotto_client.c ../funzioni.h
	gcc -c -Wall lotto_client.c -o lotto_client.o
../funzioni.o : ../funzioni.c ../funzioni.h
	gcc -c -Wall ../funzioni.c -o ../funzioni.o
clean:
	rm -f lotto_client
	rm -f ../*.o
	rm -f *.o
	echo "pulizia completata"
