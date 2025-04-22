//ESAME DEL 04-02-2024

//interfaccia: traffico anno
//anno e' un nome relativo di directory che rappresenta un anno nel formato YYYY
//le info relative ad ogni giornoi sono in u file di testo  nel formato MMDD.trf
//ogni riga del file contiene le info relative a quelo giorno. #veicoliTransitati, orario(HH:MM), nomeStrada, statoTraffico.
////p0 riceve da terminale il giorno e la strada (no spazi, camelCase)
//ordinato in base al numero dei veicoli transitati
//per ogni richiesta p0 crea 3 figli
//p1 seleziona tutte le righe relative alla data e alla strada (grep) e le invia a p2
//p2 ordina le informazioni in modo decrescente sul #veicoliTransitati (sort) e lo invia a p3
//p3 stampa a video
//termina quando si invia Ctrl-C e prima di terminare stampa a video il numero di richieste servite fino a quel momento

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

int nrichieste=0;

void sigint_handler(){
    printf("Numero di richieste servite: %d\n",nrichieste);
    printf("+++Programma terminato+++\n");
    exit(1);
}

int main(int argc, char **argv){

    int fd, status;
    int p1, p2, p3, p1p2[2], p2p3[0];
    char via[100], giorno[5], to_open[100], res[1024];
    
    //controllo argomenti
    if(argc!=2){
        fprintf(stderr,"uso: traffico <anno>\n");
        exit(2);
    }

    //controllo che anno sia un nome relativo di directory
    if(argv[1][0]=='/'){
        fprintf(stderr,"Errore: anno deve essere un nome relativo di directory\n");
        exit(4);
    }

    //controllo esistenza della directory
    fd=open(argv[1], __O_DIRECTORY);
    if(fd<0){
        perror("Errore nell'apertura della directory\n");
        exit(5);
    }
    close(fd);

    signal(SIGINT,sigint_handler);

    while(1){
        printf("\nInserire il giorno: ");
        scanf("%s",giorno);
    
        printf("\nInserire la via (senza spazi, notazione camelCase): ");
        scanf("%s",via);

        sprintf(to_open, "%s/%s.trf", argv[1], giorno); //costruisco il  nome del file da aprire
        fd=open(to_open, O_RDONLY);
        if(fd<0){
            fprintf(stderr,"Errore nell'apertura del file\n");
            exit(6);
        }
        close(fd);

        pipe(p1p2);
        p1=fork();
        if(p1<0){
            perror("Errore nel processo p1\n");
            exit(3);
        }
        if(p1==0){ //sono nel figlio p1 screma per via
            close(1); //chiude l'output STANDARD
            dup(p1p2[1]);
            close(p1p2[0]);
            close(p1p2[1]);
            execlp("grep","grep",via,to_open,(char *)0);
            perror("Errore nella execlp\n");
            exit(7);
        }

        pipe(p2p3);
        p2=fork();
        if(p2<0){
            perror("Errore nel processo p2\n");
            exit(8);
        }
        if(p2==0){ //sono nel fig  lio p2 che ordina in base al numero di veicoli
            close(0);
            close(p1p2[0]);
            dup(p1p2[0]);
            close(p1p2[0]);
            execlp("sort","sort","-r","-n",(char *)0);
            perror("Errore nella execlp\n");
            exit(9);
        }
        close(p1p2[0]);
        close(p1p2[1]);
        close(p2p3[1]);

        p3=fork();
        if(p3<0){
            perror("Errore nel processo p3\n");
            exit(10);
        }
        if(p3==0){
            close(0);
            while(read(p2p3[0],res,sizeof(res)>0)){
                write(1,res,strlen(res)+1);
            }
            close(p2p3[0]);
        }
        close(p2p3[0]);
        wait(&status);
        wait(&status);
        wait(&status);

        nrichieste++;

    }

    return 0;
}
