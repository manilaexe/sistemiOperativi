//permette di scrvere su un file delle stringhe fornite dall'utente in input. 
//interfaccia: copy_input <nomefile>
//deve terminare quando l'utente scrive "fine" 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){

    int fd;
    char s[1024];

    //controllo degli argomenti
    if(argc != 2){
        fprintf(stderr,"uso: copy_input <nomefile>\n");
        exit(1);
    }

    //creo il file con la system call open, i flag e i permessi (0644)
    fd=open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if(fd<0){
        perror("Errore nell'apertura del file");
        exit(2);
    }

    printf("\nInserire una stringa di caratteri (il programma termina all'inserimento della parola: fine): \n");

    do{  
        fgets(s,sizeof(s),stdin);//come la scanf ma meglio 
        write(fd,s,strlen(s));
    }while(strcmp(s,"fine\n")!=0);

    return 0;
}