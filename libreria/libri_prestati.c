//ESAME DEL 05-07-2023

//interfaccia: libri_prestati dir
//P0 che riceve dall’utente (via terminale) un identificativo di un utente e il numero N (intero) di risultati da mostrare a video.
//P0 crea tre processi figli: P1, P2 e P3.
//P1 ordina in modo cronologico i prestiti (sulla base della data di ritiro presente nel primo campo di ogni riga) e invia tali informazioni a P2.
//P2 seleziona solo i prestiti non ancora restituiti (sulla base del secondo campo) e invia tali informazioni a P3.
//P3 seleziona gli N prestiti non ancora restituiti più recenti e invia tali informazioni a P0, che stampa tali informazioni a video.
//P1 non inizia immediatamente l’ordinamento cronologico dei prestiti, ma si sospende in attesa di un segnale di sincronizzazione da parte di P0, che deve inviare a P1 il segnale SIGUSR1.
//L’applicazione termina quando l’utente invia un ctrl-C da terminale e, prima di terminare, stampa a video il numero di richieste servite fino a quel momento.

    /*commenti*/
//non funziona sigusr
//al secondo giro nel while stampa in modo anomalo

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>

int nric=0;

void signal_handler(int signo){
    printf("Richieste soddisfatte: %d \n",nric);
    printf("++++Programma terminato++++\n");
    exit(1);

}

void signUsr_handler(int signo){
    printf("p1 riceve SIGUSR1 e inizia l'ordinamento...\n");
}

int main(int argc, char **argv){

    int fd, p1, p2, p3, p1p2[2], p2p3[2], p3p0[2], status;
    char id[10], to_open[1024], buff[256], pern[10];
    int n;

    if(argc!=2){
        fprintf(stderr,"uso: %s <dir>\n",argv[0]);
        exit(3);
    }

    if(argv[1][0]!='/'){ //dovrebbe essere assoluto ma e` relativo
        fprintf(stderr,"La directory deve essere in un percorso assoluto\n");
        printf("++++Programma terminato++++\n");
        exit(4);
    }

    
    DIR *dir=opendir(argv[1]);
    if(dir==NULL){
        printf("Errore nell'apertura della directory\n");
        printf("++++Programma terminato++++\n");
        exit(5);
    }
    closedir(dir);

    signal(SIGINT,signal_handler);

    while(1){
        printf("Inserire il codice identificativo utente: ");
        scanf("%s",id);
        printf("Inserire il numero di prestiti da visualizzare: ");
        scanf("%d",&n);
        if(n<=0){
            printf("Errore, il numero deve essere maggiore di zero\n");
            printf("++++Programma terminato++++\n");
            exit(6);
        }

        sprintf(to_open, "%s/%s.txt", argv[1],id);
        fd=open(to_open, O_RDONLY);
        if(fd<0){
            printf("Errore nell'apertura del file\n");
            printf("++++Programma terminato++++\n");
            exit(7);
        }
        close(fd);

        pipe(p1p2);
        p1=fork();
        if(p1<0){
            printf("Errore nel processo p1\n");
            printf("++++Programma terminato++++\n");
            exit(8);
        }
        if(p1==0){//figlio p1
            //signal(SIGUSR1, signUsr_handler);
            //pause();
            close(1);
            dup(p1p2[1]);
            close(p1p2[1]);
            close(p1p2[0]);
            execlp("sort","sort","-n",to_open,(char *)0);
            printf("Errore nella execlp\n");
            exit(9);       
        }

        pipe(p2p3);
        p2=fork();
        if(p2<0){
            printf("Errore nel processo p2\n");
            printf("++++Programma terminato++++\n");
            exit(10);
        }
        if(p2==0){//figlio p2
            close(1);
            close(0);           
            dup(p1p2[0]);
            close(p1p2[0]);
            close(p1p2[1]);
            dup(p2p3[1]);
            close(p2p3[1]);
            close(p2p3[0]);
            execlp("grep","grep","NON RESTITUITO",(char *)0);
            printf("Errore nella execlp\n");
            exit(11);  
        }
        close(p1p2[0]);
        close(p1p2[1]);

        pipe(p3p0);
        p3=fork();
        if(p3<0){
            printf("Errore nel processo p3\n");
            printf("++++Programma terminato++++\n");
            exit(12);
        }
        
        if(p3==0){

            //chiudo stdin
            close(0);
            close(p2p3[1]);
            dup(p2p3[0]);
            close(p2p3[0]);

            //chiudo stdout
            close(1);
            close(p3p0[0]);
            dup(p3p0[1]);
            close(p3p0[1]);

            sprintf(pern,"%d",n);
            execlp("tail","tail","-n",pern,(char *)0);
            printf("Errore nella execlp\n");
            exit(13);              
        }

        close(p2p3[0]);
        close(p2p3[1]);
        close(p3p0[1]);

        while(read(p3p0[0], buff, sizeof(buff))>0){
            write(1,buff,strlen(buff)+1);
        }

        close(p3p0[0]);
        //close(fd);
        //kill(p1, SIGUSR1);

        wait(&status);
        wait(&status);
        wait(&status);
        
        nric++;

    }


    return 0;
}
