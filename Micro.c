//MICROBASH
//Tommaso Coronati - Luca Ferrari

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define N 1024	//Dimensione buffer input e dimensione buffer percorso
#define ARG 100	//Dimensione buffer argomenti/parametri del comando


//Controlla la correttezza del comando inserito in pipeline (controlla eventuali errori di "parsing")
bool controlCommand(char *filename)
{
	char *out = strstr(filename, "> ");	//Controlla se ci sono spazi tra il nome del file e l'operatore di redirezione
	char *in = strstr(filename, "< ");
	
	if (out != NULL || in != NULL)	//Qui avviene il controllo degli spazi
	{
		printf("\e[91;1mERROR:\e[m the file for the redirection is not specified\n");
		printf("\e[1mHINT:\e[m there must be no spaces between the operator and the file name\n");
		return false;
	}

	//Controlla se l'operatore di redirezione dell'input è stato inserito nel primo comando (nel caso di pipeline)
	int inpos = strchr(filename, '<') - filename;
	int pipepos = strchr(filename, '|') - filename;

	if (inpos > pipepos && strchr(filename, '<')!=NULL)	//Banalmente controlla le posizioni dei due operatori 
	{
		printf("\e[91;1mERROR:\e[m only the first command can have input redirection\n");
		return false;
	}


	char *dollaro = strchr(filename, '$');	//Controlla se è stata inserita una variabile d'ambiente

	if (dollaro != NULL)
	{
		dollaro = &dollaro[1];	//Qui elimino il carattere "$" e lascio intatto il nome della variabile
		if (!getenv(dollaro))	//Controlla se esiste la variabile d'ambiente
		{
			printf("\e[91;1mERROR:\e[m  The variable %s does not exist\n", dollaro);
			return false;
		}
	}

	return true;	//Se non ha trovato errori ritorna true
}


//Controlla la correttezza degli argomenti passati a CD (controlla eventuali errori di "parsing")
bool controlCD(char *comando)
{	
	//Qui verifica se sono stati inseriti operatori che non sono supportati da cd (redirezione e pipe)
	char *pipe = strchr(comando, '|');
	char *out = strchr(comando, '>');
	char *in = strchr(comando, '<');
	if (pipe != NULL)
	{
		printf("\e[91;1mERROR:\e[m you can't use cd with the pipe operator\n");
		return false;
	}
	if (out != NULL || in != NULL)
	{
		printf("\e[91;1mERROR:\e[m cd does not support redirection\n");
		return false;
	}
	return true;
}

//Qui eseguo i comandi passati come pipe e redirezione
void exec(char *filename)
{
	if (controlCommand(filename))	//Faccio un controllo di correttezza del comando
	{
		char c;	//Mi permette di stampare a schermo l'output

		FILE * fptr;	//Creo una variabile di tipo FILE per eseguire le pipe

		fptr = popen(filename, "w");	//Con la popen eseguo direttamente il comando inserito in standard input
		c = fgetc(fptr);	//Acquisisco il risultato della popen
		while (c != EOF)	//Stampo uno dopo l'altro i caratteri
		{
			printf("%c", c);
			c = fgetc(fptr);
		}

		fclose(fptr);	//Chiudo la pipe
	}
}


//Qui eseguo il cd
void openDir(char *comando)
{
	if (controlCD(comando))	//Controllo la correttezza degli argomenti
	{
		char *dir;	//Puntatore all'argomento da passare a chdir

		if (strchr(comando, '$') != NULL)	//Controllo se è stata inserita una variabile d'ambiente da espandere
		{
			dir = getenv(&comando[4]);	//In caso affermativo lo inserisco all'interno degli argomenti
		}
		else	//Altrimenti inserisco gli argomenti all'interno del puntatore
		{
			dir = &comando[3];
		}
		if (chdir(dir))	//Eseguo la chdir
		{
			fprintf(stderr, "\e[91;1mERROR:\e[m %s\n", strerror(errno));	//Se ci sono problemi mando il segnale di errore
		}
	}
}

//Funzione che va ad identificare i comandi
void readcommand(char *comando)
{
	//Qui controlla se sono stati inseriti comandi dove sono presenti pipe o redirezioni
	char *pipe = strchr(comando, '|');
	char *in = strchr(comando, '<');
	char *out = strchr(comando, '<');
	char *dollaro = strchr(comando, '$');
	if (pipe != NULL || in != NULL || out != NULL || dollaro != NULL) exec(comando);	//In caso affermativo chiamo la exec


	else	//Altrimenti eseguo il comando
	{
		char *argv[ARG];	//Buffer degli argomenti/parametri del comando

		char *token = strtok(comando, " ");	//Con strtok estraggo il comando e lo inserisco nel buffer
		argv[0] = token;	//Lo inserisco nella prima posizione
		int count = 1;	//Variabile segnaposto

		while (token != NULL)	//Ciclo il comando
		{
			token = strtok(NULL, " ");	//Estraggo i parametri
			argv[count] = token;	//Li inserisco all'interno del buffer
			count++;
		}

		argv[ARG - 1] = NULL;	//Posiziono il puntatore a NULL alla fine del buffer

		int pid = fork();	//Eseguo la fork
		if (pid == 0)
		{
			if (execvp(argv[0], argv) == -1)	//Eseguo il comando (agv[0] e il comando e argv sono i parametri)
			{
				perror(NULL);
				exit(EXIT_SUCCESS);
			}
		}
		else
		{
			wait(NULL);
		}
	}
	return;
}


int main()
{
	char filename[N];	//Buffer input da stdin
	char buffer[N];		//Buffer percorso
	for (;;)
	{
		//Stampo nome del programma e percorso corrente
		printf("\e[32;1mMicroBash $: \e[m");
		printf("\e[93;1m %s \e[m", getcwd(buffer, sizeof(buffer)));
		printf("%s ", "  ");
	
		//Acquisisco input da stdin
		fgets(filename, sizeof(filename), stdin);
		filename[strlen(filename) - 1] = '\0';	//Elimino il terminatore di riga
	
		if (feof(stdin) || strcmp(filename, "exit") == 0)		//Controllo se l'utente ha premuto CTRLDX + D (feof) o ha scritto exit
		{
			printf("\n");		//In caso affermativo termino il programma
			kill(0, SIGINT);
		}

		if (filename[0] == 'c' && filename[1] == 'd' && filename[2] == ' ')	//controllo se il comando da eseguire è cd
		{
			openDir(filename); 	//Chiamo la funzione corrispondente
			continue;	//riavvio il ciclo del main
		}

		readcommand(filename);	//Se non è cd chiamo la funzione di lettura del comando

		//resetto il buffer per prendere nuovi comandi
       		 memset(filename,0,sizeof(filename));
	}
}
