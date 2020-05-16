#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define IP_ADDRESS "120.79.219.131"
#pragma pack(1)

typedef struct package{
    int function;
    int extendedFlag;
    int dataArray[2];
    int bufferLength;
//    char buf[];
} Package;

Package* mock_create_package();
void printPack(Package *package);
int main(){
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(serv_sock, 20);
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock;
    Package * pkg;
    char str[30];
    char buffer[200]={0};
    while(1)
    {
        printf("start listening...");
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        printf("\nconnected!\n");

        pkg=mock_create_package();
        int FIRST=pkg->extendedFlag;
        write(clnt_sock, pkg, sizeof(Package));

        if(FIRST)
        {
            while(1)
            {
                read(clnt_sock, buffer, sizeof(buffer));
                pkg=(Package*)buffer;
                if(pkg->function==0&&pkg->extendedFlag==2)
                    break;
                printPack(pkg);
                pkg = mock_create_package();
                write(clnt_sock, pkg, sizeof(Package));

        //        write(clnt_sock, pkg, sizeof(Package)+30);
            }
        }
        else
        {
            while(1)
            {
                pkg = mock_create_package();
                write(clnt_sock, pkg, sizeof(Package));

                read(clnt_sock, buffer, sizeof(buffer));
                pkg=(Package*)buffer;
                if(pkg->function==0&&pkg->extendedFlag==2)
                    break;
                printPack(pkg);


        //        write(clnt_sock, pkg, sizeof(Package)+30);
            }
        }


        close(clnt_sock);
    }
    close(serv_sock);

    return 0;
}

Package* mock_create_package(){
//    char s[30] = {0};
//    printf("\nplease input someting:");
//    gets(s);
    Package * pkg = (Package*)malloc(sizeof(Package));
//    Package * pkg = (Package*)malloc(sizeof(Package)+sizeof(s));
//    memcpy(pkg->buf,s,sizeof(s));
    pkg->bufferLength = 0;
    pkg->function = 2;
    pkg->extendedFlag = 0;
    pkg->dataArray[0] = (rand() % (15-1+1))+ 1;
    pkg->dataArray[1] = (rand() % (15-1+1))+ 1;
    return pkg;
}

void printPack(Package *package)
{
    printf("Function: %d\n",package->function);
    printf("extendedFlag: %d\n",package->extendedFlag);
    printf("dataArray[0]: %d\n",package->dataArray[0]);
    printf("dataArray[1]: %d\n",package->dataArray[1]);
    printf("bufferLength: %d\n",package->bufferLength);
//    printf("buf: %s\n",package->buf);
}
