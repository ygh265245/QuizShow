
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>

#include <sys/ipc.h>
#include "tcp.h"

#define PORT_NUM 17214

#define IP_ADD "210.117.181.113"

extern int connect_to_server(char *, int);

int main(int ac, char *av[])
{
    int fd;
    fd = connect_to_server(IP_ADD, PORT_NUM);
    if (fd == -1)
        exit(1);


    MsgType msg;
    int n;
    char str[80];
    char name[80];

    printf("\nInput your NAME: ");
    scanf("%s", name);

    sprintf(msg.data, "%s", name);

    sleep(2);

    if (write(fd, (char*)&msg, sizeof(msg)) < 0){
        perror("write");
        exit(1);
    }
    do{
        printf("\ninput READY: ");

       scanf("%s", str);
      sleep(2);
        sprintf(msg.data, "%s", str);
        if(write(fd, (char*)&msg, sizeof(msg)) <0)
        {
            perror("write");
            exit(1);
        }
        sleep(2);
        if((n=read(fd, (char*)&msg, sizeof(msg))) <0)
        {
            perror("read");
            exit(1);
        }
        printf("\t%s\n", msg.data);

    }while(!strncmp(msg.data, "please", 6));
    //ready 입력



    while(1){
        sleep(2);
        if((n=read(fd, (char*)&msg, sizeof(msg))) <0){
            perror("read");
            exit(1);
        }
        sleep(2);
        if(strncmp(msg.data, "Game Over!", 4)){
            printf("\n%s", msg.data);
            printf("Answer (O or X)-> ");
            scanf("%s", str);
               sleep(2);
            sprintf(msg.data, "%s", str);
           sleep(2);
            if(write(fd, (char*)&msg, sizeof(msg)) <0)
              {
                perror("write");
                exit(1);
            }
            sleep(2);
        }

        else{
            printf("\n%s\n", msg.data);
            break;
        }
    }

    sleep(3);

    if((n=read(fd, (char*)&msg, sizeof(msg)))<0){
        perror("read");
        exit(1);
    }
    printf("\n%s\n", msg.data);
sleep(2);
    if((n=read(fd, (char*)&msg, sizeof(msg)))<0){
        perror("read");
        exit(1);
    }

    printf("Winner: %s\n\n", msg.str_rank);
    sleep(2);
    close(fd);

}
