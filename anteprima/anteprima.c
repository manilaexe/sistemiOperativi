//visualizza una anteprima di un file di testo nella directory corrente
//interfaccia: anteprima <numRighe>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#define N 50

int main(int argc, char **argv){

    int pid, status, fd;
    char name[N];

    //controllo numero argomenti
    if(argc!=2){
        fprintf(stderr,"uso: anteprima <numRighe>\n");
        exit(1);
    }

    if(atoi(argv[1])<=0){ //atoi="ASCII to integer", converte la stringa argv[1] in un numero intero
        perror("padre atoi");
        exit(2);
    }

    printf("Inserisci il nome del file da visualizzare o \"fine\" per uscire: ");
    scanf("%s",name);

    while(strcmp(name,"fine")!=0){
    
        fd=open(name, O_RDONLY);
        if(fd<0){
            perror("Errore nell'apertura del file: file inesistente");
            exit(3);
        }else{
            close(fd); //lo chiudo tanto lo leggo con l'execlp
            pid=fork();
            if(pid<0){
                perror("Errore nella fork\n");
                exit(4);
            }
            if(pid==0){ //sono nel figlio
                printf("\nAnteprima del file %s",name);
                execlp("head","head","-n",argv[1],name,(char *)0);
                perror("Errore nella execlp\n");
                exit(5);
            }
            wait(&status);
        }
        printf("\nInserisci il nome del file da visualizzare o \"fine\" per uscire: ");
        scanf("%s",name);
    }
    return 0;
}