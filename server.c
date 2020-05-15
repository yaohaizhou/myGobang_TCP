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

/*global variable define*/
int clnt_wait = 0;
pthread_mutex_t mut;

int main(){
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(5000); 
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    //printf("%d\n",ntohs(serv_addr.sin_port));

    listen(serv_sock, 20);
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock[2];
    pthread_t thread[2] = {0};
    //Package * pkg;
    //char str[30];
    while(1)
    {
        printf("start listening,,,\n");
        //scanf("%d",&clnt_sock[1]);  // mock for a accept
        clnt_sock[1] = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        //printf("port:%d\n",ntohs(clnt_addr.sin_port));
        printf("connected!\n");
        if(clnt_wait==0)  // no clnt wait
        {
            clnt_sock[0] = clnt_sock[1];  // temp store the sock 
            clnt_wait++;
        }
        else
        {
            clnt_wait--;
            game_start(thread,clnt_sock);
        }
        
        /*pkg = mock_create_package();
        //printf("%s\n",pkg->buf);
        //printf("%d\n",sizeof(pkg));
        write(clnt_sock, pkg, sizeof(Package)+30);
        read(clnt_sock, str, sizeof(str));
        printf("recive:%s\n",str);
        close(clnt_sock);*/
    }
    close(serv_sock);

    return 0;
}

/*start a new game and initialize a new game!*/
void game_start(pthread_t Thread[],int Clnt_Sock[])
{
    pthread_mutex_init(&mut,NULL);  // intialize the mutex lock
    // initialize game msg
    Game_Msg* gameMsg = (Game_Msg*)malloc(sizeof(Game_Msg));
    memset(gameMsg,0,sizeof(Game_Msg)); // put to zero
    gameMsg->clnt_sock[0] = Clnt_Sock[0];
    gameMsg->clnt_sock[1] = Clnt_Sock[1];
    gameMsg->worb = blackside;
    //create two pthreads
    pthread_create(&Thread[0],NULL,pthread_func,(void*)gameMsg);
    pthread_create(&Thread[1],NULL,pthread_func,(void*)gameMsg);
    thread_wait(Thread);
    close_game(gameMsg);
}

/*pthread_func: when a clnt is accepted, one pthread is created
and this function is used*/
void* pthread_func(void* args)
{
    Game_Msg* gameMsg = args;
    /*player msg define*/
    Chess_Msg player_side;
    int player_num;
    int self_sock,competitor_sock;

    char buffer[BUFFER_SIZE] = {0}; // the buffer for read
    Package* pkg = NULL;  // package ptr

    pthread_mutex_lock(&mut); // lock the gameMsg
    if(gameMsg->thread[0]==0){  // to determine player1 or player2
        gameMsg->thread[0] = pthread_self();
        player_num = 0;
        player_side = blackside;
        self_sock = gameMsg->clnt_sock[0];
        competitor_sock = gameMsg->clnt_sock[1];
        printf("player0's side is black\n");
    }
    else{
        gameMsg->thread[1] = pthread_self();
        player_num = 1;
        player_side = whiteside;
        self_sock = gameMsg->clnt_sock[1];
        competitor_sock = gameMsg->clnt_sock[0];
        printf("player1's side is white\n");
    }
    pthread_mutex_unlock(&mut);  //unlock the gameMsg
    //printf("I am player%d\nmy sock is %d\n",player_num,self_sock);
    //sleep(3);
    printf("player%d:start to receive the function_0\n",player_num);
    read(self_sock,buffer,BUFFER_SIZE);  // receive function_0 pkg
    pkg = (Package*)buffer;
    if(pkg->function == 0)   // start game
    {
        if(player_side==whiteside)
            create_package((Package*)buffer,1,1,0,0,0,NULL); //create function_1 pkg
        else
            create_package((Package*)buffer,1,0,0,0,0,NULL);
        write(self_sock,buffer,sizeof(Package));  // send function_1 pkg
        printf("function_1 pkg for player%d is sent\n",player_num);
    }
    else
    {
        perror("can't receive function_0 pkg!\n");
        pthread_exit(NULL);
    }

    int if_error = 0;  // if error occured, it store the pkg->function
    printf("start to listen to player%d for chess msg\n",player_num);
    while(1)  // start to loop to collect chess msg
    {
        read(self_sock,buffer,BUFFER_SIZE);  // receive a package
        pkg = (Package*)buffer;    // ctrl the pkg
        if_error = pkg_process(pkg,gameMsg,self_sock,competitor_sock,player_side);
        if(if_error!=0)
            printf("player%d:something wrong in pkg, function:%d\n",player_num,if_error);
        /*if(check_win());  // need to supplement!
            break;*/
    }

    pthread_exit(NULL);
}



