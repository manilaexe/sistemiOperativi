//ESAME 13-06-2024
//interfaccia: prestiti_biblioteca <dir>
//P0 che riceve da terminale il codice del libro (stringa), il mese (AAAAMM) e il numero N (intero maggiore di zero) dirisultati da mostrare a video.
//P0 crea tre processi figli: P1, P2 e P3.
//P1 seleziona solo i prestiti per il libro specificato nel mese di interesse e passa queste informazioni a P2.
//P2 ordina i prestiti in base alla data della richiesta in ordine decrescente e invia queste informazioni a P3.
//P3 seleziona gli N prestiti più recenti e li mostra a video.
//L’applicazione termina quando l’utente invia un ctrl-C dal terminale e, prima di terminare, mostra il numero totale di richieste processate fino a quel momento.


//DA PROVARE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

int nreq=0;

void signal_handler(int signo){
    printf("\nRichieste soddisfatte: %d\n",nreq);
    printf("++++Programma Terminato++++\n");
    exit(1);
}

int main(int argc, char **argv){

    int fd, p1, p2, p3, p1p2[2], p2p3[2], status;
    int n;
    char cod[100], data[100], to_open[100], forn[10];

    if(argc!=2){
        fprintf(stderr,"\nuso: %s <dir>\n",argv[0]);
        printf("++++Programma Terminato++++\n");
        exit(2);
    }

    if(argv[1][0]=='/'){
        fprintf(stderr,"Il percorso della cartella deve essere relativo\n");
        printf("++++Programma Terminato++++\n");
        exit(3);
    }
    
    fd=open(argv[1],__O_DIRECTORY);
    if(fd<0){
        fprintf(stderr,"Impossibile aprire la cartella\n");
        printf("++++Programma Terminato++++\n");
        exit(4);
    }
    close(fd);

    signal(SIGINT,signal_handler);

    while(1){
        printf("Inserire la data di interesse (formato YYYYMM): ");
        scanf("%s",data);
        printf("Inserire il codice identificativo del libro: ");
        scanf("%s",cod);
        printf("Inserire il numero massimo di riche che si vogliono visualizzare: ");
        scanf("%d",&n);
        if(n<=0){
            printf("Il numero di righe da visualizzare deve esserer maggiore di zero\n");
            printf("++++Programma Terminato++++\n");
            exit(5);
        }

        sprintf(to_open,"%s/%s.txt",argv[1],data);

        pipe(p1p2);
        p1=fork();
        if(p1<0){
            perror("Errore nel processo p1\n");
            printf("++++Programma Terminato++++\n");
            exit(6);
        }
        if(p1==0){
            close(1);
            dup(p1p2[1]);
            close(p1p2[1]);
            close(p1p2[0]);
            execlp("grep","grep",cod,to_open,(char *)0);
            printf("Errore nella execlp di grep\n");
            printf("++++Programma Terminato++++\n");
            exit(7);
        }

        pipe(p2p3);
        p2=fork();
        if(p2<0){
            perror("Errore nel processo p2\n");
            printf("++++Programma Terminato++++\n");
            exit(8);          
        }
        if(p2==0){
            close(1);
            close(0);
            dup(p1p2[0]);
            close(p1p2[0]);
            close(p1p2[1]);
            dup(p2p3[1]);
            close(p2p3[1]);
            close(p2p3[0]);
            execlp("sort","sort","-r",(char *)0);
            printf("Errore nella execlp di sort\n");
            printf("++++Programma Terminato++++\n");
            exit(9);      
        }

        close(p1p2[0]);
        close(p1p2[1]);
        close(p2p3[1]);

        p3=fork();
        if(p3<0){
            perror("Errore nel processo p3\n");
            printf("++++Programma Terminato++++\n");
            exit(10);          
        }
        if(p3==0){
            close(0);
            dup(p2p3[0]);
            close(p2p3[0]);
            close(p2p3[1]);
            sprintf(forn,"%d",n);
            execlp("head","head","-n",forn,(char *)0);
            printf("Errore nella execlp di sort\n");
            printf("++++Programma Terminato++++\n");
            exit(11); 

        }

        close(p2p3[0]);
        close(p2p3[1]);

        wait(&status);
        wait(&status);
        wait(&status);

        nreq++;

    }

    return 0;
}
