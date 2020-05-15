#include <pthread.h>

#define DEBUG   //for debug
#define IP_ADDRESS "39.99.228.164"   // the server's ip
#define BUFFER_SIZE 200
#pragma pack(1)   //close the byte alignment

/*global variable define*/
//extern int clnt_wait = 0;
//extern pthread_mutex_t mut;

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
} Game_Msg;


/*mock to create a package for test*/
Package* mock_create_package();
void game_start(pthread_t Thread[],int Clnt_Sock[]);
void* pthread_func(void* args);
void thread_wait(pthread_t thread[]);
void close_game(Game_Msg* gameMsg);
void create_package(Package* head,int F,int Ex,int arr1,int arr2,int BFL,char* BUF);
int pkg_process(Package* pkg, Game_Msg* msg, int self_sock, int competitor_sock, Chess_Msg side);


