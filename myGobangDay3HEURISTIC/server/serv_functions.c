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
extern int error_count;

#ifdef DEBUG
typedef enum chess_msg{
    blackside = 1,
    whiteside = -1,
    blank = 0
} Chess_Msg;

/*package define*/
typedef struct package{
    int function;
    int extendedFlag;
    int dataArray[2];
    int bufferLength;
    char buf[];
} Package;

/*the clnt list node define*/
typedef struct clnt_node{
    int clnt_sock;
    struct clnt_node* nextptr;
} Clnt_Node;

/*game_msg*/
typedef struct game_msg{
    pthread_t thread[2];
    int clnt_sock[2];
    Chess_Msg bored[15][15];
    int bored_round[15][15];
    Chess_Msg worb;
    char player[2][30];
    int round_num;
    Chess_Msg winner;
} Game_Msg;

#endif

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
    int x = pkg->dataArray[0];//1-15
    int y = pkg->dataArray[1];//1-15
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
            msg->winner = check_win(msg->bored,x,y,side);  // check winner
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
Chess_Msg check_win(Chess_Msg board[][17] , int x , int y , Chess_Msg target)  // to check if one side win/return the result
{
   int number = 1;  // to check how many cheese are in a row
   int i = 0 , i2 = 0; // counter

   for(i = -1 ; i != 3 ; i += 2)
   {
      for(i2 = 1 ; i2 < 5 ; i2++)
      {
         if(board[x+i*i2][y]==target)
            number++;
         else
            break;
      }
   }
   if(number>=5)
      return target;
   else
      number = 1;

   for(i = -1 ; i != 3 ; i += 2)
   {
      for(i2 = 1 ; i2 < 5 ; i2++)
      {
         if(board[x][y+i*i2]==target)
            number++;
         else
            break;
      }
   }
   if(number>=5)
      return target;
   else
      number = 1;

   for(i = -1 ; i != 3 ; i += 2)
   {
      for(i2 = 1 ; i2 < 5 ; i2++)
      {
         if(board[x+i*i2][y+i*i2]==target)
            number++;
         else
            break;
      }
   }
   if(number>=5)
      return target;
   else
      number = 1;

   for(i = -1 ; i != 3 ; i += 2)
   {
      for(i2 = 1 ; i2 < 5 ; i2++)
      {
         if(board[x-i*i2][y+i*i2]==target)
            number++;
         else
            break;
      }
   }
   if(number>=5)
      return target;
   else
      number = 1;

   return blank;
}
