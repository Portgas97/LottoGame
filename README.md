Purtroppo non ho utilizzato percorsi relativi per il progetto perchè mi sono accorto all'ultimo di aver lavorato con quelli assoluti (non ho modificato il codice perchè ormai molte parti ragionavano sulla manipolazione di questi percorsi, ad esempio per accedere al file di registro di un utente).

Quindi possibilmente il pacchetto andrebbe posizionato in:
/home/studenti/NetBeansProjects/

Comunque i percorsi assoluti sono pochi e facilmente modificabili in quanto costanti nel file funzioni.h.

Per compilare è possibile usare il comando make per entrambe le parti, server e client, e dopo un'esecuzione completa si può tornare nella situazione iniziale tramite make clean.

I file funzioni.c e funzioni.h sono condivisi da entrambe le parti, per comodità le ho messe nella cartella superiore rispetto a lotto_server e lotto_client. Ovviamente in un'implementazione distribuita ognuno avrebbe i propri file.

Nel progetto faccio uso di due cartelle ausiliari (consuntivi e file_registro).

Ho cancellato molte printf di debug nel file server, se l'output risulta un po' spoglio è per questo (ma sono velocemente de-commentabili).

Spero che vada tutto bene.

Buone giocate


- Francesco Venturini

