#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define DEBUG   //for debug
#define IP_ADDRESS "39.99.228.164"   // the server's ip
#pragma pack(1)   //close the byte alignment

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

/*mock to create a package for test*/
Package* mock_create_package();

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
    int clnt_sock;
    Package * pkg;
    char str[30];
    while(1)
    {
        printf("start listening,,,\n");
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        //printf("port:%d\n",ntohs(clnt_addr.sin_port));
        printf("connected!\n");
        
        pkg = mock_create_package();
        //printf("%s\n",pkg->buf);
        //printf("%d\n",sizeof(pkg));
        write(clnt_sock, pkg, sizeof(Package)+30);
        read(clnt_sock, str, sizeof(str));
        printf("recive:%s\n",str);
        close(clnt_sock);
    }
    close(serv_sock);

    return 0;
}

Package* mock_create_package(){
    char s[30] = {0};
    printf("please input someting:");
    gets(s);
    Package * pkg = (Package*)malloc(sizeof(Package)+sizeof(s));
    memcpy(pkg->buf,s,sizeof(s));
    pkg->bufferLength = sizeof(s);
    pkg->function = 10;
    pkg->extendedFlag = 23;
    pkg->dataArray[0] = 33;
    pkg->dataArray[1] = 44;
    return pkg;
}

