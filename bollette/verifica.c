//ESAME 14-07-2020
//interfaccia: verifica file.txt
//p0 riceve da terminale la tipologia di bollette da verificare e crea 3 figli
//p1 deve selezionare le bollette della tipologia di interesse e inviare tali informazioni al processo P2
//p2 si incarica di selezionare le bollette ancora da pagare e inviarle a P3
//p3 è incaricato di visualizzarle a terminale in ordine decrescente, da quella con importo maggiore a quella di importo minore.
//termina con ctrl-c o esci
//visualizza il numero di richieste soddisfatte prima di terminare

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int nric=0;

void signal_handler(int signo){
    printf("Numero di richieste soddisfatte: %d\n");
    printf("\n++++Programma Terminato++++\n");
    exit(1);
}


int main(int argc, char **argv){

    int p1, p2, p3, p1p2[2], p2p3[2], status, fd;
    char tipo[40];

    if(argc!=2){
        fprintf(stderr, "uso: %s <file.txt>\n",argv[0]);
        exit(2);
    }

    fd=open(argv[1], O_RDONLY);
    if(fd<0){
        printf("Errore nell'apertura del file\n");
        exit(3);
    }
    close(fd);

    signal(SIGINT, signal_handler);

    while(1){
        printf("Inserire la tipologia di bollette che si desidera visualizzare (\"esci\" per terminare): ");
        scanf("%s",tipo);
        if(strcmp(tipo,"esci")==0){
            printf("Numero di richieste soddisfatte: %d\n");
            printf("\n++++Programma Terminato++++\n");
            exit(4);
        }

        pipe(p1p2);
        p1=fork();
        if(p1<0){
            printf("Errore nela creazione del processo p1\n");
            exit(5);
        }
        if(p1==0){ //figlio p1
            close(1);
            dup(p1p2[1]);
            close(p1p2[1]);
            close(p1p2[0]);
            execlp("grep","grep",tipo,argv[1],(char *)0);
            printf("Errore nella execlp\n");
            exit(6);
        }
        close(p1p2[1]);

        pipe(p2p3);
        p2=fork();
        if(p2<0){
            printf("Errore nela creazione del processo p2\n");
            exit(7);
        }
        if(p2==0){//figlio p1
            close(1);
            close(0);
            dup(p1p2[0]); //
            close(p1p2[0]); //
            dup(p2p3[1]);
            close(p2p3[1]);
            close(p2p3[0]);            
            execlp("grep","grep","da_pagare",(char *)0);
            printf("Errore nella execlp\n");
            exit(8);    
        }
        close(p1p2[0]);
        close(p2p3[1]);

        p3=fork();
        if(p3<0){
            printf("Errore nela creazione del processo p2\n");
            exit(9);
        }    
        if(p3==0){
            //close(1);
            close(0);
            dup(p2p3[0]); //
            close(p2p3[0]);
            execlp("sort","sort","-r","-n",(char *)0);
            printf("Errore nella execlp\n");
            exit(10);             
        }   
        close(p2p3[0]);
        wait(&status);
        wait(&status);
        wait(&status);

        nric++;
    }

    return 0;
}
