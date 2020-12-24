#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>
#include "tcp.h"
#include <sys/ipc.h>

#define MAXQUEST 10
#define MAXQUEST_SORCE 14
#define MAX_BUF 256
#define PORT_NUM 17214
#define MAX_CLIENT 3

int score1[10];

int questList[MAXQUEST_SORCE];
int answerList[MAXQUEST_SORCE];

extern int make_server_socket(int);
int main(int ac, char *av[])
{
    int i, n, sock, fd, client_num=0;
    sock = make_server_socket(PORT_NUM);

    while (1){  //client 네 명까지 들어올 수 있음
        fd = accept(sock, NULL, NULL);
        client_num++;
        MsgType msg;

        //question.txt, answer.txt에서 랜덤하게 출제
        srand((unsigned int)time(NULL));

        int x[MAXQUEST_SORCE];
        int j;
        int m;

        for (j = 0 ; j < MAXQUEST_SORCE; j++)   {
            x[j] = j;
        }
        for (j = 0; j < MAXQUEST_SORCE; j++)    {
            int tmp = (rand()*3);
            int k=0;
            int l=0;

            k=(tmp % (MAXQUEST_SORCE-j)) +j;
            l=x[j];
            x[j] = x[k];
            x[k] = l;
      }
        for(i=0;i<MAXQUEST_SORCE;i++)   {
            questList[i]    =   x[i];
            answerList[i]   =   x[i];
        }
 //rand

        listen(fd, 1);
        printf("Server started...\n");

        char ans_str[MAX_BUF];
        char r_ans_str[MAX_BUF];
        char Question[MAXQUEST_SORCE][MAX_BUF];
        char answer[MAXQUEST_SORCE][MAX_BUF];
        char userlist[5][100];
        char userscore[5][100];
        FILE *fp1;
        FILE *fp2;
        FILE *fp3;
        FILE *fp4;
        char name[80];

        if((fp1 = fopen("./question.txt", "rt")) == NULL){
            perror("fopen");
            exit(1);
        }
        printf("Question_File Open\n");

        for(i=0;i<MAXQUEST_SORCE;i++)   {
            fgets(Question[i], MAX_BUF, fp1);
       } //txt 파일에서 문제 배열로 읽어오기

        fclose(fp1);
        printf("Question_File Close\n");

        if((fp2 = fopen("./answer.txt", "rt")) == NULL){
            perror("fopen");
            exit(1);
        }
        printf("Answer_File Open\n");

        for(i=0;i<MAXQUEST_SORCE;i++)   {
            fgets(answer[i], MAX_BUF, fp2);
        }//answer 파일에서 답 배열로 읽어오기

        fclose(fp2);
        printf("Answer_File Close\n");


        //이름 입력받기
        if ((n=read(fd, (char *)&msg, sizeof(msg))) < 0){
            perror("read");
            exit(1);
        }
        strncpy(name, msg.data, sizeof(msg.data));

        if(!strcmp(name, "end"))
            close(fd);

        printf("\n\nclient's name is %s\n", name);
        if(client_num ==1){
            if((fp3 = fopen("./userList.txt", "w")) == NULL){
                perror("fopen");
                exit(1);
            }
            fprintf(fp3, "%s", name);
            fclose(fp3);
        }
        else{
            if((fp3 = fopen("./userList.txt", "a+t")) == NULL){
                perror("fopen");
               exit(1);
            }
            fprintf(fp3, "\n%s", name);
            fclose(fp3);
      }

        //이름 입력받아  순서대로 저장
   sleep(2);
        //ready 입력받기

        if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0)  {
            perror("read");
            exit(1);
        }
       sleep(2);
        while(strncmp("ready", msg.data, 5))    {
            strcpy(msg.data, "please press 'ready'\n");
            printf("please press 'ready' - %ld\n",strlen(msg.data));

            if (write(fd, (char *)&msg, sizeof(msg)) < 0)  {
                perror("write");
                exit(1);
            }
            sleep(2);
            if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0)  {
                perror("read");
                exit(1);
            }
        }

        sleep(2);


        sprintf(msg.data, "\nGame Start!!");

        sleep(2);

        if (write(fd, (char *)&msg, sizeof(msg)) < 0)  {
            perror("write");
            exit(1);
        }

        int score=0;

        for(i=0;i<5;i++)     {//5문제 출력
            strcpy(msg.data, Question[questList[i]]);//랜덤출력
            strcpy(ans_str, answer[answerList[i]]);

            sleep(2);
            if (write(fd, (char *)&msg, sizeof(msg)) < 0)  {
                perror("write");
                exit(1);
            }

            printf("\nwaiting\n");

             sleep(2);
            if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0)  {
                perror("read");
                exit(1);
            }


            strcpy(r_ans_str, msg.data);
            strcat(r_ans_str,"\n");
            int ans_strl = (strlen(ans_str)-1);
            int r_ans_strl = strlen(r_ans_str);
            int strl;

            if(ans_strl>=r_ans_strl)
                strl = ans_strl;
            else
                strl = r_ans_strl;
           sleep(2);

            if(!strcmp(ans_str, r_ans_str)){
                score++;
                printf("Score - %d\n",score);
            }
            else
                printf("Score - %d\n", score);
        }

        printf("\nFinal score is %d\n", score);

     strcpy(msg.data, "Game Over!\n");
     sleep(2);
        if (write(fd, (char *)&msg, sizeof(msg)) < 0)  {
            perror("write");
            exit(1);
        }
     sleep(2);
    //final socre 클라이언트로 출력하기
        char temp[80];
        sprintf(temp, "Final score is %d", score);
        strcpy(msg.data, temp);
        sleep(2);
        write(fd, (char *)&msg, sizeof(msg));
        score1[client_num]=score;
//4번째 클라이언트일 때 Ranking 출력

        if(client_num ==1){
            if((fp4 = fopen("./userScore.txt", "w")) == NULL){
                perror("fopen");
              exit(1);
            }
            fprintf(fp4, "%d", score);
            fclose(fp4);
        }
        else{
            if((fp4 = fopen("./userScore.txt", "a+t")) == NULL){
                perror("fopen");
                exit(1);
            }
            fprintf(fp4, "\n%d", score);
            fclose(fp4);
        }

        if((fp3 = fopen("./userList.txt", "r+t")) == NULL){
            perror("fopen");
            exit(1);
        }

        for(i=0; i<client_num; i++){
            fgets(userlist[i], MAX_BUF, fp3);
        }
        fclose(fp3);

        if((fp4 = fopen("./userScore.txt", "r+t")) == NULL){
            perror("fopen");
            exit(1);
        }
        for(i=0; i<client_num; i++){
            fgets(userscore[i], MAX_BUF, fp4);
        }
        fclose(fp4);

        char tmp[256]={0, }, tmp2[256]={0, };
        int tmp_score[client_num];

        printf("\n\n************Score**************\n");
        for(i=0; i<client_num; i++){
            tmp_score[i] = atoi(userscore[i]);
        }

        i=0;
        do{
        if(client_num==1)
                break;
            for(j=1; j<client_num; j++){
                if(tmp_score[i] > tmp_score[j]){
                    strcpy(tmp, userlist[i]);
                    strcpy(userlist[i], userlist[j]);
                    strcpy(userlist[j], tmp);

                    strcpy(tmp, userscore[i]);
                    strcpy(userscore[i], userscore[j]);
                    strcpy(userscore[j], tmp);
                }
            }
            i++;
        }while(i<client_num-1);

        for(i=0; i<client_num ; i++){
            printf("\nuserid: %s", userlist[i]);
            printf("\nuserscore: %s", userscore[i]);
        }
       strcpy(tmp2, userlist[client_num-1]);
        strcat(tmp2, "  ");
        strcat(tmp2, userscore[client_num-1]);
        strcpy(msg.str_rank, tmp2);
        sleep(2);

        if(write(fd, (char *)&msg, sizeof(msg)) < 0){
            perror("write");
            exit(1);
        }
        printf("\n\n\n");
        usleep(10000);

        if(client_num > MAX_CLIENT - 1)
            break;
    }
    sleep(2);
    close(fd);
}
