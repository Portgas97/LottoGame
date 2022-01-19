#include "/home/studenti/NetBeansProjects/funzioni.h"


int main(int argc, char* argv[]){

    // variabili per il socket
    int ret, socket_locale;
    struct sockaddr_in server_address;
    socklen_t address_len;
    char *connection_result;

    // ID di sessione per l'utente
    char sessionID[SESSION_ID_LEN];

    // username e password dopo la fase di login
    char USERNAME_LOGIN[CREDENTIAL_LEN];
    char PASSWORD_LOGIN[CREDENTIAL_LEN];

    memset(sessionID, 0, CREDENTIAL_LEN);
    memset(USERNAME_LOGIN, 0, CREDENTIAL_LEN);
    memset(PASSWORD_LOGIN, 0, CREDENTIAL_LEN);

    // validazione dell'input
    if(argc == 1 || argc == 2){
        printf("È necessario inserire due parametri: indirizzo IP del server e porta\n");
        exit(1);
    }
    else if(argc == 3){

        if(strcmp(argv[2],"4242") == 0 && strcmp(argv[1], "127.0.0.1") == 0){ // il secondo controllo non è del tutto corretto
            printf("Porta e indirizzo inseriti correttamente\n");
        } else{
            printf("La porta %s o l'indirizzo %s non sono corretti:\n", argv[2], argv[1]);
            exit(1);
        }
    }
    else{ // default safe
        printf("Numero di argomenti passati non corretto. Inserire indirizzo IP del server e porta\n");
        exit(1);
    }

    // creazione di una socket
    socket_locale = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_locale < 0){
        printf("La primitiva socket() ha fallito, termino con un errore\n");
        perror("Error in socket(): \n");
        exit(1);
    }
    // creazione indirizzo del server
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_address.sin_addr);

    // connessione
    address_len = (socklen_t) sizeof(server_address);
    ret = connect(socket_locale, (struct sockaddr*)&server_address, address_len);

    if(ret < 0){
        printf("La primitiva connect() ha fallito, termino con un errore\n");
        perror("perror in connect() :: \n");
        exit(1);
    }



    while(1){

        // variabili per i comandi
        int i;
        int tmp;
        int index;
        char bool;
        char terminatore;
        char *comando;
        char *buffer;
        char *risposta;

        // per la chiusura della connessione
        int termina_client;

        // variabili per gli utenti
        char *username;
        char *password;

        termina_client = 0;

        // fflush(stdin);
        connection_result = calloc(RESPONSE_LEN, sizeof(char));
        ricevi(connection_result, socket_locale);
        if(strcmp(connection_result, "Sei stato bannato, riprova più tardi") == 0){
            printf("Sei stato bannato per eccesso di tentativi di login, riprova più tardi\n");
            exit(0);
        }
        free(connection_result);

        // printf("LOG_DEBUG: connessione effettuata con successo\n");
        stampa_menu(0);


        // prelievo del comando, contiene l'intera stringa con comando e parametri
        printf("Inserisci un comando: \n");
        comando = (char*)calloc(BUFFER_LENGHT, sizeof(char));
        if(!comando){
            printf("Memoria esaurita\n");
            exit(1);
        }

        // per permettere la lettura degli spazi e per consumare \n rimasti nel buffer di input
        scanf(" %[^\n]s", comando);

        // printf("LOG_DEBUG: comando inserito: ");
        // printf("%s\n", comando);
        // printf("\n");
        comando[strlen(comando)] = '\0';

        if(comando[0] != '!'){
            printf("Il comando deve iniziare per !\n");
            free(comando);
            exit(1);
        }

        // elimino il carattere !
        for(i = 0; i < strlen(comando) + 1; i++)
            comando[i] = comando[i+1];


        // printf("\n ~ ~ ~ ~ ~ ~ ~ ANALISI DEL COMANDO ~ ~ ~ ~ ~ ~ ~ \n");
        // printf("\n");

        // estraggo il comando e lo inserisco nel buffer
        index = 0;
        buffer = calloc(MAX_SIZE_PARAMETER, sizeof(char));
        terminatore = sotto_stringa(comando, buffer, &index);
        buffer[strlen(buffer)] = '\0';
        // associo un numero al comando
        tmp = rileva_comando(buffer);
        // printf("LOG_DEBUG: Valore di tmp: %d\n", tmp);
        // printf("LOG_DEBUG: È stato inserito il comando numero %d\n", tmp);

         //printf("\nLOG_DEBUG: esecuzione dello switch nel client\n");

        // SELEZIONE DEL COMANDO
        switch(tmp){

            case 0: // ######################################################## errore

                free(comando);
                free(buffer);

                printf("Nessun comando rilevato\n");

                termina_client = 1;
                break;

            case 1: // ######################################################## help

                // invio il comando per coerenza con il codice del server per gli altri comandi
                trasmetti(buffer, socket_locale);
                free(buffer);

                printf("\n ----> Elaborazione del comando help <---- \n");

                bool = rileva_parametro(comando, &index, terminatore);

                if(bool == 'n'){ // non è presente un parametro
                    free(comando);
                    stampa_menu(0);
                }
                else if(bool == 's'){ // può esserci solo un parametro
                    buffer = calloc(MAX_SIZE_PARAMETER, sizeof(char));
                    terminatore = sotto_stringa(comando, buffer, &index);

                    tmp = rileva_comando(buffer);
                    if(tmp == 0){
                        printf("Il parametro inserito non è ammesso \n");
                        termina_client = 1;
                        free(comando);
                        free(buffer);
                        break;
                    }

                    free(comando);
                    free(buffer);
                    stampa_menu(tmp-1);

                } else{
                    free(comando);
                    printf("Errore inaspettato, termino\n");
                    termina_client = 1;
                }

                // printf("LOG_DEBUG: fine comando di help\n");
                break;

            case 2: // ######################################################## signup

                printf("\n ----> Elaborazione del comando signup <---- \n");

                // 1) invio il comando ricevuto
                trasmetti(buffer, socket_locale);
                free(buffer);

                // printf("Ho inviato il comando al server! \n");

                // printf("LOG_DEBUG: comando: %s\n", comando);
                username = calloc(CREDENTIAL_LEN, sizeof(char));
                // 2) invio username (INVIARE UN MESSAGGIO PER QUANDO USERNAME È VUOTO)
                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, username, &index);

                trasmetti(username, socket_locale);

                password = calloc(CREDENTIAL_LEN, sizeof(char));
                // 3) invio password (INVIARE UN MESSAGGIO PER QUANDO PASSWORD È VUOTO)
                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, password, &index);

                trasmetti(password, socket_locale);

                free(comando);

                // 4) leggo la risposta
                printf("Risposta del server: ");

                risposta = calloc(RESPONSE_LEN, sizeof(char));
                ricevi(risposta, socket_locale);

                if(strcmp(risposta, "username esistente") == 0){
                    printf("È necessario rieseguire il comando !signup, username già esistente\n");

                } else if(strcmp(risposta, "password non adatta") == 0){
                    printf("È necessario rieseguire il comando !signup, minima lunghezza della password consentita: 4 caratteri\n");

                } else if(strcmp(risposta, "signup effettuato") == 0){
                    printf("signup effettuato correttamente!\n");

                } else {
                    printf("Errore inaspettato, termino\n");
                    free(username);
                    free(password);
                    free(risposta);
                    exit(1);

                }

                free(username);
                free(password);
                free(risposta);
                break;

            case 3: // ######################################################## login

                printf("\n ----> Elaborazione del comando login <---- \n");

                // invio il comando
                trasmetti(buffer, socket_locale);
                // printf("Ho inviato il comando %s al server! \n", buffer);
                free(buffer);

                risposta = calloc(RESPONSE_LEN, sizeof(char));
                ricevi(risposta, socket_locale);
                if(strcmp("login effettuato, cambio utente", risposta) == 0){
                    printf("La fase di login è già stata effettuata, cambio utente o genero un nuovo sessionID\n");
                }
                free(risposta);

                // invio lo username
                username = calloc(CREDENTIAL_LEN, sizeof(char));
                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, username, &index);
                trasmetti(username, socket_locale);
                strncpy(USERNAME_LOGIN, username, strlen(username) + 1);
                free(username);

                // invio la password
                password = calloc(CREDENTIAL_LEN, sizeof(char));
                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, password, &index);
                trasmetti(password, socket_locale);
                strncpy(PASSWORD_LOGIN, password, strlen(password) + 1);
                free(password);

                free(comando);

                while(1){

                    risposta = calloc(RESPONSE_LEN, sizeof(char));
                    ricevi(risposta, socket_locale);

                    if(strcmp(risposta, "username inesistente, reinserire credenziali") == 0){ // username da cambiare

                        free(risposta);
                        username = calloc(CREDENTIAL_LEN, sizeof(char));
                        printf("Username inesistente, è necessario reinserire le credenziali \n");
                        printf("Username: ");
                        scanf("%s", username);
                        trasmetti(username, socket_locale);
                        strncpy(USERNAME_LOGIN, username, strlen(username) + 1);
                        free(username);

                        password = calloc(CREDENTIAL_LEN, sizeof(char));
                        printf("Password: ");
                        scanf("%s", password);
                        trasmetti(password, socket_locale);
                        strncpy(PASSWORD_LOGIN, password, strlen(password) + 1);
                        free(password);

                    } else if(strcmp(risposta, "troppi tentativi, BAN di 30 minuti") == 0) { // BAN
                        free(risposta);
                        printf("Utente bannato per 30 minuti, troppi tentativi\n");
                        termina_client = 1;
                        break;
                    } else if(strcmp(risposta, "Login effettuato con successo") == 0){ // successo
                        free(risposta);
                        ricevi(sessionID, socket_locale);
                        printf("Login effettuato con successo, il tuo session ID è: %s\n", sessionID);
                        break;
                    } else {
                        free(risposta);
                        printf("Errore inaspettato, termino con un errore\n");
                        exit(1);
                    }
                }

                break;

            case 4:// ######################################################## invia_giocata
            {
                int i;
                int ok;
                char *opzione;
                char *parametro;
                char *RUOTE_LOTTO[] = {
                    "bari",
                    "cagliari",
                    "firenze",
                    "genova",
                    "milano",
                    "napoli",
                    "palermo",
                    "roma",
                    "torino",
                    "venezia",
                    "nazionale"
                };

                int ruote_usate[11];
                for(i = 0; i < 11; i++)
                    ruote_usate[i] = 0;

                printf("\n ----> Elaborazione del comando invia_giocata <---- \n");

                trasmetti(buffer, socket_locale);
                // printf("Ho inviato il comando %s al server! \n", buffer); // ELIMINABILE
                free(buffer);

                // può terminare l'esecuzione
                ok = richiesta_autenticazione(sessionID, socket_locale);
                if(!ok){
                    free(comando);
                    break;
                }
                // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Lettura delle ruote ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
                opzione = calloc(2, sizeof(char));
                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, opzione, &index);

                if(strcmp(opzione, "-r") != 0){
                    free(comando);
                    free(opzione);
                    printf("Errore, parametro -r non rilevato\n");
                    exit(1);
                }
                // printf("lOG_DEBUG: Lettura dei parametri per l'opzione -r\n");
                free(opzione);

                parametro = calloc(WHEEL_LENGHT, sizeof(char));
                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, parametro, &index);

                i = 0;
                while(strcmp(parametro, "-n") != 0){
                    int j;

                    if(strcmp(parametro, "tutte") == 0 && i != 0){ // non si può scrivere "tutte" dopo aver scritto altre ruote
                        free(comando);
                        free(parametro);
                        printf("Non si può scrivere \'tutte\' dopo aver inserito altre ruote\n");
                        exit(1);
                    }
                    else if(strcmp(parametro, "tutte") == 0 && i == 0){
                        trasmetti("tutte", socket_locale);
                    }
                    else {

                        int pass_check;
                        pass_check = 0;
                        for(j = 0; j < 11; j++){
                            int ret_val;
                            ret_val = link_wheel(parametro);
                            if(ret_val == -1){
                                printf("Termino con un errore\n");
                                exit(1);
                            }

                            if(strcmp(RUOTE_LOTTO[j], parametro) == 0){

                                if(ruote_usate[ret_val] == 1){
                                    printf("Ruota già inserita\n");
                                    free(comando);
                                    free(parametro);
                                    exit(1);
                                }

                                pass_check = 1;
                                ruote_usate[ret_val] = 1;

                            }
                        }

                        if(!pass_check){
                            printf("La ruota inserita non è valida\n");
                            free(comando);
                            free(parametro);
                            exit(1); // ------------------------------------------------------------- eliminare sessionID
                        }
                        trasmetti(parametro, socket_locale);
                    }

                    i++;
                    free(parametro);
                    parametro = calloc(WHEEL_LENGHT, sizeof(char));
                    bool = rileva_parametro(comando, &index, terminatore);
                    acquisisci_parametro(bool, comando, parametro, &index);
                }

                trasmetti("fine -r", socket_locale);

                // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Lettura dei numeri giocati ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
                opzione = calloc(2, sizeof(char));
                strncpy(opzione, parametro, 3); // per leggibilità le opzioni le mettiamo nella variabile opzione
                free(parametro);

                if(strcmp(opzione, "-n") != 0){
                    free(comando);
                    free(opzione);
                    printf("Errore, parametro -n non rilevato\n");
                    exit(1);
                }
                // printf("lOG_DEBUG: Lettura dei parametri per l'opzione -n\n");
                free(opzione);

                parametro = calloc(2, sizeof(char));
                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, parametro, &index);
                i = 0; // conto i numeri giocati

                while(strcmp(parametro, "-i") != 0){

                    if(atoi(parametro) < 1 || atoi(parametro) > 90){
                        printf("Non è possibile giocare il numero: %d\n", atoi(parametro));
                        free(parametro);
                        free(comando);
                        exit(1); // -------------------------------------------------------- eilminare il sessionID
                    }
                    trasmetti(parametro, socket_locale);
                    free(parametro);

                    parametro = calloc(2, sizeof(char));
                    bool = rileva_parametro(comando, &index, terminatore);
                    acquisisci_parametro(bool, comando, parametro, &index);

                    if(bool == 'n'){
                        printf("Parametro mancante\n");
                        trasmetti("Parametro mancante", socket_locale);
                        free(parametro);
                        free(comando);
                        exit(1);
                    }

                    if(i > 10){
                        trasmetti("Troppi numeri", socket_locale);
                        printf("Hai inserito più di 10 numeri, termino\n");
                        free(parametro);
                        free(comando);
                        exit(1);
                    }
                    i++;
                }

                trasmetti("fine -n", socket_locale);

                // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Lettura degli importi ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
                opzione = calloc(2, sizeof(char));
                strncpy(opzione, parametro, 3); // per leggibilità le opzioni le mettiamo nella variabile opzione
                free(parametro);

                if(strcmp(opzione, "-i") != 0){
                    free(comando);
                    free(opzione);
                    printf("Errore, parametro -n non rilevato\n");
                    exit(1);
                }
                // printf("lOG_DEBUG: Lettura dei parametri per l'opzione -i\n");
                free(opzione);

                parametro = calloc(3, sizeof(char)); // importo massimo pari a 200€
                bool = rileva_parametro(comando, &index, terminatore);


                while(bool != 'n'){

                    if(index > strlen(comando)){
                        break;
                    }

                    acquisisci_parametro(bool, comando, parametro, &index);
                    if(atof(parametro) > MAX_AMOUNT){
                        trasmetti("Errore nell'importo", socket_locale);
                        printf("Importo inserito non corretto\n");
                        free(parametro);
                        free(comando);
                        exit(1);
                    }
                    trasmetti(parametro, socket_locale);
                    free(parametro);
                    parametro = calloc(3, sizeof(char));
                    bool = rileva_parametro(comando, &index, terminatore);
                }

                free(parametro);
                free(comando);
                trasmetti("fine comando invia_giocata", socket_locale);

                risposta = calloc(RESPONSE_LEN, sizeof(char));
                ricevi(risposta, socket_locale);

                if(strcmp(risposta, "Schedina memorizzata") == 0){
                    printf("Il server ha memorizzato la schedina\n");
                } else if(strcmp(risposta, "Errore nella costruzione della schedina") == 0){
                    printf("Errore nella creazione della schedina\n");
                    free(risposta);
                    exit(1);
                }

                free(risposta);
                break;
            }

            case 5: // ######################################################## vedi_giocata
            {
                int ok;
                char tipo[2]; // definito come array per poter riutilizzare certe funzioni
                char *risposta;

                printf("\n ----> Elaborazione del comando vedi_giocata <---- \n");

                trasmetti(buffer, socket_locale);
                free(buffer);

                ok = richiesta_autenticazione(sessionID, socket_locale);
                if(!ok){
                    free(comando);
                    break;
                }

                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, tipo, &index);
                free(comando);

                // SCHEDINE_GIOCATE
                if(tipo[0] == '0'){
                    // printf("LOG_DEBUG: ricevuto parametro 0\n");
                    trasmetti("0", socket_locale);

                // SCHEDINE_ATTIVE
                } else if(tipo[0] == '1'){
                    // printf("LOG_DEBUG: ricevuto parametro 1\n");
                    trasmetti("1", socket_locale);

                // errore inaspettato
                } else {
                    trasmetti("Errore inaspettato", socket_locale);
                    printf("Errore inaspettato, possibile parametro sbagliato\n");
                    exit(1);
                }

                risposta = calloc(RESPONSE_LEN, sizeof(char));
                ricevi(risposta, socket_locale);
                if(strcmp(risposta, "ok") == 0){
                    free(risposta);
                } else if(strcmp(risposta, "Errore nel server") == 0){
                    free(risposta);
                    printf("Errore inaspettato nel server\n");
                    exit(1);
                }

                if(tipo[0] == '1'){
                    printf("Ricevo le schedine attive\n");
                    ricevi_schedine_attive(socket_locale);
                }
                else if(tipo[0] == '0'){
                    printf("Ricevo le schedine giocate\n");
                    ricevi_schedine_giocate(socket_locale);
                }



                break;
            }

            case 6: // ######################################################## vedi_estrazioni
            {
                // in questo comando non è necessaria una fase di autenticazione
                char *quante;
                char *ruota;

                printf("\n ----> Elaborazione del comando vedi_estrazioni <---- \n");

                trasmetti(buffer, socket_locale);
                free(buffer);
                quante = calloc(2, sizeof(char)); //massimo 11
                if(!quante){
                    printf("Memoria esaurita\n");
                    exit(1);
                }
                bool = rileva_parametro(comando, &index, terminatore);
                acquisisci_parametro(bool, comando, quante, &index);
                // printf("LOG_DEBUG: primo parametro = %s\n", quante);

                ruota = calloc(WHEEL_LENGHT, sizeof(char));
                if(!ruota){
                    printf("Memoria esaurita\n");
                    exit(1);
                }
                bool = rileva_parametro(comando, &index, terminatore);

                if(bool == 'n'){ // non è presente un parametro, tutte le ruote
                    strncpy(ruota, "tutte", strlen("tutte") + 1);
                    //printf("LOG_DEBUG: secondo parametro = %s\n", ruota);
                }
                else if(bool == 's'){
                    terminatore = sotto_stringa(comando, ruota, &index);
                    //printf("LOG_DEBUG: secondo parametro = %s\n", ruota);
                }


                trasmetti(quante, socket_locale);
                trasmetti(ruota, socket_locale);

                ricevi_estrazioni(atoi(quante), ruota, socket_locale);

                free(quante);
                free(comando);
                free(ruota);
                break;
            }

            case 7: // ######################################################## vedi_vincite
            {
                int ok;
                char *risposta;
                char importo_vincita[10];
                char tipo_giocata[10];
                char data[20];
                char ruota[WHEEL_LENGHT];
                char quanti_numeri_stringa[10];
                int quanti_numeri_intero;
                char numero[2];

                printf("\n ----> Elaborazione del comando vedi_vincite <---- \n");

                trasmetti(buffer, socket_locale);
                free(buffer);
                free(comando);

                ok = richiesta_autenticazione(sessionID, socket_locale);
                if(!ok){
                    free(comando);
                    break;
                }

                while(1){
                    risposta = calloc(RESPONSE_LEN, sizeof(char));
                    ricevi(risposta, socket_locale);
                    if(strcmp(risposta, "Termine") == 0){
                        printf("Lettura delle vincite terminata\n");
                        break;
                    } else if(strcmp(risposta, "Nessuna vincita") == 0){
                        printf("Nessuna vincita\n");
                        break;
                    } else if(strcmp(risposta, "ESTRATTO") == 0){
                        printf("Consuntivo delle vincite:\n");
                        printf("%s  ", risposta);
                        ricevi(importo_vincita, socket_locale);
                        printf("%3f\n", atof(importo_vincita));

                        // ambo
                        ricevi(tipo_giocata, socket_locale);
                        printf("%s  ", tipo_giocata);
                        ricevi(importo_vincita, socket_locale);
                        printf("%3f\n", atof(importo_vincita));

                        // terno
                        ricevi(tipo_giocata, socket_locale);
                        printf("%s  ", tipo_giocata);
                        ricevi(importo_vincita, socket_locale);
                        printf("%3f\n", atof(importo_vincita));

                        // quaterna
                        ricevi(tipo_giocata, socket_locale);
                        printf("%s  ", tipo_giocata);
                        ricevi(importo_vincita, socket_locale);
                        printf("%3f\n", atof(importo_vincita));

                        // cinquina
                        ricevi(tipo_giocata, socket_locale);
                        printf("%s  ", tipo_giocata);
                        ricevi(importo_vincita, socket_locale);
                        printf("%3f\n", atof(importo_vincita));

                    } else if(strcmp(risposta, "vincita") == 0){
                        int i;
                        ricevi(ruota, socket_locale);
                        printf("\nVincita su %s\n", ruota);

                        printf("Numeri vincenti: ");
                        ricevi(quanti_numeri_stringa, socket_locale);
                        quanti_numeri_intero = atoi(quanti_numeri_stringa);
                        for(i = 0; i < quanti_numeri_intero; i++){
                            ricevi(numero, socket_locale);
                            printf("%s ", numero);
                        }
                        printf("\n");

                        ricevi(tipo_giocata, socket_locale);
                        ricevi(importo_vincita, socket_locale);
                        printf("Realizzato %s, vincita %.3f\n", tipo_giocata, atof(importo_vincita));


                    } else if(strcmp(risposta, "data") == 0){
                        printf("\nData dell'estrazione: ");
                        ricevi(data, socket_locale);
                        printf("%s\n", data);
                    }
                    free(risposta);
                }

                break;
            }

            case 8: // ######################################################## esci
            {
                char risposta[20];
                int ok;

                printf("\n ----> Elaborazione del comando esci <---- \n");

                trasmetti(buffer, socket_locale);
                free(buffer);
                free(comando);

                ok = richiesta_autenticazione(sessionID, socket_locale);
                if(!ok){
                    free(comando);
                    break;
                }

                trasmetti("Chiusura", socket_locale);
                ricevi(risposta, socket_locale);
                if(strcmp(risposta, "Conferma Chiusura") == 0)
                   termina_client = 1;
                else
                    printf("Errore nella fase di chiusura: fallita\n");

                break;

            }
            default: // ####################################################### default

                printf("Errore nello switch di selezione del comando\n");
                exit(1);

        }

        if(termina_client != 0)
            break;

        //DENTRO AL CICLO INFINITO
    }
        // FUORI DAL CICLO INFINITO

    printf("Terminazione del client\n");
    close(socket_locale);
    exit(0);
}
