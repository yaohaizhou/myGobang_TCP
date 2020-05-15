#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>
#include "serv_define.h"

extern int clnt_wait;
extern pthread_mutex_t mut;

Package* mock_create_package(){
    char s[30] = {0};
    printf("please input someting:");
    fgets(s,30,stdin);
    Package * pkg = (Package*)malloc(sizeof(Package)+sizeof(s));
    memcpy(pkg->buf,s,sizeof(s));
    pkg->bufferLength = sizeof(s);
    pkg->function = 10;
    pkg->extendedFlag = 23;
    pkg->dataArray[0] = 33;
    pkg->dataArray[1] = 44;
    return pkg;
}

/*wait two thread to finish*/
void thread_wait(pthread_t thread[])
{
    /*等待线程结束*/
    if(thread[0] !=0)
    {       
            pthread_join(thread[0],NULL);
            printf("线程1已经结束\n");
    }
    if(thread[1] !=0) 
    {  
            pthread_join(thread[1],NULL);
            printf("线程2已经结束\n");
    }
}

/*close the game and free everything here*/
void close_game(Game_Msg* gameMsg)
{
    close(gameMsg->clnt_sock[0]);
    close(gameMsg->clnt_sock[1]);
    free(gameMsg);
    printf("game closed!\n");
}

/*create a package to head ptr*/
void create_package(Package* head,int F,int Ex,int arr1,int arr2,int BFL,char* BUF)
{
    head->function = F;
    head->bufferLength = BFL;
    head->dataArray[0] = arr1;
    head->dataArray[1] = arr2;
    head->extendedFlag = Ex;
    if(BFL!=0){
        memcpy(head->buf,BUF,BFL);
    }
}

/*deal with the package*/
int pkg_process(Package* pkg, Game_Msg* msg, int self_sock, int competitor_sock, Chess_Msg side)
{
    int if_error = 0;
    int x = pkg->dataArray[0]-1;//1-15 to 0-14
    int y = pkg->dataArray[1]-1;//1-15 to 0-14
    switch(pkg->function){
        case 2:  // chess position
            pthread_mutex_lock(&mut);
            printf("f2 recv:pos: %d,%d\n",x,y);
            printf("side:%d\n",side);
            if(msg->worb==side && msg->bored[x][y]==blank)   // it's your turn
            {
                msg->round_num = msg->round_num + 1; // round ++
                msg->bored_round[x][y] = msg->round_num;  //  record round
                msg->bored[x][y] = msg->worb;  // bored position
                msg->worb = (side==whiteside?blackside:whiteside);  // change turn
            }
            else
            {
                perror("sent chess position wrong\n");
                if_error = 2;
            }
            pthread_mutex_unlock(&mut);
            if(if_error==0)
            {
                write(competitor_sock,pkg,sizeof(Package)); //forward to the competitor
            }
            
            break;
        case 4:   // message
            write(competitor_sock,pkg,sizeof(Package)+pkg->bufferLength); // forward without check
            break;
        default:
            if_error = pkg->function;
            break;
    }


    return if_error;
}

/*to check if the game is over*/
void check_win(){
    
}
