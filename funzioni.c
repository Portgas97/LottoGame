#include "/home/studenti/NetBeansProjects/funzioni.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                          FUNZIONI DI UTILITÀ GENERICHE
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


// stampa le funzioni disponibili, se viene passato 0 si stampa l'intero menù
// altrimenti la sola riga associata all'intero passato
void stampa_menu(int i){
    int j;
    char vettore_comandi[NUMERO_COMANDI][LEN_COMANDI] = {   "help <comando>                 --> mostra i dettagli di un comando",
                                                            "signup <username> <password>   --> crea un nuovo utente",
                                                            "login <username> <password>    --> autentica un utente",
                                                            "invia_giocata g                --> invia giocata g al server",
                                                            "vedi_giocata tipo              --> visualizza le giocate precedenti dove tipo = {0,1} e permette di visualizzare le giocate passate \'0\' oppure le giocate attive \'1\' (ancora non estratte)",
                                                            "vedi_estrazione <n> <ruota>    --> mostra i numeri delle ultime n estrazioni sulla ruota specificata",
                                                            "vedi_vincite                   --> mostra tutte le vincite e un consuntivo delle vittorie",
                                                            "esci                           --> termina il client"};

    if(i != 0){
        printf("\n");
        printf("!");
        for(j = 0; j < LEN_COMANDI; j++)
            if(vettore_comandi[i][j] == '\0'){
                printf("\n");
                return;
            }
            else
                printf("%c", vettore_comandi[i][j]);

        printf("\n");
        return;
    }

    printf("\n");
    printf("****************************** GIOCO DEL LOTTO ****************************** \n\n");
    for(; i < NUMERO_COMANDI; i++){
        printf("%d) !", i);
        for(j = 0; j < LEN_COMANDI; j++)
            if(vettore_comandi[i][j] == '\0')
                break;
            else
                printf("%c", vettore_comandi[i][j]);
        printf("\n\n");
    }
    printf("***************************************************************************** \n");

}


// data una stringa ricava la seguente sottostringa partendo dall'indice index
// utilizza vari delimitatori (' ', '\n', '\0')
// il valore di index ritornato è quello dello spazio bianco o di ritorno carrello successivo
char sotto_stringa(char* comando, char* buffer, int *index){

    int i = 0;
    while(1){
        //printf("Eseguo ciclo numero %d\n", i);
        //printf("Valore di index %d\n", *index);

        if(comando[*index] == '\0'){
            buffer[i] = '\0';
            return '\0';
        }

        if(comando[*index] == ' '){
            buffer[i] = '\0';
            return ' ';
        }

        if(comando[*index] == '\n'){
            buffer[i] = '\0';
            return '\n';
        }

        //printf("carattere trasferito: %c\n", comando[*index]);
        buffer[i] = comando[*index];
/*
        if(*index == MAX_SIZE_INPUT){
            printf("Il comando inserito è troppo lungo\n");
            exit(1);
        }
*/      //printf("\n");

        (*index)++;
        i++;
    }

    buffer[i] = '\0';
}


// associa una certa stringa/comando ad un intero convenzionale
int rileva_comando(char *buffer){

    // nella lettura dei parametri potrebbe esserci ancora !
    if(buffer[strlen(buffer) - 1] == ' '){
        buffer[strlen(buffer) - 1] = '\0';
    }

    if(strncmp("help", buffer, 4) == 0)
        return 1;

    else if(strncmp("signup", buffer, 6) == 0)
        return 2;

    else if(strncmp("login", buffer, 5) == 0)
        return 3;

    else if(strncmp("invia_giocata", buffer, 13) == 0)
        return 4;

    else if(strncmp("vedi_giocata", buffer, 12) == 0)
        return 5;

    else if(strncmp("vedi_estrazione", buffer, 15) == 0)
        return 6;

    else if(strncmp("vedi_vincite", buffer, 12) == 0)
        return 7;

    else if(strncmp("esci", buffer, 4) == 0)
        return 8;

    else
        return 0;
}


// controlla se è presente una sottostringa / parametro
char rileva_parametro(char *comando, int *index, char terminatore){

    if(comando[*index + 1] != '\n' && comando[*index + 1] != ' ' && comando[*index + 1] != '\0'){ // non accettiamo due ' ' di seguito
        // aggiorno index in modo che si punti all'inizio del parametro
        // printf("LOG_DEBUG: è presente un parametro \n");
        (*index)++;
        return 's'; // si, c'è un parametro
    }
    // printf("LOG_DEBUG: non è presente alcun parametro\n");
    return 'n'; //no, non c'è un parametro

}

// legge il parametro (sottostringa), se presente
void acquisisci_parametro(char bool, char *comando, char *buffer, int *index){
    // char terminatore;

    if(bool == 'n'){ // non è presente un parametro
        printf("Parametro non inserito\n");
        exit(1);
    }
    else if(bool == 's')
        /* terminatore = */ sotto_stringa(comando, buffer, index);

    // per motivi di debug
    // printf("Terminatore: %c", terminatore);

}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                              FUNZIONI PER LO SCAMBIO DATI
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


// trasmissione di una stringa
void trasmetti(char *buffer, int socket){

    int len, ret;
    uint16_t msg_len;

    //printf("\n");
    //printf("\nLOG_DEBUG: TRASMETTO %s\n", buffer);

    len = strlen(buffer);
    msg_len = htons(len);

    // invio il numero di dati
    ret = send(socket, (void*) &msg_len, sizeof(uint16_t), 0);

    if(ret < 0){
        printf("Errore nella send() di invio quantità\n");
        perror("perror --> errno :");
        exit(1);
    }

    if(ret < sizeof(uint16_t)){
        printf("Errore nella send() di invio quantità, inviati %d dati anziché %d\n", ret, (int)sizeof(uint16_t));
        exit(1);
    }

    //perror("errno nella send di quanti dati :");

    // invio i dati
    ret = send(socket, (void*)buffer, len, 0);

    if(ret < 0){
        printf("Errore nella send() di invio dati\n");
        perror("perror --> errno :");
        exit(1);
    }

    if(ret < len){
        printf("Errore nella send() di invio dati, inviati %d dati anziché %d\n", ret, len);
        exit(1);
    }
    //perror("errno nella send del messaggio :");

    //printf("\n");
}


// ricezione di una stringa
void ricevi(char *buffer, int socket){

    int len, ret;
    uint16_t msg_len;

    //printf("\n");

    // ricevo quanti dati
    ret = recv(socket, (void*) &msg_len, sizeof(uint16_t), 0);
    len = ntohs(msg_len);


    if(ret == 0){
        printf("Errore nella recv di quanti dati, l'host remoto si è chiuso\n");
        exit(1);
    }

    if(ret < 0){
        printf("Errore nella recv di quanti dati\n");
        perror("perror --> errno :");
        exit(1);
    }

    if(ret < sizeof(uint16_t)){
        printf("Errore nella recv() della quantità, ricevuti %d dati anziché %d\n", ret, (int)sizeof(uint16_t));
        exit(1);
    }

    //perror("errno nella recv di quanti dati :");

    // ricevo i dati
    ret = recv(socket, (void*)buffer, len,  MSG_WAITALL);

    if(ret == 0){
        printf("Errore nella recv dei dati, l'host remoto si è chiuso\n");
        exit(1);
    }

    if(ret < 0){
        printf("Errore nella recv di dati\n");
        perror("perror --> errno :");
        exit(1);
    }

    if(ret < len){
        printf("Errore nella recv() dei dati, ricevuti %d dati anziché %d\n", ret, len);
        exit(1);
    }

    //perror("errno nella recv del messaggio :");

    buffer[len] = '\0';

    //printf("LOG_DEBUG: RICEVUTO %s\n", buffer);

    //printf("\n");
}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                                  FUNZIONI PER IL SIGNUP
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


// dato un certo username ricava il percorso del suo file di registro
void percorso_file_registro(char *username, char *stringa){
    strncpy(stringa, "/home/studenti/NetBeansProjects/lotto_server/file_registro/", PATH_LEN);
    strncat(stringa, username, strlen(username) + 1);
    strncat(stringa, ".txt", 5);
}


FILE* crea_file_registro(char *username, char *stringa){

    FILE *fd;

    if(username[strlen(username) - 1] == ' ' || username[strlen(username) - 1] == '\n')
        username[strlen(username) - 1] = '\0';

    percorso_file_registro(username, stringa);

    printf("%s\n", stringa);
    fd = fopen(stringa, "w");

    if(fd == NULL){
        perror("Errore nella creazione del file di registro dell'utente\n");
        exit(1);
    }

    printf("Creato il file di registro %s\n", stringa);

    fclose(fd);

    return fd;
}


// ricava il percorso del file consuntivo per username
void percorso_consuntivo(char *username, char *stringa){
    strncpy(stringa, FILE_CONSUNTIVI, PATH_LEN);
    strncat(stringa, username, strlen(username) + 1);
    strncat(stringa, ".txt", 5);
}


FILE* crea_consuntivo(char *username, char *stringa){

    FILE *fd;

    percorso_consuntivo(username, stringa);

    // printf("LOG_DEBUG: File consuntivo: %s\n", stringa);
    fd = fopen(stringa, "w");

    if(fd == NULL){
        perror("Errore nella creazione del file consuntivo dell'utente\n");
        exit(1);
    }

    //printf("Creato il file consuntivo %s\n", stringa);
    fclose(fd);
    return fd;
}


// scrive buffer nel file situato in path, seguito da \n
// richiede anche l'accesso in lettura per poter memorizzare il contenuto
FILE* scrivi_file(char *buffer, const char *path, char* modality){

    FILE *fd;
    int ret = 0;

    fd = fopen(path, modality);
    if(fd == NULL){
        perror("Errore nell'apertura del file");
        printf("%s\n", path);
        exit(1);
    }


    strncat(buffer, "\n", 2);
    ret = fprintf(fd, "%s", buffer);
    if(ret <= 0){
        printf("Valore di ret: %d\n", ret);
        perror("Errore scrittura su file :");
        exit(1);
    }

    fclose(fd);
    return fd;
}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                                  FUNZIONI PER IL LOGIN
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


// cerca username nel file lista_utenti.txt
int trova_username(char *username){

    FILE *fd;

    fd = fopen("/home/studenti/NetBeansProjects/lotto_server/lista_utenti.txt", "r");

    if(fd == NULL){
        perror("Errore nell'apertura del file");
        printf("/home/studenti/NetBeansProjects/lotto_server/lista_utenti.txt\n");
        exit(1);
    }

    while(1){
        int success;
        char *stringa;

        stringa = calloc(CREDENTIAL_LEN, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd, "%s", stringa);

        if(success == EOF){
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            break;
        } else if(strcmp(stringa, username) == 0){
            fclose(fd);
            free(stringa);
            return 1;
        }

        free(stringa);
    }

    fclose(fd);
    return 0;
}


// controlla che la password inserita sia uguale a quella presente nel file di registro
int verifica_password(char *username, char *password){

    FILE *fd;
    char *stringa;
    char *pass;

    stringa = calloc(BUFFER_LENGHT, sizeof(char));
    if(!stringa){
        printf("Memoria esaurita\n");
        exit(1);
    }

    if(username[strlen(username) - 1] == ' ' || username[strlen(username) - 1] == '\n')
        username[strlen(username) - 1] = '\0';

    percorso_file_registro(username, stringa);

    fd = fopen(stringa, "r");
    if(fd == NULL){
        perror("Errore nell'apertura del file di registro dell'utente\n");
        free(stringa);
        exit(1);
    }
    free(stringa);

    pass = calloc(CREDENTIAL_LEN, sizeof(char));
    fscanf(fd, "%s", pass); // la prima informazione nel file  è la password

    if(pass[strlen(pass) - 1] == '\n')
        pass[strlen(pass) - 1] = '\0';

    if(password[strlen(password) - 1] == ' ' || password[strlen(password) - 1] == '\n')
        password[strlen(password) - 1] = '\0';

    fclose(fd);

    if(strcmp(pass, password) == 0){
        free(pass);
        return 1;
    }

    free(pass);
    return 0;
}

// guarda se l'IP è nella lista di quelli bannati
int check_IP(char *saved_IP){

    FILE *fd;
    char *stringa;
    int index;
    char address[INET_ADDRSTRLEN];
    int success;

    index = 0;
    success = 0;

    stringa = calloc(BUFFER_LENGHT, sizeof(char));
    if(!stringa){
        printf("Memoria esaurita\n");
        exit(1);
    }

    fd = fopen(IP_BANNATI, "r");
    if(fd == NULL){
        perror("Errore nell'apertura del file degli IP bannati\n");
        free(stringa);
        exit(1);
    }


    while(1){

        success = fscanf(fd, "%s", stringa);

        if(success == EOF)
            break;

        if(!success)
            break;

        sotto_stringa(stringa, address, &index);

        if(strcmp(saved_IP, address) == 0){
            fclose(fd);
            free(stringa);
            return 1;
        }
    }

    fclose(fd);
    free(stringa);
    return 0;
}


// VANNO CAMBIATI I CICLI IN CICLI INFINITI, LA LETTURA VA DENTRO
void aggiorna_bannati(char *saved_IP){
    FILE *fd_src, *fd_dest;
    int index;
    char *stringa;
    char address[INET_ADDRSTRLEN];
    int res;

    fd_src = fopen(IP_BANNATI, "r");
    if(fd_src == NULL){
        perror("Errore nell'apertura del file degli IP bannati\n");
        exit(1);
    }

    fd_dest = fopen(FILE_BUFFER, "w");
    if(fd_dest == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    while(1){
        int success;
        success = fscanf(fd_src, "%s", stringa);

        if(success == EOF){
            break;
        } else if(!success){
            printf("Errore nella lettura del file IP_BANNATI\n");
            exit(1);
        }

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }

        index = 0;

        sotto_stringa(stringa, address, &index);

        if(strcmp(saved_IP, address) == 0){
            free(stringa);
            continue;
        }

        fprintf(fd_dest, "%s", stringa);
        free(stringa);
    }

    fclose(fd_src);
    fclose(fd_dest);

   // elimino il vecchio file
    res = remove(IP_BANNATI);
    if(res != 0)
      perror("Errore nella rimozione del file");

    // aggiorno
    fd_dest = fopen(IP_BANNATI, "w");
    if(fd_dest == NULL){
        perror("Errore nell'apertura del file degli IP bannati\n");
        exit(1);
    }

    fd_src = fopen(FILE_BUFFER, "r");
    if(fd_src == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    while(1){

        int success;
        success = fscanf(fd_src, "%s", stringa);

        if(success == EOF){
            break;
        } else if(!success){
            printf("Errore nella lettura del file di buffer\n");
            exit(1);
        }

        stringa = calloc(CREDENTIAL_LEN, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }

        index = 0;
        fprintf(fd_dest, "%s", stringa);
        free(stringa);
    }

    fclose(fd_src);
    fclose(fd_dest);

    res = remove(FILE_BUFFER);
    if(res != 0)
      perror("Errore nella rimozione del file");

    //printf("File IP_bannati.txt aggiornato\n");

}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                          FUNZIONI DI UTILITÀ PER LE SCHEDINE
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


void stampa_schedina(struct schedina* punt_schedina){

    int i;
    printf("Contenuto della schedina:\n");

    // printf("quanti_numeri: %d\n", punt_schedina->quanti_numeri);
    printf("Numeri giocati: ");
    for(i = 0; i < punt_schedina->quanti_numeri; i++){
        printf("%d ", punt_schedina->numeri_giocati[i]);
    }
    printf("\n");

    // printf("quanti_ruote: %d\n", punt_schedina->quante_ruote);
    printf("Ruote: ");
    if(punt_schedina->flag_tutte == 's'){
        printf("tutte");
    }
    else {
        for(i = 0; i < punt_schedina->quante_ruote; i++){
            printf("%s ", punt_schedina->ruote[i]);
        }
    }
    printf("\n");

    // printf("quanti_importi: %d\n", punt_schedina->quanti_importi);
    printf("Importi giocati: \n");
    for(i = 0; i < punt_schedina->quanti_importi; i++){
        switch(i){
            case 0:
                printf("Estratto semplice: €%.2f\n", punt_schedina->importi[i]);
                break;
            case 1:
                printf("Ambo: €%.2f\n", punt_schedina->importi[i]);
                break;
            case 2:
                printf("Terno: €%.2f\n", punt_schedina->importi[i]);
                break;
            case 3:
                printf("Quaterna: €%.2f\n", punt_schedina->importi[i]);
                break;
            case 4:
                printf("Cinquina: €%.2f\n", punt_schedina->importi[i]);
                break;
            default:
                printf("Errore nello switch degli importi\n");
                break;
        }
    }
    printf("\n");

}


// attenzione perchè prosegue la lettura da un file già aperto
// salva la schedina nel file di registro dell'utente
void scrivi_schedina(FILE *fd_dest, struct schedina *punt_schedina){

    int i = 0;

    fprintf(fd_dest, "%s\n", "schedina");
    fprintf(fd_dest, "%s\n", "numeri_giocati");
    for(i = 0; i < punt_schedina->quanti_numeri; i++)
        fprintf(fd_dest, "%d\n", punt_schedina->numeri_giocati[i]);

    fprintf(fd_dest, "%s\n", "ruote");
    if(punt_schedina->flag_tutte == 's'){
        fprintf(fd_dest, "%s\n", "tutte");
    } else {
        for(i = 0; i < punt_schedina->quante_ruote; i++)
            fprintf(fd_dest, "%s\n", punt_schedina->ruote[i]);
    }

    fprintf(fd_dest, "%s\n", "importi");
    for(i = 0; i < punt_schedina->quanti_importi; i++){
        // per stampare solo due cifre dopo la virgola
        fprintf(fd_dest, "%.2f\n", punt_schedina->importi[i]);
    }
    // separatore per leggibilità
    fprintf(fd_dest, "%c\n", ' ');
}


// chiama scrivi_schedine e aggiorna correttamente il file di registro
void nuova_schedina(char *username, struct schedina *punt_schedina){

    FILE *fd_src, *fd_dest;
    char *path;
    char *stringa;
    int schedine_attive_presenti;
    int res;

    schedine_attive_presenti = 0;
    path = calloc(BUFFER_LENGHT, sizeof(char));
    percorso_file_registro(username, path);

    fd_src = fopen(path, "r");
    if(fd_src == NULL){
        perror("Errore nell'apertura del file di registro\n");
        free(path);
        exit(1);
    }

    fd_dest = fopen(FILE_BUFFER, "w");
    if(fd_dest == NULL){
        perror("Errore nell'apertura del file buffer\n");
        free(path);
        exit(1);
    }

    // copio il contenuto del file di registro in un file buffer e aggiungo la schedina
    while(1){

        int success;
        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }

        success = fscanf(fd_src, "%s", stringa);

        if(success == EOF){
            if(schedine_attive_presenti == 0){ // è la prima schedina attiva
                fprintf(fd_dest, "%s\n", "SCHEDINE_ATTIVE");
                scrivi_schedina(fd_dest, punt_schedina);

            }
            free(stringa);
            // printf("File di registro letto\n");
            break;
        }

        if(!success){
            free(path);
            free(stringa);
            // printf("Errore nella lettura del file di registro\n");
            exit(1);
        }

        if(strncmp(stringa, "SCHEDINE_ATTIVE", 15) == 0){
            fprintf(fd_dest, "%s\n", stringa);
            schedine_attive_presenti = 1;
            scrivi_schedina(fd_dest, punt_schedina);
            free(stringa);
            continue; // non riscrivo SCHEDINE_ATTIVE

        } else if(strncmp(stringa, "SCHEDINE_GIOCATE", 16) == 0){
            if(schedine_attive_presenti == 0){ // è la prima schedina attiva
                schedine_attive_presenti = 1;
                fprintf(fd_dest, "%s\n", "SCHEDINE_ATTIVE");
                scrivi_schedina(fd_dest, punt_schedina);
            }
        }

        fprintf(fd_dest, "%s\n", stringa);
        free(stringa);
    }

    fclose(fd_src);
    fclose(fd_dest);

    res = remove(path); // elimino il vecchio file di registro
    if(res != 0)
      perror("Errore nella rimozione del file");

    fd_dest = fopen(path, "w"); // riscrivo da capo
    free(path);
    if(fd_dest == NULL){
        perror("Errore nell'apertura del file di registro\n");
        exit(1);
    }

    fd_src = fopen(FILE_BUFFER, "r");
    if(fd_src == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    while(1){
        int success;
        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd_src, "%s", stringa);
        if(success == EOF){
            // printf("Aggiornato il file registro dell'utente\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file di registro\n");
            break;
        } else {
            fprintf(fd_dest, "%s\n", stringa);
        }
        free(stringa);
    }

    fclose(fd_src);
    fclose(fd_dest);
}


int link_wheel(char *parametro){

    if(strcmp(parametro, "bari") == 0)
        return 0;
    if(strcmp(parametro, "cagliari") == 0)
        return 1;
    if(strcmp(parametro, "firenze") == 0)
        return 2;
    if(strcmp(parametro, "genova") == 0)
        return 3;
    if(strcmp(parametro, "milano") == 0)
        return 4;
    if(strcmp(parametro, "napoli") == 0)
        return 5;
    if(strcmp(parametro, "palermo") == 0)
        return 6;
    if(strcmp(parametro, "roma") == 0)
        return 7;
    if(strcmp(parametro, "torino") == 0)
        return 8;
    if(strcmp(parametro, "venezia") == 0)
        return 9;
    if(strcmp(parametro, "nazionale") == 0)
        return 10;

    printf("Errore nella link_wheel\n");
    return -1;
}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                      FUNZIONI PER IL CORRETTO AGGIORNAMENTO DEL SESSION ID
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


// nel file di registro di un utente, elimina la riga contenente il sessionID
// se viene passato NULL, altrimenti aggiorna tale riga con il valore new_sessionID
void aggiorna_sessionID(char *USERNAME_LOGIN, char *new_sessionID){

    FILE *fd_src, *fd_dest;
    char *path;
    int i, res;

    if(USERNAME_LOGIN[0] == '\0'){
        printf("LOG_DEBUG: login non ancora effettuato, impossibile eliminare sessionID\n");
        return;
    }
    path = calloc(PATH_LEN, sizeof(char));
    percorso_file_registro(USERNAME_LOGIN, path);

    fd_src = fopen(path, "r");
    if(fd_src == NULL){
        perror("Errore nell'apertura del file di registro\n");
        free(path);
        exit(1);
    }

    fd_dest = fopen(FILE_BUFFER, "w");
    if(fd_dest == NULL){
        perror("Errore nell'apertura del file buffer\n");
        free(path);
        exit(1);
    }

    i = 0;
    while(1){
        int success;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            free(path);
            exit(1);
        }
        success = fscanf(fd_src, "%s", stringa);

        if(success == EOF){
            // se sono arrivato qui e i == 1 allora è il primo login
            if(i == 1)
                fprintf(fd_dest, "%s\n", new_sessionID);
            //printf("Copiato il file di registro nel file buffer\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file di registro\n");
            break;
        } else {
            // se esiste un sessionID non lo riscrivo (è la seconda riga ed
            // è sempre costituito da 10 caratteri) --> FORSE NON È NECESSARIO,
            // se c'è una seconda riga è per forza il sessionID
            if(i == 1 && strlen(stringa) == 10){
                free(stringa);
                if(new_sessionID != NULL){ // posso anche solo togliere il vecchio sessionID
                    fprintf(fd_dest, "%s\n", new_sessionID);
                    //printf("LOG_DEBUG: AGGIORNAMENTO sessionID, scrivo nel file buffer: %s\n", stringa);
                } else
                    //printf("LOG_DEBUG: parametro NULL, non scrivo nessun valore nel file buffer\n");
                i++;
                continue;
            }
            // printf("LOG_DEBUG: scrivo nel file buffer: %s\n", stringa);
            fprintf(fd_dest, "%s\n", stringa);
        }
        free(stringa);
        i++;
    }

    fclose(fd_src);
    fclose(fd_dest);

    res = remove(path); // elimino il vecchio file di registro
    if(res != 0)
      perror("Errore nella rimozione del file");

    fd_src = fopen(FILE_BUFFER, "r");
    if(fd_src == NULL){
        perror("Errore nell'apertura del file buffer\n");
        free(path);
        exit(1);
    }

    fd_dest = fopen(path, "w");
    if(fd_dest == NULL){
        perror("Errore nell'apertura del file di registro\n");
        free(path);
        exit(1);
    }

    free(path);

    while(1){
        int success;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd_src, "%s", stringa);

        if(success == EOF){
            //printf("Copiato il file di buffer nel file di registro\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file di registro\n");
            break;
        } else {
            // printf("LOG_DEBUG: scrivo nel file di registro: %s\n", stringa);
            fprintf(fd_dest, "%s\n", stringa);
        }
        free(stringa);
    }

    fclose(fd_src);
    fclose(fd_dest);

}


// usata dal client per inviare il proprio sessionID
int richiesta_autenticazione(char *sessionID, int socket){
    char *risposta;
    // trasmetto l'id dell'utente
    //printf("LOG_DEBUG: sessionID: %s\n", sessionID);
    trasmetti(sessionID, socket);

    risposta = calloc(RESPONSE_LEN, sizeof(char));
    ricevi(risposta, socket);

    if(strcmp(risposta, "Login non effettuato") == 0){
        printf("Eseguire prima la fase di login\n");
        free(risposta);
        return 0;
    }

    if(strcmp(risposta, "sessionID non rilevato") == 0){
        printf("Errore in ricezione sul corretto scambio di sessionID\n");
        free(risposta);
        exit(1);
    }

    if(strcmp(risposta, "sessionID corretto") == 0){
        printf("Autenticazione avvenuta con successo\n");
        free(risposta);
        return 1;
    }

    // esecuzione inaspettata
    return 0;
}


// funzione per la risposta all'autenticazione basata su sessionID
int risposta_autenticazione(char *sessionID, char *receivedID, int socket){
    // ricevo il session ID e controllo
    ricevi(receivedID, socket);

    if(sessionID[0] == '\0'){
        trasmetti("Login non effettuato", socket);
        return 0;
    }

    // printf("LOG_DEBUG: sessionID: %s\n", sessionID);
    // printf("LOG_DEBUG: receivedID: %s\n", receivedID);
    if(strncmp(receivedID, sessionID, 10) != 0){
        trasmetti("sessionID non rilevato", socket);
        printf("receivedID %s non corretto\n", receivedID);
        printf("sessiond ID: %s\n", sessionID);
        printf("Termino con un errore\n");
        exit(1);
    }

    trasmetti("sessionID corretto", socket);
    return 1;
}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                      FUNZIONI PER LO SCAMBIO E LA VISUALIZZAZIONE DELLE SCHEDINE
 *                                                  (comando !vedi_giocate tipo)
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


// usata lato server
void invia_schedine_attive(char *USERNAME_LOGIN, int socket){
    char *path, *stringa;
    FILE *fd;
    int stampa, ruote, numeri_giocati, importi;
    stampa = 0;
    ruote = 0;
    numeri_giocati = 0;
    importi = 0;

    path = calloc(PATH_LEN, sizeof(char));
    if(!path){
        printf("Memoria esaurita\n");
        exit(1);
    }

    percorso_file_registro(USERNAME_LOGIN, path);
    fd = fopen(path, "r");
    if(fd == NULL){
        printf("Errore nell'apertura del file di registro\n");
        trasmetti("Errore", socket);
        free(path);
        exit(1);
    }

    while(1){
        int success;
        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            trasmetti("Errore", socket);
            exit(1);
        }
        success = fscanf(fd, "%s", stringa);
        if(success == EOF){
            //printf("Terminata la lettura del file di registro\n");
            trasmetti("Lettura terminata", socket);
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file di registro\n");
            trasmetti("Errore", socket);
            break;
        } else if(strcmp(stringa, "SCHEDINE_ATTIVE") == 0) {
            stampa = 1;
        } else if(strcmp(stringa, "SCHEDINE_GIOCATE") == 0){
            stampa = 0;
            trasmetti("Fine giocate attive", socket);
            free(stringa);
            break;
        }

        if(stampa){
            if(strcmp(stringa, "schedina") == 0){
                importi = 0;
                trasmetti(stringa, socket);
                free(stringa);
                continue;
            }
            else if(strcmp(stringa, "numeri_giocati") == 0){
                numeri_giocati = 1;
                trasmetti(stringa, socket);
                free(stringa);
                continue;
            }
            else if(strcmp(stringa, "ruote") == 0){
                numeri_giocati = 0;
                ruote = 1;
                trasmetti(stringa, socket);
                free(stringa);
                continue;
            }
            else if(strcmp(stringa, "importi") == 0){
                ruote = 0;
                importi = 1;
                trasmetti(stringa, socket);
                free(stringa);
                continue;
            }
        }

        if(ruote)
            trasmetti(stringa, socket);


        if(numeri_giocati)
            trasmetti(stringa, socket);


        if(importi)
            trasmetti(stringa, socket);

        free(stringa);
    }

    fclose(fd);
}

// controparte di invia_schedine_attive chiamata dal client
void ricevi_schedine_attive(int socket){
    char *risposta;
    char *nomi_giocate[5] = {"estratto", "ambo", "terno", "quaterna", "cinquina" };
    int i, j, ruote, importi, numeri_giocati;

    ruote = 0;
    importi = 0;
    numeri_giocati = 0;
    i = 0;
    j = 0;

    printf("\nSchedine attualmente attive: \n");
    while(1){
        risposta = calloc(RESPONSE_LEN, sizeof(char));

        ricevi(risposta, socket);

        if(strcmp(risposta, "Errore") == 0){
            printf("Ricevuto un errore\n");
            free(risposta);
            exit(1);
        }

        if(strcmp(risposta, "Lettura terminata") == 0){
            // printf("\nTerminata la fase di ricezione delle schedine attive\n");
            free(risposta);
            return;
        }

        if(strcmp(risposta, "schedina") == 0){
            importi = 0;
            j = 0;
            printf("\n = = = = = = Nuova schedina = = = = = = \n");
            i++;
            printf("%d) ", i);
            free(risposta);
            continue;
        }

        if(strcmp(risposta, "numeri_giocati") == 0){
            printf("\nNumeri giocati: ");
            numeri_giocati = 1;
            free(risposta);
            continue;
        }

        if(strcmp(risposta, "ruote") == 0){
            printf("\nRuote: ");
            numeri_giocati = 0;
            ruote = 1;
            free(risposta);
            continue;
        }

        if(strcmp(risposta, "importi") == 0){
            printf("\nImporti:");
            ruote = 0;
            importi = 1;
            free(risposta);
            continue;
        }

        if(strcmp(risposta, "Fine giocate attive") == 0){
            //printf("\nTerminata la fase di ricezione delle schedine attive\n");
            free(risposta);
            return;
        }

        if(ruote)
            printf("%s ", risposta);

        if(numeri_giocati)
            printf("%d ", atoi(risposta));

        if(importi){
            printf(" %.2f %s *", atof(risposta), nomi_giocate[j]);
            j++;
        }

        free(risposta);
    }
}

// codice ripreso da invia_schedine_attive
void invia_schedine_giocate(char *USERNAME_LOGIN, int socket){
    char *path, *stringa;
    FILE *fd;
    int stampa, ruote, numeri_giocati, importi;
    stampa = 0;
    ruote = 0;
    numeri_giocati = 0;
    importi = 0;

    path = calloc(PATH_LEN, sizeof(char));
    if(!path){
        printf("Memoria esaurita\n");
        exit(1);
    }

    percorso_file_registro(USERNAME_LOGIN, path);
    fd = fopen(path, "r");
    if(fd == NULL){
        printf("Errore nell'apertura del file di registro\n");
        trasmetti("Errore", socket);
        free(path);
        exit(1);
    }

    while(1){
        int success;
        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            trasmetti("Errore", socket);
            exit(1);
        }
        success = fscanf(fd, "%s", stringa);
        if(success == EOF){
            //printf("Terminata la lettura del file di registro\n");
            trasmetti("Lettura terminata", socket);
            free(stringa);
            break;
        }
        else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file di registro\n");
            trasmetti("Errore", socket);
            break;
        }
        else if(strcmp(stringa, "SCHEDINE_GIOCATE") == 0) {
            stampa = 1;
        }

        if(stampa){
            if(strcmp(stringa, "schedina") == 0){
                importi = 0;
                trasmetti(stringa, socket);
                free(stringa);
                continue;
            }
            else if(strcmp(stringa, "numeri_giocati") == 0){
                numeri_giocati = 1;
                trasmetti(stringa, socket);
                free(stringa);
                continue;
            }
            else if(strcmp(stringa, "ruote") == 0){
                numeri_giocati = 0;
                ruote = 1;
                trasmetti(stringa, socket);
                free(stringa);
                continue;
            }
            else if(strcmp(stringa, "importi") == 0){
                ruote = 0;
                importi = 1;
                trasmetti(stringa, socket);
                free(stringa);
                continue;
            }
        }

        if(ruote)
            trasmetti(stringa, socket);

        if(numeri_giocati)
            trasmetti(stringa, socket);


        if(importi)
            trasmetti(stringa, socket);


        free(stringa);
    }

    fclose(fd);
}

// analogo a ricevi_schedine_attive
void ricevi_schedine_giocate(int socket){
    char *risposta;
    char *nomi_giocate[5] = {"estratto", "ambo", "terno", "quaterna", "cinquina" };
    int i, j, ruote, importi, numeri_giocati;

    ruote = 0;
    importi = 0;
    numeri_giocati = 0;
    i = 0;
    j = 0;
    printf("\nSchedine precedentemente giocate:\n");
    while(1){
        risposta = calloc(RESPONSE_LEN, sizeof(char));
        ricevi(risposta, socket);

        if(strcmp(risposta, "Errore") == 0){
            printf("Ricevuto un errore\n");
            free(risposta);
            exit(1);
        }
        if(strcmp(risposta, "Lettura terminata") == 0){
            //printf("\nTerminata la fase di ricezione delle schedine giocate\n");
            free(risposta);
            return;
        }

        if(strcmp(risposta, "schedina") == 0){
            importi = 0;
            j = 0;
            printf("\n = = = = = = Nuova schedina = = = = = = \n");
            i++;
            printf("%d) ", i);
            free(risposta);
            continue;
        }

        if(strcmp(risposta, "numeri_giocati") == 0){
            printf("\nNumeri giocati: ");
            numeri_giocati = 1;
            free(risposta);
            continue;
        }

        if(strcmp(risposta, "ruote") == 0){
            printf("\nRuote: ");
            numeri_giocati = 0;
            ruote = 1;
            free(risposta);
            continue;
        }

        if(strcmp(risposta, "importi") == 0){
            printf("\nImporti:");
            ruote = 0;
            importi = 1;
            free(risposta);
            continue;
        }

        if(ruote)
            printf("%s ", risposta);

        if(numeri_giocati)
            printf("%d ", atoi(risposta));

        if(importi){
            printf(" * %.2f %s", atof(risposta), nomi_giocate[j]);
            j++;
        }

        free(risposta);
    }
}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                      FUNZIONI PER SALVATAGGIO E STAMPA DI UN'ESTRAZIONE
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


void stampa_estrazione(struct estrazione *punt_estrazione){

    char *RUOTE_LOTTO[11] = { "bari", "cagliari", "firenze", "genova", "milano", "napoli", "palermo", "roma", "torino", "venezia", "nazionale" };
    int i, j;
    printf(" # # # Ultima estrazione effettuata # # #\n");
    j = 0;
    for(i = 0; i < 11 * 5; i++){
        if(i % 5 == 0){
            printf("\n%s: ", RUOTE_LOTTO[j]);
            j++;
        }
        printf("%d ", punt_estrazione->numeri_estratti[i]);
    }
    printf("\n");
    printf("Data: %s\n", ctime(&punt_estrazione->istante_estrazione));

}


// salva l'estrazione di un file delle estrazioni, inserendo l'ultima estrazione effettuata in cima
void scrivi_estrazione(struct estrazione *punt_estrazione){

    FILE *fd_estrazioni, *fd_buffer;
    char *RUOTE_LOTTO[11] = { "bari", "cagliari", "firenze", "genova", "milano", "napoli", "palermo", "roma", "torino", "venezia", "nazionale" };
    int i, j, res;

    fd_estrazioni = fopen(FILE_ESTRAZIONI, "r");
    if(fd_estrazioni == NULL){
        perror("Errore nell'apertura del file delle estrazioni\n");
        exit(1);
    }

    fd_buffer = fopen(FILE_BUFFER, "w");
    if(fd_buffer == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    // scrivo l'ultima estrazione in cima al file buffer
    fprintf(fd_buffer, "%s\n", "estrazione");
    j = 0;
    for(i = 0; i < 11 * 5; i++){
        if(i % 5 == 0){
            fprintf(fd_buffer, "\n%s ", RUOTE_LOTTO[j]);
            j++;
        }
        fprintf(fd_buffer, "%d\n", punt_estrazione->numeri_estratti[i]);
    }
    // fprintf(fd_buffer, "\nData: %s\n", ctime(&punt_estrazione->istante_estrazione));

    while(1){
        int success;
        char *stringa;
        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd_estrazioni, "%s", stringa);

        if(success == EOF){
            //printf("Copiato il file_estrazioni nel file buffer\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file_estrazioni\n");
            break;
        } else {
            fprintf(fd_buffer, "%s\n", stringa);
        }
        free(stringa);
    }

    fclose(fd_estrazioni);
    fclose(fd_buffer);

    res = remove(FILE_ESTRAZIONI); // elimino il vecchio file
    if(res != 0)
      perror("Errore nella rimozione del file_estrazioni");

    // adesso ricopio dal file buffer al file estrazioni
    fd_buffer = fopen(FILE_BUFFER, "r");
    if(fd_buffer == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    fd_estrazioni = fopen(FILE_ESTRAZIONI, "w");
    if(fd_estrazioni == NULL){
        perror("Errore nell'apertura del file_estrazioni\n");
        exit(1);
    }

    while(1){
        int success;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd_buffer, "%s", stringa);

        if(success == EOF){
            //printf("Copiato il file di buffer nel nuovo file_estrazioni\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file_estrazioni\n");
            break;
        } else {
            // printf("LOG_DEBUG: scrivo nel file_estrazioni: %s\n", stringa);
            fprintf(fd_estrazioni, "%s\n", stringa);
        }
        free(stringa);
    }

    res = remove(FILE_BUFFER);
    if(res != 0)
      perror("Errore nella rimozione del file");

    fclose(fd_buffer);
    fclose(fd_estrazioni);

}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                      FUNZIONI PER IL CONTROLLO DELLE VINCITE
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


// file di registro già aperto dai chiamanti
void disattiva_schedine(char *path){
    FILE *fd_buffer3, *fd_buffer, *file_registro;
    int stampa;
    int schedine_giocate;
    int schedine_attive;
    schedine_giocate = 0;
    schedine_attive = 0;
    stampa = 0;

    fd_buffer3 = fopen(FILE_BUFFER3, "w");
    if(fd_buffer3 == NULL){
        perror("Errore nell'apertura del file buffer3\n");
        exit(1);
    }

    fd_buffer = fopen(FILE_BUFFER, "w");
    if(fd_buffer == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }
    file_registro = fopen(path, "r");
    if(file_registro == NULL){
        perror("Errore nell'aggiornamento del file di registro\n");
        exit(1);
    }

    while(1){
        int success;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            fclose(file_registro);
            exit(1);
        }
        success = fscanf(file_registro, "%s", stringa);

        if(success == EOF){
            // printf("File di registro letto\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento di registro\n");
            fclose(file_registro);
            break;
        } else if(strcmp(stringa, "SCHEDINE_GIOCATE") == 0) {
            // printf("LOG_DEBUG: disattiva_schedine(), trovate schedine giocate\n");
            stampa = 0;
            schedine_giocate = 1;
        } else if(strcmp(stringa, "SCHEDINE_ATTIVE") == 0){
            // printf("LOG_DEBUG: disattiva_schedine(), trovate schedine attive\n");
            schedine_attive = 1;
            stampa = 1;
            free(stringa);
            continue;
        }

        // scrivo nel file buffer3 le schedine che devono diventare "giocate"
        if(stampa)
            fprintf(fd_buffer3, "%s\n", stringa);
        else
            fprintf(fd_buffer, "%s\n", stringa);

        free(stringa);
    }

    fclose(fd_buffer);
    fclose(fd_buffer3);
    fclose(file_registro);

    // non erano presenti schedine attive
    if(schedine_attive == 0){
        printf("Errore, non sono presenti schedine attive\n"); // errore perchè in base a come viene chiamata devono esserci
        exit(1);
    }

    // adesso apro in append il file buffer e ci scrivo le schedine che diventano giocate (scritte in file buffer3)
    fd_buffer = fopen(FILE_BUFFER, "a");
    if(fd_buffer == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    fd_buffer3 = fopen(FILE_BUFFER3, "r");
    if(fd_buffer3 == NULL){
        perror("Errore nell'apertura del file buffer3\n");
        exit(1);
    }

    if(schedine_giocate == 0){ // inserisco le prime schedine giocate
        fprintf(fd_buffer, "%s\n", "SCHEDINE_GIOCATE");
    }

    while(1){
        int success;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd_buffer3, "%s", stringa);

        if(success == EOF){
            // printf("Copiato il contenuto di buffer3 nel file buffer\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file di registro per la disattivazione delle schedine\n");
            break;
        } else {
            fprintf(fd_buffer, "%s\n", stringa);
        }
        free(stringa);
    }

    fclose(fd_buffer3);
    fclose(fd_buffer);

    fd_buffer = fopen(FILE_BUFFER, "r");
    if(fd_buffer == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    file_registro = fopen(path, "w");
    if(file_registro == NULL){
        perror("Errore nell'aggiornamento del file di registro\n");
        exit(1);
    }
    // riscrivo il file di registro
    while(1){
        int success;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }

        success = fscanf(fd_buffer, "%s", stringa);

        if(success == EOF){
            // printf("Copiato il file di buffer nel nuovo file di registro, aggiornamento schedine riuscito\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file di registro per la disattivazione delle schedine\n");
            break;
        } else {
            fprintf(file_registro, "%s\n", stringa);
        }
        free(stringa);
    }

    fclose(file_registro);
    fclose(fd_buffer);
}

// somma gli importi delle nuove vincite e scrive quest'ultime in coda
void aggiorna_consuntivo(char *username, struct estrazione *punt_estrazione, double vincita_estratto_totale, double vincita_ambo_totale, double vincita_terno_totale, double vincita_quaterna_totale, double vincita_cinquina_totale){

    char *path;
    int res;
    FILE *fd_consuntivo, *fd_buffer, *fd_buffer2;
    int prima_schedina = 1;
    struct tm *data_estrazione;

    if(vincita_estratto_totale == 0 && vincita_ambo_totale == 0 && vincita_terno_totale == 0 && vincita_quaterna_totale == 0 && vincita_cinquina_totale == 0){
        printf("Non ci sono vincite, nulla da aggiornare nel file consuntivo\n");
        return;
    }
    path = calloc(BUFFER_LENGHT, sizeof(char));
    percorso_consuntivo(username, path);
    // printf("LOG_DEBUG: File consuntivo: %s\n", path);

    // PRIMA FASE, LEGGO IL CONSUNTIVO E LO COPIO DEL FILE BUFFER, AGGIORNANDOLO (gestisco anche il caso in cui è vuoto)
    fd_consuntivo = fopen(path, "r");
    if(fd_consuntivo == NULL){
        perror("Errore nell'apertura del file consuntivo dell'utente\n");
        exit(1);
    }

    fd_buffer = fopen(FILE_BUFFER, "w");
    if(fd_buffer == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    while(1){
        int success;
        char *stringa;
        double vecchio_totale = 0;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd_consuntivo, "%s", stringa);

        if(success == EOF){
            if(prima_schedina){
                // printf("LOG_DEBUG: prima schedina nel file consuntivo\n");
                fprintf(fd_buffer, "%s\n", "ESTRATTO");
                fprintf(fd_buffer, "%lf\n", vincita_estratto_totale);
                fprintf(fd_buffer, "%s\n", "AMBO");
                fprintf(fd_buffer, "%lf\n", vincita_ambo_totale);
                fprintf(fd_buffer, "%s\n", "TERNO");
                fprintf(fd_buffer, "%lf\n", vincita_terno_totale);
                fprintf(fd_buffer, "%s\n", "QUATERNA");
                fprintf(fd_buffer, "%lf\n", vincita_quaterna_totale);
                fprintf(fd_buffer, "%s\n", "CINQUINA");
                fprintf(fd_buffer, "%lf\n", vincita_cinquina_totale);
            }
            // printf("Letto il file consuntivo dell'utente e aggiornato nel file buffer\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del consuntivo\n");
            break;
        } else if(strcmp(stringa, "ESTRATTO") == 0){
            prima_schedina = 0;
            fprintf(fd_buffer, "%s\n", stringa);
            free(stringa);

            success = fscanf(fd_consuntivo, "%lf", &vecchio_totale);
            vincita_estratto_totale += vecchio_totale;
            fprintf(fd_buffer, "%lf\n", vincita_estratto_totale);
            continue;
        } else if(strcmp(stringa, "AMBO") == 0){
            fprintf(fd_buffer, "%s\n", stringa);
            free(stringa);

            success = fscanf(fd_consuntivo, "%lf", &vecchio_totale);
            vincita_ambo_totale += vecchio_totale;
            fprintf(fd_buffer, "%lf\n", vincita_ambo_totale);
            continue;
        } else if(strcmp(stringa, "TERNO") == 0){
            fprintf(fd_buffer, "%s\n", stringa);
            free(stringa);

            success = fscanf(fd_consuntivo, "%lf", &vecchio_totale);
            vincita_terno_totale += vecchio_totale;
            fprintf(fd_buffer, "%lf\n", vincita_terno_totale);
            continue;
        } else if(strcmp(stringa, "QUATERNA") == 0){
            fprintf(fd_buffer, "%s\n", stringa);
            free(stringa);

            success = fscanf(fd_consuntivo, "%lf", &vecchio_totale);
            vincita_quaterna_totale += vecchio_totale;
            fprintf(fd_buffer, "%lf\n", vincita_quaterna_totale);
            continue;
        } else if(strcmp(stringa, "CINQUINA") == 0){
            fprintf(fd_buffer, "%s\n", stringa);
            free(stringa);

            success = fscanf(fd_consuntivo, "%lf", &vecchio_totale);
            vincita_cinquina_totale += vecchio_totale;
            fprintf(fd_buffer, "%lf\n", vincita_cinquina_totale);
            continue;
        }

        fprintf(fd_buffer, "%s\n", stringa);
        free(stringa);
    }

    fclose(fd_consuntivo);
    fclose(fd_buffer);


    // ADESSO ELIMINO IL VECCHIO FILE CONSUNTIVO
    res = remove(path);
    if(res != 0)
        perror("Errore nella rimozione del vecchio file consuntivo\n");

    // POSSO RICOPIARE IL CONTENUTO DEL BUFFER (AGGIORNATO) NEL NUOVO CONSUNTIVO
    fd_consuntivo = fopen(path, "w");
    if(fd_consuntivo == NULL){
        perror("Errore nell'apertura del file consuntivo dell'utente\n");
        exit(1);
    }


    fd_buffer = fopen(FILE_BUFFER, "r");
    if(fd_buffer == NULL){
        perror("Errore nell'apertura del file buffer\n");
        exit(1);
    }

    while(1){
        int success;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd_buffer, "%s", stringa);

        if(success == EOF){
            // printf("Copiato il file di buffer nel nuovo file_consuntivo\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file consuntivo\n");
            break;
        } else {
            // printf("LOG_DEBUG: scrivo nel file_estrazioni: %s\n", stringa);
            fprintf(fd_consuntivo, "%s\n", stringa);
        }
        free(stringa);
    }

    fclose(fd_consuntivo);
    fclose(fd_buffer);

    // adesso devo aggiungere in append le nuove vincite

    // printf("LOG_DEBUG: procedo con la scrittura delle nuove vincite nel file consuntivo\n");

    // INFINE AGGIUNGO IN FONDO LE NUOVE VINCITE
    fd_consuntivo = fopen(path, "a");
    if(fd_consuntivo == NULL){
        perror("Errore nell'apertura del file consuntivo dell'utente\n");
        exit(1);
    }
    free(path);

    fd_buffer2 = fopen(FILE_BUFFER2, "r");
    if(fd_buffer2 == NULL){
        perror("Errore nel'apertura del file buffer2.txt\n");
        exit(1);
    }

    // la prima informazione è la data dell'estrazione
    //printf("LOG_DEBUG: scrivo la data %s\n", ctime(&punt_estrazione->istante_estrazione));
    //fprintf(fd_consuntivo, "Data: %s", ctime(&punt_estrazione->istante_estrazione));

    data_estrazione = localtime(&punt_estrazione->istante_estrazione);
    fprintf(fd_consuntivo, "%s\n", "data");
    fprintf(fd_consuntivo,"%d/%d-%d:%d:%d\n", data_estrazione->tm_mday, data_estrazione->tm_mon, data_estrazione->tm_hour, data_estrazione->tm_min, data_estrazione->tm_sec);

    while(1){
        int success;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            exit(1);
        }
        success = fscanf(fd_buffer2, "%s", stringa);

        if(success == EOF){
            // printf("Scritto in append il file di buffer2 nel nuovo file_consuntivo\n");
            free(stringa);
            break;
        } else if(!success){
            free(stringa);
            printf("Errore nell'aggiornamento del file consuntivo, scrittura di buffer2 fallita\n");
            break;
        } else if(strcmp(stringa, "vincita") == 0) {

            char ruota[WHEEL_LENGHT];
            int quante_vincite;
            int numeri_vincenti[10];
            char tipo_giocata[9];
            double vincita;
            int i;

            fprintf(fd_consuntivo, "%s\n", stringa); // "vincita"

            fscanf(fd_buffer2, "%s", ruota);
            fprintf(fd_consuntivo, "%s\n", ruota);

            fscanf(fd_buffer2, "%d", &quante_vincite);
            fprintf(fd_consuntivo, "%d\n", quante_vincite);

            for(i = 0; i < quante_vincite; i++){
                fscanf(fd_buffer2, "%d", &numeri_vincenti[i]);
                fprintf(fd_consuntivo, "%d\n", numeri_vincenti[i]);
            }

            fscanf(fd_buffer2, "%s", tipo_giocata);
            fprintf(fd_consuntivo, "%s\n", tipo_giocata);

            fscanf(fd_buffer2, "%lf", &vincita);
            fprintf(fd_consuntivo, "%lf\n", vincita);

        }

        free(stringa);
    }

    fclose(fd_consuntivo);
    fclose(fd_buffer2);
}


// funzione per accedere correttamente all'array numeri_estratti
int ruota_to_index(char ruota[]){

    if(strcmp(ruota, "bari") == 0)
        return 0;
    if(strcmp(ruota, "cagliari") == 0)
        return 1;
    if(strcmp(ruota, "firenze") == 0)
        return 2;
    if(strcmp(ruota, "genova") == 0)
        return 3;
    if(strcmp(ruota, "milano") == 0)
        return 4;
    if(strcmp(ruota, "napoli") == 0)
        return 5;
    if(strcmp(ruota, "palermo") == 0)
        return 6;
    if(strcmp(ruota, "roma") == 0)
        return 7;
    if(strcmp(ruota, "torino") == 0)
        return 8;
    if(strcmp(ruota, "venezia") == 0)
        return 9;
    if(strcmp(ruota, "nazionale") == 0)
        return 10;

    return -1;
}


// calcolo del fattoriale in modo ricorsivo
int fattoriale(int n){
  if (n < 0) return -1; /* Fattoriale non e' definito per interi negativi! */
  if (n == 0) return 1;
  else return n*fattoriale(n-1);
}


// definizione combinazioni semplici
double combinazioni_semplici(int n, int k){

    return fattoriale(n) / (fattoriale(n-k) * fattoriale(k));
}


// funzione che controlla se una data schedina contiene delle vincite per l'ultima estrazione
double check_vincite(struct schedina *punt_schedina, struct estrazione *punt_estrazione, char *tipo_giocata){
    int i, j;
    char *RUOTE_LOTTO[11] = { "bari", "cagliari", "firenze", "genova", "milano", "napoli", "palermo", "roma", "torino", "venezia", "nazionale" };
    double vincita_totale = 0;

    // printf("LOG_DEBUG: punt_schedina->quante_ruote: %d\n", punt_schedina->quante_ruote);
    if(punt_schedina->quante_ruote == 11){
        for(i = 0; i < 11; i++){
            strncpy(punt_schedina->ruote[i], RUOTE_LOTTO[i], strlen(RUOTE_LOTTO[i]) + 1);
            // printf("LOG_DEBUG: copiato %s, in %s, strlen + 1: %d\n", RUOTE_LOTTO[i], punt_schedina->ruote[i], (int)strlen(RUOTE_LOTTO[i]) + 1);
        }
    }


    // controllo per ogni ruota
    for(i = 0; i < punt_schedina->quante_ruote; i++){
        int numeri_vincenti[10]; // al massimo 10, sovradimensionato in base alle combinazioni semplici dei casi
        int numeri_su_ruota[5]; // su ogni ruota sono estratti 5 numeri
        int quante_vincite = 0; // totale di estratti/ambi/terni ecc. indovinati per ogni ruota
        double vincita_su_ruota = 0; // vincita sulla ruota corrente
        int index = 0;

        // funzione per accedere correttamente all'array numeri_estratti
        index = ruota_to_index(punt_schedina->ruote[i]);

        // printf("LOG_DEBUG: index: %d\n", index);
        printf("LOG_DEBUG: ruota: %s\n", RUOTE_LOTTO[index]);
        printf("LOG-DEBUG: numeri_su_ruota: ");
        for(j = 0; j < 5; j++){
            numeri_su_ruota[j] = punt_estrazione->numeri_estratti[index * 5 + j];
            printf("%d ", numeri_su_ruota[j]);
        }
        printf("\n");



        /*
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
         *
         *
         *                                                   E S T R A T T O
         *
         *
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
        */
        if(strcmp(tipo_giocata, "ESTRATTO") == 0){
            if(punt_schedina->importi[0] == 0){
                printf("Non è stato giocato l'estratto\n");
                return vincita_totale;
            }

            for(j = 0; j < punt_schedina->quanti_numeri; j++){
                // salvo i numeri che vincono e quanti sono
                if(numeri_su_ruota[0] == punt_schedina->numeri_giocati[j]){
                    numeri_vincenti[quante_vincite] = punt_schedina->numeri_giocati[j];
                    quante_vincite++;
                } else if(numeri_su_ruota[1] == punt_schedina->numeri_giocati[j]){
                    numeri_vincenti[quante_vincite] = punt_schedina->numeri_giocati[j];
                    quante_vincite++;
                } else if(numeri_su_ruota[2] == punt_schedina->numeri_giocati[j]){
                    numeri_vincenti[quante_vincite] = punt_schedina->numeri_giocati[j];
                    quante_vincite++;
                } else if(numeri_su_ruota[3] == punt_schedina->numeri_giocati[j]){
                    numeri_vincenti[quante_vincite] = punt_schedina->numeri_giocati[j];
                    quante_vincite++;
                } else if(numeri_su_ruota[4] == punt_schedina->numeri_giocati[j]){
                    numeri_vincenti[quante_vincite] = punt_schedina->numeri_giocati[j];
                    quante_vincite++;
                }
            }
            // il numero di estratti generabili è pari al numero di numeri giocati
            vincita_su_ruota = (( (VINCITA_ESTRATTO / punt_schedina->quante_ruote) / punt_schedina->quanti_numeri ) * quante_vincite) * punt_schedina->importi[0];
            vincita_totale += vincita_su_ruota;

            printf("LOG_DEBUG: vincita_su_ruota: %.3f\n", vincita_su_ruota);

            if(vincita_su_ruota != 0){
                FILE *fd_tmp;

                printf("LOG_DEBUG: trovati %d estratti sulla ruota %s, vincita %.3f\n", quante_vincite, punt_schedina->ruote[i], vincita_su_ruota);
                fd_tmp = fopen(FILE_BUFFER2, "a");
                if(fd_tmp == NULL){
                    printf("Errore nell'apertura del file buffer2\n");
                    exit(1);
                }
/*
                printf("LOG_DEBUG: controllo del contenuto dei numeri vincenti: \n");
                for(j = 0; j < quante_vincite; j++){
                    printf("numeri_vincenti[%d] = %d\n", j, numeri_vincenti[j]);
                }
*/

                fprintf(fd_tmp, "%s\n", "vincita");
                fprintf(fd_tmp, "%s\n", punt_schedina->ruote[i]);
                fprintf(fd_tmp, "%d\n", quante_vincite);

                for(j = 0; j < quante_vincite; j++){
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j]);
                }

                // fprintf(fd_tmp, "%s", " >>  ");
                fprintf(fd_tmp, "%s\n", "Estratto");
                fprintf(fd_tmp, "%.3f\n", vincita_su_ruota);

                fclose(fd_tmp);
            }
        }
        /*
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
         *
         *
         *                                                       A M B O
         *
         *
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
        */
        else if(strcmp(tipo_giocata, "AMBO") == 0){
            int k;
            if(punt_schedina->importi[1] == 0){
                printf("Non è stato giocato l'ambo\n");
                return vincita_totale;
            }

            for(j = 0; j < punt_schedina->quanti_numeri; j++){
                int primo_numero = 0;
                int secondo_numero = 0;
                if( punt_schedina->numeri_giocati[j] == numeri_su_ruota[0] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[1] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[2] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[3] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[4] ){

                    primo_numero = punt_schedina->numeri_giocati[j];
                    // printf("LOG_DEBUG: primo_numero: %d\n", primo_numero);
                    // guardo se formo un ambo con i numeri *successivi*
                    for(k = j + 1; k < punt_schedina->quanti_numeri; k++){
                        if( punt_schedina->numeri_giocati[k] == numeri_su_ruota[0] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[1] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[2] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[3] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[4] ){

                            secondo_numero = punt_schedina->numeri_giocati[k];
                            // printf("LOG_DEBUG: secondo_numero: %d\n", secondo_numero);
                            // ho trovato un ambo, lo salvo
                            numeri_vincenti[quante_vincite] = primo_numero;
                            numeri_vincenti[quante_vincite + 1] = secondo_numero;
                            quante_vincite += 2; // per accedere correttamente all'array, in realtà la vincita è 1
                        }
                    }
                }
            }

            quante_vincite /= 2; // per il motivo scritto sopra
            vincita_su_ruota = (( (VINCITA_AMBO / punt_schedina->quante_ruote) / combinazioni_semplici(punt_schedina->quanti_numeri, 2) ) * quante_vincite) * punt_schedina->importi[1];
            printf("LOG_DEBUG: vincita_su_ruota: %.3f\n", vincita_su_ruota);
            vincita_totale += vincita_su_ruota;

            // scrittura su file
            if(vincita_su_ruota != 0){
                FILE *fd_tmp;
                printf("LOG_DEBUG: trovati %d ambi sulla ruota %s, vincita %.3f\n", quante_vincite, punt_schedina->ruote[i], vincita_su_ruota);
                fd_tmp = fopen(FILE_BUFFER2, "a");
                if(fd_tmp == NULL){
                    printf("Errore nell'apertura del file buffer2\n");
                    exit(1);
                }
/*
                printf("LOG_DEBUG: controllo del contenuto dei numeri vincenti: \n");
                // per 2 perchè parliamo degli ambi
                for(j = 0; j < quante_vincite * 2; j += 2){
                    printf("numeri_vincenti[%d] = %d\n", j, numeri_vincenti[j]);
                    printf("numeri_vincenti[%d] = %d\n", j + 1, numeri_vincenti[j+1]);
                }
*/

                fprintf(fd_tmp, "%s\n", "vincita");
                fprintf(fd_tmp, "%s\n", punt_schedina->ruote[i]);
                fprintf(fd_tmp, "%d\n", quante_vincite * 2);
                // per 2 perchè parliamo degli ambi
                for(j = 0; j < quante_vincite * 2; j += 2){
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 1]);
                }

                //fprintf(fd_tmp, "%s", " >>  ");
                fprintf(fd_tmp, "%s\n", "Ambo");
                fprintf(fd_tmp, "%.3f\n", vincita_su_ruota);

                fclose(fd_tmp);
            }
        }
        /*
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
         *
         *
         *                                                       T E R N O
         *
         *
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
        */
        else if(strcmp(tipo_giocata, "TERNO") == 0){
            int k, z;
            if(punt_schedina->importi[2] == 0){
                printf("Non è stato giocato il terno\n");
                return vincita_totale;
            }

            for(j = 0; j < punt_schedina->quanti_numeri; j++){
                int primo_numero = 0;
                int secondo_numero = 0;
                int terzo_numero = 0;
                if( punt_schedina->numeri_giocati[j] == numeri_su_ruota[0] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[1] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[2] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[3] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[4] ){

                    primo_numero = punt_schedina->numeri_giocati[j];
                    // printf("LOG_DEBUG: primo_numero: %d\n", primo_numero);
                    // guardo se formo un ambo con i numeri *successivi*
                    for(k = j + 1; k < punt_schedina->quanti_numeri; k++){
                        if( punt_schedina->numeri_giocati[k] == numeri_su_ruota[0] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[1] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[2] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[3] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[4] ){

                            secondo_numero = punt_schedina->numeri_giocati[k];
                            // printf("LOG_DEBUG: secondo_numero: %d\n", secondo_numero);
                            for(z = k+1; z < punt_schedina->quanti_numeri; z++){
                                if( punt_schedina->numeri_giocati[z] == numeri_su_ruota[0] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[1] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[2] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[3] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[4]) {

                                    terzo_numero = punt_schedina->numeri_giocati[z];
                                    // printf("LOG_DEBUG: terzo_numero: %d\n", terzo_numero);
                                    // ho trovato un terno, lo salvo
                                    numeri_vincenti[quante_vincite] = primo_numero;
                                    numeri_vincenti[quante_vincite + 1] = secondo_numero;
                                    numeri_vincenti[quante_vincite + 2] = terzo_numero;
                                    quante_vincite += 3; // per accedere correttamente all'array, in realtà la vincita è 1
                                }
                            }
                        }
                    }
                }
            }

            quante_vincite /= 3; // per il motivo scritto sopra
            vincita_su_ruota = (( (VINCITA_TERNO / punt_schedina->quante_ruote) / combinazioni_semplici(punt_schedina->quanti_numeri, 3) ) * quante_vincite) * punt_schedina->importi[2];
            printf("LOG_DEBUG: vincita_su_ruota: %.3f\n", vincita_su_ruota);
            vincita_totale += vincita_su_ruota;

            // scrittura su file
            if(vincita_su_ruota != 0){
                FILE *fd_tmp;
                printf("LOG_DEBUG: trovati %d terni sulla ruota %s, vincita %.3f\n", quante_vincite, punt_schedina->ruote[i], vincita_su_ruota);
                fd_tmp = fopen(FILE_BUFFER2, "a");
                if(fd_tmp == NULL){
                    printf("Errore nell'apertura del file buffer2\n");
                    exit(1);
                }
/*
                printf("LOG_DEBUG: controllo del contenuto dei numeri vincenti: \n");
                // per 3 perchè parliamo dei terni
                for(j = 0; j < quante_vincite * 3; j += 3){
                    printf("numeri_vincenti[%d] = %d\n", j, numeri_vincenti[j]);
                    printf("numeri_vincenti[%d] = %d\n", j + 1, numeri_vincenti[j + 1]);
                    printf("numeri_vincenti[%d] = %d\n", j + 2, numeri_vincenti[j + 2]);
                }
*/

                fprintf(fd_tmp, "%s\n", "vincita");
                fprintf(fd_tmp, "%s\n", punt_schedina->ruote[i]);
                fprintf(fd_tmp, "%d\n", quante_vincite * 3);
                // per 3 perchè parliamo dei terni
                for(j = 0; j < quante_vincite * 3; j += 3){
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 1]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 2]);
                }

                //fprintf(fd_tmp, "%s", " >>  ");
                fprintf(fd_tmp, "%s\n", "Terno");
                fprintf(fd_tmp, "%.3f\n", vincita_su_ruota);

                fclose(fd_tmp);
            }
        }
        /*
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
         *
         *
         *                                                   Q U A T E R N A
         *
         *
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
        */
        else if(strcmp(tipo_giocata, "QUATERNA") == 0){
            int k, z, h;

            if(punt_schedina->importi[3] == 0){
                printf("Non è stata giocata la quaterna\n");
                return vincita_totale;
            }

            for(j = 0; j < punt_schedina->quanti_numeri; j++){
                int primo_numero = 0;
                int secondo_numero = 0;
                int terzo_numero = 0;
                int quarto_numero = 0;
                if( punt_schedina->numeri_giocati[j] == numeri_su_ruota[0] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[1] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[2] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[3] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[4] ){

                    primo_numero = punt_schedina->numeri_giocati[j];
                    // printf("LOG_DEBUG: primo_numero: %d\n", primo_numero);
                    // guardo se formo un ambo con i numeri *successivi*
                    for(k = j + 1; k < punt_schedina->quanti_numeri; k++){
                        if( punt_schedina->numeri_giocati[k] == numeri_su_ruota[0] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[1] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[2] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[3] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[4] ){

                            secondo_numero = punt_schedina->numeri_giocati[k];
                            // printf("LOG_DEBUG: secondo_numero: %d\n", secondo_numero);
                            // guardo se formo un terno
                            for(z = k+1; z < punt_schedina->quanti_numeri; z++){
                                if( punt_schedina->numeri_giocati[z] == numeri_su_ruota[0] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[1] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[2] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[3] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[4]) {

                                    terzo_numero = punt_schedina->numeri_giocati[z];
                                    // printf("LOG_DEBUG: terzo_numero: %d\n", terzo_numero);
                                    for(h = z + 1; h < punt_schedina->quanti_numeri; h++){
                                        if( punt_schedina->numeri_giocati[h] == numeri_su_ruota[0] ||
                                            punt_schedina->numeri_giocati[h] == numeri_su_ruota[1] ||
                                            punt_schedina->numeri_giocati[h] == numeri_su_ruota[2] ||
                                            punt_schedina->numeri_giocati[h] == numeri_su_ruota[3] ||
                                            punt_schedina->numeri_giocati[h] == numeri_su_ruota[4]) {

                                            quarto_numero = punt_schedina->numeri_giocati[h];
                                            // printf("LOG_DEBUG: quarto_numero: %d\n", quarto_numero);
                                            // ho trovato una quaterna, la salvo
                                            numeri_vincenti[quante_vincite] = primo_numero;
                                            numeri_vincenti[quante_vincite + 1] = secondo_numero;
                                            numeri_vincenti[quante_vincite + 2] = terzo_numero;
                                            numeri_vincenti[quante_vincite + 3] = quarto_numero;
                                            quante_vincite += 4;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            quante_vincite /= 4; // per il motivo scritto sopra
            vincita_su_ruota = (( (VINCITA_QUATERNA / punt_schedina->quante_ruote) / combinazioni_semplici(punt_schedina->quanti_numeri, 4) ) * quante_vincite) * punt_schedina->importi[3];
            printf("LOG_DEBUG: vincita_su_ruota: %.3f\n", vincita_su_ruota);
            vincita_totale += vincita_su_ruota;

            if(vincita_su_ruota != 0){
                FILE *fd_tmp;

                printf("LOG_DEBUG: trovati %d quaterne sulla ruota %s, vincita %.3f\n", quante_vincite, punt_schedina->ruote[i], vincita_su_ruota);
                fd_tmp = fopen(FILE_BUFFER2, "a");
                if(fd_tmp == NULL){
                    printf("Errore nell'apertura del file buffer2\n");
                    exit(1);
                }
/*
                printf("LOG_DEBUG: controllo del contenuto dei numeri vincenti: \n");
                for(j = 0; j < quante_vincite * 4; j += 4){
                    printf("numeri_vincenti[%d] = %d\n", j, numeri_vincenti[j]);
                    printf("numeri_vincenti[%d] = %d\n", j + 1, numeri_vincenti[j + 1]);
                    printf("numeri_vincenti[%d] = %d\n", j + 2, numeri_vincenti[j + 2]);
                    printf("numeri_vincenti[%d] = %d\n", j + 3, numeri_vincenti[j + 3]);
                }
*/

                fprintf(fd_tmp, "%s\n", "vincita");
                fprintf(fd_tmp, "%s\n", punt_schedina->ruote[i]);
                fprintf(fd_tmp, "%d\n", quante_vincite * 4);
                for(j = 0; j < quante_vincite * 4; j += 4){
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 1]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 2]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 3]);
                }

                // fprintf(fd_tmp, "%s", " >>  ");
                fprintf(fd_tmp, "%s\n", "Quaterna");
                fprintf(fd_tmp, "%.3f\n", vincita_su_ruota);

                fclose(fd_tmp);
            }
        }
        /*
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
         *
         *
         *                                                   C I N Q U I N A
         *
         *
         *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
        */
        else if(strcmp(tipo_giocata, "CINQUINA") == 0){
            int k, z, h, r;

            if(punt_schedina->importi[4] == 0){
                printf("Non è stata giocata la cinquina\n");
                return vincita_totale;
            }
            for(j = 0; j < punt_schedina->quanti_numeri; j++){
                int primo_numero = 0;
                int secondo_numero = 0;
                int terzo_numero = 0;
                int quarto_numero = 0;
                int quinto_numero;
                // per coerenza con le altre vincite mantengo il codice, anche se ovviamente non è il milgior modo di risolvere questo problema
                if( punt_schedina->numeri_giocati[j] == numeri_su_ruota[0] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[1] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[2] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[3] ||
                    punt_schedina->numeri_giocati[j] == numeri_su_ruota[4] ){

                    primo_numero = punt_schedina->numeri_giocati[j];
                    // printf("LOG_DEBUG: primo_numero: %d\n", primo_numero);
                    for(k = j + 1; k < punt_schedina->quanti_numeri; k++){
                        if( punt_schedina->numeri_giocati[k] == numeri_su_ruota[0] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[1] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[2] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[3] ||
                            punt_schedina->numeri_giocati[k] == numeri_su_ruota[4] ){

                            secondo_numero = punt_schedina->numeri_giocati[k];
                            // printf("LOG_DEBUG: secondo_numero: %d\n", secondo_numero);
                            for(z = k+1; z < punt_schedina->quanti_numeri; z++){
                                if( punt_schedina->numeri_giocati[z] == numeri_su_ruota[0] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[1] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[2] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[3] ||
                                    punt_schedina->numeri_giocati[z] == numeri_su_ruota[4]) {

                                    terzo_numero = punt_schedina->numeri_giocati[z];
                                    // printf("LOG_DEBUG: terzo_numero: %d\n", terzo_numero);
                                    for(h = z + 1; h < punt_schedina->quanti_numeri; h++){
                                        if( punt_schedina->numeri_giocati[h] == numeri_su_ruota[0] ||
                                            punt_schedina->numeri_giocati[h] == numeri_su_ruota[1] ||
                                            punt_schedina->numeri_giocati[h] == numeri_su_ruota[2] ||
                                            punt_schedina->numeri_giocati[h] == numeri_su_ruota[3] ||
                                            punt_schedina->numeri_giocati[h] == numeri_su_ruota[4]) {

                                            quarto_numero = punt_schedina->numeri_giocati[h];
                                            // printf("LOG_DEBUG: quarto_numero: %d\n", quarto_numero);
                                            for(r = h + 1; r < punt_schedina->quanti_numeri; r++){
                                                if( punt_schedina->numeri_giocati[r] == numeri_su_ruota[0] ||
                                                    punt_schedina->numeri_giocati[r] == numeri_su_ruota[1] ||
                                                    punt_schedina->numeri_giocati[r] == numeri_su_ruota[2] ||
                                                    punt_schedina->numeri_giocati[r] == numeri_su_ruota[3] ||
                                                    punt_schedina->numeri_giocati[r] == numeri_su_ruota[4]) {

                                                    quinto_numero = punt_schedina->numeri_giocati[r];
                                                    // printf("LOG_DEBUG: quinto_numero: %d\n", quinto_numero);
                                                    numeri_vincenti[quante_vincite] = primo_numero;
                                                    numeri_vincenti[quante_vincite + 1] = secondo_numero;
                                                    numeri_vincenti[quante_vincite + 2] = terzo_numero;
                                                    numeri_vincenti[quante_vincite + 3] = quarto_numero;
                                                    numeri_vincenti[quante_vincite + 4] = quinto_numero;
                                                    quante_vincite += 5;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            quante_vincite /= 5;
            vincita_su_ruota = (( (VINCITA_CINQUINA / punt_schedina->quante_ruote) / combinazioni_semplici(punt_schedina->quanti_numeri, 5) ) * quante_vincite) * punt_schedina->importi[4];
            printf("LOG_DEBUG: vincita_su_ruota: %.3f\n", vincita_su_ruota);
            vincita_totale += vincita_su_ruota;

            if(vincita_su_ruota != 0){
                FILE *fd_tmp;
                printf("LOG_DEBUG: trovata una cinquina sulla ruota %s, vincita %.3f\n", punt_schedina->ruote[i], vincita_su_ruota);
                fd_tmp = fopen(FILE_BUFFER2, "a");
                if(fd_tmp == NULL){
                    printf("Errore nell'apertura del file buffer2\n");
                    exit(1);
                }
/*
                printf("LOG_DEBUG: controllo del contenuto dei numeri vincenti: \n");
                for(j = 0; j < quante_vincite * 5; j += 5){
                    printf("numeri_vincenti[%d] = %d\n", j, numeri_vincenti[j]);
                    printf("numeri_vincenti[%d] = %d\n", j + 1, numeri_vincenti[j + 1]);
                    printf("numeri_vincenti[%d] = %d\n", j + 2, numeri_vincenti[j + 2]);
                    printf("numeri_vincenti[%d] = %d\n", j + 3, numeri_vincenti[j + 3]);
                    printf("numeri_vincenti[%d] = %d\n", j + 4, numeri_vincenti[j + 4]);
                }
*/

                fprintf(fd_tmp, "%s\n", "vincita");
                fprintf(fd_tmp, "%s\n", punt_schedina->ruote[i]);
                fprintf(fd_tmp, "%d\n", quante_vincite * 5);
                for(j = 0; j < quante_vincite * 5; j += 5){
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 1]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 2]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 3]);
                    fprintf(fd_tmp, "%d\n", numeri_vincenti[j + 4]);
                }

                // fprintf(fd_tmp, "%s", " >>  ");
                fprintf(fd_tmp, "%s\n", "Cinquina");
                fprintf(fd_tmp, "%.3f\n", vincita_su_ruota);

                fclose(fd_tmp);
            }
        }
        // caso default safe, errore
        else {
            printf("Errore inaspettato\n");
            exit(1);
        }

    }
    // uscito dal ciclo sulle ruote

    return vincita_totale;
}


// controlla se sono presenti delle vincite per l'estrazione corrente e in caso di esito positivo aggiorna il file di registro
// il file di registro è aperto e chiuso dai chiamanti
void analizza_vincite(struct schedina *punt_schedina, struct estrazione *punt_estrazione, char *username){
    int i;
    double vincita_estratto_totale = 0;
    double vincita_ambo_totale = 0;
    double vincita_terno_totale = 0;
    double vincita_quaterna_totale = 0;
    double vincita_cinquina_totale = 0;
    FILE *fd;

    // per ogni schedina ricreo il file in cui salvo le vincite (serve poi per aggiornare il consuntivo)
    fd = fopen(FILE_BUFFER2, "w");
    if(fd == NULL){
        printf("Errore nell'apertura del file buffer2\n");
        exit(1);
    }
    fclose(fd);

    // se ho scommesso sull'estratto
    // printf("LOG_DEBUG: punt_schedina->quanti_importi: %d\n", punt_schedina->quanti_importi);
    for(i = 1; i < punt_schedina->quanti_importi + 1; i++){
        // printf("LOG_DEBUG: valore di i: %d\n",  i);

        switch(i){
                case 1:
                    printf("\nLOG_DEBUG: Controllo sugli estratti\n");
                    vincita_estratto_totale = check_vincite(punt_schedina, punt_estrazione, "ESTRATTO");
                    printf("\nLOG_DEBUG: vincita_estratto_totale: %lf\n", vincita_estratto_totale);
                    break;

                case 2:
                    printf("\nLOG_DEBUG: Controllo sugli ambi\n");
                    vincita_ambo_totale = check_vincite(punt_schedina, punt_estrazione, "AMBO");
                    printf("\nLOG_DEBUG: vincita_ambo_totale: %lf\n", vincita_ambo_totale);
                    break;

                case 3:
                    printf("\nLOG_DEBUG: Controllo sugli terni\n");
                    vincita_terno_totale = check_vincite(punt_schedina, punt_estrazione, "TERNO");
                    printf("\nLOG_DEBUG: vincita_terno_totale: %lf\n", vincita_terno_totale);
                    break;

                case 4:
                    printf("\nLOG_DEBUG: Controllo sulle quaterne\n");
                    vincita_quaterna_totale = check_vincite(punt_schedina, punt_estrazione, "QUATERNA");
                    printf("\nLOG_DEBUG: vincita_quaterna_totale: %lf\n", vincita_quaterna_totale);
                    break;

                case 5:
                    printf("\nLOG_DEBUG: Controllo sulle cinquine\n");
                    vincita_cinquina_totale = check_vincite(punt_schedina, punt_estrazione, "CINQUINA");
                    printf("\nLOG_DEBUG: vincita_cinquina_totale: %lf\n", vincita_cinquina_totale);
                    break;

                default:
                    printf("Errore nello switch di selezione della funzione di vincita\n");
                    exit(1);
        }
    }

    // forse il consuntivo conviene metterlo in un'altra cartella, in un file col nome dell'utente ad esempio e farlo modificare dalla check_vincita
    // printf("\nLOG_DEBUG: chiamata a aggiorna_consuntivo()\n");
    aggiorna_consuntivo(username, punt_estrazione, vincita_estratto_totale, vincita_ambo_totale, vincita_terno_totale, vincita_quaterna_totale, vincita_cinquina_totale);

}

// Estrae le schedine attive di un utente memorizzandole in una struttura schedina
// per poi passarle alla funzione analizza_vincite()
// NOTA: lavora su un file di registro aperto dal chiamante
int analizza_schedine_attive(struct estrazione *punt_estrazione, FILE *file_registro, char *username){

    struct schedina *punt_schedina;
    int numeri_giocati, ruote, importi, i, j;
    int schedine_attive_presenti;


    i = 0;
    j = 0;
    ruote = 0;
    importi = 0;
    numeri_giocati = 0;
    schedine_attive_presenti = 0;

    punt_schedina = (struct schedina*)calloc(1, sizeof(struct schedina));
    if(!punt_schedina){
        printf("Memoria esaurita\n");
        fclose(file_registro);
        exit(1);
    }

    // printf("LOG_DEBUG: analizza_schedine_attive --> Comincia l'analisi del file di registro\n");
    // Semplicemente arriva fino al punto in cui nel file di registro cominciano le schedine attualmente attive
    while(1){
        int ret;
        char *string;
        string = calloc(BUFFER_LENGHT, sizeof(char));
        if(!string){
            printf("Memoria esaurita\n");
            free(punt_schedina);
            fclose(file_registro);
            exit(1);
        }

        ret = fscanf(file_registro, "%s", string);

        if(ret == EOF){
            // printf("Operazione di lettura file registro terminata\n");
            free(punt_schedina);
            free(string);
            return schedine_attive_presenti;
        } else if(!ret){
            free(string);
            free(punt_schedina);
            printf("Errore nell'aggiornamento delle estrazioni\n");
            exit(1);
        } else if(strcmp(string, "SCHEDINE_ATTIVE") == 0){
            schedine_attive_presenti = 1;
            free(string);
            break;
        }

        free(string);
    }

    if(schedine_attive_presenti == 0){
        printf("Non sono presenti schedine attive in questo file di registro\n");
        return 0;
    }

    // printf("LOG_DEBUG: analizza_schedine_attive --> Schedine Attive raggiunte\n");

    // ciclo su tutte le possibili schedine attive
    // per ogni schedina nel file di registro costruisce una struttura schedina che viene passata
    // alla funzione analizza_vincite()
    while(1){
        int ret;
        char *stringa;

        stringa = calloc(BUFFER_LENGHT, sizeof(char));
        if(!stringa){
            printf("Memoria esaurita\n");
            fclose(file_registro);
            exit(1);
        }

        ret = fscanf(file_registro, "%s", stringa);

        if(ret == EOF){
            // l'ultima schedina può dover essere ancora analizzata
            // se sono qui ad esempio la variabile importi non è stata ripulita
            if(importi == 1){
                importi = 0;
                punt_schedina->quanti_importi = i;
                i = 0;

                // per debug
                // printf("LOG_DEBUG: stampa della schedina in analizza_schedine_attive\n");
                // stampa_schedina(punt_schedina);

                analizza_vincite(punt_schedina, punt_estrazione, username);
                free(punt_schedina);
            }

            // printf("LOG_DEBUG: terminato l'aggiornamento delle schedine per l'utente\n");
            free(stringa);
            return 1;

        } else if(!ret){
            printf("Errore nell'aggiornamento delle vincite\n");
            free(stringa);
            fclose(file_registro);
            exit(1);

        } else if(strcmp(stringa, "SCHEDINE_GIOCATE") == 0){
            // come nel caso di EOF
            if(importi == 1){
                importi = 0;
                punt_schedina->quanti_importi = i;
                i = 0;
                // printf("LOG_DEBUG: stampa della schedina in analizza_schedine_attive\n");
                // stampa_schedina(punt_schedina);

                analizza_vincite(punt_schedina, punt_estrazione, username);
                free(punt_schedina);
            }

            // printf("LOG_DEBUG: terminato l'aggiornamento delle schedine per l'utente\n");
            free(stringa);
            return 1;

        } else {

            if(strcmp(stringa, "schedina") == 0){

                if(j == 0) // prima schedina
                    j = 1;
                else{ // procedo all'analisi delle vincite della schedina costruita
                    importi = 0;
                    punt_schedina->quanti_importi = i;
                    i = 0;
                    // printf("LOG_DEBUG: stampa della schedina in analizza_schedine_attive\n");
                    // stampa per debug
                    // stampa_schedina(punt_schedina);
                    // printf("\nLOG_DEBUG: Analisi delle vincite per la schedina...\n");

                    analizza_vincite(punt_schedina, punt_estrazione, username);
                }

                free(stringa);
                free(punt_schedina);
                punt_schedina = (struct schedina*)calloc(1, sizeof(struct schedina));
                if(!punt_schedina){
                    printf("Memoria esaurita\n");
                    exit(1);
                }
                continue;
            }

            if(strcmp(stringa, "numeri_giocati") == 0){
                numeri_giocati = 1;
                free(stringa);
                continue;

            } else if(strcmp(stringa, "ruote") == 0){
                numeri_giocati = 0;
                ruote = 1;
                punt_schedina->quanti_numeri = i;
                i = 0;
                free(stringa);
                continue;

            } else if(strcmp(stringa, "importi") == 0){
                ruote = 0;
                importi = 1;
                if(punt_schedina->flag_tutte == 's')
                    punt_schedina->quante_ruote = 11;
                else
                    punt_schedina->quante_ruote = i;
                i = 0;
                free(stringa);
                continue;

            }

            if(numeri_giocati == 1){
                punt_schedina->numeri_giocati[i] = atoi(stringa);
                i++;
            }

            if(ruote == 1){
                if(strcmp(stringa, "tutte") == 0)
                    punt_schedina->flag_tutte = 's';
                else{
                    strncpy(punt_schedina->ruote[i], stringa, strlen(stringa) + 1);
                    i++;
                }
            }

            if(importi == 1){
                punt_schedina->importi[i] = atof(stringa);
                i++;
            }

        }
        free(stringa);
    }
    return 1;

}


/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                                      FUNZIONI PER LA VISUALIZZAZIONE DELLE ESTRAZIONI
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

// trasmette le ultime quante estrazioni su una certa ruota (su tutte se ruota == tutte)
void trasmetti_estrazioni(int quante, char *ruota, int socket){

    FILE *fd_estrazioni;
    char ruota_letta[WHEEL_LENGHT];
    char estrazione[strlen("estraione") + 1];
    char numero_estratto[2];


    fd_estrazioni = fopen(FILE_ESTRAZIONI, "r");
    if(fd_estrazioni == NULL){
        printf("Errore nell'apertura del file delle estrazioni\n");
        exit(1);
    }
    // qui si potrebbe mettere un ciclo che conta il numero di stringhe "estrazioni" presenti nel file
    // questo deve essere maggiore o uguale a quante
    // poi si fa il rewind dell'I/O pointer

    if(strcmp(ruota, "tutte") == 0){
        int i, j;
        for(i = 0; i < quante; i++){
            fscanf(fd_estrazioni, "%s", estrazione);
            trasmetti(estrazione, socket);
            for(j = 0; j < 11; j++){
                int num;
                fscanf(fd_estrazioni, "%s", ruota_letta);
                trasmetti(ruota_letta, socket);

                // dal primo al quinto numero della ruota
                fscanf(fd_estrazioni, "%d", &num);
                sprintf(numero_estratto, "%d", num);
                trasmetti(numero_estratto, socket);

                fscanf(fd_estrazioni, "%d", &num);
                sprintf(numero_estratto, "%d", num);
                trasmetti(numero_estratto, socket);

                fscanf(fd_estrazioni, "%d", &num);
                sprintf(numero_estratto, "%d", num);
                trasmetti(numero_estratto, socket);

                fscanf(fd_estrazioni, "%d", &num);
                sprintf(numero_estratto, "%d", num);
                trasmetti(numero_estratto, socket);

                fscanf(fd_estrazioni, "%d", &num);
                sprintf(numero_estratto, "%d", num);
                trasmetti(numero_estratto, socket);

            }
        }
    } else {
        int i, j;
        for(i = 0; i < quante; i++){
            fscanf(fd_estrazioni, "%s", estrazione);
            trasmetti(estrazione, socket);
            for(j = 0; j < 11; j++){
                int num;

                fscanf(fd_estrazioni, "%s", ruota_letta);
                if(strcmp(ruota_letta, ruota) == 0){
                    trasmetti(ruota, socket);
                    // dal primo al quinto numero della ruota
                    fscanf(fd_estrazioni, "%d", &num);
                    sprintf(numero_estratto, "%d", num);
                    trasmetti(numero_estratto, socket);

                    fscanf(fd_estrazioni, "%d", &num);
                    sprintf(numero_estratto, "%d", num);
                    trasmetti(numero_estratto, socket);

                    fscanf(fd_estrazioni, "%d", &num);
                    sprintf(numero_estratto, "%d", num);
                    trasmetti(numero_estratto, socket);

                    fscanf(fd_estrazioni, "%d", &num);
                    sprintf(numero_estratto, "%d", num);
                    trasmetti(numero_estratto, socket);

                    fscanf(fd_estrazioni, "%d", &num);
                    sprintf(numero_estratto, "%d", num);
                    trasmetti(numero_estratto, socket);

                } else {
                    int i;
                    // consumo i numeri della ruota, faccio scorrere I/O pointer
                    for(i = 0; i < 5; i++)
                        fscanf(fd_estrazioni, "%d", &num);
                }

            }
        }
    }
    fclose(fd_estrazioni);
}


void ricevi_estrazioni(int quante, char *ruota, int socket){

    int i, j;
    char estrazione[strlen("estrazione") + 1];
    char ruota_letta[WHEEL_LENGHT];
    char numero_estratto[2];

    for(i = 0; i < quante; i++){
        ricevi(estrazione, socket);
        printf("\nEstrazione n° %d prima dell'ultima\n", i);
        if(strcmp(ruota, "tutte") == 0){
            for(j = 0; j < 11; j++){
                ricevi(ruota_letta, socket);
                printf("%s    ", ruota_letta);

                ricevi(numero_estratto, socket);
                printf("%d ", atoi(numero_estratto) );
                ricevi(numero_estratto, socket);
                printf("%d ", atoi(numero_estratto) );
                ricevi(numero_estratto, socket);
                printf("%d ", atoi(numero_estratto) );
                ricevi(numero_estratto, socket);
                printf("%d ", atoi(numero_estratto) );
                ricevi(numero_estratto, socket);
                printf("%d ", atoi(numero_estratto) );

                printf("\n");

            }
        } else {
            ricevi(ruota_letta, socket);
            printf("%s    ", ruota_letta);

            ricevi(numero_estratto, socket);
            printf("%d ", atoi(numero_estratto) );
            ricevi(numero_estratto, socket);
            printf("%d ", atoi(numero_estratto) );
            ricevi(numero_estratto, socket);
            printf("%d ", atoi(numero_estratto) );
            ricevi(numero_estratto, socket);
            printf("%d ", atoi(numero_estratto) );
            ricevi(numero_estratto, socket);
            printf("%d ", atoi(numero_estratto) );
        }
    }

}
