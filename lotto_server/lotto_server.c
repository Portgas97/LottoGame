#include "/home/studenti/NetBeansProjects/funzioni.h"


int main(int argc, char* argv[]){

    // secondi che intercorrono fra le estrazioni
    int periodo;

    // per l'approccio multiprocesso
    pid_t pid, processo_estrazioni;

    // per i socket
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int socket_listener;
    socklen_t address_len;
    // int error_saved;

    // varie
    int i, ret, tmp;

    // comando letto
    char *comando;

    // file
    FILE * fd;
    int size;
    struct stat info;
    char *stringa;
    char *buffer;

    /*
    printf("LOG_DEBUG: ci sono %d argomenti:\n", argc);
    printf("LOG_DEBUG:\n");
    for(i = 0; i < argc; ++i){
        printf("%s\n", argv[i]);
    }
    */

    // # # # # # # # # # # VALIDAZIONE DEI PARAMETRI IN INGRESSO # # # # # # # # # #
    if(argc == 2){ // solo porta

        if(strcmp(argv[1], "4242") == 0){
            printf("Porta inserita correttamente\n");
            periodo = 5 * 60; // default
        }
        else{
            printf("La porta: %s inserita non è valida\n", argv[1]);
            exit(1);
        }
    }
    else if(argc == 3){ // porta e periodo

        if(strcmp(argv[1],"4242") == 0){
            if(atoi(argv[2]) > 0 && atoi(argv[2]) < 60){
                printf("LOG_DEBUG: porta %s e periodo %s inseriti correttamente\n", argv[1], argv[2]);
                printf("porta e periodo corretti\n");
                periodo = atoi(argv[2]) * 60;
            }
            else{
                printf("Valore del periodo %s non permesso\n", argv[2]);
                exit(1);
            }
        }

        else {
            printf("Porta %s non consentita\n", argv[1]);
            exit(1);
        }

    }
    else{
        printf("I parametri inseriti non sono permessi. Inserire numero di porta e periodo\n");
        exit(1);
    }



    printf("LOG_DEBUG: creazione della socket in lotto_server.c\n");

    // protocollo IPv4, socket TCP di tipo non bloccante
    // resituisce un descrittore di file
    socket_listener = socket(AF_INET, SOCK_STREAM, 0); // DA AGGIUNGERE non bloccante

    if(socket_listener < 0){
        printf("La primitiva socket() ha fallito, termino con un errore\n");
        perror("Error in socket(): \n");
        exit(1);
    }


    /* Ripasso:
     *
     *   struct sockaddr_in {
     *      sa_family_t     sin_family;
     *      in_port_t       sin_port;
     *      struct in_addr  sin_addr;
     *  };
     *
     *   struct in_addr {
     *      uint32_t    s_addr;
     *  };
     *
     */

    printf("LOG_DEBUG: inizializzazione dei campi per server_address\n");

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // come richiesto dalle specifiche ascolto su tutte le interfacce
    server_address.sin_port = htons(atoi(argv[1])); // espresso in network order

    printf("LOG_DEBUG: esecuzione della primitiva bind()\n");

    // Assegno l'indirizzo appena costruito al socket
    ret = bind(socket_listener, (struct sockaddr*)&server_address, sizeof(server_address));

    if(ret < 0){
        perror("Error in bind(): \n");
        exit(1);
    }

    printf("LOG_DEBUG: chiamata a listen() \n");

    // specifico che è un socket passivo, usato per ricevere richieste
    // riguarda cosa è 10/50 e setta come costante
    ret = listen(socket_listener, 50);

    if(ret < 0){
        perror("Error in listen(): \n");
        exit(1);
    }

    // creo il file per gli IP bannati
    fd = fopen(IP_BANNATI, "w");
    if(fd == NULL){
        perror("Errore nella creazione del file IP_bannati.txt :\n");
        exit(1);
    }

    printf("LOG_DEBUG: creato il file IP_bannati.txt\n");
    fclose(fd);

    fd = fopen(FILE_ESTRAZIONI, "w");
    if(fd == NULL){
        perror("Errore nella creazione del file_estrazioni\n");
        exit(1);
    }
    printf("LOG_DEBUG: creato il file file_estrazioni.txt\n");
    fclose(fd);

    processo_estrazioni = fork();

    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PROCESSO PER LE ESTRAZIONI ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    if(processo_estrazioni == 0){
        while(1){
            time_t istante_estrazione;
            struct estrazione *punt_estrazione;
            int array_estratti[5];
            int numero_casuale;
            int i, j, index;
            FILE *file_utenti;

            // printf("LOG_DEBUG: processo_estrazioni entra nel ciclo infinito\n");

            printf("ATTESA DELLA PROSSIMA ESTRAZIONE\n");
            sleep(periodo);
            printf("\n");
            printf("######  ####  ##### #####    ##   ###### #  ####  #    # ###### \n");
            printf("#      #        #   #    #  #  #      #  # #    # ##   # #      \n");
            printf("#####   ####    #   #    # #    #    #   # #    # # #  # #####  \n");
            printf("#           #   #   #####  ######   #    # #    # #  # # #      \n");
            printf("#      #    #   #   #   #  #    #  #     # #    # #   ## #      \n");
            printf("######  ####    #   #    # #    # ###### #  ####  #    # ###### \n");
            printf("\n");

            time(&istante_estrazione);
            punt_estrazione = (struct estrazione*) calloc(1, sizeof(struct estrazione));
            punt_estrazione->istante_estrazione = istante_estrazione;

            srand(time(NULL));
            for(index = 0; index < 11; index++){
                int ricalcola = 0;
                i = 0;
                while(1){
                    numero_casuale = 1 + rand() % 90;
                    for(j = 0; j < i; j++){
                        if(numero_casuale == array_estratti[j]){
                            ricalcola = 1;
                            break;
                        }
                    }

                    if(ricalcola){
                        ricalcola = 0;
                        continue;
                    }
                    // controllo che il numero non sia già uscito in questa ruota
                    array_estratti[i] = numero_casuale;
                    // salvo nell'array di tutte le estrazioni
                    punt_estrazione->numeri_estratti[index * 5 + i] = numero_casuale;

                    i++;
                    if(i == 5)
                        break;
                }
            }

            // su stdout
            stampa_estrazione(punt_estrazione);
            // su file_estrazioni.txt
            scrivi_estrazione(punt_estrazione);

            // scorro il file lista_utenti.txt
            // per ogni utente apro il file di registro
            // per ogni schedina attiva calcolo le eventuali vincite e le scrivo nel file buffer
            // Poi scrivo queste vincite in coda al file registro

            // apro il file registro in lettura, tengo la somma delle varie vincite
            // le scrivo in fondo al file sostituendo le precedenti somme
            file_utenti = fopen(FILE_UTENTI, "r");
            if(file_utenti == NULL){
                perror("Errore nell'apertura del file lista_utenti.txt\n");
                exit(1);
            }

            while(1){
                FILE *file_registro;
                int success;
                char *stringa;

                stringa = calloc(BUFFER_LENGHT, sizeof(char));
                if(!stringa){
                    printf("Memoria esaurita\n");
                    exit(1);
                }
                success = fscanf(file_utenti, "%s", stringa);

                if(success == EOF){
                    printf("Operazione di lettura file lista_utenti.txt terminata\n");
                    free(stringa);
                    break;
                } else if(!success){
                    free(stringa);
                    printf("Errore nell'aggiornamento delle estrazioni\n");
                    break;
                } else {
                    char *path;
                    int schedine_attive_presenti = 0;
                    path = calloc(PATH_LEN, sizeof(char));
                    percorso_file_registro(stringa, path);

                    file_registro = fopen(path, "r");
                    if(file_registro == NULL){
                        perror("Errore nell'apertura del file registro\n");
                        exit(1);
                    }

                    schedine_attive_presenti = analizza_schedine_attive(punt_estrazione, file_registro, stringa);

                    if(schedine_attive_presenti != 0){
                        // modifico il file di registro tramite buffer2.txt
                        // printf("\nLOG_DEBUG: chiamata a disattiva_schedine()\n");
                        fclose(file_registro);
                        disattiva_schedine(path);
                    }

                    free(path);
                }

                free(stringa);
            }

            fclose(file_utenti);
        }

    } else if(processo_estrazioni < 0){
        printf("LOG_DEBUG: errore nella chiamata a fork()\n");
        exit(1);
    }


    // printf("LOG_DEBUG: fine della prima fork()\n");

    // Sono sempre in ascolto, se arriva una richiesta di connessione creo un
    // processo figlio per gestirla
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CICLO INFINITO processo PADRE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    while(1){

        int new_socket;

        fflush(stdin); // non so se è necessario

        address_len = (socklen_t)sizeof(client_address);

        printf("Server esegue accept()\n");

        new_socket = accept(socket_listener, (struct sockaddr*)&client_address, &address_len);

        // per approccio non bloccante
        /*
        // salvo il valore perchè altre chiamate (come ad esempio le printf di debug possono cambiarne il valore)
        error_saved = errno;

        //printf("Valore di new_socket: %d\n", new_socket);

        if(error_saved == EWOULDBLOCK){
            errno = 0;
            //printf("LOG_DEBUG: errno settato a EWOULDBLOCK da accept(), non sono presenti dati disponibili\n");
            sleep(5);

            // polling attivo, non sto sfruttando il socket non bloccante
            continue;
        }
        */

        if(new_socket < 0){
            perror("Error in accept : \n");
            exit(1);
            // forse qui devo mettere un continue ??
        }

        printf("LOG_DEBUG: Creata socket %d per la connessione con il client\n", new_socket);

        pid = fork();

        if(pid == 0){ // caso figlio

            // utenti
            char USERNAME_LOGIN[CREDENTIAL_LEN]; // dopo il login
            char PASSWORD_LOGIN[CREDENTIAL_LEN]; // dopo il login
            char *username;
            char *password;
            int trovato; // per verificare che non esista un utente con lo stesso username
            char sessionID[SESSION_ID_LEN];
            char receivedID[SESSION_ID_LEN];
            int contatore; // tentativi di login

            // per la memorizzazione dell'indirizzo
            struct sockaddr_in *tmp_sockaddr_in;
            struct in_addr IPaddress;
            char savedIP[INET_ADDRSTRLEN];
            int errore;

            int termina_server;

            // inizializzazioni importanti
            memset(USERNAME_LOGIN, 0, CREDENTIAL_LEN);
            memset(PASSWORD_LOGIN, 0, CREDENTIAL_LEN);
            termina_server = 0;
            sessionID[0] = '\0';
            errore = 0;

            // printf("~~~~~~~~~~~~~~~~ Sono il processo FIGLIO creato dalla accept() nel server! ~~~~~~~~~~~~~~~~ \n");

            // memorizzo l'indirizzo del client
            tmp_sockaddr_in = (struct sockaddr_in*)&client_address;
            IPaddress = tmp_sockaddr_in -> sin_addr;
            inet_ntop(AF_INET, &IPaddress, savedIP, INET_ADDRSTRLEN);

            // printf("LOG_DEBUG: indirizzo IP del client: %s\n", savedIP);

            // il processo figlio non ascolta per nuova connessioni
            ret = close(socket_listener);
            if(ret < 0){
                perror("Error in close : \n");
                exit(1);
            }


            // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CICLO INFINITO processo FIGLIO %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            while(1){

                // printf("Processo figlio entra nel while infinito\n");

                errore = check_IP(savedIP);
                if(errore){
                    printf("IP bannato tenta di riconnettersi\n");
                    trasmetti("Sei stato bannato, riprova più tardi", new_socket);
                    close(new_socket);
                    exit(0);
                } else
                    trasmetti("Connessione effettuata", new_socket);

                comando = calloc(LEN_COMANDI, sizeof(char));
                ricevi(comando, new_socket);
                tmp = rileva_comando(comando);
                free(comando);

                // SELEZIONE DEL COMANDO
                switch(tmp){

                    case 0: // #################################################### errore

                        printf("Nessun comando rilevato, termino\n");
                        termina_server = 1;
                        aggiorna_sessionID(USERNAME_LOGIN, NULL);
                        break;

                    case 1: // #################################################### help

                        printf("Il client esegue un comando di help\n");
                        break;

                    case 2: // #################################################### signup

                        printf(" > > > > > > > > >  Ricevuto un comando di signup < < < < < < < < < \n");

                        // ricevo username e passowrd e li salvo nelle relative variabili
                        username = calloc(CREDENTIAL_LEN, sizeof(char));
                        password = calloc(CREDENTIAL_LEN, sizeof(char));
                        ricevi(username, new_socket);
                        ricevi(password, new_socket);
                        // printf("Username: %s\n", username);
                        // printf("Password: %s\n", password);

                        // se è il primo creo i file
                        fd = fopen(FILE_UTENTI, "a");
                        if(fd == NULL){
                            perror("Errore nella apertura del file lista_utenti.txt :\n");
                            exit(1);
                        }

                        fclose(fd);

                        ret = stat(FILE_UTENTI, &info);
                        size = info.st_size;
                        // printf("LOG_DEBUG: dimensione del file lista_utenti.txt: %d\n", size);

                        ret = 0; // per controllare i valori di ritorno
                        /*
                        if(size == 0)
                            printf("Primo utente collegato\n");
                        else
                            printf("Nuovo utente collegato\n");
                        */

                        // attenzione questo non è nelle specifiche, aggiunto come restrizione
                        if(strlen(password) < 4){
                            printf("LOG_DEBUG: vincolo sull password non rispettato\n");
                            trasmetti("password non adatta", new_socket);
                            break; // mi metto in ascolto di un nuovo comando
                        }


                        if(size != 0){
                            fd = fopen("/home/studenti/NetBeansProjects/lotto_server/lista_utenti.txt", "r");
                            if(fd == NULL){
                                perror("Errore nell'apertura del file lista_utenti.txt\n");
                                exit(1);
                            }

                            // controllo se esiste già un utente con questo username
                            trovato = 0;
                            while(!feof(fd)){
                                stringa = calloc(BUFFER_LENGHT, sizeof(char));
                                ret = fscanf(fd, "%s", stringa);
                                if(strcmp(stringa, username) == 0){
                                    trovato = 1;
                                    free(stringa);
                                    break;
                                }
                                free(stringa);
                            }

                            fclose(fd);

                            if(trovato){
                               printf("LOG_DEBUG: username esistente\n");
                               trasmetti("username esistente", new_socket);
                               free(username);
                               free(password);
                               break; // esco dallo switch, poi tornerò nel ciclo in attesa di un nuovo username
                            }
                        }

                        fd = scrivi_file(username, "/home/studenti/NetBeansProjects/lotto_server/lista_utenti.txt", "a");

                        stringa = calloc(BUFFER_LENGHT, sizeof(char));
                        fd = crea_file_registro(username, stringa);

                        // come prima informazione mantengo la password
                        // utilizza il valore stringa modificato da crea_file_registro
                        fd = scrivi_file(password, stringa, "a");
                        printf("LOG_DEBUG: percorso file registro: %s\n", stringa);
                        free(stringa);

                        stringa = calloc(BUFFER_LENGHT, sizeof(char));
                        fd = crea_consuntivo(username, stringa);
                        printf("LOG_DEBUG: percorso file consuntivo: %s\n", stringa);
                        free(stringa);

                        trasmetti("signup effettuato", new_socket);

                        printf("signup effettuato correttamente\n");
                        free(username);
                        free(password);
                        break;

                    case 3: // #################################################### login

                        printf(" > > > > > > > > >  Ricevuto un comando di login < < < < < < < < < \n");

                        if(sessionID[0] != '\0'){
                            trasmetti("login effettuato, cambio utente", new_socket);
                        }
                        else
                            trasmetti("fase di login", new_socket);

                        contatore = 0;
                        while(contatore < 3){

                            // ricevo username e passowrd e li salvo nelle relative variabili
                            username = calloc(CREDENTIAL_LEN, sizeof(char));
                            password = calloc(CREDENTIAL_LEN, sizeof(char));
                            ricevi(username, new_socket);
                            ricevi(password, new_socket);
                            printf("Username: %s\n", username);
                            printf("Password: %s\n", password);

                            trovato = trova_username(username);
                            if(trovato){
                                // utente presente, convalido la password
                                ret = verifica_password(username, password);
                                if(ret)
                                    break;
                            }

                            if(contatore != 2) // escludo l'ultimo tentativo
                                trasmetti("username inesistente, reinserire credenziali", new_socket);

                            free(username);
                            free(password);
                            contatore++;

                        }


                        if(contatore == 3){ // login fallito

                            // per lavorare con il tempo
                            time_t ban_time;
                            time_t current_time;
                            struct tm *ts;
                            char buf[TIME_LEN];

                            time(&ban_time);
                            printf("LOG_DEBUG: ban_time: %s\n", ctime(&ban_time));

                            ts = localtime(&ban_time);
                            strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts); // ------------------------------>> non ci vuole strlen???
                            // printf("LOG_DEBUG: risultato strftime: %s\n", buf);

                            // printf("Scrivo IP e timestamp nel file IP_bannati.txt\n");

                            buffer = calloc(BUFFER_LENGHT, sizeof(char));
                            strncpy(buffer, savedIP, INET_ADDRSTRLEN);
                            strncat(buffer, " ", 2);
                            strncat(buffer, buf, TIME_LEN);
                            fd = scrivi_file(buffer, IP_BANNATI, "a");
                            free(buffer);

                            trasmetti("troppi tentativi, BAN di 30 minuti", new_socket);

                            ret = close(new_socket);
                            if(ret < 0){
                                perror("Error in close : \n");
                                free(username);
                                free(password);
                                exit(1);
                            }

                            do {
                              time(&current_time);
                              // printf("Controllo sullo scorrere del tempo\n");
                              sleep(5); // Controllo ogni minuto, per efficienza

                            } while((int)current_time < (int)ban_time + 60); // DEBUG, non come nelle specifiche

                            // printf("LOG_DEBUG: uscito dal ciclo while sullo scorrere del tempo\n");

                            // se sono qui è passato il tempo necessario
                            aggiorna_bannati(savedIP);
                            termina_server = 1;

                        } else if(contatore < 3){ // login riuscito
                            static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
                            int i;

                            srand(time(0));
                            for(i = 0; i < SESSION_ID_LEN; i++){
                                int index;
                                index = (int)((double)rand() / ((double)RAND_MAX + 1) * strlen(alphanum));
                                sessionID[i] = alphanum[index];

                            }
                            sessionID[SESSION_ID_LEN] = '\0';
                            printf("LOG_DEBUG: sessionID <---> %s\n", sessionID);


                            strncpy(USERNAME_LOGIN, username, strlen(username) + 1);
                            strncpy(PASSWORD_LOGIN, password, strlen(password) + 1);
                            free(username);
                            free(password);

                            aggiorna_sessionID(USERNAME_LOGIN, sessionID);

                            trasmetti("Login effettuato con successo", new_socket);

                            trasmetti(sessionID, new_socket);

                        } else { // errore inaspettato
                            printf("Errore inaspettato, termino con un errore\n");
                            free(username);
                            free(password);
                            exit(1);
                        }

                        break;

                    case 4: // #################################################### invia_giocata
                    {
                        struct schedina *punt_schedina;
                        int ok;

                        printf(" > > > > > > > > >  Ricevuto un comando di invia_giocata < < < < < < < < < \n");

                        // fase di autneticazione
                        ok = risposta_autenticazione(sessionID, receivedID, new_socket);
                        if(!ok)
                            break;

                        punt_schedina = (struct schedina*)calloc(1, sizeof(struct schedina));

                        // lettura delle ruote
                        // printf("LOG_DEBUG: Lettura delle ruote\n");
                        i = 0;
                        punt_schedina->quante_ruote = 0;
                        while(1){
                            char *parametro;

                            parametro = calloc(WHEEL_LENGHT, sizeof(char));
                            ricevi(parametro, new_socket);

                            if(strcmp(parametro, "tutte") == 0){ // tutte le ruote
                                punt_schedina->quante_ruote = 11;
                                punt_schedina->flag_tutte = 's';
                                free(parametro);

                            } else if(strcmp(parametro, "fine -r") == 0){ // terminano le ruote
                                free(parametro);
                                break;

                            } else { // ricevo una ruota specifica
                                punt_schedina->quante_ruote++;
                                parametro[strlen(parametro)] = '\0';
                                strncpy(punt_schedina->ruote[i], parametro, strlen(parametro) + 1);
                                free(parametro);
                                i++;
                            }
                        }

                        // lettura dei numeri
                        // printf("LOG_DEBUG: Lettura dei numeri giocati\n");
                        punt_schedina->quanti_numeri = 0;
                        i = 0;
                        while(1){
                            char *parametro;

                            parametro = calloc(RESPONSE_LEN, sizeof(char));
                            ricevi(parametro, new_socket);

                            if(strcmp(parametro, "Troppi numeri") == 0){ // errore
                                free(parametro);
                                printf("Ricevuto un messaggio di troppi numeri inseriti\n");
                                printf("Terminazione del server\n");
                                close(new_socket);
                                free(punt_schedina);
                                exit(1);

                            } else if(strcmp(parametro, "fine -n") == 0){ // proseguo con la prossima opzione
                                free(parametro);
                                break;

                            } else if(strcmp(parametro, "Parametro mancante") == 0) {
                                free(parametro);
                                printf("Ricevuto un messaggio di parametro mancante\n");
                                printf("Terminazione del server\n");
                                close(new_socket);
                                free(punt_schedina);
                                exit(1);

                            } else { // ricevo un numero
                                punt_schedina->quanti_numeri++;
                                punt_schedina->numeri_giocati[i] = atoi(parametro);
                                free(parametro);
                                i++;
                            }
                        }

                        // leggo gli importi
                        // printf("LOG_DEBUG: Lettura degli importi\n");
                        punt_schedina->quanti_importi = 0;
                        i = 0;
                        while(1){
                            char *parametro;

                            parametro = calloc(RESPONSE_LEN, sizeof(char));
                            ricevi(parametro, new_socket);

                            if(strcmp(parametro, "Errore nell'importo") == 0){ // errore
                                free(parametro);
                                printf("Ricevuto un messaggio di errore nell'importo\n");
                                printf("Terminazione del server\n");
                                close(new_socket);
                                free(punt_schedina);
                                exit(1);

                            } else if(strcmp(parametro, "fine comando invia_giocata") == 0){ // proseguo
                                free(parametro);
                                break;

                            } else { // nuovo importo ricevuto
                                punt_schedina->quanti_importi++;
                                punt_schedina->importi[i] = atof(parametro);
                                free(parametro);
                                i++;
                            }
                        }

                        if(punt_schedina->quanti_numeri < punt_schedina->quanti_importi){
                            printf("Errore nella costruzione della schedina");
                            trasmetti("Schedina erronea", new_socket);
                            free(punt_schedina);
                            exit(1);
                        }

                        printf("---  ---  ---  STAMPA DELLA SCHEDINA  ---  ---  ---\n");
                        stampa_schedina(punt_schedina);

                        nuova_schedina(USERNAME_LOGIN, punt_schedina);
                        // printf("LOG_DEBUG: file di registro aggiornato\n");
                        trasmetti("Schedina memorizzata", new_socket);

                        free(punt_schedina);
                        break;
                    }
                    case 5: // #################################################### vedi_giocata
                    {
                        int ok;
                        char *risposta;

                        printf(" > > > > > > > > >  Ricevuto un comando di vedi_giocata < < < < < < < < < \n");

                        ok = risposta_autenticazione(sessionID, receivedID, new_socket);
                        if(!ok)
                            break;

                        risposta = calloc(RESPONSE_LEN, new_socket);
                        ricevi(risposta, new_socket);

                        // SCHEDINE_GIOCATE
                        if(strcmp(risposta, "0") == 0){
                            trasmetti("ok", new_socket);
                            invia_schedine_giocate(USERNAME_LOGIN, new_socket) ;

                        // SCHEDINE_ATTIVE
                        } else if(strcmp(risposta, "1") == 0){
                            trasmetti("ok", new_socket);
                            invia_schedine_attive(USERNAME_LOGIN, new_socket);

                        // errore nel client
                        } else if(strcmp(risposta, "Errore inaspettato") == 0){
                            printf("Errore inaspettato nel client\n");
                            free(risposta);
                            exit(1);

                        // errore nel server
                        } else {
                            trasmetti("Errore nel server", new_socket);
                            printf("Errore inaspettato nel server\n");
                            free(risposta);
                            exit(1);
                        }

                        free(risposta);
                        break;
                    }
                    case 6: // #################################################### vedi_estrazioni
                    {
                        char *quante;
                        char *ruota;

                        printf(" > > > > > > > > >  Ricevuto un comando di vedi_estrazioni  < < < < < < < < < \n");

                        quante = calloc(2, sizeof(char));
                        if(!quante){
                            printf("Memoria esaurita!");
                            exit(1);
                        }
                        ruota = calloc(WHEEL_LENGHT, sizeof(char));
                        if(!ruota){
                            printf("Memoria esaurita!");
                            exit(1);
                        }
                        ricevi(quante, new_socket);
                        // printf("LOG_DEBUG: quante = %d\n", atoi(quante));
                        ricevi(ruota, new_socket);
                        // printf("LOG_DEBUG: ruota = %s\n", ruota);
                        trasmetti_estrazioni(atoi(quante), ruota, new_socket);

                        free(quante);
                        free(ruota);
                        break;
                    }
                    case 7: // #################################################### vedi_vincite
                    {
                        int ok;
                        char *path;
                        FILE *fd_consuntivo;
                        int non_vuoto = 0;


                        printf(" > > > > > > > > >  Ricevuto un comando di vedi_vincite < < < < < < < < < \n");

                        ok = risposta_autenticazione(sessionID, receivedID, new_socket);
                        if(!ok)
                            break;

                        path = calloc(PATH_LEN, sizeof(char));
                        if(!path){
                            printf("Memoria esaurita\n");
                            exit(1);
                        }

                        percorso_consuntivo(USERNAME_LOGIN, path);
                        fd_consuntivo = fopen(path, "r");
                        if(fd_consuntivo == NULL){
                            printf("Errore nell'apertura del file consuntivo\n");
                            free(path);
                            exit(1);
                        }
                        free(path);

                        while(1){

                            int success;
                            char *stringa;

                            stringa = calloc(CREDENTIAL_LEN, sizeof(char));
                            if(!stringa){
                                printf("Memoria esaurita\n");
                                exit(1);
                            }
                            success = fscanf(fd_consuntivo, "%s", stringa);

                            if(success == EOF){
                                free(stringa);
                                if(non_vuoto)
                                    trasmetti("Termine", new_socket);
                                else
                                    trasmetti("Nessuna vincita", new_socket);

                                break;

                            } else if(!success){
                                free(stringa);
                                printf("Errore nella lettura del file consuntivo\n");
                                exit(1);

                            } else if(strcmp(stringa, "ESTRATTO") == 0){
                                non_vuoto = 1;
                            }

                            trasmetti(stringa, new_socket);
                            free(stringa);
                        }

                        fclose(fd_consuntivo);
                        break;
                    }

                    case 8: // #################################################### esci
                    {
                        int ok;
                        char messaggio[20];

                        printf(" > > > > > > > > >  Ricevuto un comando di vedi_vincite < < < < < < < < < \n");

                        ok = risposta_autenticazione(sessionID, receivedID, new_socket);
                        if(!ok)
                            break;

                        ricevi(messaggio, new_socket);
                        if(strcmp(messaggio, "Chiusura") == 0){
                            trasmetti("Conferma Chiusura", new_socket);
                            termina_server = 1;

                            // elimina il sessionID dal file di registro
                            aggiorna_sessionID(USERNAME_LOGIN, NULL);
                            // non si eliminano il file di registro e il consuntivo (mantenuti per i prossimi login)

                        } else {
                            printf("Errore nella fase di chiusura della connessione con il client\n");
                        }

                        break;
                    }
                    default: // #################################################### default

                        break;

                }

                // TERMINO
                if(termina_server != 0){
                    printf("Terminazione del server, chiusura del socket %d\n", new_socket);
                    close(new_socket);
                    exit(0);
                }

                // printf("LOG_DEBUG: inizia un nuovo ciclo while del figlio\n");

            } // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FINE CICLO INFINITO processo FIGLIO %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        } else if(pid < 0){ // errore

            printf("Errore nella chiamata a fork()\n");
            exit(1);

        } else if(pid > 0) { // caso padre

            //printf(" ~~~~~~~~~~~~~~~~ Sono il processo PADRE in lotto_server! ~~~~~~~~~~~~~~~~ \n");
            close(new_socket);

        }

    } // // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FINE CICLO INFINITO processo PADRE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    exit(0);
}
