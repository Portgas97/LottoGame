#include <arpa/inet.h> // funzioni di conversione 
#include <errno.h> // gestione degli errori
#include <sys/stat.h>
#include <time.h> // per time_t

// costanti
#define MAX_SIZE_INPUT      100
#define MAX_SIZE_PARAMETER  16
#define NUMERO_COMANDI      8
#define LEN_COMANDI         200
#define BUFFER_LENGHT       1024
#define RESPONSE_LEN        100
#define CREDENTIAL_LEN      8
#define PATH_LEN            100
#define TIME_LEN            80
#define SESSION_ID_LEN      10
#define WHEEL_LENGHT        10

#define VINCITA_ESTRATTO    11.23
#define VINCITA_AMBO        250
#define VINCITA_TERNO       4500
#define VINCITA_QUATERNA    120000
#define VINCITA_CINQUINA    6000000
#define MAX_AMOUNT          200

// percorsi di alcuni file/cartelle importanti
static const char IP_BANNATI[] = "/home/studenti/NetBeansProjects/lotto_server/IP_bannati.txt";
static const char FILE_UTENTI[] = "/home/studenti/NetBeansProjects/lotto_server/lista_utenti.txt";
static const char FILE_BUFFER[] = "/home/studenti/NetBeansProjects/lotto_server/buffer.txt";
static const char FILE_BUFFER2[] = "/home/studenti/NetBeansProjects/lotto_server/buffer2.txt";
static const char FILE_BUFFER3[] = "/home/studenti/NetBeansProjects/lotto_server/buffer3.txt";
static const char FILE_ESTRAZIONI[] = "/home/studenti/NetBeansProjects/lotto_server/file_estrazioni.txt";
static const char FILE_CONSUNTIVI[] = "/home/studenti/NetBeansProjects/lotto_server/consuntivi/";

// struttura per la schedina
struct schedina {
    int numeri_giocati[10];
    int quanti_numeri;
    char ruote[11][WHEEL_LENGHT];
    int quante_ruote;
    char flag_tutte;
    double importi[5];
    int quanti_importi;
};

// struttura per un'estrazione
struct estrazione {
    int numeri_estratti[11 * 5];
    time_t istante_estrazione;
};

// dichiarazione delle funzioni in funzioni.c
int rileva_comando(char *buffer);
char sotto_stringa(char *comando, char *buffer, int *index);
void stampa_menu(int i);
char rileva_parametro(char *comando, int *index, char terminatore);
void acquisisci_parametro(char bool, char *comando, char *buffer, int *index);

void trasmetti(char *buffer, int socket);
void ricevi(char *buffer, int socket);

void percorso_file_registro(char *username, char *stringa);
FILE* crea_file_registro(char *username, char *stringa);
FILE* scrivi_file(char *buffer, const char *path, char* modality);
void percorso_consuntivo(char *username, char *stringa);
FILE* crea_consuntivo(char *username, char *stringa);
int trova_username(char *username);
int verifica_password(char *username, char *password);
int check_IP(char *saved_IP);
void aggiorna_bannati(char *saved_IP);
void aggiorna_sessionID(char *USERNAME_LOGIN, char *new_sessiondID);
int richiesta_autenticazione(char *sessionID, int socket);
int risposta_autenticazione(char *sessionID, char *receivedID, int socket);

void stampa_schedina(struct schedina* punt_schedina);
void nuova_schedina(char *username, struct schedina *punt_schedina);
void invia_schedine_attive(char *USERNAME_LOGIN, int socket);
void ricevi_schedine_attive(int socket);
void invia_schedine_giocate(char *USERNAME_LOGIN, int socket);
void ricevi_schedine_giocate(int socket);

void stampa_estrazione(struct estrazione *punt_estrazione);
void scrivi_estrazione(struct estrazione *punt_estrazione);

int analizza_schedine_attive(struct estrazione *punt_estrazione, FILE *file_registro, char *username);
void analizza_vincite(struct schedina *punt_schedina, struct estrazione *punt_estrazione, char *username);

int ruota_to_index(char ruota[]);
int link_wheel(char *parametro);

int fattoriale(int n);
double combinazioni_semplici(int n, int k);
double check_vincite(struct schedina *punt_schedina, struct estrazione *punt_estrazione, char *tipo_giocata);

void disattiva_schedine(char *path);
void aggiorna_consuntivo(char *username, struct estrazione *punt_estrazione, double vincita_estratto_totale, double vincita_ambo_totale, double vincita_terno_totale, double vincita_quaterna_totale, double vincita_cinquina_totale);

void trasmetti_estrazioni(int quante, char *ruota, int socket);
void ricevi_estrazioni(int quante, char *ruota, int socket);