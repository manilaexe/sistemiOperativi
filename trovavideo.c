//ESAME 16-06-2020

//interfaccia: trova_video <dir>
//visualizzare i video piu richiesti su youtube
//dir contiene tutte le informazioni sulle visualizzazioni dei video, conservate in molteplici file di testo in formato YYYYMM.txt 
//ciascuna riga del file contiene le informazioni relative a un video, con il numero di visualizzazioni, il nome del video, il tipo di video
//p0 chiede da terminale il tipo di video e la data (YYYYMM) e crea du figli (p1, p2)
//p1 seleziona i video della tipologia del mese richiesto e li invia a p2
//p2 li ordina in modo decrescente sulle visualizzazioni e stampa
//p0 termina al ctrl-c dopo aver stampato il numero di richieste servite

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int nric=0;

void signal_handler(int signo){
    printf("\nRichieste servite: %d\n", nric);
    printf("\n++++Programma terminato++++\n");
    exit(1);
}

int main(int argc, char **argv){
    int p1, p2, p1p2[2], fd, status;
    char data[7], tipo[40], to_open[20];

    if(argc!=2){
        fprintf(stderr,"uso: trova_parola <dir>\n");
        exit(2);
    }

    if(argv[1][0]=='/'){
        fprintf(stderr,"Erorre: dir deve essere il nome relativo della directory\n");
        exit(3);
    }

    fd=open(argv[1], __O_DIRECTORY);
    if(fd<0){
        printf("Errore nell'apertura della directory\n");
        exit(4);
    }

    close(fd);

    signal(SIGINT,signal_handler);

    while(1){
        printf("\nInserire la tipologia di video desiderata: ");
        scanf("%s",tipo);
        printf("\nInserire il mese desiderato (formato: YYYYMM): ");
        scanf("%s",data);

        sprintf(to_open, "%s/%s.txt", argv[1], data);
        //strcat(data,".txt");
        printf("NOME FILE: %s", to_open);
        
        fd=open(to_open,O_RDONLY);
        if(fd<0){
            fprintf(stderr,"Errore nell'apertura del file\n");
            exit(5);
        }
        close(fd);
        
        pipe(p1p2);
        p1=fork();
        if(p1<0){
            printf("Errore nel fork\n");
            exit(6);
        }
        if(p1==0){//figlio p1
            close(1);
            dup(p1p2[1]);
            close(p1p2[0]); //chiude la lettura
            close(p1p2[1]); //chiude la scrittura
            execlp("grep","grep",tipo,to_open,(char *)0);
            perror("Errore nella execlp\n");
            exit(7);
        }

        p2=fork();
        if(p2<0){
            printf("Errore nel fork\n");
            exit(8);
        }
        if(p2==0){//figlio p2
            close(p1p2[1]);
            close(0);
            dup(p1p2[0]);
            close(p1p2[0]);
            execlp("sort","sort","-r",(char *)0);    
            perror("Errore nella execlpn\n");
            exit(9);
        }
        //chiudo tutti gli estremi della pipe
        close(p1p2[0]);
        close(p1p2[1]);

        wait(&status);
        wait(&status);

        nric++;
    }

    return 0;
}