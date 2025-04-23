    //ESAME 15-06-2023
    //interfaccia: voli_effettuati <dir>
    //i voli di un aereo sono salvati in un file dentro alla cartella (es. V0001.txt ha tutti i voli effettuati dal velivolo di codice identificativo 0001)
    //ciascuna riga contiene: durata del volo (in minuti), codice volo, città di partenza, città di arrivo, data del volo (nel formato AAAAMMGG)
    //p0 riceve da terminale un identificativo, una data e un intero per visualizzare N risultati a video
    //p0 genera 3 figli: p1, p2, p3
    //p1 seleziona i voli effettuati dall’aereo indicato e nella data specificata dall’utente e invia tali informazioni a p2
    //p2 ordina i voli in modo crescente (sulla base della durata del volo) e invia tali informazioni a p3
    //p3 mostra a video gli N voli più lunghi e inviare il segnale SIGUSR2 a p0 che tiene traccia del numero di segnali SIGUSR2 ricevuti e lo stampa a video ogniqualvolta riceve un segnale SIGUSR2
    //termina quando si inserisce la stringa "fine"


    #include <stdlib.h>
    #include <stdio.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <string.h>

    static volatile sig_atomic_t count_sigusr2=0;

    void handler_sigusr2(){
        count_sigusr2++;
        printf("Numero di segnali SIGUSR2 ricevuti: %d\n",count_sigusr2);
    }

    int main(int argc, char **argv){

        int fd, p1, p2, p3, p1p2[2], p2p3[2], p3p0[2], status, n;
        char icao[10], data[10], to_open[20], buff[50];

        if(argc!=2){
            fprintf(stderr, "uso: %s <dir>\n",argv[0]);
            printf("\n++++Programma Terminato++++\n");
            exit(1);
        }

        if(argv[1][0]=='/'){
            perror("Il percorso deve essere relativo\n");\
            printf("\n++++Programma Terminato++++\n");
            exit(2);
        }

        fd=open(argv[1], __O_DIRECTORY);
        if(fd<0){
            fprintf(stderr,"Errore nell'apertura della directory\n");
            printf("\n++++Programma Terminato++++\n");
            exit(3);
        }
        close(fd);

        signal(SIGUSR2,handler_sigusr2);

        while(1){
            printf("\nInserire il codice ICAO (fine per terminare): ");
            scanf("%s",icao);
            if(strcmp(icao,"fine")==0){
                printf("\n++++Programma Terminato++++\n");
                exit(4);
            }

            printf("\nInserire la data (formato: YYYYMMDD): ");
            scanf("%s",data);
            if(strcmp(data,"fine")==0){
                printf("\n++++Programma Terminato++++\n");
                exit(4);
            }

            printf("\nInserire il numero massimo di voli che si desidera visualizzare (0 per terminare): ");
            scanf("%d",&n);
            if(n==0){
                printf("\n++++Programma Terminato++++\n");
                exit(4);
            }
            
            sprintf(to_open, "%s/%s.txt", argv[1], icao);
            fd=open(to_open, O_RDONLY);
            if(fd<0){
                fprintf(stderr,"Errore nell'apertura del file\n");
                printf("\n++++Programma Terminato++++\n");
                exit(5);
            }  
            close(fd);

            pipe(p1p2);
            p1=fork();
            if(p1<0){
                perror("Errore durante il fork di p1\n");
                printf("\n++++Programma Terminato++++\n");
                exit(6);
            }
            if(p1==0){//figlio p1
                // imposto gestione default di SIGUSR2
                signal(SIGUSR2, SIG_DFL);

                close(1);
                dup(p1p2[1]);
                close(p1p2[1]);
                close(p1p2[0]);
                execlp("grep","grep",data,to_open,(char *)0);
                perror("Errore durante la execlp\n");
                printf("\n++++Programma Terminato++++\n");
                exit(7);
            }

            pipe(p2p3);
            p2=fork();
            if(p2<0){
                perror("Errore durante il fork di p2\n");
                printf("\n++++Programma Terminato++++\n");
                exit(8);
            }
            if(p2==0){//figlio p2
                // imposto gestione default di SIGUSR2
                signal(SIGUSR2, SIG_DFL);
            
                //chiudo le pipe non necessarie
                close(p1p2[1]);
                close(p2p3[0]);
                //redirezione stdin
                close(0);
                dup(p1p2[0]);
                close(p1p2[0]);
                //redirezione stdout
                close(1);
                dup(p2p3[1]);
                close(p2p3[1]);
                execlp("sort","sort","-n",(char *)0);
                perror("Errore durante la execlp\n");
                printf("\n++++Programma Terminato++++\n");
                exit(9);           
            }
            close(p1p2[0]);
            close(p1p2[1]);

            p3=fork();
            if(p3<0){
                perror("Errore durante il fork di p3\n");
                printf("\n++++Programma Terminato++++\n");
                exit(10);
            }
            if(p3==0){//figlio p3
                // imposto gestione default di SIGUSR2
                signal(SIGUSR2, SIG_DFL);
                // invio segnale SIGUSR2 al padre
                kill(getppid(), SIGUSR2);        
                //chiudo le pipe non necessarie
                close(p2p3[1]);
                //redirezione stdin
                close(0);
                dup(p2p3[0]);
                close(p2p3[0]);
                //redirezione stdout
                sprintf(buff,"%d",n);
                execlp("tail","tail","-n",buff,(char *)0);
                perror("Errore durante la execlp\n");
                printf("\n++++Programma Terminato++++\n");
                exit(11);             
            }

            close(p2p3[0]);
            close(p2p3[1]);

            wait(&status);
            wait(&status);
            wait(&status);

        }

        return 0;
    }
