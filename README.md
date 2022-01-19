Purtroppo non ho utilizzato percorsi relativi per il progetto, comunque i percorsi assoluti sono pochi e facilmente modificabili in quanto costanti nel file funzioni.h.

Per compilare è possibile usare il comando make per server e client, e dopo un'esecuzione completa si può tornare nella situazione iniziale tramite make clean.

I file funzioni.c e funzioni.h sono condivisi da entrambe le parti, per comodità le ho messe nella cartella superiore rispetto a lotto_server e lotto_client. 

Nel progetto faccio uso di due cartelle ausiliari (consuntivi e file_registro).

Ho cancellato molte printf di debug nel file server, se l'output risulta un po' spoglio è per questo (ma sono velocemente de-commentabili).

Buone giocate!

