//PROGRAMMA DA ESAME

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int richiesteServite=0;

void  sigint_handler(int signo){

}

int main(int argc, char **argv){
    int fd, p1, p2, p1p2[2], p2p0[2], status;
    char corso[80];
    char res[1024];

    //controllo argomenti
    if(argc != 2) { 
        fprintf(stderr, "Uso: %s <file>\n", argv[0]);
        exit(1);
    }
   
    signal(SIGINT, sigint_handler); //installo gestore per il segnale

    //parte principale nel testo

    printf("Inserisci il nome del corso:\n");
    scanf("%s",corso);

    while(strcmp(corso,"fine")!=0){
       pipe(p1p2); //canale di comunicazione unidirezionale tra processi
                   //p1p2[0]=lettura 
                   //p1p2[1]=scrittura
       p1=fork();
       //fa il grep e cerca tutte le righe del file che hanno la stringa inserita dall'utente
       if(p1==0){//operazione figlio p1
            close(1); //chiude l'output standard nel processo figlio
            dup(p1p2[1]); //copia il file descriptor di scrittura della pipe
                          //qualsiasi output generato dal processo figlio verrà inviato attraverso la pipe, invece che essere stampato sulla console
            close(p1p2[0]); //chiude la lettura dalla pipe
            close(p1p2[1]); //chiude la scrittura della pipe originale per evitare conflitti
            execlp("grep","grep",corso,argv[1],(char *)0);
            perror("Errore dopo l'exec di P1");
       }
       pipe(p2p0);
       p2=fork();
       //legge dalla pipe

       if(p2==0){//operazione figlio p2
            close(0);
            dup(p1p2[0]);
            close(p1p2[0]);
            close(p1p2[1]);
            //redirezione in uscita
            close(1);
            dup(p2p0[1]);//estremo di scrittura
            close(p2p0[0]);
            close(p2p0[1]);
            execlp("sort","sort","-r",(char *)0);//-r perche decrescente
            perror("Errore dopo l'exec di P2");
       }

       //sono del padre (P0)

        //chiudo tutti gli estremi delle pipe che non uso (devo solo leggere dalla pipe p2p0)
        close(p1p2[0]);
        close(p1p2[1]);
        close(p2p0[1]);
        
        while(read(p2p0[0],res,sizeof(res))>0){ //res=buffer
            write(1, res, strlen(res)+1); //scrive su console
        }

        close(p2p0[0]);

        //NON LASCIARE PROCESSI ZOMBIE, un wait per ogni processo
        wait(&status);
        wait(&status);

        richiesteServite++;

        printf("Inserisci il nome del corso:\n");
        scanf("%s",corso);

    }
}
