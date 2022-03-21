//Tommaso Coronati - Luca Ferrari
//Laboratorio MicroBash

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N 1024	//Dimensione buffer input e dimensione buffer percorso
#define ARG 100 //Dimensione buffer argomenti

char *espansione(char *); //Funzione che si occupa di espandere le variabili d'ambiente
int redirezione(char* );  //Redirige I/O
int argnumber(char* []);  //Ritorna il numero degli argomenti del comando
bool controlCD(char *);   //Controlla la correttezza del comando CD
bool controlCommand(char *);  //Controlla la correttezza del comando
void singleparsing(char *, char *, char * [], char *[]);  //Esegue il parsing nel caso di singola pipe
void multiparsing(char *, char *, char *, char *[], char *[], char *[]);  //Esegue il parsing in caso di pipe multiple
void exec_pipe(char* , char * [], char * [], char * [], int );  //Esegue la pipe
void exec(char *);  //Controlla quante pipe sono state inserite (successivamente chiama le funzioni di parsing ed esecuzione)
void readCommand(char *); //Leggge il comando e chiama le funzioni corrispondenti
void openDir(char *);   //Esegue il comando CD
