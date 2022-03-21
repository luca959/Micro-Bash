//Tommaso Coronati - Luca Ferrari
//Laboratorio MicroBash
#include "header.h"

int main(int argc, char
	const *argv[])
{
	//Buffer input
	char dr[N];
	char filename[N];
	for (;;)
	{
		//Stampo nome del programma e percorso corrente
		char *bashstring = "\e[32;1mMicroBash $: \e[m \e[93;1m";
		printf("%s%s \e[m", bashstring, getcwd(dr, sizeof(dr)));
		printf("%s ", "  ");
		fgets(filename, sizeof(filename), stdin);

		//controllare per string vuota
		if (filename != NULL) filename[strlen(filename) - 1] = '\0';	//Elimino il terminatore di riga

		//Controllo se l'utente ha premuto CTRLDX + D (feof) o ha scritto exit
		if (feof(stdin) || strcmp(filename, "exit") == 0) {printf("\n");
		exit(0);
	}

		//controllo se il comando da eseguire è cd, chiamo la funzione corrispondente e riavvio il ciclo del main
		if (filename[0] == 'c' && filename[1] == 'd' && filename[2] == ' ') {openDir(filename);
		continue;
	}

		//se no eseguo readCommand
		readCommand(filename);
	}
}
