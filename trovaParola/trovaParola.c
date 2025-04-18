//ESERCIZIO DA ESAME
//P0 prende dal terminale il cognome di un cliente e un prefisso per ogni richiesta genera 2 figli 
//P1 seleziona all'interno dell'anagrafica tutti iclienti con il nome indicato e li invia a P2
//P2 filtra i dati ricevuti e stampa a video solo i clienti che risiedono nell'area data dal prefisso
//termina con la stringa "fine" o ^C
//interfaccia: trovaParola <nomeFile>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int nrichieste=0;

void sigint_handler(int signo){
    printf("\n+++Terminazione forzata del programma.+++\n");
    printf("Numero di richieste eseguite: %d\n",nrichieste);
    exit(1);
}

int main(int argc, char **argv){

    int p1, p2, p1p2[2], status;
    char cog[56];
    char pre[40];

    //controllo degli argomenti
    if(argc != 2){
        fprintf(stderr,"uso: trovaParola <nomeFile>\n");
        exit(2);
    }

    signal(SIGINT, sigint_handler);

    printf("inserire il cognome del cliente: ");
    scanf("%s",cog);
    if(strcmp(cog,"fine")==0){
        printf("\n+++Programma terminato.+++\n");
        printf("Numero di richieste eseguite: %d\n",nrichieste);
        exit(3);
    }

    printf("inserire il prefisso telefonico: ");
    scanf("%s",pre);

    while(strcmp(cog,"fine")!=0){
        pipe(p1p2);
        p1=fork();
        if(p1==0){ //figlio p1
            close(1);
            dup(p1p2[1]);
            close(p1p2[0]);
            execlp("grep","grep",cog,argv[1],(char *)0);
            perror("Errore dopo l'execl di P1");
        }

        p2=fork();

        if(p2==0){
            close(p1p2[1]);
            close(0);    
            dup(p1p2[0]);    
            close(p1p2[0]);
            execlp("grep","grep",pre,(char *)0);
            perror("Errore dopo l'execlp di P2");
        }

        close(p1p2[0]);
        close(p1p2[1]);

        wait(&status);
        wait(&status);

        nrichieste++;

        printf("inserire il cognome del cliente: ");
        scanf("%s",cog);

        printf("inserire il prefisso telefonico: ");
        scanf("%s",pre);

    }

    close(p1p2[0]);
    close(p1p2[1]);

    printf("Numero di richieste servite: %d \n",nrichieste);

    return 0;
}

