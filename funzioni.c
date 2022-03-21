//Tommaso Coronati - Luca Ferrari
//Laboratorio MicroBash
#include "header.h"

//Funzione che si occupa di espandere le variabili d'ambiente
char *espansione(char *token)
{
	token = &token[1];	//Qui elimino il carattere "$" e lascio intatto il nome della variabile  
	if (strchr(token, '$') != NULL) token++;
	if (getenv(token) == NULL)	//Controlla se esiste la variabile d'ambiente
	{
		printf("\e[91;1mERROR:\e[m  The variable '%s' does not exist\n", token);
		return getenv(token);
	}
	return getenv(token);
}

//Redirige I/O
int redirezione(char *filename)
{
	//Controlla che tipo di redirezione deve fare
	if (strchr(filename, '>') != NULL)
	{
		filename++;
		char *temp = filename;
		if (temp != NULL && strchr(temp, '$') != NULL) temp = espansione(temp);	//Nel caso espande la varibile
		int filedes = open(temp, O_WRONLY | O_CREAT, 0644);	//Apre o crea il file
		if (filedes == -1) perror("ERRORE open non andata a buon fine");	//Verifico se la open è andata a buon fine
		return filedes;
	}
	if (strchr(filename, '<') != NULL)
	{
		filename++;
		char *temp = filename;
		if (temp != NULL && strchr(temp, '$') != NULL) temp = espansione(temp);
		int filedes = open(temp, O_RDONLY | O_CREAT, 0644);
		if (filedes == -1) perror("ERRORE open non andata a buon fine");
		return filedes;
	}
	return -1;
}

//Ritorna il numero degli argomenti del comando
int argnumber(char *command[])
{
	int counter = 0;
	for (;;)
	{
		if (command[counter] == NULL) return counter - 1;	//Conta ogni argomento
		counter++;
	}
}

//Controlla la correttezza degli argomenti passati a CD (controlla eventuali errori di "parsing")
bool controlCD(char *comando)
{
	//Qui verifica se sono stati inseriti operatori che non sono supportati da cd (redirezione e pipe)
	if (strchr(comando, '|') != NULL)
	{
		printf("\e[91;1mERROR:\e[m you can't use cd with the pipe operator\n");
		return false;
	}
	if (strchr(comando, '>') != NULL || strchr(comando, '<') != NULL)
	{
		printf("\e[91;1mERROR:\e[m cd does not support redirection\n");
		return false;
	}
	return true;
}

//Controlla la correttezza del comando
bool controlCommand(char *filename)
{
	if (strstr(filename, "cd") != NULL)	//Qui controllo se è presente il comando CD
	{
		printf("\e[91;1mERROR:\e[m you cannot use the 'cd' command with the pipe operator\n");
		return false;
	}

	if (strstr(filename, "| |") != NULL || strstr(filename, "||") != NULL)	//Qui controllo se è l'operatore pipe vuoto
	{
		printf("\e[91;1mERROR:\e[m Empty pipe command\n");
		return false;
	}

	if (strstr(filename, "> ") != NULL || strstr(filename, "< ") != NULL)	//Qui avviene il controllo degli spazi
	{
		printf("\e[91;1mERROR:\e[m the file for the redirection is not specified\n");
		printf("\e[1mHINT:\e[m there must be no spaces between the operator and the file name\n");
		return false;
	}

	//Controlla se l'operatore di redirezione dell'input è stato inserito nel primo comando (nel caso di pipeline)
	int inpos = strchr(filename, '<') - filename;
	int pipepos = strchr(filename, '|') - filename;

	if (inpos > pipepos && strchr(filename, '<') != NULL)	//Banalmente controlla le posizioni dei due operatori
	{
		printf("\e[91;1mERROR:\e[m only the first command can have input redirection\n");
		return false;
	}
	return true;	//Se non ha trovato errori ritorna true
}

//Esegue il parsing nel caso di singola pipe
void singleparsing(char *comando1, char *comando2, char *comandovet[], char *comandovet2[])
{
	char *tokenargc = strtok(comando1, " ");	//Con strtok estraggo il comando e lo inserisco nel buffer
	comandovet[0] = tokenargc;
	int count = 1;
	while (tokenargc != NULL)	//Ciclo il comando
	{
		tokenargc = strtok(NULL, " ");	//Estraggo i parametri
		if (tokenargc != NULL && strchr(tokenargc, '$') != NULL && strchr(tokenargc, '<') == NULL && strchr(tokenargc, '>') == NULL) tokenargc = espansione(tokenargc);
		comandovet[count] = tokenargc;	//Li inserisco all'interno del buffer
		count++;
	}

	tokenargc = strtok(comando2, " ");
	comandovet2[0] = tokenargc;
	count = 1;

	while (tokenargc != NULL)
	{
		//Ciclo il comando
		tokenargc = strtok(NULL, " ");	//Estraggo i parametri
		if (tokenargc != NULL && strchr(tokenargc, '$') != NULL && strchr(tokenargc, '<') == NULL && strchr(tokenargc, '>') == NULL) tokenargc = espansione(tokenargc);
		comandovet2[count] = tokenargc;	//Li inserisco all'interno del buffer
		count++;
	}
}

//Esegue il parsing in caso di pipe multiple
void multiparsing(char *comando1, char *comando2, char *comando3, char *comandovet[], char *comandovet2[], char *comandovet3[])
{
	//Buffer degli argomenti/parametri del comando
	char *tokenargc = strtok(comando1, " ");	//Con strtok estraggo il comando e lo inserisco nel buffer
	comandovet[0] = tokenargc;
	int count = 1;
	while (tokenargc != NULL)	//Ciclo il comando
	{
		tokenargc = strtok(NULL, " ");	//Estraggo i parametri
		if (tokenargc != NULL && strchr(tokenargc, '$') != NULL && strchr(tokenargc, '<') == NULL && strchr(tokenargc, '>') == NULL) tokenargc = espansione(tokenargc);
		comandovet[count] = tokenargc;	//Li inserisco all'interno del buffer
		count++;
	}

	tokenargc = strtok(comando2, " ");
	comandovet2[0] = tokenargc;
	count = 1;
	while (tokenargc != NULL)
	{
		//Ciclo il comando
		tokenargc = strtok(NULL, " ");	//Estraggo i parametri
		if (tokenargc != NULL && strchr(tokenargc, '$') != NULL && strchr(tokenargc, '<') == NULL && strchr(tokenargc, '>') == NULL) tokenargc = espansione(tokenargc);
		comandovet2[count] = tokenargc;	//Li inserisco all'interno del buffer
		count++;
	}

	tokenargc = strtok(comando3, " ");
	comandovet3[0] = tokenargc;
	count = 1;
	while (tokenargc != NULL)	//Ciclo il comando
	{
		tokenargc = strtok(NULL, " ");	//Estraggo i parametri
		if (tokenargc != NULL && strchr(tokenargc, '$') != NULL && strchr(tokenargc, '<') == NULL && strchr(tokenargc, '>') == NULL) tokenargc = espansione(tokenargc);
		comandovet3[count] = tokenargc;	//Li inserisco all'interno del buffer
		count++;
	}
}

//Esegue la pipe
void exec_pipe(char *comando, char *cmd1[], char *cmd2[], char *cmd3[], int num_cmd)
{
	pid_t pid1, pid2, pid3;
	int fd[2];
	int fd2[2];
	if (pipe(fd) < 0)
	{
		perror("ERRORE creazione pipe");
		return;
	}
	if (pipe(fd2) < 0)
	{
		perror("ERRORE creazione pipe");
		return;
	}
	int filenameNum2 = -1;
	int filedes2 = -1;

	//faccio la redirezione in input
	int filenameNum = argnumber(cmd1);
	int filedes = redirezione(cmd1[filenameNum]);
	if (filedes == -1) cmd1[ARG - 1] = NULL;
	else cmd1[filenameNum] = NULL;

	//faccio la redirezione in output o sul 3 o sul 2 dipendentemente da quanti comandi ci sono
	if (num_cmd == 2)
	{
		filenameNum2 = argnumber(cmd2);
		filedes2 = redirezione(cmd2[filenameNum2]);
		if (filedes2 != -1) cmd2[filenameNum2] = NULL;
	}
	if (num_cmd == 3)
	{
		filenameNum2 = argnumber(cmd3);
		filedes2 = redirezione(cmd3[filenameNum2]);
		if (filedes2 != -1) cmd3[filenameNum2] = NULL;
	}

	pid1 = fork();
	if (pid1 < 0)
	{
		perror("ERRORE creazion fork");
		return;
	}
	if (pid1 == 0)	//primo figlio
	{
		// redirigere stdout su fd scrittura (1) pipe1;
		if (dup2(fd[1], 1) == -1)
		{
			perror("ERRORE DUP");
			return;
		}
		close(fd[1]);
		close(fd[0]);	// chiudere fd1;
		if (num_cmd == 3)
		{
			close(fd2[1]);close(fd2[0]);
		}

		if (filedes != -1)
		{
			if (dup2(filedes, 0) == -1)
			{
				perror("ERRORE DUP");
				return;
			}
		}
		//Eseguo il comando (agv[0] e il comando e argv sono i parametri)
		if (execvp(cmd1[0], cmd1) == -1)
		{
			perror("ERRORE EXEC");
			exit(0);
		}
	}
	else
	{
		pid2 = fork();
		if (pid2 < 0)
		{
			perror("ERRORE CREAZIONE FORK");
			return;
		}
		if (pid2 == 0)	//Secondo figlio
		{
			if (dup2(fd[0], 0) == -1)
			{
				perror("ERRORE DUP");
				return;
			}	//redirigere stdin su fd lettura (0) pipe1;
			if (num_cmd == 3)
			{
				if (dup2(fd2[1], 1) == -1)
				{
					perror("ERRORE DUP");
					return;
				}	//redirigere stdout su fd scrittura (1) pipe2;
				close(fd2[1]);
				close(fd2[0]);	//chiudere fd2;
			}

			close(fd[1]);
			close(fd[0]);	// chiudere fd1;

			if (filedes2 != -1 && num_cmd != 3)
			{
				if (dup2(filedes2, 1) == -1)
				{
					perror("ERRORE DUP");
					return;
				}
			}
			//Eseguo il comando (agv[0] e il comando e argv sono i parametri)
			if (execvp(cmd2[0], cmd2) == -1)
			{
				perror("ERRORE EXEC ");
				exit(0);
			}
		}
		else if (num_cmd == 3)
		{
			pid3 = fork();
			if (pid3 < 0)
			{
				perror("ERRORE FORK");
				return;
			}
			if (pid3 == 0)	//Secondo figlio
			{
			 	//redirigere stdin su fd lettura (0) pipe2;
				if (dup2(fd2[0], 0) == -1)
				{
					perror("ERRORE DUP");
					return;
				}
				close(fd2[1]);close(fd2[0]);close(fd[1]);close(fd[0]);	// chiusura fd;
				if (filedes2 != -1)
				{
					if (dup2(filedes2, 1) == -1)
					{
						perror("ERRORE DUP");
						return;
					}
				}
				//Eseguo il comando (agv[0] e il comando e argv sono i parametri)
				if (execvp(cmd3[0], cmd3) == -1)
				{
					perror("ERRORE EXEC");
					exit(0);
				}
			}
		}
		//chiudere fd1 e fd2;
		close(fd2[1]);close(fd2[0]);close(fd[1]);close(fd[0]);	
		for (int i = 0; i < num_cmd; i++)
		{
			wait(NULL);
		}
	}
}

//Controlla quante pipe sono state inserite (successivamente chiama le funzioni di parsing ed esecuzione)
void exec(char *comando)
{
	if (controlCommand(comando))	//Faccio un controllo di correttezza del comando
	{
		char *argv[ARG];
		//Buffer degli argomenti/parametri del comando
		char *token = strtok(comando, "|");	//Con strtok estraggo il comando e lo inserisco nel buffer
		argv[0] = token;	//Lo inserisco nella prima posizione
		int count = 1;	//Variabile segnaposto
		int valorepipe = 0;
		while (token != NULL)	//Ciclo il comando
		{
			token = strtok(NULL, "|");	//Estraggo i parametri
			valorepipe++;
			argv[count] = token;	//Li inserisco all'interno del buffer
			count++;
		}

		argv[ARG - 1] = NULL;
		char *argc[ARG];
		char *argc1[ARG];
		char *argc2[ARG];
		memset(argc, 0, sizeof(argc));
		memset(argc1, 0, sizeof(argc1));
		memset(argc2, 0, sizeof(argc2));

		if (valorepipe == 3)
		{
			char *pt0 = argv[0];	//puntatore al primo comando
			char *pt1 = argv[1];	//puntatore al secondo comando
			char *pt2 = argv[2];	//puntatore al terzo comando,scorrere il comando e vedere quanti pipe ci sono
			multiparsing(pt0, pt1, pt2, argc, argc1, argc2);
			exec_pipe(comando, argc, argc1, argc2, valorepipe);
		}
		else
		{
			char *pt0 = argv[0];	//puntatore al primo comando
			char *pt1 = argv[1];	//puntatore al secondo comando
			singleparsing(pt0, pt1, argc, argc1);
			exec_pipe(comando, argc, argc1, argc2, valorepipe);
		}
	}
}

//Leggge il comando e chiama le funzioni corrispondenti
void readCommand(char *comando)
{
	//Qui controlla se sono stati inseriti comandi dove sono presenti pipe o redirezioni
	char *pipe = strchr(comando, '|');

	if (pipe != NULL) exec(comando);	//In caso affermativo chiamo la exec per la pipe
	else
	{
		char *argv[ARG];	//Buffer degli argomenti/parametri del comando
		char *token = strtok(comando, " ");	//Con strtok estraggo il comando e lo inserisco nel buffer
		argv[0] = token;	//Lo inserisco nella prima posizione
		int count = 1;	//Variabile segnaposto
		while (token != NULL)	//Ciclo il comando
		{
			token = strtok(NULL, " ");	//Estraggo i parametri
			//Controlla se è stata inserita una variabile d'ambiente
			if (token != NULL && strchr(token, '$') != NULL) token = espansione(token);
			argv[count] = token;	//Li inserisco all'interno del buffer
			count++;
		}

		//controllo se è presente redirezione
		int filedes = -1;
		char namef[ARG];
		count = count - 2;
		strcpy(namef, argv[count]);
		if (*argv[count] == '>' || *argv[count] == '<')
		{
			filedes = redirezione(namef);
		}

		//eseguo il comando
		int pid = fork();	//Eseguo la fork
		if (pid < 0)
		{
			perror("ERRORE FORK");
			return;
		}
		if (pid == 0)
		{
			if (filedes != -1)
			{
				if (*argv[count] == '>')
				{
					if (dup2(filedes, 1) == -1)
					{
						perror("ERRORE DUP");
						return;
					}
				}
				else
				{
					if (dup2(filedes, 0) == -1)
					{
						perror("ERRORE DUP");
						return;
					}
				}
				argv[count] = NULL;
			}
			//Eseguo il comando (agv[0] e il comando e argv sono i parametri)
			if (execvp(argv[0], argv) == -1)
			{
				perror("ERRORE EXEC");
				exit(0);
			}
		}
		else
		{
			wait(NULL);
		}
	}
}

//Esegue il comando CD
void openDir(char *buffer)
{
	char *cartella = &buffer[3];
	if (controlCD(cartella))
	{
		if (chdir(cartella))
		{
			fprintf(stderr, "\e[91;1mERROR:\e[m %s\n", strerror(errno));
		}	//Se ci sono problemi mando il segnale di errore
	}
}
