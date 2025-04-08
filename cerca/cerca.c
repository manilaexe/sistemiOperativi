//conta il numero di righe in cui le stringhe richiste da;;'utente compaiono in u file
//interfaccia: cerca <nomefile> <stringa1> ... <stringaN>
//per ogni stringa il programma crea un processo figlio che conta il numero di righe in cui la stringa compare all'interno del file passato come parametro e le stampa a video

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char **argv){

    int pid, status;

    if(argc<3){
        fprintf(stderr,"uso: cerca <nomefile> <stringa1> ... <stringaN>\n");
        exit(1);
    }

    for(int i=2; i<argc; i++){
        pid=fork();
        if(pid<0){
            perror("Errore nel fork");
            exit(2);
        }else if(pid==0){ //figlio
            execlp("grep","grep","-c",argv[i],argv[1], (char *)0);
            perror("Errore nella execlp");
            exit(3);
        }else{ //padre
            wait(&status);
        }
    }
    return 0;
}
