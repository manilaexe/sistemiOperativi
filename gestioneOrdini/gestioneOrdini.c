//ESAME 11-09-2024
//interfaccia: gestione_ordini dir
//le informazioni sugli ordini sono salvate in file di testo con estensione “YYYYMMDD.txt” che contengono contiene gli ordini effettuati in un giorno specifico.
//P0 che riceve da terminale il codice del corriere di interesse, la data di interesse (formato YYYYMMDD) e il numero N (intero maggiore di 0) di risultati da mostrare a video.
//P0 crea tre processi figli: P1, P2 e P3.
//P1 seleziona solo gli ordini per il corriere specificato nella data di interesse e passa queste informazioni a P2.
//P2 ordina le informazioni ricevute in base al peso (ordine crescente) e invia queste informazioni a P3.
//P3 seleziona gli N ordini con il peso maggiore e li manda a P0 che li mostra a video.
//L’applicazione termina quando l’utente invia un Ctrl-C dal terminale e, prima di terminare, mostra il numero totale di richieste processate fino a quel momento.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int nric=0;

void signal_handler(int signo){
    printf("\nNumero di richieste soddisfatte: %d\n",nric);
    printf("++++Programma terminato++++\n");
    exit(1);
}

int main(int argc, char **argv){

    int fd, p1, p2, p3, p1p2[2], p2p3[2], p3p0[2], status;
    char data[10], to_open[20], cod[10], buff[100], pern[5];
    int n;
    
    if(argc!=2){
        fprintf(stderr,"\nuso: %s <dir>\n",argv[0]);
        printf("++++Programma terminato++++\n");
        exit(2);
    }

    if(argv[1][0]=='/'){
        fprintf(stderr,"Il percorso della directory deve essere relativo\n");
        printf("++++Programma terminato++++\n");
        exit(3);
    }

    fd=open(argv[1],__O_DIRECTORY);
    if(fd<0){
        fprintf(stderr,"Errore nell'apertura della directory\n");
        printf("++++Programma terminato++++\n");
        exit(4);
    }
    close(fd);

    signal(SIGINT,signal_handler);

    while(1){
        printf("Inserire la data di interesse (YYYYMMDD): ");
        scanf("%s",data);
        printf("Inserire in codice indentificativo: ");
        scanf("%s",cod);
        printf("Inserire il numero di righe da visualizzare: ");
        scanf("%d",&n);
        if(n<=0){
            printf("Errore: il numero deve essere maggiore di 0\n");
            printf("++++Programma terminato++++\n");
            exit(5);
        }
        sprintf(to_open,"%s/%s.txt",argv[1],data);
        //printf("\n NOME: %s",to_open);

        pipe(p1p2);
        p1=fork();
        if(p1<0){
            printf("Errore nel processo p1\n");
            printf("++++Programma terminato++++\n");
            exit(6);
        }
        if(p1==0){//figlio p1
            close(1);
            dup(p1p2[1]);
            close(p1p2[1]);
            close(p1p2[0]);
            execlp("grep","grep",cod,to_open,(char *)0);
            printf("Errore nella execlp grep\n");
            printf("++++Programma terminato++++\n");
            exit(7);
        }

        pipe(p2p3);
        p2=fork();
        if(p2<0){
            printf("Errore nel processo p2\n");
            printf("++++Programma terminato++++\n");
            exit(8);
        }
        if(p2==0){
            close(1);
            close(0);
            close(p1p2[1]);
            dup(p1p2[0]);
            close(p1p2[0]);
            dup(p2p3[1]);
            close(p2p3[1]);
            close(p2p3[0]);
            execlp("sort","sort","-n",(char *)0);
            printf("Errore nella execlp sort\n");
            printf("++++Programma terminato++++\n");
            exit(9);
        }
        close(p1p2[0]);
        close(p1p2[1]);

        pipe(p3p0);
        p3=fork();
        if(p3<0){
            printf("Errore nel processo p3\n");
            printf("++++Programma terminato++++\n");
            exit(10);
        }    
        if(p3==0){
            close(0);
            close(1);
            close(p2p3[1]);
            dup(p2p3[0]);
            close(p2p3[0]);
            dup(p3p0[1]);
            close(p3p0[1]);
            close(p3p0[0]);
            sprintf(pern,"%d",n);
            execlp("tail","tail","-n",pern,(char *)0);
            printf("Errore nella execlp tail\n");
            printf("++++Programma terminato++++\n");
            exit(11);
        }  
        close(p2p3[0]);
        close(p2p3[1]);
        close(p3p0[1]);
        
        while(read(p3p0[0],buff,sizeof(buff))>0){
            write(1, buff, strlen(buff)+1);
        }

        close(p3p0[0]);
        
        wait(&status);
        wait(&status);
        wait(&status);

        nric++;
    }

    return 0;
}
