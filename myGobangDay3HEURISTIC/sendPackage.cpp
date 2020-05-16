#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define IP_ADDRESS "39.99.228.164"
//#define IP_ADDRESS "127.0.0.1"
#define Port 5000
#pragma pack(1)

struct Package{
  int function;
  int extendedFlag;
  int dataArray[2];
  int bufferLength;
  char buf[];
};

void printPack(struct Package *package)
{
    printf("Function: %d\n",package->function);
    printf("extendedFlag: %d\n",package->extendedFlag);
    printf("dataArray[0]: %d\n",package->dataArray[0]);
    printf("dataArray[1]: %d\n",package->dataArray[1]);
    printf("bufferLength: %d\n",package->bufferLength);
    printf("buf: %s\n",package->buf);
}

int main()
{
    int ret = 0;

    struct Package *recv_package;

    while(1)
    {
        //创建套接字
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == -1)
        {
            printf("Create socket failed!\n");
            return 0;
        }
        else
        {
            printf("Create socket success!\n");
        }

        //向服务器（特定的IP和端口）发起请求
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
        serv_addr.sin_family = AF_INET;  //使用IPv4地址
        serv_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);  //具体的IP地址
        serv_addr.sin_port = htons(Port);  //端口

        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        {
            printf("Connect failed!\n");
            return 0;
        }
        else
        {
            printf("Connect success!\n");
        }

        while(1)
        {
            ///读取服务器传回的数据
            char buffer[200]={0};

            ret = read(sock, buffer, sizeof(buffer) - 1);

            if(ret < 0)
            {
                printf("Read message error.\n");
                break;
            }

            recv_package=(struct Package *)buffer;
            printPack(recv_package);

            char response[30] = "quick response";

            ret = write(sock,response,sizeof(response));
            if(ret<0)
            {
                printf("Write message error.\n");
                break;
            }

            break;
        }

        //关闭套接字
        close(sock);
        break;
    }

    return 0;
}
